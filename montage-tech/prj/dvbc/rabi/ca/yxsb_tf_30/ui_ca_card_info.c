/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_ca_card_info.h"
#include "ui_ca_public.h"

enum prog_info_cont_ctrl_id
{
	IDC_CA_SMART_CARD_NUMBER = 1,
	IDC_CA_CARD_VERSION,
	IDC_CA_STB_ID,
	IDC_CA_WATCH_LEVE,
	IDC_CA_WORK_TIME,
	IDC_CA_CARD_UPG_TIME,
	IDC_CA_CARD_UPG_STATE,
	IDC_CA_PAIR_STATE,
	IDC_CA_PAIRED_STB1_ID,
	IDC_CA_PAIRED_STB2_ID,
	IDC_CA_PAIRED_STB3_ID,
	IDC_CA_PAIRED_STB4_ID,
	IDC_CA_PAIRED_STB5_ID,
};

u16 card_info_root_keymap(u16 key);
RET_CODE card_info_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
	control_t *p_cont,*p_ctrl;
	u16 stxt_card_info[]=
	{
		IDS_CA_SMART_CARD_NUMBER,
		IDS_CA_CARD_VER, 
		IDS_CUR_STBID,
		IDS_CA_WATCH_LEVEL,
		IDS_CA_WORK_TIME,
		IDS_TF_PATCH_TIME,
		IDS_TF_PATCH_STATUS,
		IDS_CA_PAIR_STATE,
		IDS_TF_PAIRED_STB_ID,
		IDS_TF_PAIRED_STB_ID,
		IDS_TF_PAIRED_STB_ID,
		IDS_TF_PAIRED_STB_ID,
		IDS_TF_PAIRED_STB_ID,
	};
	u16 y=0, i=0;

	p_cont = ui_comm_right_root_create(ROOT_ID_CA_CARD_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, card_info_root_keymap);
	ctrl_set_proc(p_cont, card_info_root_proc);

	y = ACCEPT_INFO_CA_INFO_ITEM_Y;
	for(i=0; i < TF_CARD_INFO_ITEM_CNT; i++)
	{
		p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_SMART_CARD_NUMBER + i),
				                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
				                           ACCEPT_INFO_CA_INFO_ITEM_LW,
				                           ACCEPT_INFO_CA_INFO_ITEM_RW);
		ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
		if((IDC_CA_PAIR_STATE == (i + IDC_CA_SMART_CARD_NUMBER))||
			(IDC_CA_CARD_UPG_STATE == (i + IDC_CA_SMART_CARD_NUMBER)))
			ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
		else
			ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
		
		ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
		if(IDC_CA_SMART_CARD_NUMBER + i > IDC_CA_PAIR_STATE)
			ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);

		y += ACCEPT_INFO_CA_INFO_ITEM_H + ACCEPT_CA_INFO_ITEM_V_GAP;
	}

	ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);
	
	#ifndef WIN32
	ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
	ui_ca_do_cmd(CAS_CMD_PAIRE_STATUS_GET, 0, 0);
	#endif
	return SUCCESS;
}

static void conditional_accept_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
	u8 asc_str[64] = {0};
	utc_time_t card_upg_time = {0,};

	control_t *p_card_num 		= ctrl_get_child_by_id(p_cont, IDC_CA_SMART_CARD_NUMBER);
	control_t *p_cas_ver 		= ctrl_get_child_by_id(p_cont, IDC_CA_CARD_VERSION);
	control_t *p_stb_id 		= ctrl_get_child_by_id(p_cont, IDC_CA_STB_ID);
	control_t *p_watch_level 	= ctrl_get_child_by_id(p_cont, IDC_CA_WATCH_LEVE); 
	control_t *p_work_time 		= ctrl_get_child_by_id(p_cont, IDC_CA_WORK_TIME); 
	control_t *p_ca_upg_time 	= ctrl_get_child_by_id(p_cont, IDC_CA_CARD_UPG_TIME);
	control_t *p_ca_upg_satue 	= ctrl_get_child_by_id(p_cont, IDC_CA_CARD_UPG_STATE);

	memset(asc_str, 0, sizeof(asc_str));
	if(strlen((char *)p_card_info->stb_sn) != 0)
	{
		sprintf((char*)asc_str, "%02X%02X%02X%02X%02X%02X",p_card_info->stb_sn[0], p_card_info->stb_sn[1], p_card_info->stb_sn[2],
								p_card_info->stb_sn[3], p_card_info->stb_sn[4], p_card_info->stb_sn[5]);
		ui_comm_static_set_content_by_ascstr(p_stb_id, asc_str);
	}
	else
	{
		return;
	}

	sprintf((char *)asc_str, "%s", p_card_info->sn);
	ui_comm_static_set_content_by_ascstr(p_card_num, asc_str);

	sprintf((char *)asc_str, "%02X%02X%02X%02X",p_card_info->cas_ver[3],
	  				p_card_info->cas_ver[2], p_card_info->cas_ver[1], p_card_info->cas_ver[0]);
	ui_comm_static_set_content_by_ascstr(p_cas_ver, asc_str);

	sprintf((char *)asc_str, "%02d:%02d:%02d~%02d:%02d:%02d ",
					p_card_info->work_time.start_hour,p_card_info->work_time.start_minute,
					p_card_info->work_time.start_second,p_card_info->work_time.end_hour,
					p_card_info->work_time.end_minute,p_card_info->work_time.end_second);
	ui_comm_static_set_content_by_ascstr(p_work_time, asc_str);

	sprintf((char *)asc_str, "%d", p_card_info->cas_rating);
	ui_comm_static_set_content_by_ascstr(p_watch_level, asc_str);


	//upg time
	memset(asc_str, 0, sizeof(asc_str));
	sys_status_get_card_upg_time(&card_upg_time);
	DEBUG(CAS,INFO,"@@@@@ %.4d.%.2d.%.2d %.2d:%.2d\n", card_upg_time.year, card_upg_time.month, card_upg_time.day,
		  card_upg_time.hour, card_upg_time.minute);
	if((card_upg_time.month > 12) || (card_upg_time.day > 31))
	{
		sprintf((char*)asc_str, " ");
    }
    else
	{
		sprintf((char*)asc_str, "%.4d.%.2d.%.2d %.2d:%.2d", card_upg_time.year, card_upg_time.month, card_upg_time.day,
		  card_upg_time.hour, card_upg_time.minute);
		ui_comm_static_set_content_by_ascstr(p_ca_upg_time, asc_str);
	}

	//upg status
	memset(asc_str, 0, sizeof(asc_str));
	if(sys_status_get_card_upg_state() == 1)
	{
		ui_comm_static_set_content_by_strid(p_ca_upg_satue, IDS_TF_CARD_UPGRADE_SUCCESS);
	}
	else if(sys_status_get_card_upg_state() == 0)
	{
		ui_comm_static_set_content_by_strid(p_ca_upg_satue, IDS_TF_CARD_UPGRADE_FAILURE);
	}
	
	
}

