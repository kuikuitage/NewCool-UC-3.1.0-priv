/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
// std headers
#include "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

// util headers
#include "class_factory.h"

// os headers
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"
#include "mtos_task.h"


// driver
#include "lib_util.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal_irq_jazz.h"
#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"


#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "spi.h"
#include "cas_ware.h"
#include "driver.h"
#include "lpower.h"

// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "monitor_service.h"
#include "mosaic.h"
#include "cat.h"
#include "pmt.h"
#include "pat.h"
#include "emm.h"
#include "ecm.h"
#include "nit.h"
#include "cas_ware.h"
#include "Data_manager.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
// ap headers
#include "ap_framework.h"
#include "ap_cas.h"
#include "sys_cfg.h"
#include "db_dvbs.h"
#include "ss_ctrl.h"
#include "sys_status.h"
#include "config_cas.h"
#include "sys_dbg.h"
#include "ui_dbase_api.h"

//#define CONFIG_CAS_DEBUG
#ifdef CONFIG_CAS_DEBUG
	#define CONFIG_CAS_PRINTF OS_PRINTF
#else
	#define CONFIG_CAS_PRINTF DUMMY_PRINTF
#endif

#define INVALID_PG_ID 0xFFFF
static u8 *p_ca_buffer = NULL;
extern u32 get_flash_addr(void);

static u32 nvram_read(cas_module_id_t module_id, u16 node_id, u16 offset, u32 length, u8 *p_buffer)
{
  u32 read_len = 0;
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID, 
            node_id, offset, length, p_buffer);

  CONFIG_CAS_PRINTF("[CONFIG_CAS] %s:%d read_len = %d \n",__FUNCTION__,__LINE__,read_len);
  return read_len;
}
static RET_CODE nvram_write(cas_module_id_t module_id, u16 *p_node_id, u8 *p_buffer, u16 len)
{
  dm_ret_t ret = DM_SUC;
  ret = dm_write_node(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID, 
                  p_node_id, p_buffer, len);
  if (DM_SUC == ret)
  {
    CONFIG_CAS_PRINTF("[CONFIG_CAS] %s:%d \n",__FUNCTION__,__LINE__);
    return SUCCESS;
  }
  else
  {
    CONFIG_CAS_PRINTF("[CONFIG_CAS] %s:%d \n",__FUNCTION__,__LINE__);
    return ERR_FAILURE;
  }
}

static RET_CODE nvram_del(cas_module_id_t module_id, u16 node_id)
{
  dm_ret_t ret = DM_SUC;
  ret = dm_del_node(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID, node_id, TRUE);
  if (DM_SUC == ret)
  {
    CONFIG_CAS_PRINTF("[CONFIG_CAS] %s:%d \n",__FUNCTION__,__LINE__);
    return SUCCESS;
  }
  else
  {
    CONFIG_CAS_PRINTF("[CONFIG_CAS] %s:%d \n",__FUNCTION__,__LINE__);
    return ERR_FAILURE;
  }
}
static RET_CODE nvram_node_list(cas_module_id_t module_id, u16 *p_buffer, u16 *p_max)
{
  u32 total_num;
  total_num = dm_get_node_id_list(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID, 
                    p_buffer, *p_max);
  *p_max = (u16)total_num;
  CONFIG_CAS_PRINTF("[CONFIG_CAS] %s:%d total_num = %d\n",__FUNCTION__,__LINE__,total_num);
  return SUCCESS;
}

static void register_monitor_table(void)
{
  m_register_t reg;
  m_svc_t *p_svc = NULL;

  p_svc = class_get_handle_by_id(M_SVC_CLASS_ID);
  reg.app_id = APP_CA;
  reg.num = 0;
  reg.table[reg.num].t_id = M_PMT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;

  reg.table[reg.num].t_id = M_CAT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;

  reg.table[reg.num].t_id = M_NIT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;

  dvb_monitor_register_table(p_svc, &reg);

}


