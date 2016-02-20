/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_tr_accept_level.h"
#include "ui_ca_public.h"

enum ca_level_control_id
{
	IDC_CA_LEVEL_CONTROL_PIN = 1,
	IDC_CA_LEVEL_CONTROL_LEVEL,
	IDC_CA_LEVEL_CHANGE_OK,
	IDC_CA_LEVEL_CHANGE_RESULT,
};

static cas_rating_set_t rate_info;

u16 tr_accept_level_cont_keymap(u16 key);
RET_CODE tr_accept_level_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 tr_accept_level_pwdedit_keymap(u16 key);
RET_CODE tr_accept_level_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static void tr_accept_level_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
	control_t *p_level = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CONTROL_LEVEL);
	u8 focus = 0;

	if(p_card_info == NULL)
	{
		ui_comm_select_set_focus(p_level, 0);
	}
	else
	{
		switch(p_card_info->cas_rating)
		{
			case 0:
				focus = 0;
				break;
			case 1:
				focus = 1;
				break;
			case 2:
				focus = 2;
				break;
			case 3:
				focus = 3;
				break;
			case 4:
				focus = 4;
				break;
			case 5:
				focus = 5;
				break;
			case 6:
				focus = 6;
				break;
			case 7:
				focus = 7;
				break;
			case 8:
				focus = 8;
				break;
			case 9:
				focus = 9;
				break;
			default:
				focus = 1;
				break;
		}

		if(focus > 0)
		{
			focus -= 1;
		}
		ui_comm_select_set_focus(p_level, focus);
	}
}

RET_CODE open_tr_accept_level(u32 para1, u32 para2)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl = NULL;

	u8 i;
	u16 y;
	u16 stxt_level_control[2] =
	{
		IDS_CA_INPUT_PIN, IDS_CA_SELECT_LEVEL,
	};

	p_cont = ui_comm_right_root_create(ROOT_ID_TR_LEVEL, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	if (p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, tr_accept_level_cont_keymap);
	ctrl_set_proc(p_cont, tr_accept_level_cont_proc);

	//level control
	y = TR_LEVEL_CONTROL_ITEM_Y;
	for (i = 0; i<TR_LEVEL_CONTROL_ITEM_CNT; i++)
	{
		switch(i)
		{
			case 0:
				p_ctrl = ui_comm_pwdedit_create(p_cont, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
						                   TR_LEVEL_CONTROL_ITEM_X, y,
						                   TR_LEVEL_CONTROL_ITEM_LW,
						                   TR_LEVEL_CONTROL_ITEM_RW);
				ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_level_control[i]);
				ui_comm_pwdedit_set_param(p_ctrl, 6);
				ui_comm_ctrl_set_keymap(p_ctrl, tr_accept_level_pwdedit_keymap);
				ui_comm_ctrl_set_proc(p_ctrl, tr_accept_level_pwdedit_proc);
				break;

			case 1:
				p_ctrl = ui_comm_select_create(p_cont, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
						                   TR_LEVEL_CONTROL_ITEM_X, y,
						                   TR_LEVEL_CONTROL_ITEM_LW,
						                   TR_LEVEL_CONTROL_ITEM_RW);
				ui_comm_select_set_static_txt(p_ctrl, stxt_level_control[i]);
				ui_comm_select_set_param(p_ctrl, TRUE,
				           CBOX_WORKMODE_STATIC, TR_ACCEPT_WATCH_LEVEL_TOTAL,
				           CBOX_ITEM_STRTYPE_STRID,
				           NULL);

				ui_comm_select_set_content(p_ctrl, 0, IDS_CA_LEVEL1);
				ui_comm_select_set_content(p_ctrl, 1, IDS_CA_LEVEL2);
				ui_comm_select_set_content(p_ctrl, 2, IDS_CA_LEVEL3);
				ui_comm_select_set_content(p_ctrl, 3, IDS_CA_LEVEL4);
				ui_comm_select_set_content(p_ctrl, 4, IDS_CA_LEVEL5);
				ui_comm_select_set_content(p_ctrl, 5, IDS_CA_LEVEL6);
				ui_comm_select_set_content(p_ctrl, 6, IDS_CA_LEVEL7);
				ui_comm_select_set_content(p_ctrl, 7, IDS_CA_LEVEL8);
				ui_comm_select_set_content(p_ctrl, 8, IDS_CA_LEVEL9);
				break;

			case 2:
				p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
							      TR_LEVEL_CONTROL_ITEM_X,y,
							      (TR_LEVEL_CONTROL_ITEM_LW + TR_LEVEL_CONTROL_ITEM_RW),
							      TR_LEVEL_CONTROL_ITEM_H,
							      p_cont, 0);
				ctrl_set_rstyle(p_ctrl,RSI_PBACK,RSI_SELECT_F,RSI_PBACK);
				text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
				text_set_offset(p_ctrl, COMM_CTRL_OX, 0);
				text_set_font_style(p_ctrl,FSI_WHITE_24,FSI_WHITE_24,FSI_WHITE_24);
				text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
				text_set_content_by_strid(p_ctrl,IDS_START);
				break;
			default:
				p_ctrl = NULL;
				break;
		}
		if(NULL  == p_ctrl)
		{
			break;
		}

		ctrl_set_related_id(p_ctrl,
		0,                                     /* left */
		(u8)((i - 1 +
		      TR_LEVEL_CONTROL_ITEM_CNT) %
		     TR_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN),           /* up */
		0,                                     /* right */
		(u8)((i + 1) % TR_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN));/* down */

		y += TR_LEVEL_CONTROL_ITEM_H + TR_LEVEL_CONTROL_ITEM_V_GAP;
	}

	//change result
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_LEVEL_CHANGE_RESULT,
					      TR_ACCEPT_LEVEL_CHANGE_RESULT_X,
					      y + 100,
					      TR_ACCEPT_LEVEL_CHANGE_RESULT_W,
					      TR_ACCEPT_LEVEL_CHANGE_RESULT_H,
					      p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_COMM_CONT_SH, RSI_COMM_CONT_HL, RSI_COMM_CONT_GRAY);
	text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
	ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);

	ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CONTROL_PIN), MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

	#ifndef WIN32
	ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
	#endif

	return SUCCESS;
}

