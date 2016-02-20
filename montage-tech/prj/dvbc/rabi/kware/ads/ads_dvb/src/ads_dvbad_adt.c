/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "pti.h"
#include "ads_api.h"
#include "ui_common.h"
#include "ads_adapter.h"

#include "ads_dvbad_adt.h"
#include "ads_drv_dvbad.h"


#define ADS_DESAI_ADT_PRINTF

#ifdef ADS_DESAI_ADT_PRINTF
    #define ADS_ADT_PRINTF OS_PRINTF
#else
    #define ADS_ADT_PRINTF DUMMY_PRINTF
#endif

#define GET_AD_PID_MAX_TIMES    5000

extern RET_CODE dvbad_get_ad(dvbad_program_para para, dvbad_show_pic_info *p_pic, DVBAD_POS_TYPE ad_type);

ads_module_priv_t *p_dvbad_priv = NULL;
get_ad_flag g_ads_get_flag = GET_AD_START;
static ads_adapter_priv_t *g_ads_priv = NULL;

#if 0 //for test, will be delete
#define TEMP_NVRAM (512 * KBYTES)
u8 *g_temp_nvram = NULL;
#endif

RET_CODE ads_adt_dvbad_init(void *param)
{
	return SUCCESS;
}


RET_CODE ads_adt_dvbad_deinit()
{
	return SUCCESS;
}

void ADS_SetDataPid(u16 usADPid)
{
	usADPid=g_ads_get_flag;
}

void dvb_set_ad_flag(get_ad_flag flag)
{
    g_ads_get_flag = flag;
}

get_ad_flag dvb_get_ad_flag()
{
    return g_ads_get_flag;
}

static RET_CODE ads_adt_dvbad_open()
{
	BOOL ret = FALSE;
	u32 index = 0;
	get_ad_flag ad_flag = GET_AD_START;

	DEBUG(ADS,INFO,"ads_adt_dvbad_open start \n");

	//start monitor task
	dvbad_ads_client_init();

	ret = dvb_ad_nim_lock(p_dvbad_priv->channel_frequency,
										p_dvbad_priv->channel_symbolrate,
										p_dvbad_priv->channel_qam);
	if(ret == FALSE)
	{
		ADS_ADT_PRINTF("[adt] %s,%d ERROR! \n",__func__,__LINE__);
		return ERR_FAILURE;
	}

	dvbad_set_filter_bat();

	for(index = 0; index < GET_AD_PID_MAX_TIMES; index++)
	{
		ad_flag = dvb_get_ad_flag();
		if(GET_AD_START!=ad_flag)
			return SUCCESS;
		mtos_task_sleep(10);
	}

	DEBUG(ADS, ERR, "ad monitor time out\n");
	return ERR_FAILURE;
}



RET_CODE ads_adt_dvbad_close()
{
	return SUCCESS;
}

static DVBAD_POS_TYPE dvbad_type_get(u32 ads_type)
{
    DVBAD_POS_TYPE ad_type = DVBAD_STB_POS_UNKNOWN;

    switch(ads_type)
    {
        case ADS_AD_TYPE_LOGO:
		case ADS_AD_TYPE_WELCOME:
            ad_type = DVBAD_STB_LOG;
            break;

		case ADS_AD_TYPE_MAIN_MENU:
			ad_type = DVBAD_STB_MAINMENU;
			break;
			
        case ADS_AD_TYPE_CHLIST:
            ad_type = DVBAD_STB_CHLIST;
            break;

		case ADS_AD_TYPE_CHBAR:
			ad_type = DVBAD_STB_CURRENT_FOLLOW;
			break;
			
		case ADS_AD_TYPE_EPG:
			ad_type = DVBAD_STB_EPG;
			break;
			
		case ADS_AD_TYPE_VOLBAR:
			ad_type = DVBAD_STB_VOLUME;
			break;

		case ADS_AD_TYPE_BANNER:
			ad_type = DVBAD_STB_EPG;
				break;

		case ADS_AD_TYPE_UNAUTH_PG:
            ad_type = DVBAD_STB_POS_UNKNOWN;
            break;

    }
	
	DEBUG(ADS, INFO, "get ad_type end.ad_type:%d\n", ad_type);
    return ad_type;
}

