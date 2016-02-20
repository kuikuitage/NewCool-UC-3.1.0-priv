/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_card_update.h"
#include "ui_ca_public.h"

enum control_id
{
	IDC_INVALID = 0,
	IDC_TITLE,
	IDC_BG,
	IDC_UPDATE_BAR,
	IDC_CONTENT,
};

static u8 card_upgrade_type;

u16 ca_card_update_keymap(u16 key);
RET_CODE ca_card_update_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_card_update_info(u32 para1, u32 para2)
{
	control_t *p_content = NULL, *p_bar = NULL, *p_title = NULL, *p_cont = NULL;
	u8 title_type = ((para2 >> 8) & (0XFF));
	card_upgrade_type = title_type;

	if(fw_find_root_by_id(ROOT_ID_CA_CARD_UPDATE) != NULL)
	{
		DEBUG(CAS,INFO,"ROOT_ID_CA_CARD_UPDATE already opened............!!! \n");
		return ERR_FAILURE;
	}
	p_cont = fw_create_mainwin(ROOT_ID_CA_CARD_UPDATE,
							   CA_UP_CONT_FULL_X, CA_UP_CONT_FULL_Y,
							   CA_UP_CONT_FULL_W, CA_UP_CONT_FULL_H,
							   ROOT_ID_INVALID, 0,OBJ_ATTR_INACTIVE, 0);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_rstyle(p_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
	ctrl_set_keymap(p_cont, ca_card_update_keymap);
	ctrl_set_proc(p_cont, ca_card_update_proc);
	//title
	p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_TITLE,
	                     CA_UP_TITLE_FULL_X, CA_UP_TITLE_FULL_Y,
	                     CA_UP_TITLE_FULL_W,CA_UP_TITLE_FULL_H,
	                     p_cont, 0);
	ctrl_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_content_type(p_title, TEXT_STRTYPE_STRID);
	if(title_type == 1)
	{
		text_set_content_by_strid(p_title, IDS_CA_CARD_UPGRADE_PROGRESS);
	}
	else
	{
		text_set_content_by_strid(p_title, IDS_CA_CARD_UPDATE_PROG);
	}
	//bar
	p_bar = ui_comm_bar_create(p_cont, IDC_UPDATE_BAR,
	                   CA_STATUS_BAR_MX, CA_STATUS_BAR_MY, CA_STATUS_BAR_MW, CA_STATUS_BAR_MH,
	                   CA_STATUS_BAR_LX, CA_STATUS_BAR_LY, CA_STATUS_BAR_LW, CA_STATUS_BAR_LH,
	                   CA_STATUS_BAR_RX, CA_STATUS_BAR_RY,CA_STATUS_BAR_RW, CA_STATUS_BAR_RH);
	ui_comm_bar_set_param(p_bar, IDS_PROGRESS, 0, 100, 100);
	ui_comm_bar_set_style(p_bar,
	                      RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_MID_SKY_BLUE,
	                      RSI_PBACK, FSI_WHITE,
	                      RSI_PBACK, FSI_WHITE);
	ui_comm_bar_update(p_bar, 0, TRUE);
	DEBUG(CAS,INFO,"IN\n");

	//content
	p_content = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
	                   CA_UP_CONTENT_FULL_X, CA_UP_CONTENT_FULL_Y,
	                   CA_UP_CONTENT_FULL_W, CA_UP_CONTENT_FULL_H,
	                   p_cont, 0);
	ctrl_set_rstyle(p_content, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_content, FSI_WHITE,FSI_WHITE,FSI_WHITE);
	text_set_align_type(p_content, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_content, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_content, IDS_CA_CARD_UPGRADING);

	ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);
	return SUCCESS;
}

static RET_CODE on_ca_card_up_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	u8 percent 		= (para2 & 0XFF);
	u8 title_type 	= ((para2 >> 8) & (0XFF));
	control_t *p_bar   = NULL;
	control_t *p_title = NULL;

	DEBUG(CAS,INFO,"PROCESS -> update progress\n");

	p_bar   = ctrl_get_child_by_id(p_cont, IDC_UPDATE_BAR);
	p_title = ctrl_get_child_by_id(p_cont, IDC_TITLE);

	if(p_bar == NULL )
	{
		return SUCCESS;
	}
	
	if(ctrl_get_sts(p_bar) != OBJ_STS_HIDE)
	{
		DEBUG(CAS,INFO,"UPDATE PROGRESS -> %d\n", percent);
		ui_comm_bar_update(p_bar, percent, TRUE);
		ui_comm_bar_paint(p_bar, TRUE);

		if(card_upgrade_type != title_type)
		{
			if(title_type == 1)
			{
				text_set_content_by_strid(p_title, IDS_CA_CARD_UPGRADE_PROGRESS);
			}
			else
			{
				text_set_content_by_strid(p_title, IDS_CA_CARD_UPDATE_PROG);
			}
			card_upgrade_type = title_type;
		}
	}

	ctrl_paint_ctrl(p_cont, TRUE);

	return SUCCESS;
}

BEGIN_KEYMAP(ca_card_update_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(ca_card_update_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_card_update_proc, cont_class_proc)
	ON_COMMAND(MSG_CA_CARD_UPDATE_PROGRESS, on_ca_card_up_info_update)
END_MSGPROC(ca_card_update_proc, cont_class_proc)

