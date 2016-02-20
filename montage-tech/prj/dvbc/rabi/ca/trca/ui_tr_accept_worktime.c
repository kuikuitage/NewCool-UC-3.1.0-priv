/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_tr_accept_worktime.h"
#include "ui_ca_public.h"
 
enum ca_work_duration_id
{
  IDC_CA_WORK_DURATION_PIN = 1,
  IDC_CA_WORK_DURATION_START_TIME,
  IDC_CA_WORK_DURATION_END_TIME, 
  IDC_CA_WORK_DURATION_OK,
  IDC_CA_WORK_DURATION_CHANGE_RESULT,
};

static cas_card_work_time_t card_word_time; 
 
u16 tr_accept_worktime_cont_keymap(u16 key);
RET_CODE tr_accept_worktime_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
RET_CODE tr_accept_worktime_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE tr_accept_worktime_timedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static void tr_accept_worktime_set_content(control_t *p_cont, cas_card_work_time_t *p_card_info)
{
	control_t *p_start = ctrl_get_child_by_id(p_cont, IDC_CA_WORK_DURATION_START_TIME);
	control_t *p_end = ctrl_get_child_by_id(p_cont, IDC_CA_WORK_DURATION_END_TIME);
	utc_time_t time = {0};

	if(p_card_info == NULL)
	{
		ui_comm_timedit_set_time(p_start, &time);
		ui_comm_timedit_set_time(p_end, &time);
	}
	else
	{
		DEBUG(CAS,INFO,"@@@@@ start hour:%d min:%d sec:%d\n", p_card_info->start_hour,p_card_info->start_minute,p_card_info->start_second);
		DEBUG(CAS,INFO,"@@@@@ end hour:%d min:%d sec:%d\n", p_card_info->end_hour,p_card_info->end_minute,p_card_info->end_second);   

		time.hour = p_card_info->start_hour;
		time.minute = p_card_info->start_minute;
		time.second = p_card_info->start_second;
		ui_comm_timedit_set_time(p_start, &time);

		time.hour = p_card_info->end_hour;
		time.minute = p_card_info->end_minute;
		time.second = p_card_info->end_second;
		ui_comm_timedit_set_time(p_end, &time);
	}
}

RET_CODE open_tr_accept_worktime(u32 para1, u32 para2)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl = NULL;
	u8 i;
	u16 y;
	u16 stxt_work[TR_ACCEPT_WORKTIME_WORK_ITEM_CNT] =
	{ 
		IDS_CA_INPUT_PIN, IDS_CA_START_TIME, IDS_CA_END_TIME
	};

	p_cont = ui_comm_right_root_create(ROOT_ID_TR_WORKTIME, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	if (p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, tr_accept_worktime_cont_keymap);
	ctrl_set_proc(p_cont, tr_accept_worktime_cont_proc);

	//work duration
	y = TR_ACCEPT_WORKTIME_WORK_ITEM_Y;
	for (i = 0; i<TR_ACCEPT_WORKTIME_WORK_ITEM_CNT; i++)
	{
		switch(i)
		{
			case 0:
				p_ctrl = ui_comm_pwdedit_create(p_cont, (u8)(IDC_CA_WORK_DURATION_PIN + i),
				                           TR_ACCEPT_WORKTIME_WORK_ITEM_X, y,
				                           TR_ACCEPT_WORKTIME_WORK_ITEM_LW,
				                           TR_ACCEPT_WORKTIME_WORK_ITEM_RW);
				ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_work[i]);
				ui_comm_pwdedit_set_param(p_ctrl, 6);
				ui_comm_ctrl_set_proc(p_ctrl, tr_accept_worktime_pwdedit_proc);
				break;

			case 1:
			case 2:
				p_ctrl = ui_comm_timedit_create(p_cont, (u8)(IDC_CA_WORK_DURATION_PIN + i),
				                           TR_ACCEPT_WORKTIME_WORK_ITEM_X, y,
				                           TR_ACCEPT_WORKTIME_WORK_ITEM_LW,
				                           TR_ACCEPT_WORKTIME_WORK_ITEM_RW);
				ui_comm_timedit_set_static_txt(p_ctrl, stxt_work[i]);
				ui_comm_ctrl_set_proc(p_ctrl, tr_accept_worktime_timedit_proc);
				ui_comm_timedit_set_param(p_ctrl, 0, TBOX_ITEM_HOUR, TBOX_HOUR| TBOX_MIN|TBOX_SECOND, 
				                        TBOX_SEPARATOR_TYPE_UNICODE, 18);
				ui_comm_timedit_set_separator_by_ascchar(p_ctrl, TBOX_ITEM_HOUR, ':');
				ui_comm_timedit_set_separator_by_ascchar(p_ctrl, TBOX_ITEM_MIN, ':');
				break;
			case 3:
				p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_WORK_DURATION_PIN + i),
				              TR_ACCEPT_WORKTIME_WORK_ITEM_X, 
				              y,
				              (TR_ACCEPT_WORKTIME_WORK_ITEM_LW + TR_ACCEPT_WORKTIME_WORK_ITEM_RW), 
				              TR_ACCEPT_WORKTIME_WORK_ITEM_H,
				              p_cont, 0);
				ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
				text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
				text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
				text_set_offset(p_ctrl, COMM_CTRL_OX, 0);
				text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
				text_set_content_by_strid(p_ctrl,IDS_START);
				break;
			default:
				p_ctrl = NULL;
				break;
		}
		if(NULL == p_ctrl)
		{
			break;
		}
		ctrl_set_related_id(p_ctrl,
		        0,                                     /* left */
		        (u8)((i - 1 +
		              TR_ACCEPT_WORKTIME_WORK_ITEM_CNT) %
		             TR_ACCEPT_WORKTIME_WORK_ITEM_CNT + IDC_CA_WORK_DURATION_PIN),           /* up */
		        0,                                     /* right */
		        (u8)((i + 1) % TR_ACCEPT_WORKTIME_WORK_ITEM_CNT + IDC_CA_WORK_DURATION_PIN));/* down */

		y += TR_ACCEPT_WORKTIME_WORK_ITEM_H + TR_ACCEPT_WORKTIME_WORK_ITEM_V_GAP;
	}

	//change result
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_WORK_DURATION_CHANGE_RESULT,
	                          TR_ACCEPT_WORKTIME_CHANGE_RESULT_X, 
	                          y+100,
	                          TR_ACCEPT_WORKTIME_CHANGE_RESULT_W, 
	                          TR_ACCEPT_WORKTIME_CHANGE_RESULT_H,
	                          p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);

	ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CA_WORK_DURATION_PIN), MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

	ui_ca_do_cmd(CAS_CMD_WORK_TIME_GET, 0 ,0);

	return SUCCESS;
}

