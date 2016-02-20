/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "ui_common.h"
#include "ads_ware.h"
#include "config_ads.h"
#include "ui_ads_display.h"
#include "ui_ad_api.h"

#define		DESAI_ADS_FLASH_DIRECT_WRITE	(0)

/*!
  DVBC lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;
  /*!
    sym
    */
  u32 tp_sym;
  /*!
    nim modulate
    */
  u16 nim_modulate;

} ad_lock_t;

u8 *p_ads_mem = NULL;

static u32 adm_id = ADS_ID_ADT_DESAI;
static ad_lock_t ad_maintp = {307000,6875,2};
static ads_module_cfg_t module_config;
static u8 version[25];

static RET_CODE nvram_write(u32 offset, u8 *p_buf, u32 size)
{
#if	DESAI_ADS_FLASH_DIRECT_WRITE
	void *p_dm_handle = NULL;

	DEBUG(ADS,INFO,"get in \n");
	p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
	MT_ASSERT(p_dm_handle != NULL);

	DEBUG(ADS,INFO,"ad write addr[0x%x], size[0x%x]\n", offset, size);
	if(size!=0)
	{
		dm_direct_write(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset,size,p_buf);
	}
	DEBUG(ADS,INFO,"AD nvram_write end\n");
#else
	void *p_dev = NULL;

	p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_dev);
	
	offset 	 +=  module_config.flash_start_adr;
    if (0 != size)
    {
      if(SUCCESS != charsto_writeonly(p_dev, offset, p_buf, size))
      {
        DEBUG(ADS,INFO,"[DS]Write data to nvram failed!\n");
      }
    }
#endif
	return TRUE;
}

static RET_CODE nvram_read(u32 offset, u8 *p_buf, u32 *p_size)
{
#if	DESAI_ADS_FLASH_DIRECT_WRITE
	void *p_dm_handle = NULL;

	MT_ASSERT(p_buf != NULL);
	MT_ASSERT(p_size != NULL);

	p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
	MT_ASSERT(p_dm_handle != NULL);

	DEBUG(ADS,INFO,"offset:[0x%x], size[0x%x]\n", offset,*p_size);
	dm_direct_read(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset,*p_size,p_buf);
#else
	void *p_dev = NULL;

	p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
	MT_ASSERT(NULL != p_dev);
	
	offset 	 +=  module_config.flash_start_adr;
    if(SUCCESS != charsto_read(p_dev, offset, p_buf, *p_size))
    {
        DEBUG(ADS,INFO,"[DS]Read data from nvram failed!\n");
    }

#endif
	return TRUE; 
}

static RET_CODE nvram_erase(u32 size)
{
#if	DESAI_ADS_FLASH_DIRECT_WRITE
  dm_ret_t ret = DM_SUC;
  void *p_dm_handle = NULL;
  u32 offset = 0;

  DEBUG(ADS,INFO,"Erase size:%d\n\n", size);
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  ret = dm_direct_erase(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset, size);
  MT_ASSERT(ret == DM_SUC);
#else
	void *p_dev = NULL;

	p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
	MT_ASSERT(NULL != p_dev);
	
	if(SUCCESS != charsto_erase(p_dev, module_config.flash_start_adr, size))
	{
		DEBUG(ADS,INFO,"charsto_erase fail!\n");
	}
#endif
  return TRUE;
}

static RET_CODE nvram_erase2(u32 offset, u32 size)
{
#if	DESAI_ADS_FLASH_DIRECT_WRITE
	void *p_dm_handle;
	u32 flash_size;

	p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
	MT_ASSERT(p_dm_handle != NULL);
	flash_size = dm_get_block_size(p_dm_handle, DEC_RW_ADS_BLOCK_ID);

	DEBUG(ADS,INFO,"Erase size:%x  offset = %x\n\n", size, offset);
	if (offset + size > flash_size)
	{
		DEBUG(ADS,INFO,"Erase Failed! offset:%d, size:%d\n", offset, size);
		return ERR_FAILURE;
	}

	dm_direct_erase(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset, size);
#else
	void *p_dev = NULL;

	p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
	MT_ASSERT(NULL != p_dev);

	offset	 +=  module_config.flash_start_adr;
	if(SUCCESS != charsto_erase(p_dev, offset, size))
	{
		DEBUG(ADS,INFO,"charsto_erase fail!\n");
	}

#endif
	return TRUE;
}

