/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "ui_common.h"
#include "ads_xml_parser.h"
#include "ui_config.h"
#include "pdec.h"
#include "fcrc.h"
#include "ui_signal.h"
#include "ads_api.h"
#include "ui_ca_public.h"

static BOOL is_ads_osd_roll = FALSE;
static u8 uc_current_osd_index = 0;
static dvbad_osd_program_show_info osd_prom_show_info;
static ST_ADS_OSD_INFO osd_display_info[MAX_ROLL_OSD_NUM];
static BOOL is_full_screen_ad_on_showing = FALSE;

extern void on_desktop_start_roll(u16 *p_uni_str, u32 roll_mode, u32 value,u32 dis_pos);


//desai ads display func
void ads_dvbad_set_type(ads_ad_type_t type, u32 * cmd, ads_info_t * p_cfg)
{
	switch(type)
	{
		case ADS_AD_TYPE_MENU_UP:
		case ADS_AD_TYPE_MENU_DOWN:
		case ADS_AD_TYPE_MAIN_MENU:
		case ADS_AD_TYPE_SUB_MENU:
		case ADS_AD_TYPE_OTHER_GIF:
		case ADS_AD_TYPE_SEARCHING:
		case ADS_AD_TYPE_SIGNAL:
			p_cfg->ads_type = ADS_AD_TYPE_MAIN_MENU;
			*cmd = ADS_IOCMD_AD_NO_PROGRAM;
			break;
		case ADS_AD_TYPE_LOGO:
			p_cfg->ads_type = ADS_AD_TYPE_LOGO;
			*cmd = ADS_IOCMD_AD_NO_PROGRAM;
			break;			
		case ADS_AD_TYPE_CHLIST:
			p_cfg->ads_type =  ADS_AD_TYPE_CHLIST;
	  		*cmd = ADS_IOCMD_AD_NO_PROGRAM;
			break;

		case ADS_AD_TYPE_CHBAR:
			p_cfg->ads_type =  ADS_AD_TYPE_CHBAR;
			*cmd = ADS_IOCMD_AD_PROGRAM;
			break;
		case ADS_AD_TYPE_EPG:
			p_cfg->ads_type =  ADS_AD_TYPE_EPG;
  	  		*cmd = ADS_IOCMD_AD_NO_PROGRAM;
			break;
		case ADS_AD_TYPE_VOLBAR:
			p_cfg->ads_type =  ADS_AD_TYPE_VOLBAR;
      		*cmd = ADS_IOCMD_AD_NO_PROGRAM;
			break;
		case ADS_AD_TYPE_BANNER:
			p_cfg->ads_type =  ADS_AD_TYPE_BANNER;
      		*cmd = ADS_IOCMD_AD_NO_PROGRAM;
			break;
		case ADS_AD_TYPE_UNAUTH_PG:
			p_cfg->ads_type =  ADS_AD_TYPE_UNAUTH_PG;
			*cmd = ADS_IOCMD_AD_NO_PROGRAM;
			break;
		default:
			break;           
	}
}

void ui_set_full_scr_ad_status(BOOL status)
{
	is_full_screen_ad_on_showing = status;
}

BOOL ui_get_full_scr_ad_status(void)
{
	return is_full_screen_ad_on_showing;
}

BOOL ui_get_ads_osd_status(void)
{
	return is_ads_osd_roll;
}