static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS, INFO," \n");
	if(para1 != SUCCESS)
		return SUCCESS;

	conditional_accept_info_set_content(p_cont, (cas_card_info_t *)para2);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

	return SUCCESS;
}

static RET_CODE on_conditional_accept_pair_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	u16 i;
	u8 asc_str[32] = {0};
	card_stb_paired_list_t *p_paired_list = (card_stb_paired_list_t *)para2;
	control_t *p_state = ctrl_get_child_by_id(p_cont, IDC_CA_PAIR_STATE);
	
	DEBUG(CAS,INFO,"@@@@@ state: %d, %x \n", p_paired_list->paired_status, p_paired_list->STB_list);
	switch(p_paired_list->paired_status)
	{
	  case CAS_EVT_NONE:
		ui_comm_static_set_content_by_strid(p_state,IDS_CA_NOT_PAIRED);

		for (i = 0; i < p_paired_list->paired_num; i++)
		{
			memset(asc_str, 0, sizeof(asc_str));
			sprintf((char*)asc_str, "%.2X%.2X%.2X%.2X%.2X%.2X", p_paired_list->STB_list[6 * i], 
					p_paired_list->STB_list[6 * i + 1], p_paired_list->STB_list[6 * i + 2],
					p_paired_list->STB_list[6 * i + 3], p_paired_list->STB_list[6 * i + 4],
					p_paired_list->STB_list[6 * i + 5]);
			
			p_state = ctrl_get_child_by_id(p_cont, (IDC_CA_PAIRED_STB1_ID + i));
			ui_comm_static_set_content_by_ascstr(p_state,asc_str);
			ctrl_set_sts(p_state, OBJ_STS_SHOW);
		}
		DEBUG(CAS,INFO,"CAS_EVT_NONE\n");
		break;
	  case CAS_E_ILLEGAL_CARD:
		ui_comm_static_set_content_by_strid(p_state,IDS_CA_NO_CARD_INVALID);
		DEBUG(CAS,INFO,"CAS_E_ILLEGAL_CARD\n");
		break;
	  case CAS_E_CARD_PAIROTHER:
		ui_comm_static_set_content_by_strid(p_state, IDS_TF_CARD_PAIR_OTHER_STB);
		DEBUG(CAS,INFO,"CAS_E_CARD_PAIROTHER\n");
		break;
	  case CAS_E_CARD_DIS_PARTNER:
		ui_comm_static_set_content_by_strid(p_state, IDS_TF_CARD_NO_PAIR_STB);
		DEBUG(CAS,INFO,"CAS_E_CARD_DIS_PARTNER\n");
		break;
	  default:
		ui_comm_static_set_content_by_strid(p_state, IDS_CAS_E_UNKNOWN_ERROR);
		DEBUG(CAS,INFO,"UNKNOWN_ERROR\n");
		break;
	}

	DEBUG(CAS,INFO,"@@@@@ paired_num:%d\n", p_paired_list->paired_num);
	ctrl_paint_ctrl(p_cont, TRUE);
	return SUCCESS;
}

RET_CODE on_exit_card_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	fw_destroy_mainwin_by_id(ROOT_ID_CA_CARD_INFO);
	return SUCCESS;
}


BEGIN_KEYMAP(card_info_root_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(card_info_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(card_info_root_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_info_update)
  ON_COMMAND(MSG_CA_PAIRE_INFO, on_conditional_accept_pair_update)
  ON_COMMAND(MSG_EXIT, on_exit_card_info)
END_MSGPROC(card_info_root_proc, ui_comm_root_proc)


