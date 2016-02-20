/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_conditional_accept_level.h"
#include "ui_ca_public.h"

enum ca_level_control_id
{
	IDC_CA_LEVEL_CONTROL_PIN = 1,
	IDC_CA_LEVEL_CONTROL_LEVEL,
	IDC_CA_LEVEL_CONTROL_ENTER,
	IDC_CA_LEVEL_CHANGE_OK,
	IDC_CA_LEVEL_CHANGE_RESULT,
};

static cas_rating_set_t rate_info;

u16 tf_level_cont_keymap(u16 key);
RET_CODE tf_level_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE tf_level_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 tf_level_pwdedit_keymap(u16 key);
RET_CODE tf_level_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 tf_level_enter_keymap(u16 key);
RET_CODE tf_level_enter_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void tf_level_set_content(control_t *p_cont, u16 p_rating)
{
	control_t *p_level = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CONTROL_LEVEL);
	control_t *p_result = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CHANGE_RESULT);
	u16 focus = 0;
	DEBUG(CAS,INFO,"LEVER : %d\n",p_rating);

	if(p_rating == 0)
	{
		text_set_content_by_strid(p_result,IDS_CA_RECEIVED_DATA_FAIL);
	}
	else
	{
		ui_set_smart_card_rate(p_rating);
		focus = p_rating - 4;     //4years-18years
		ui_comm_select_set_focus(p_level, focus);
		DEBUG(CAS,INFO,"set LEVER : %d\n",focus);
	}
}

