/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
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

// driver headers
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
//#include "ap_ota.h"
#include "sys_status.h"
#include "customer_config.h"
//#include "config_prj.h"
#include "config_cas.h"
#include "ui_dbase_api.h"

//#define WRITE_DATA_TO_MEM
static u8 *p_ca_buffer = NULL;


/*!
  CA Data1 block
  */
#define CADATA1_BLOCK_ID              0xAC
/*!
  CA Data2 block
  */
#define CADATA2_BLOCK_ID              0xAD

static RET_CODE nvram_read(u32 cadata_block_id, u8 *p_buf, u32 *size)
{
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(dm_handle != NULL);

  if(cadata_block_id == 1)
  {
    dm_direct_read(dm_handle, CADATA1_BLOCK_ID, 0, *size, p_buf);
  }
  else if(cadata_block_id == 2)
  {
    dm_direct_read(dm_handle, CADATA2_BLOCK_ID, 0, *size, p_buf);
  }
  OS_PRINTF("nvram_read success!\n");
  return SUCCESS;
}

static RET_CODE nvram_write(u32 cadata_block_id, u8 *p_buf, u32 size)
{
  RET_CODE ret = SUCCESS;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  u8 block_id = 0;
  OS_PRINTF("nvram_write success!\n");

  if(cadata_block_id == 1)
  {
    //base_addr =  dm_get_block_addr(dm_handle, CADATA1_BLOCK_ID) - get_flash_addr();
    block_id = CADATA1_BLOCK_ID;
  }
  else if(cadata_block_id == 2)
  {
    //base_addr =  dm_get_block_addr(dm_handle, CADATA2_BLOCK_ID) - get_flash_addr();
    block_id = CADATA2_BLOCK_ID;
  }
  MT_ASSERT(dm_handle != NULL);

  dm_direct_read(dm_handle, block_id, 0, CHARSTO_SECTOR_SIZE, p_ca_buffer);
  
  memset(p_ca_buffer, 0, CHARSTO_SECTOR_SIZE);
  
  memcpy(p_ca_buffer, p_buf, CHARSTO_SECTOR_SIZE);
  
  ret = dm_direct_erase(dm_handle, block_id, 0, CHARSTO_SECTOR_SIZE);
  MT_ASSERT(ret != DM_FAIL);
  
  ret = dm_direct_write(dm_handle, block_id, 0, CHARSTO_SECTOR_SIZE, p_ca_buffer);
  MT_ASSERT(ret != DM_FAIL);

  OS_PRINTF("nvram_write success!\n");
  return ret;

}

RET_CODE machine_serial_get(u8 *p_buf, u32 size)
{
	u32 i = 0;
	u8 cdcas_stb_id[CDCAS_CA_STDID_SIZE] = {0};
	u8 first_byte = 0;
	u8 second_byte = 0;

	if(!sys_get_serial_num(cdcas_stb_id, CDCAS_CA_STDID_SIZE))
	{
		return ERR_FAILURE;
	}
	
	for(i = 0; i < size; i++)
	{
		OS_PRINTF("yxsb_stb_id[i * 2] = %d, yxsb_stb_id[i * 2 + 1] = %d\n",cdcas_stb_id[i * 2], cdcas_stb_id[i * 2 + 1]);
		if(cdcas_stb_id[i * 2] >= 'a' && cdcas_stb_id[i * 2] <= 'f')
		{
			first_byte = cdcas_stb_id[i * 2] - 'a' + 10;
		}
		else if(cdcas_stb_id[i * 2] >= 'A' && cdcas_stb_id[i * 2] <= 'F')
		{
			first_byte = cdcas_stb_id[i * 2] - 'A' + 10;
		}
		else if(cdcas_stb_id[i * 2] >= '0' && cdcas_stb_id[i * 2] <= '9')
		{
			first_byte = cdcas_stb_id[i * 2] - '0';
		}

		if(cdcas_stb_id[i * 2 + 1] >= 'a' && cdcas_stb_id[i * 2 + 1] <= 'f')
		{
			second_byte = cdcas_stb_id[i * 2 + 1] - 'a' + 10;
		}
		else if(cdcas_stb_id[i * 2 + 1] >= 'A' && cdcas_stb_id[i * 2 + 1] <= 'F')
		{
			second_byte = cdcas_stb_id[i * 2 + 1] - 'A' + 10;
		}
		else if(cdcas_stb_id[i * 2 + 1] >= '0' && cdcas_stb_id[i * 2 + 1] <= '9')
		{
			second_byte = cdcas_stb_id[i * 2 + 1] - '0';
		}

		p_buf[i] = first_byte * 16 + second_byte;
	}
	return SUCCESS;

}


static void register_monitor_table(void)
{
  
  m_register_t reg;
  m_svc_t *p_svc = NULL;
  OS_PRINTF("CALL register_monitor_table\n");

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

void on_cas_init(void)
{
  u32 cas_id = 0;
  cas_adapter_cfg_t cas_cfg = {0};
  cas_module_cfg_t cas_module_cfg = {0};

  
  p_ca_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
  MT_ASSERT(NULL != p_ca_buffer);
  memset(p_ca_buffer, 0, CHARSTO_SECTOR_SIZE);
  //config cas adapter
  cas_cfg.p_smc_drv[0] = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);

  cas_cfg.slot_num = 1;

  cas_cfg.p_task_stack = mtos_malloc(4096);
  cas_cfg.stack_size = 4096;
  cas_cfg.task_prio = DRV_CAS_ADAPTER_TASK_PRIORITY;
#ifndef WIN32
  cas_init(&cas_cfg);
#endif
  cas_module_cfg.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  cas_module_cfg.nvram_read = nvram_read;
  cas_module_cfg.nvram_write = nvram_write;
  cas_module_cfg.machine_serial_get = machine_serial_get;
  cas_module_cfg.flash_start_adr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), CADATA1_BLOCK_ID)
      - get_flash_addr();
  OS_PRINTF("set ca addr is 0x%x\n", cas_module_cfg.flash_start_adr);

  cas_module_cfg.flash_size = CA_DATA_SIZE;
  cas_module_cfg.task_prio = MDL_CAS_TASK_PRIO_BEGIN;
  cas_module_cfg.end_task_prio = MDL_CAS_TASK_PRIO_END;
#ifndef WIN32
  cas_yxsb_attach(&cas_module_cfg, &cas_id);
  cas_module_init(CAS_ID_TF);
#endif
  register_monitor_table();
}


