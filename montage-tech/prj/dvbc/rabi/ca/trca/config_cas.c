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
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
// driver headers
#include "drv_dev.h"
#include "nim.h"

// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "cat.h"
#include "mosaic.h"
#include "pmt.h"
#include "pat.h"
#include "emm.h"
#include "ecm.h"
#include "nit.h"
#include "pti.h"
#include "cas_ware.h"
#include "data_manager.h"

// ap headers
#include "ap_framework.h"
#include "ap_cas.h"
#include "sys_cfg.h"
#include "monitor_service.h"
#include "sys_cfg.h"
#include "db_dvbs.h"

#include "ui_common.h"
#include "cas_manager.h"

u8 *p_ca_buffer = NULL;

static RET_CODE nvram_read(u32 offset, u8 *p_buf, u32 *size)
{
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  RET_CODE ret = SUCCESS;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  u32 base_addr =  dm_get_block_addr(dm_handle, CADATA_BLOCK_ID) - get_flash_addr();

  mtos_task_lock();

  MT_ASSERT(dm_handle != NULL);

  ret = charsto_read(p_charsto_dev, base_addr + offset, p_buf, *size);
  if (ret != SUCCESS)
  {
    mtos_task_unlock();
    OS_PRINTF("read error\n");
    return ERR_FAILURE;
  }
  OS_PRINTF("nvram_read success!\n");

  mtos_task_unlock();
  return SUCCESS;
}

static RET_CODE nvram_write(u32 offset, u8 *p_buf, u32 size)
{
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  RET_CODE ret = SUCCESS;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  u32 base_addr = dm_get_block_addr(dm_handle, CADATA_BLOCK_ID) - get_flash_addr();
  u32 start_addr = 0;
  u32 padding = 0;
  u32 wrlen = 0;

  mtos_task_lock();

  MT_ASSERT(dm_handle != NULL);
  OS_PRINTF("ca write addr[0x%x], size[0x%x]\n", offset, size);

  start_addr = ((base_addr + offset) / CHARSTO_SECTOR_SIZE) * CHARSTO_SECTOR_SIZE;
  padding = (base_addr + offset) % CHARSTO_SECTOR_SIZE;

  while(size)
  {
    if (size > (CHARSTO_SECTOR_SIZE - padding))
    {
      wrlen = CHARSTO_SECTOR_SIZE - padding;
    }
    else
    {
      wrlen = size;
    }
    //read sector data
    ret = charsto_read(p_charsto_dev, start_addr, p_ca_buffer, CHARSTO_SECTOR_SIZE);
    if (ret != SUCCESS)
    {
      mtos_task_unlock();
      OS_PRINTF("write/read error1\n");
      return ERR_FAILURE;
    }
    memcpy(p_ca_buffer + padding, p_buf, wrlen);
    //charsto_unprotect_all(p_charsto_dev);
    ret = charsto_erase(p_charsto_dev, start_addr, 1);
    //charsto_protect_all(p_charsto_dev);
    if (ret != SUCCESS)
    {
      mtos_task_unlock();
      OS_PRINTF("write/erase error2\n");
      return ERR_FAILURE;
    }
    //charsto_unprotect_all(p_charsto_dev);
    charsto_writeonly(p_charsto_dev, start_addr, p_ca_buffer, CHARSTO_SECTOR_SIZE);
    //charsto_protect_all(p_charsto_dev);
    if (ret != SUCCESS)
    {
      mtos_task_unlock();
      OS_PRINTF("write error3\n");
      return ERR_FAILURE;
    }
    start_addr += CHARSTO_SECTOR_SIZE;
    padding = 0;
    p_buf += wrlen;
    size -= wrlen;
  }
  OS_PRINTF("nvram_write success!\n");

  mtos_task_unlock();
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
	p_ca_buffer = mtos_malloc(CA_DATA_SIZE);
	MT_ASSERT(NULL != p_ca_buffer);
	memset(p_ca_buffer, 0, CA_DATA_SIZE);

	//config cas adapter
	cas_cfg.p_smc_drv[0] = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
	cas_cfg.slot_num = 1;
	cas_cfg.p_task_stack = mtos_malloc(4096);
	MT_ASSERT(NULL != cas_cfg.p_task_stack);
	cas_cfg.stack_size = 4096;
	cas_cfg.task_prio = DRV_CAS_ADAPTER_TASK_PRIORITY;
	cas_cfg.p_data_task_stack = mtos_malloc(4096);
	MT_ASSERT(NULL != cas_cfg.p_data_task_stack);
	cas_cfg.data_stack_size = 4096;
	//cas_cfg.data_task_prio = MDL_CAS_TASK_PRIO_BEGIN+1;

#ifndef WIN32
	cas_init(&cas_cfg);
#endif
	cas_module_cfg.cas_lib_type = CAS_LIB_TEST;
	cas_module_cfg.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	cas_module_cfg.max_mail_num = 30;
	cas_module_cfg.mail_policy = POLICY_BY_ID;

	cas_module_cfg.flash_start_adr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID)
	  - get_flash_addr();
	OS_PRINTF("set ca addr is 0x%x\n", cas_module_cfg.flash_start_adr);
	cas_module_cfg.flash_size = CA_DATA_SIZE;
	cas_module_cfg.level = 0;
	cas_module_cfg.task_prio = MDL_CAS_TASK_PRIO_BEGIN;
	cas_module_cfg.end_task_prio = MDL_CAS_TASK_PRIO_END;
	cas_module_cfg.stack_size = 8 * KBYTES;
	cas_module_cfg.p_task_stack = mtos_malloc(8 * KBYTES);
	cas_module_cfg.filter_mode = ECM_FILTER_CONTINUOUS_DISABLED;
	cas_module_cfg.nvram_read = nvram_read;
	cas_module_cfg.nvram_write = nvram_write;

	cas_module_cfg.queue_task_prio = LOWEST_TASK_PRIORITY -2; //LOWEST_TASK_PRIORITY -1; //panhui


	MT_ASSERT(cas_module_cfg.p_task_stack != NULL);
#ifndef WIN32
	cas_tr_attach(&cas_module_cfg, &cas_id);
	cas_module_init(CAS_ID_TR);
#endif
	register_monitor_table();
}