static RET_CODE on_tr_accept_level_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	tr_accept_level_set_content(p_cont, (cas_card_info_t *)para2);
	ctrl_paint_ctrl(p_cont, TRUE);

	return SUCCESS;
}

static RET_CODE on_tr_accept_level_set(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_result = NULL;
	RET_CODE ret;

	p_result = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CHANGE_RESULT);
	ret = (RET_CODE)para1;

	if(ui_is_smart_card_insert() == FALSE)
	{
		text_set_content_by_strid(p_result, IDS_CA_CARD_INVALID);
	}
	else if(ret == SUCCESS)
	{
		text_set_content_by_strid(p_result, IDS_CA_SET_SUCCESS);
	}
	else
	{
		text_set_content_by_strid(p_result, IDS_CA_PASSWORD_ERROR);
	}

	ctrl_paint_ctrl(p_result,TRUE);
	return SUCCESS;
}

static RET_CODE on_tr_accept_level_pwdedit_maxtext(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont;
	control_t *p_active;

	p_active = ctrl_get_parent(p_ctrl);
	p_cont = ctrl_get_parent(p_active);

	ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);

	return SUCCESS;
}

static RET_CODE on_tr_accept_level_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#define	UI_MAX_PIN_LEN	6

	control_t *p_pin,*p_result, *p_level = NULL;
	u32 card_pin = 0;
	BOOL pin_code_is_full = FALSE;
	s8 i =0, pin = 0;

	p_pin = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_PIN);
	p_level = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_LEVEL);
	p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CHANGE_RESULT);
	
	card_pin = ui_comm_pwdedit_get_realValue(p_pin);
	pin_code_is_full = ui_comm_pwdedit_is_full(p_pin);
  
	for (i = UI_MAX_PIN_LEN -1; i >= 0; i--)
	{
		pin = card_pin%10;
		rate_info.pin[i] = (u8)pin;
		card_pin = card_pin/10;
	}
  
	rate_info.rate = (u8)ui_comm_select_get_focus(p_level) + 1;
	DEBUG(CAS,INFO,"[ca_accept_level]rate_info.rate= %d \n",rate_info.rate);
	if(pin_code_is_full)
	{
		ui_ca_do_cmd((u32)CAS_CMD_RATING_SET, (u32)&rate_info, 0);
	}
	else
	{
		//PIN invalid
		text_set_content_by_strid(p_result, IDS_CA_PIN_INVALID);
		ctrl_paint_ctrl(p_result, TRUE);
	}
	ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
	ctrl_paint_ctrl(p_pin, TRUE);
	
	return SUCCESS;
}

BEGIN_KEYMAP(tr_accept_level_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(tr_accept_level_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tr_accept_level_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_CA_CARD_INFO, on_tr_accept_level_update)
	ON_COMMAND(MSG_CA_RATING_SET, on_tr_accept_level_set)
	ON_COMMAND(MSG_SELECT, on_tr_accept_level_ca_frm_ok)
END_MSGPROC(tr_accept_level_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(tr_accept_level_pwdedit_keymap, ui_comm_edit_keymap)
END_KEYMAP(tr_accept_level_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(tr_accept_level_pwdedit_proc, ui_comm_edit_proc)
	ON_COMMAND(MSG_MAXTEXT, on_tr_accept_level_pwdedit_maxtext)
END_MSGPROC(tr_accept_level_pwdedit_proc, ui_comm_edit_proc)