static RET_CODE on_tr_accept_worktime_pwdedit_maxtext(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	control_t *p_cont;
	control_t *p_active;

	p_active = ctrl_get_parent(p_ctrl);
	p_cont = ctrl_get_parent(p_ctrl);

	ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);
	return SUCCESS;
}


static RET_CODE on_tr_accept_worktime_timedit_unselect(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	return ERR_NOFEATURE;
}

static RET_CODE on_tr_accept_worktime_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#define	UI_MAX_PIN_LEN	6

	control_t *p_pin, *p_start, *p_end, *p_result;
	utc_time_t p_star_time, p_end_time ;
	u32 card_pin = 0;
	u8 pin = 0;
	s8 i = 0;
	BOOL pin_code_is_full = FALSE;
	
	p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_CHANGE_RESULT);

	p_pin = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_PIN);
	p_start = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_START_TIME);
	p_end = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_END_TIME);
	
	card_pin = ui_comm_pwdedit_get_realValue(p_pin);
	pin_code_is_full = ui_comm_pwdedit_is_full(p_pin);

	if(FALSE == ui_is_smart_card_insert())
	{
		text_set_content_by_strid(p_result, IDS_CA_CARD_NOT_INSERT);
		ctrl_paint_ctrl(p_result, TRUE);
		return SUCCESS;
	}

	if(pin_code_is_full)
	{
		for (i = UI_MAX_PIN_LEN - 1; i >= 0; i--)
		{
			pin = card_pin%10;
			card_word_time.pin[i] = pin;
			card_pin = card_pin/10;
		}
		
		ui_comm_timedit_get_time(p_start,&p_star_time);
		ui_comm_timedit_get_time(p_end,&p_end_time);
		card_word_time.start_hour = p_star_time.hour;
		card_word_time.start_minute= p_star_time.minute;
		card_word_time.start_second= p_star_time.second;
		card_word_time.end_hour= p_end_time.hour;
		card_word_time.end_minute= p_end_time.minute;
		card_word_time.end_second= p_end_time.second;
		
		ui_ca_do_cmd((u32)CAS_CMD_WORK_TIME_SET, (u32)&card_word_time ,0);
	}
	else
	{
		text_set_content_by_strid(p_result, IDS_CA_PIN_INVALID);
		ctrl_paint_ctrl(p_result, TRUE);
	}
	ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
	ctrl_paint_ctrl(p_ctrl, TRUE);

	return SUCCESS;
}

static RET_CODE on_tr_set_worktime_right(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_frm;
	RET_CODE ret;
	
	p_frm = ctrl_get_child_by_id(p_cont, IDC_CA_WORK_DURATION_CHANGE_RESULT);
	ret = (RET_CODE)para1;
	
	if(ui_is_smart_card_insert() == FALSE)
	{
		text_set_content_by_strid(p_frm, IDS_CA_CARD_INVALID);
	}
	else if(ret == SUCCESS)
	{
		text_set_content_by_strid(p_frm, IDS_CA_SET_SUCCESS);
	}
	else
	{
		text_set_content_by_strid(p_frm, IDS_CA_PASSWORD_ERROR);
	}
	
	ctrl_paint_ctrl(p_frm, TRUE);
	return SUCCESS;
}

static RET_CODE on_tr_accept_worktime_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	tr_accept_worktime_set_content(p_cont, (cas_card_work_time_t *)para2);
	ctrl_paint_ctrl(p_cont, TRUE);

	return SUCCESS;
}


BEGIN_KEYMAP(tr_accept_worktime_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(tr_accept_worktime_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tr_accept_worktime_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_CA_WORK_TIME_INFO, on_tr_accept_worktime_update)
	ON_COMMAND(MSG_CA_WORK_TIME_SET, on_tr_set_worktime_right)
	ON_COMMAND(MSG_SELECT, on_tr_accept_worktime_ca_frm_ok)  
END_MSGPROC(tr_accept_worktime_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(tr_accept_worktime_pwdedit_proc, ui_comm_edit_proc)
	ON_COMMAND(MSG_MAXTEXT, on_tr_accept_worktime_pwdedit_maxtext)
END_MSGPROC(tr_accept_worktime_pwdedit_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(tr_accept_worktime_timedit_proc, ui_comm_time_proc)  
	ON_COMMAND(MSG_UNSELECT, on_tr_accept_worktime_timedit_unselect) 
END_MSGPROC(tr_accept_worktime_timedit_proc, ui_comm_time_proc)