RET_CODE open_conditional_accept_level(u32 para1, u32 para2)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl = NULL;
	u8 i;
	u16 y;
	u16 stxt_level_control[TF_LEVEL_CONTROL_ITEM_CNT] =
	{
		IDS_CA_INPUT_PIN, IDS_CA_SELECT_LEVEL,
	};

	p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	if (p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, tf_level_cont_keymap);
	ctrl_set_proc(p_cont, tf_level_cont_proc);

	//level control
	y = TF_LEVEL_CONTROL_ITEM_Y;
	for (i = 0; i<TF_LEVEL_CONTROL_ITEM_CNT; i++)
	{
		switch(i)
		{
			case 0:
				p_ctrl = ui_comm_pwdedit_create(p_cont, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
				                       TF_LEVEL_CONTROL_ITEM_X, y,
				                       TF_LEVEL_CONTROL_ITEM_LW,
				                       TF_LEVEL_CONTROL_ITEM_RW);
				ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_level_control[i]);
				ui_comm_pwdedit_set_param(p_ctrl, 6);
				ui_comm_ctrl_set_keymap(p_ctrl, tf_level_pwdedit_keymap);
				ui_comm_ctrl_set_proc(p_ctrl, tf_level_pwdedit_proc);
				break;
			case 1:
				p_ctrl = ui_comm_select_create(p_cont, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
				                       TF_LEVEL_CONTROL_ITEM_X, y,
				                       TF_LEVEL_CONTROL_ITEM_LW,
				                       TF_LEVEL_CONTROL_ITEM_RW);
				ui_comm_select_set_static_txt(p_ctrl, stxt_level_control[i]);
				ui_comm_select_set_param(p_ctrl, TRUE,CBOX_WORKMODE_STATIC, TF_WATCH_LEVEL_TOTAL,
				               CBOX_ITEM_STRTYPE_STRID,NULL);

				ui_comm_select_set_content(p_ctrl, 0, IDS_CA_LEVEL4);
				ui_comm_select_set_content(p_ctrl, 1, IDS_CA_LEVEL5);
				ui_comm_select_set_content(p_ctrl, 2, IDS_CA_LEVEL6);
				ui_comm_select_set_content(p_ctrl, 3, IDS_CA_LEVEL7);
				ui_comm_select_set_content(p_ctrl, 4, IDS_CA_LEVEL8);
				ui_comm_select_set_content(p_ctrl, 5, IDS_CA_LEVEL9);
				ui_comm_select_set_content(p_ctrl, 6, IDS_CA_LEVEL10);
				ui_comm_select_set_content(p_ctrl, 7, IDS_CA_LEVEL11);
				ui_comm_select_set_content(p_ctrl, 8, IDS_CA_LEVEL12);
				ui_comm_select_set_content(p_ctrl, 9, IDS_CA_LEVEL13);
				ui_comm_select_set_content(p_ctrl, 10, IDS_CA_LEVEL14);
				ui_comm_select_set_content(p_ctrl, 11, IDS_CA_LEVEL15);
				ui_comm_select_set_content(p_ctrl, 12, IDS_CA_LEVEL16);
				ui_comm_select_set_content(p_ctrl, 13, IDS_CA_LEVEL17);
				ui_comm_select_set_content(p_ctrl, 14, IDS_CA_LEVEL18);
				if(ui_get_smart_card_rate() >= 4)
					ui_comm_select_set_focus(p_ctrl, (u16)ui_get_smart_card_rate() - 4);
				else
					ui_comm_select_set_focus(p_ctrl,0);
				break;
			case 2:
				p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_CA_LEVEL_CONTROL_ENTER,
					          TF_LEVEL_CONTROL_ITEM_X,y,
					          (TF_LEVEL_CONTROL_ITEM_LW + TF_LEVEL_CONTROL_ITEM_RW),
					          TF_LEVEL_CONTROL_ITEM_H
					          ,p_cont, 0);
				ctrl_set_keymap(p_ctrl, tf_level_enter_keymap);
				ctrl_set_proc(p_ctrl, tf_level_enter_proc);
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
				TF_LEVEL_CONTROL_ITEM_CNT) %
				TF_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN),           /* up */
				0,                                     /* right */
				(u8)((i + 1) % TF_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN));/* down */

		y += TF_LEVEL_CONTROL_ITEM_H + TF_LEVEL_CONTROL_ITEM_V_GAP;
	}

	//change result
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_LEVEL_CHANGE_RESULT,
			                      TF_CHANGE_RESULT_X,
			                      TF_CHANGE_RESULT_Y,
			                      TF_CHANGE_RESULT_W,
			                      TF_CHANGE_RESULT_H,
			                      p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_COMM_CONT_SH, RSI_COMM_CONT_HL, RSI_COMM_CONT_GRAY);
	text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
	ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);

	ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CONTROL_PIN), MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
	#ifndef WIN32
	ui_ca_do_cmd(CAS_CMD_RATING_GET, 0 ,0);
	#endif

	return SUCCESS;
}

static RET_CODE on_tf_level_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	tf_level_set_content(p_cont, (u8)para2);
	ctrl_paint_ctrl(p_cont, TRUE);

	return SUCCESS;
}

static RET_CODE on_tf_level_set(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_result = NULL;
	control_t *p_level = NULL;
	control_t *p_parent = NULL;
	u32 level;

	p_parent = ctrl_get_parent(p_cont);
	p_result = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CHANGE_RESULT);
	p_level = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CONTROL_LEVEL);

	DEBUG(CAS,INFO,"RETURN result : %d\n",para1);

	if(ui_is_smart_card_insert())
	{
		if(para1 == 0)
		{
			DEBUG(CAS,INFO,"on_tf_level_set ok\n");
			level = ui_comm_select_get_focus(p_level);
			ui_set_smart_card_rate(level + 4);
			text_set_content_by_strid(p_result, IDS_CA_SET_SUCCESS);
		}
		else
		{
			text_set_content_by_strid(p_result, IDS_CA_PIN_INVALID2);
		}
	}
	else
	{
		text_set_content_by_strid(p_result, IDS_CA_CARD_NOT_INSERT);
	}

	ctrl_paint_ctrl(p_result,TRUE);
	return SUCCESS;
}


