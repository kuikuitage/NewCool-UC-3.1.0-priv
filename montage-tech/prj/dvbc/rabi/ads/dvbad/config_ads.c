/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ads_api.h"


typedef struct
{
	/*!freq*/
	u32 tp_freq;
	/*!sym*/
	u32 tp_sym;
	/*!nim modulate*/
	u16 nim_modulate;
} ad_lock_t;

static ad_lock_t ad_maintp = {307000,6875,2};

u8 *p_ads_mem = NULL;
u32 adm_id = ADS_ID_ADT_DVBAD;
static ads_module_cfg_t module_config = {0};
static u8 version[25];

extern RET_CODE ads_adt_dvbad_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id);
extern RET_CODE dvbad_get_logo_flag(void);
extern u32 get_flash_addr(void);
extern RET_CODE dvbad_get_logo_size(dvbad_log_save_info *p_logo_info);
extern RET_CODE dvbad_get_logo(u8 *p_log_data, u32 size);

//store the AD data to flash
static RET_CODE nvram_write(u32 offset, u8 *p_buf, u32 size)
{
	RET_CODE ret = FALSE;
	void *p_dm_handle = NULL;

	p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
	MT_ASSERT(p_dm_handle != NULL);

	DEBUG(ADS, INFO,"ad write addr[0x%x], size[0x%x]\n", offset, size);
	if(size!=0)
	{
		ret=dm_direct_write(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset,size,p_buf);
		if(DM_FAIL == ret)
		{
			DEBUG(ADS, ERR, "write ad to flash failure\n");
			return ERR_FAILURE;
		}
	}
	DEBUG(ADS, INFO,"AD nvram_write end\n");
	return SUCCESS;
}

static RET_CODE nvram_read(u32 offset, u8 *p_buf, u32 *p_size)
{
	void *p_dm_handle = NULL;
	u32 nvm_read =0;

	p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
	MT_ASSERT(p_dm_handle != NULL);

	DEBUG(ADS, INFO,"ad read dm_direct_read addr[0x%x], size[0x%x]\n", offset,*p_size);
	nvm_read=dm_direct_read(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset,*p_size,p_buf);
	DEBUG(ADS, INFO,"ad nvm_read[0x%x]\n", nvm_read);
	return SUCCESS;
}
static RET_CODE nvram_erase(u32 size)
{
	dm_ret_t ret = DM_SUC;
	void *p_dm_handle = NULL;
	u32 offset = 0;

	p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
	MT_ASSERT(p_dm_handle != NULL);

	DEBUG(ADS, INFO,"ADS:Erase size_pre===%#x\n", size);
	ret=dm_direct_erase(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset, module_config.flash_size);
	MT_ASSERT(ret != DM_FAIL);
	DEBUG(ADS, INFO,"dm_direct_erase return :%d\n", ret);

	return SUCCESS;
}

static RET_CODE nvram_erase2(u32 offset, u32 size)
{
	dm_ret_t ret = DM_SUC;
	void *p_dm_handle = NULL;

	p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
	MT_ASSERT(p_dm_handle != NULL);

	DEBUG(ADS, INFO,"ADS:Erase size_erase = 0x%x \n", size);
	ret=dm_direct_erase(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset, size);
	MT_ASSERT(ret != DM_FAIL);

	return TRUE;
}

u8* ads_get_ad_version(void)
{
	return version;
}

static void ads_module_open(u32 m_id)
{
	RET_CODE ret = ERR_FAILURE;

	ret = ads_open(m_id);
	DEBUG(ADS, INFO,"ADS: ads_module_open===%d\n", ret);
}

static RET_CODE ads_dvbad_module_init(void)
{
	RET_CODE ret = ERR_FAILURE;
	ads_adapter_cfg_t adapter_cfg = {0};

	ads_init(ADS_ID_ADT_DVBAD,&adapter_cfg);

	memset(&module_config, 0, sizeof(ads_module_cfg_t));
	module_config.channel_frequency = ad_maintp.tp_freq;
	module_config.channel_symbolrate = ad_maintp.tp_sym;
	module_config.channel_qam = ad_maintp.nim_modulate;

	module_config.flash_start_adr= 0;

	module_config.nvram_read= nvram_read;
	module_config.nvram_write= nvram_write;
	module_config.nvram_erase= nvram_erase;
	module_config.nvram_erase2 = nvram_erase2;
	module_config.flash_size= dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_BLOCK_ID);
	module_config.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	module_config.pic_in = ADS_PIC_IN_SDRAM;
	module_config.task_prio_start= AD_TASK_PRIO_START;
	module_config.task_prio_end= AD_TASK_PRIO_END;
	module_config.platform_type = ADS_PLATFORM_HD;
	DEBUG(ADS, INFO,"ADS:ads_module_config.flash_start_adr===0x%x, flash_size = 0x%x \n",   module_config.flash_start_adr, module_config.flash_size);

	ret = ads_adt_dvbad_attach(&module_config, &adm_id);
	if(ret != SUCCESS)
	{
		return ret;
	}

	ui_adv_set_adm_id(adm_id);
	ads_module_init((ads_module_id_t)adm_id);
	ads_module_open(ADS_ID_ADT_DVBAD);
	mtos_task_delay_ms(100);

	memset(version,0,25);
	ads_io_ctrl(ADS_ID_ADT_DVBAD, ADS_IOCMD_AD_VERSION_GET, version);
	DEBUG(ADS, INFO,"ad Version:%s\n",version);
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
	if(ads_dvbad_module_init() == SUCCESS)
	{
		DEBUG(ADS,INFO,"[AD_INIT] show ads logon");  
		ui_ads_show_logo();
	}
}