RET_CODE ads_adt_dvbad_io_ctrl(u32 cmd, void *param)
{
	RET_CODE ret = SUCCESS;
	DVBAD_POS_TYPE dvbad_ad_type = DVBAD_STB_POS_UNKNOWN;
	dvbad_show_pic_info program_show;
	dvbad_program_para	  program_para = {0};
	ads_info_t *p_ads_info = (ads_info_t *)param;


	memset(&program_show, 0, sizeof(dvbad_show_pic_info));

	DEBUG(ADS, INFO, "cmd:%d,ad_type:%d,network_id:%d,ts_id:%d,service_id:%d\n",
						cmd, p_ads_info->ads_type, p_ads_info->network_id,
						p_ads_info->ts_id, p_ads_info->service_id);
	dvbad_ad_type = dvbad_type_get(p_ads_info->ads_type);
	switch(cmd)
	{
		case ADS_IOCMD_AD_NO_PROGRAM:
			DEBUG(ADS,INFO,"ADS_IOCMD_AD_NO_PROGRAM: %d \n",ADS_IOCMD_AD_NO_PROGRAM);
			ret = dvbad_get_ad(program_para, &program_show, dvbad_ad_type);
			if(ret != SUCCESS)
				break;
			p_ads_info->pic_x = program_show.x;
			p_ads_info->pic_y = program_show.y;
			p_ads_info->pic_w = program_show.w;
			p_ads_info->pic_h = program_show.h;

			p_ads_info->pic_index 		= program_show.pic_id;
			p_ads_info->ads_data_len 	= program_show.data_len;

			DEBUG(ADS,INFO,"rect x(%d) y(%d) w(%d) h(%d)\n",program_show.x,program_show.y,program_show.w,program_show.h);
			DEBUG(ADS,INFO,"pic_id:%d \n",program_show.pic_id);
			DEBUG(ADS,INFO,"pos_type: %d \n",program_show.pos_type);
			DEBUG(ADS,INFO,"pic_type: %d \n",program_show.pic_type);
			DEBUG(ADS,INFO,"data_len: %d \n",program_show.data_len);
			memcpy((u8 *)p_ads_info->p_priv, program_show.pic_data, program_show.data_len);

			break;

		case ADS_IOCMD_AD_PROGRAM:
			DEBUG(ADS,INFO,"ADS_IOCMD_AD_PROGRAM: %d \n",ADS_IOCMD_AD_PROGRAM);
			program_para.usNetworkId 	= p_ads_info->network_id;
			program_para.usTsId 		= p_ads_info->ts_id;
			program_para.usServiceId 	= p_ads_info->service_id;
			ret = dvbad_get_ad(program_para, &program_show, dvbad_ad_type);
			if(ret != SUCCESS)
				break;

			p_ads_info->pic_x = program_show.x;
			p_ads_info->pic_y = program_show.y;
			p_ads_info->pic_w = program_show.w;
			p_ads_info->pic_h = program_show.h;

			p_ads_info->pic_index 		= program_show.pic_id;
			p_ads_info->ads_data_len 	= program_show.data_len;

			DEBUG(ADS,INFO,"rect x(%d) y(%d) w(%d) h(%d)\n",program_show.x,program_show.y,program_show.w,program_show.h);
			DEBUG(ADS,INFO,"pic_id:%d \n",program_show.pic_id);
			DEBUG(ADS,INFO,"data_len: %d \n",program_show.data_len);
			DEBUG(ADS,INFO,"pos_type: %d \n",program_show.pos_type);
			DEBUG(ADS,INFO,"pic_type: %d \n",program_show.pic_type);
			memcpy((u8 *)p_ads_info->p_priv, program_show.pic_data, program_show.data_len);
			
			break;

		 case ADS_IOCMD_AD_VERSION_GET:
			 ret =	get_AD_version((u16 *)param);
			 break;
	}
	return ret;
}


RET_CODE ads_adt_dvbad_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id)
{
	ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);

	g_ads_priv = ads_get_adapter_priv();

	g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].attached = 1;
	g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].inited = 0;

	g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].func.init 		= ads_adt_dvbad_init;
	g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].func.deinit	= ads_adt_dvbad_deinit;
	g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].func.open		= ads_adt_dvbad_open;
	g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].func.close		= ads_adt_dvbad_close;
	g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].func.display	= NULL;
	g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].func.hide		= NULL;
	g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].func.io_ctrl	= ads_adt_dvbad_io_ctrl;

	p_dvbad_priv = g_ads_priv->adm_op[ADS_ID_ADT_DVBAD].p_priv
	 	= mtos_malloc(sizeof(ads_module_priv_t));
	MT_ASSERT(NULL != p_dvbad_priv);
	memset(p_dvbad_priv, 0x00, sizeof(ads_module_priv_t));

	p_dvbad_priv->id = ADS_ID_ADT_DVBAD;
	p_dvbad_priv->p_dmx_dev = p_cfg->p_dmx_dev;
	p_dvbad_priv->flash_size = p_cfg->flash_size;
	p_dvbad_priv->flash_start_adr = p_cfg->flash_start_adr;
	p_dvbad_priv->nv_read = p_cfg->nvram_read;
	p_dvbad_priv->nv_write = p_cfg->nvram_write;
	p_dvbad_priv->nv_erase= p_cfg->nvram_erase;
	p_dvbad_priv->read_gif = p_cfg->read_gif;
	p_dvbad_priv->read_rec = p_cfg->read_rec;
	p_dvbad_priv->write_rec = p_cfg->write_rec;
	p_dvbad_priv->display_ad = p_cfg->display_ad;
	p_dvbad_priv->channel_frequency = p_cfg->channel_frequency;
	p_dvbad_priv->channel_symbolrate = p_cfg->channel_symbolrate;
	p_dvbad_priv->channel_qam = p_cfg->channel_qam;
	//p_dvbad_priv->task_prio_start = p_cfg->task_prio_start + 1;
	p_dvbad_priv->task_prio_start = p_cfg->task_prio_start;
	p_dvbad_priv->task_prio_end = p_cfg->task_prio_end;

	*p_adm_id = ADS_ID_ADT_DVBAD;
	return SUCCESS;
}