static RET_CODE on_tf_level_pwdedit_maxtext(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	control_t *p_cont;
	control_t *p_active;
	p_active = ctrl_get_parent(p_ctrl);
	p_cont = ctrl_get_parent(p_active);

	ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);

	return SUCCESS;
}

static RET_CODE on_tf_level_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#define	UI_MAX_PIN_LEN	6
	control_t *p_pin, *p_level = NULL, *p_result,*p_parent;
	u32 card_pin = 0;
	u8 focus = 0;
	BOOL pin_code_is_full;
	s8 i =0, pin = 0;

	p_parent = ctrl_get_parent(p_ctrl);
	p_result = ctrl_get_child_by_id(p_parent, IDC_CA_LEVEL_CHANGE_RESULT);

	p_pin = ctrl_get_child_by_id(p_parent, IDC_CA_LEVEL_CONTROL_PIN);
	p_level = ctrl_get_child_by_id(p_parent, IDC_CA_LEVEL_CONTROL_LEVEL);
	focus = ui_comm_select_get_focus(p_level);
	DEBUG(CAS,INFO,"level= %d \n",focus);

	if(FALSE == ui_is_smart_card_insert())
	{
		text_set_content_by_strid(p_result, IDS_CA_CARD_NOT_INSERT);
		ctrl_paint_ctrl(p_result, TRUE);
		return SUCCESS;
	}

	card_pin = ui_comm_pwdedit_get_realValue(p_pin);
	pin_code_is_full = ui_comm_pwdedit_is_full(p_pin);

	for (i = UI_MAX_PIN_LEN -1; i >= 0; i--)
	{
		pin = card_pin % 10;
		rate_info.pin[i] = pin;
		card_pin = card_pin / 10;
	}
  
	if(pin_code_is_full == FALSE)
	{
		DEBUG(CAS,INFO,"pin_code_is_full == FALSE\n");
		text_set_content_by_strid(p_result, IDS_DIGIT_NOT_ENOUGH);
		ctrl_paint_ctrl(p_result, TRUE);
	}
	else if(focus > (18 - 4))
	{
		text_set_content_by_strid(p_result, IDS_CA_PARAME_SET_ERROR);
		ctrl_set_attr(p_result, OBJ_ATTR_ACTIVE);
		ctrl_paint_ctrl(p_result, TRUE);
	}
	else
	{
		rate_info.rate = (u8)(focus + 4);     //4-18years
		DEBUG(CAS,INFO,"[ca_accept_level]rate_info.rate= %d \n",rate_info.rate);
		DEBUG(CAS,INFO,"PIN发出去rate_info的值是%s\n",rate_info.pin);
		ui_ca_do_cmd((u32)CAS_CMD_RATING_SET, (u32)&rate_info, 0);
	}
	
	ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
	ctrl_paint_ctrl(p_pin, TRUE);
	return SUCCESS;
}

BEGIN_KEYMAP(tf_level_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_BACK, MSG_CHANGED)
END_KEYMAP(tf_level_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tf_level_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_CA_RATING_INFO, on_tf_level_update)
	ON_COMMAND(MSG_CA_RATING_SET, on_tf_level_set)
END_MSGPROC(tf_level_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(tf_level_pwdedit_keymap, ui_comm_edit_keymap)
	ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(tf_level_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(tf_level_pwdedit_proc, ui_comm_edit_proc)
	ON_COMMAND(MSG_MAXTEXT, on_tf_level_pwdedit_maxtext)
END_MSGPROC(tf_level_pwdedit_proc, ui_comm_edit_proc)

BEGIN_KEYMAP(tf_level_enter_keymap, NULL)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(tf_level_enter_keymap, NULL)

BEGIN_MSGPROC(tf_level_enter_proc, text_class_proc)
	ON_COMMAND(MSG_SELECT, on_tf_level_ca_frm_ok)
END_MSGPROC(tf_level_enter_proc, text_class_proc)