static u16 find_pgid_by_id_in_currview(u32 ori_network_id,u32 service_id, u32 ts_id)
{   
    u8 view_id;
    u8 i = 0;
    u16 view_count = 0, pg_id = INVALID_PG_ID;
    dvbs_prog_node_t pg = {0};
    
    view_id = ui_dbase_get_pg_view_id();
    view_count = db_dvbs_get_count(view_id);
  
    for(i=0; i<view_count; i++)
    {
        pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
  
        if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
        {
            OS_PRINTF("\r\n***%s, n_id[0x%x],s_id[0x%x], ts_id[0x%x], i[%d]***",
                          __FUNCTION__, pg.orig_net_id,pg.s_id, pg.ts_id, i);
             if((pg.orig_net_id == ori_network_id) && (pg.s_id == service_id) && (pg.ts_id == ts_id))
            {
                return pg_id;
            }
        }
        if(0 == (i%50))
        {
            mtos_task_sleep(10);
        }
    }

    return INVALID_PG_ID;  
}

static RET_CODE force_channel_pg_check(u16 usNetWorkID, u16 usTransportStreamID, u16 usServiceID)
{
    RET_CODE ret = ERR_FAILURE;

    if(find_pgid_by_id_in_currview(usNetWorkID,usServiceID, usTransportStreamID) != INVALID_PG_ID)
    {
        ret = SUCCESS;
    }
    return ret;
}

static RET_CODE get_stbid(u8 *p_buf, u32 size)
{
	//strcpy((char *)p_buf,(char *)"100");
    sys_get_serial_num(p_buf, 64);
	DEBUG(CAS, ERR, "p_buf = %s\n",p_buf);
	if(strlen(p_buf) > 0)
    	return SUCCESS;
	else
		return ERR_FAILURE;
}
extern RET_CODE cas_gs_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);


void on_cas_init(void)
{

    u32 cas_id = 0;

    cas_adapter_cfg_t cas_cfg = {0};
    cas_module_cfg_t cas_module_cfg = {0};
    p_ca_buffer = mtos_malloc(CA_DATA_SIZE);
    MT_ASSERT(NULL != p_ca_buffer);
    memset(p_ca_buffer, 0, CA_DATA_SIZE);
    //config cas adapter
    cas_cfg.p_smc_drv[0] = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);

    cas_cfg.slot_num = 1;

    cas_cfg.p_task_stack = mtos_malloc(4096);
    cas_cfg.stack_size = 4096;
    cas_cfg.task_prio = DRV_CAS_ADAPTER_TASK_PRIORITY;
	
    cas_cfg.p_data_task_stack = mtos_malloc(4096);
    MT_ASSERT(NULL != cas_cfg.p_data_task_stack);
    cas_cfg.data_stack_size = 4096;
    cas_cfg.data_task_prio = MDL_CAS_TASK_PRIO_END;
    cas_cfg.nvram_read = nvram_read;
    cas_cfg.nvram_write = nvram_write;
    cas_cfg.nvram_node_list = nvram_node_list;
    cas_cfg.nvram_del = nvram_del;
  
#ifndef WIN32
    cas_init(&cas_cfg);
#endif
    cas_module_cfg.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    cas_module_cfg.nvram_read = NULL;
    cas_module_cfg.nvram_write = NULL;
  
    cas_module_cfg.flash_start_adr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID)
        - get_flash_addr();
    OS_PRINTF("set ca addr is 0x%x\n", cas_module_cfg.flash_start_adr);
    cas_module_cfg.flash_size = CA_DATA_SIZE;
    cas_module_cfg.machine_serial_get = get_stbid;
  
    
    cas_module_cfg.task_prio = MDL_CAS_TASK_PRIO_BEGIN;                            //need fixed
    cas_module_cfg.end_task_prio = MDL_CAS_TASK_PRIO_END;                    //need fixed
    cas_module_cfg.query_check = force_channel_pg_check;
	cas_module_cfg.area_limit_free = AREA_LIMIT_FREE; 
	
#ifndef WIN32
    cas_gs_attach(&cas_module_cfg, &cas_id);
    cas_module_init(CAS_ID_GS);
#endif
    register_monitor_table();
}