void ui_close_ad_logo(void)
{
	void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);

	void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_DISPLAY);

	vdec_stop(p_video_dev);
	disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
}

u8* ads_get_ad_version(void)
{
	return version;
}

static void ads_module_open(u32 m_id)
{
	RET_CODE ret = ERR_FAILURE;

	ret = ads_open(m_id);
	DEBUG(ADS,INFO,"ADS: ads_module_open===%d\n", ret);
}

static RET_CODE ads_notify_msg_to_ui(BOOL is_sync, u16 content, u32 para1, u32 para2)
{
	//pay attention, the parameter content will not be changed
	return fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG, is_sync, MSG_ADS_TO_UI, para1, para2);
}

static RET_CODE ads_desai_module_init()
{
	ads_adapter_cfg_t adapter_cfg = {0};
	RET_CODE ret = ERR_FAILURE;

	ads_init(adm_id,&adapter_cfg);

	memset(&module_config, 0, sizeof(ads_module_cfg_t));
	module_config.channel_frequency 	= ad_maintp.tp_freq;
	module_config.channel_symbolrate 	= ad_maintp.tp_sym;
	module_config.channel_qam 			= ad_maintp.nim_modulate;

#if	DESAI_ADS_FLASH_DIRECT_WRITE
	module_config.flash_start_adr		= 0;
#else
	module_config.flash_start_adr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), CADATA_BLOCK_ID)
        - get_flash_addr();
#endif
	module_config.nvram_read			= nvram_read;
	module_config.nvram_write			= nvram_write;
	module_config.nvram_erase			= nvram_erase;
	module_config.nvram_erase2 			= nvram_erase2;
	module_config.flash_size			= dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_BLOCK_ID);
	module_config.p_dmx_dev 			= dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	module_config.pic_in 				= ADS_PIC_IN_SDRAM;
	module_config.task_prio_start		= AD_TASK_PRIO_START;
	module_config.task_prio_end			= AD_TASK_PRIO_END;
	module_config.platform_type 		= ADS_PLATFORM_HD;
	module_config.notify_msg_to_ui 		= ads_notify_msg_to_ui;

	DEBUG(ADS,INFO,"ADS:ads_module_config.flash_start_adr===0x%x, flash_size = 0x%x \n",
	                  module_config.flash_start_adr, module_config.flash_size);

	ret = ads_adt_desai_attach(&module_config, &adm_id);
	MT_ASSERT(SUCCESS == ret);

	ui_adv_set_adm_id(adm_id);

	ads_module_init((ads_module_id_t)adm_id);
	ads_module_open(adm_id);
	mtos_task_delay_ms(100);

	memset(version,0,25);
	ads_io_ctrl(ADS_ID_ADT_DESAI, ADS_IOCMD_AD_VERSION_GET, version);
	DEBUG(ADS,INFO,"Desai_ADS Version:%s\n",version);

	return SUCCESS;
}

static void set_adver_maintp(u32 tp_freq,u32 tp_sym,u8 nim_modulate)
{
	ad_maintp.tp_freq = tp_freq;
	ad_maintp.tp_sym = tp_sym;
	switch(nim_modulate )
	{
		case 4:
			ad_maintp.nim_modulate = 16;
			break;

		case 5:
			ad_maintp.nim_modulate = 32;
			break;

		case 6:
			ad_maintp.nim_modulate = 64;
			break;

		case 7:
			ad_maintp.nim_modulate = 128;
			break;

		case 8:
			ad_maintp.nim_modulate = 256;
			break;

		default:
			ad_maintp.nim_modulate = 64;
			break;
	}
}

void ads_ap_init(void)
{
	dvbc_lock_t tmp_maintp = {0};

	sys_status_get_main_tp1(&tmp_maintp);
	set_adver_maintp(tmp_maintp.tp_freq,tmp_maintp.tp_sym,tmp_maintp.nim_modulate);  
	ads_desai_module_init();
	return;
}

