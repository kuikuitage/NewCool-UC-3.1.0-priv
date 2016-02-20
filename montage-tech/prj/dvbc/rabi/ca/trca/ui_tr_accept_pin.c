/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_tr_accept_pin.h"
#include "ui_ca_public.h"

enum ca_pin_modify_id
{
	IDC_CA_PIN_MODIFY_PSW_OLD = 1,
	IDC_CA_PIN_MODIFY_PSW_NEW,
	IDC_CA_PIN_MODIFY_PSW_CONFIRM,
	IDC_CA_PIN_MODIFY_PSW_OK,
	IDC_CA_PIN_CHANGE_RESULT,
};

static cas_pin_modify_t pin_modify;

u16 tr_accept_pin_cont_keymap(u16 key);
RET_CODE tr_accept_pin_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 tr_accept_pin_pwdedit_keymap(u16 key);
RET_CODE tr_accept_pin_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_tr_accept_pin(u32 para1, u32 para2)
{
	control_t *p_cont = NULL, *p_ctrl = NULL;
	u8 i;
	u16 y;
	u16 stxt_pin[TR_ACCEPT_PIN_ITEM_CNT] =
	{ 
		IDS_CA_INPUT_OLD_PIN, IDS_CA_INPUT_NEW_PIN,IDS_CA_CONFIRM_NEW_PIN
	};
  
	p_cont = ui_comm_right_root_create(ROOT_ID_TR_PIN, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	if (p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, tr_accept_pin_cont_keymap);
	ctrl_set_proc(p_cont, tr_accept_pin_cont_proc);

	//pin modify
	y = TR_ACCEPT_PIN_ITEM_Y;
	for (i = 0; i<TR_ACCEPT_PIN_ITEM_CNT; i++)
	{
		switch(i)
		{
			case 0:
			case 1:
			case 2:
				p_ctrl = ui_comm_pwdedit_create(p_cont, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
													TR_ACCEPT_PIN_ITEM_X, y,
													TR_ACCEPT_PIN_ITEM_LW,
													TR_ACCEPT_PIN_ITEM_RW);
				ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_pin[i]);
				ui_comm_pwdedit_set_param(p_ctrl, 6);
				ui_comm_ctrl_set_keymap(p_ctrl,tr_accept_pin_pwdedit_keymap);
				ui_comm_ctrl_set_proc(p_ctrl, tr_accept_pin_pwdedit_proc);
				break;
			case 3:
				p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
													TR_ACCEPT_PIN_ITEM_X, y,
													(TR_ACCEPT_PIN_ITEM_LW + TR_ACCEPT_PIN_ITEM_RW),
													TR_ACCEPT_PIN_ITEM_H,p_cont, 0);
				ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
				text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
				text_set_offset(p_ctrl, COMM_CTRL_OX, 0);
				text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
				text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
				text_set_content_by_strid(p_ctrl, IDS_START);
				break;
			default:  
				break;
		}
		ctrl_set_related_id(p_ctrl,
		      0,                                     /* left */
		      (u8)((i - 1 +
		            TR_ACCEPT_PIN_ITEM_CNT) %
		           TR_ACCEPT_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD),           /* up */
		      0,                                     /* right */
		      (u8)((i + 1) % TR_ACCEPT_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD));/* down */

		y += TR_ACCEPT_PIN_ITEM_H + TR_ACCEPT_PIN_ITEM_V_GAP;
	}

	//change result
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_PIN_CHANGE_RESULT,
				                  TR_ACCEPT_CHANGE_RESULT_X,
				                  y + 100,
				                  TR_ACCEPT_CHANGE_RESULT_W,
				                  TR_ACCEPT_CHANGE_RESULT_H,p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
	ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);

	ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CA_PIN_MODIFY_PSW_OLD), MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

	return SUCCESS;
}

static RET_CODE on_tr_accept_pin_pwdedit_maxtext(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	control_t *p_cont;
	control_t *p_active;

	p_active = ctrl_get_parent(p_ctrl);
	p_cont = ctrl_get_parent(p_active);

	ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);
	return SUCCESS;
}

