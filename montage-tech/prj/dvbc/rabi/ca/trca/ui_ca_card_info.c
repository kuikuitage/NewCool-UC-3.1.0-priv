/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_card_info.h"
#include "ui_ca_public.h"

enum prog_info_cont_ctrl_id
{
  IDC_INVALID = 0,
  IDC_CA_SMART_CARD_NUMBER = 1,
  IDC_CA_CARD_VERSION,
  IDC_CA_CAS_VER,
  IDC_CA_WATCH_LEVE,
  IDC_CA_CHILD_FEED,
};

#define CARD_SN_MAX_LEN             16
#define CARD_EIGEN_VALU_MAX_LEN     64

u16 card_info_root_keymap(u16 key);
RET_CODE card_info_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
  
RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
	control_t *p_cont = NULL, *p_ctrl = NULL;
	u16 stxt_card_info[]=
	{
		IDS_CA_SERIAL_NUMBER, 
		IDS_CA_CARD_VERSION,
		IDS_CA_CARD_VER,
		IDS_CA_WATCH_LEVEL,
		IDS_CARD_TYPE,
	};
	
	u16 y=0, i=0;
	DEBUG(DBG,INFO,"open_ca_card_info now\n");
	p_cont = ui_comm_right_root_create(ROOT_ID_CA_CARD_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, card_info_root_keymap);
	ctrl_set_proc(p_cont, card_info_root_proc);

	y = ACCEPT_INFO_CA_INFO_ITEM_Y;
	for(i=0; i < ACCEPT_INFO_CARD_INFO_ITEM_CNT; i++)
	{
		switch(i)
		{
			case 0:
			case 1:
			case 2:
			case 3:
						p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_SMART_CARD_NUMBER + i),
			                                       ACCEPT_INFO_CA_INFO_ITEM_X, y,
			                                       ACCEPT_INFO_CA_INFO_ITEM_LW,
			                                       ACCEPT_INFO_CA_INFO_ITEM_RW);
						ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
						ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
						ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
						break;
			case 4:
						p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_SMART_CARD_NUMBER + i),
			                                       ACCEPT_INFO_CA_INFO_ITEM_X, y,
			                                       ACCEPT_INFO_CA_INFO_ITEM_LW,
			                                       ACCEPT_INFO_CA_INFO_ITEM_RW);
						ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
						ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
						ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
				break;
			default:
				break;
				
		}

		y += COMM_CTRL_H + ACCEPT_CA_INFO_ITEM_V_GAP;
	}

	ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

	ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
	return SUCCESS;
}

static void accept_ca_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
	u8 asc_str[64];

	control_t *p_card_id = ctrl_get_child_by_id(p_cont, IDC_CA_SMART_CARD_NUMBER);
	control_t *p_cas_card_version = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_VERSION);
	control_t *p_cas_ver = ctrl_get_child_by_id(p_cont, IDC_CA_CAS_VER);
	control_t *p_cas_leve = ctrl_get_child_by_id(p_cont, IDC_CA_WATCH_LEVE);
	control_t *p_cas_feed = ctrl_get_child_by_id(p_cont, IDC_CA_CHILD_FEED);

	if((ui_get_smart_card_state() == FALSE)||(p_card_info == NULL))
	{
		ui_comm_static_set_content_by_ascstr(p_card_id,(u8 *) " ");
		ui_comm_static_set_content_by_ascstr(p_cas_card_version,(u8 *) " ");
		ui_comm_static_set_content_by_ascstr(p_cas_ver,(u8 *) " ");
		ui_comm_static_set_content_by_ascstr(p_cas_leve,(u8 *) " ");
	}
	else
	{
		//Smart card serial number
		sprintf((char *)asc_str, "%s", p_card_info->sn);
		ui_comm_static_set_content_by_ascstr(p_card_id, asc_str);

		//card_version
		sprintf((char *)asc_str, "%s", p_card_info->card_version);
		ui_comm_static_set_content_by_ascstr(p_cas_card_version, asc_str);

		//CA Version
		sprintf((char *)asc_str, "%s", p_card_info->cas_ver);
		ui_comm_static_set_content_by_ascstr(p_cas_ver, asc_str);

		//Watch Level
		sprintf((char *)asc_str, "%d", p_card_info->cas_rating);
		ui_comm_static_set_content_by_ascstr(p_cas_leve, asc_str);

		if(p_card_info->card_type == 0)
			ui_comm_static_set_content_by_strid(p_cas_feed, IDS_CA_MOTHER_CARD);
		else
			ui_comm_static_set_content_by_strid(p_cas_feed, IDS_CA_CHILD_CARD);
	}
	return;
}

static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS, INFO," \n");
	if(para1 != SUCCESS)
		return SUCCESS;

	accept_ca_info_set_content(p_cont, (cas_card_info_t *)para2);
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
  ON_COMMAND(MSG_EXIT, on_exit_card_info)
END_MSGPROC(card_info_root_proc, ui_comm_root_proc)