void ui_ads_osd_roll(void)
{
	dvbad_osd_show_info * osd_show_info = NULL;
	u16 uni_str[260] = {0};
	u16 index = 0;

	osd_show_info = (dvbad_osd_show_info *)(osd_prom_show_info.stTextShow + uc_current_osd_index);

	DEBUG(ADS, INFO,"[AD_OSD] **** osd_show_info->usContentSize = %d \n",osd_show_info->usContentSize);
	DEBUG(ADS, INFO,"[AD_OSD] **** osd_show_info->pucDisplayBuffer = %s \n",osd_show_info->pucDisplayBuffer);
	DEBUG(ADS, INFO,"[AD_OSD] **** osd_show_info->pucDisplayBuffer[%d] = %s \n",uc_current_osd_index, osd_display_info[uc_current_osd_index].osd_display_buffer);

	if(osd_display_info[uc_current_osd_index].b_SaveFlag == TRUE)
	{
		/******************************************************************************
		//不判断CA滚动字幕状态，直接显示广告滚动，
		******************************************************************************/
		gb2312_to_unicode(osd_display_info[uc_current_osd_index].osd_display_buffer, 
		                                                        osd_show_info->usContentSize, uni_str, MAX_ROLL_OSD_LENGTH);

		on_desktop_start_roll(uni_str, 0, osd_show_info->ucShowTimes, osd_show_info->enDisplayPos);
		set_ca_msg_rolling_status(TRUE);
		uc_current_osd_index++;
	}
	else
	{ 
		for(index = uc_current_osd_index + 1; index <= osd_prom_show_info.ucOsdNum; index++)
		{
			if(index == osd_prom_show_info.ucOsdNum)
			{
				uc_current_osd_index = 0;
				break;
			}
			else
			{
				if(osd_display_info[index].b_SaveFlag == TRUE)
				{ 
					osd_show_info = (dvbad_osd_show_info *)(osd_prom_show_info.stTextShow + index);
					gb2312_to_unicode(osd_display_info[uc_current_osd_index].osd_display_buffer, 
					                                                osd_show_info->usContentSize, uni_str, MAX_ROLL_OSD_LENGTH);

					on_desktop_start_roll(uni_str, 0, osd_show_info->ucShowTimes, osd_show_info->enDisplayPos);
					set_ca_msg_rolling_status(TRUE);
					uc_current_osd_index = ++index;
					break;
				}
			}
		}
	}

	if(uc_current_osd_index == osd_prom_show_info.ucOsdNum)
	{
		uc_current_osd_index = 0;
	}
}

void ui_ads_osd_roll_stop(void)
{
	RET_CODE ret = 0;
	is_ads_osd_roll = FALSE;
	set_ca_msg_rolling_status(FALSE);
	ret = on_ca_rolling_over(fw_find_root_by_id(ROOT_ID_BACKGROUND), 0, 0, 0);
}

RET_CODE ui_show_dvbad_osd_ad(u8 *p_data)
{
	RET_CODE ret = ERR_FAILURE;
	u8 uc_ads_osd_num = 0;
	u32 index = 0;
	u16 content_size = 0;
	dvbad_osd_program_show_info * pst_osd_ads_roll = NULL;

	pst_osd_ads_roll = (dvbad_osd_program_show_info*)p_data;
	if(pst_osd_ads_roll == NULL)
	{
		DEBUG(ADS, INFO,"[AD_OSD] ads osd null!! \n");
		return ERR_FAILURE;
	}  
	memset((void *)&osd_prom_show_info, 0, sizeof(dvbad_osd_program_show_info));
	memcpy((void *)&osd_prom_show_info, (void *)pst_osd_ads_roll, sizeof(dvbad_osd_program_show_info));

	uc_ads_osd_num = osd_prom_show_info.ucOsdNum;

	if(uc_ads_osd_num == 0)
	{
		DEBUG(ADS, INFO,"[ad_osd] ads osd num is 0, return \n");
		return ERR_FAILURE;
	}
  
	memset((void *)osd_display_info, 0, MAX_ROLL_OSD_NUM * sizeof(ST_ADS_OSD_INFO));
	DEBUG(ADS, INFO,"[ad_osd] osd_display_info mem clr num = %d \n", MAX_ROLL_OSD_NUM * sizeof(ST_ADS_OSD_INFO));
   
	for(index = 0; index < uc_ads_osd_num; index++)
	{
		content_size = pst_osd_ads_roll->stTextShow[index].usContentSize;

		if(content_size> MAX_ROLL_OSD_LENGTH)
		{
			content_size = MAX_ROLL_OSD_LENGTH;
		}

		if(pst_osd_ads_roll->stTextShow[index].pucDisplayBuffer != NULL)
		{
			memcpy(osd_display_info[index].osd_display_buffer, pst_osd_ads_roll->stTextShow[index].pucDisplayBuffer, content_size);
						*(osd_display_info[index].osd_display_buffer + content_size) = '\0';
			osd_display_info[index].b_SaveFlag = TRUE;
		}
		else
		{
			osd_display_info[index].b_SaveFlag = FALSE;
		}
	}
  
	//set current roll index to 1st osd msg
	uc_current_osd_index = 0;

	//ads osd display
	if(TRUE == get_ca_msg_rolling_status())   
	{
		ui_ads_osd_roll_stop();
	}
	ui_ads_osd_roll();

	is_ads_osd_roll = TRUE;

	return ret;
}

RET_CODE on_ads_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	return SUCCESS;
}