static RET_CODE on_tr_accept_pin_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#define	UI_MAX_PIN_LEN	6
	BOOL enable_psw_pinold, enable_psw_pinnew ,enable_psw_pinnew_confirm;
	control_t *p_result, *p_ctrl_temp;
	u32 pin_new_psw,pin_old_psw;
	u32 pin_new_psw_confirm;
	s8 i =0;
	u8 pin = 0;
  
	//change result
	p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT);
	p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_OLD);
	pin_old_psw = ui_comm_pwdedit_get_realValue(p_ctrl_temp);
	enable_psw_pinold = ui_comm_pwdedit_is_full(p_ctrl_temp);

	p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_NEW);
	pin_new_psw = ui_comm_pwdedit_get_realValue(p_ctrl_temp);
	enable_psw_pinnew = ui_comm_pwdedit_is_full(p_ctrl_temp);

	p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_CONFIRM);
	pin_new_psw_confirm = ui_comm_pwdedit_get_realValue(p_ctrl_temp);
	enable_psw_pinnew_confirm = ui_comm_pwdedit_is_full(p_ctrl_temp);

	if(!enable_psw_pinold || !enable_psw_pinnew ||!enable_psw_pinnew_confirm)
	{
		text_set_content_by_strid(p_result, IDS_CA_PIN_INVALID);
		ctrl_paint_ctrl(p_result, TRUE);
	}
	else if(pin_new_psw == pin_new_psw_confirm)
	{
		for(i = UI_MAX_PIN_LEN -1; i >= 0; i--)
		{
			pin = pin_old_psw%10;
			pin_modify.old_pin[i] = pin;
			pin_old_psw = pin_old_psw/10;
		}

		for(i = UI_MAX_PIN_LEN - 1; i >= 0; i--)
		{
			pin = pin_new_psw%10;
			pin_modify.new_pin[i] = pin;
			pin_new_psw = pin_new_psw/10;
		}
		ui_ca_do_cmd((u32)CAS_CMD_PIN_SET, (u32)&pin_modify, 0);
	}
	else
	{
		text_set_content_by_strid(p_result, IDS_CA_PIN_INVALID);
		ctrl_paint_ctrl(p_result, TRUE);
	}
	
	for(i=0; i<3; i++)
	{
		p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD+i));
		ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_ctrl_temp), MSG_EMPTY, 0, 0);
		ctrl_paint_ctrl(ui_comm_ctrl_get_ctrl(p_ctrl_temp), TRUE);
	}
  
	return SUCCESS;
}

static RET_CODE on_tr_accept_pin_ca_modify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_result;
	RET_CODE ret;

	ret = (RET_CODE)para1;
	p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT);

	if(ui_is_smart_card_insert() == FALSE)
	{
		text_set_content_by_strid(p_result, IDS_CA_CARD_INVALID);
	}
	else if(ret == SUCCESS)
	{
		text_set_content_by_strid(p_result, IDS_PASSWORD_CHANGE_FINISH);
	}
	else
	{
		text_set_content_by_strid(p_result, IDS_CA_PASSWORD_ERROR);
	}
	ctrl_paint_ctrl(p_result, TRUE);

	return SUCCESS;
}


BEGIN_KEYMAP(tr_accept_pin_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(tr_accept_pin_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tr_accept_pin_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_SELECT, on_tr_accept_pin_ca_frm_ok)  
	ON_COMMAND(MSG_CA_PIN_SET_INFO, on_tr_accept_pin_ca_modify)
END_MSGPROC(tr_accept_pin_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(tr_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)
END_KEYMAP(tr_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(tr_accept_pin_pwdedit_proc, ui_comm_edit_proc)
	ON_COMMAND(MSG_MAXTEXT, on_tr_accept_pin_pwdedit_maxtext)
END_MSGPROC(tr_accept_pin_pwdedit_proc, ui_comm_edit_proc)



