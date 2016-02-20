/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_ipp.h"
#include "ui_ca_public.h"

enum ca_ipp_cont_id
{
  IDC_IPP_FRM = 1,
  IDC_IPP_HEAD = 2,
};

enum ca_ipp_ctrl_id
{
  IDC_IPP_TYPE = 1,
  IDC_IPP_PRICE,
  IDC_IPP_DISTANCE,
  IDC_IPP_INFO,
  IDC_IPP_INPUT_PIN,
  IDC_IPP_RESULT_CONTENT,
  IDC_IPP_OTHER_HELP,
};

enum local_ca_ipp_msg_t
{
  MSG_BUY_NO_TAPING = MSG_USER_BEGIN + 320,
  MSG_BUY_TAPING,
};
 ipp_buy_info_t g_ipp_buy_info = {0};
  
struct ipp_pwdlg_int_data
{
    u8  type;               // PWDLG_T_
    u8  total_bit;          // total bit
    u8  input_bit;          // input bit
    u32 total_value;        // total value
    u32 input_value;        // input value
};
static struct ipp_pwdlg_int_data g_ipp_pwdlg_idata;
static u8 ipp_price_type = 0;

u16 ca_ipp_root_keymap(u16 key);
RET_CODE ca_ipp_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE ca_ipp_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static RET_CODE ipp_info_set_content(control_t *p_cont, ipp_buy_info_t *p_ipp_buy_info)
{
	control_t *p_frm, *p_type, *p_price, *p_distance, *p_info;
	u8 asc_str[64];
	u16 uni_str[32];
	u16 uni_str2[64];

	if(p_ipp_buy_info->state_flag == 0xff)
		return SUCCESS;

	p_frm = ctrl_get_child_by_id(p_cont, IDC_IPP_FRM);
	p_type = ctrl_get_child_by_id(p_frm, IDC_IPP_TYPE);
	p_price = ctrl_get_child_by_id(p_frm, IDC_IPP_PRICE);
	p_distance = ctrl_get_child_by_id(p_frm, IDC_IPP_DISTANCE);
	p_info = ctrl_get_child_by_id(p_frm, IDC_IPP_INFO);

	if(p_ipp_buy_info == NULL)
	{
		ui_comm_static_set_content_by_ascstr(p_type,(u8 *) " ");
		ui_comm_static_set_content_by_ascstr(p_price,(u8 *) " ");
		ui_comm_static_set_content_by_ascstr(p_distance,(u8 *) " ");
		ui_comm_static_set_content_by_ascstr(p_info,(u8 *) " ");
	}
	else
	{
		sprintf((char*)asc_str, "%s", p_ipp_buy_info->message_type?"IPPT":"IPPV");
		ui_comm_static_set_content_by_ascstr(p_type,(u8 *)asc_str);

		//TRCAS: IPP type, 0 IPPV, 1 IPPT
		if(p_ipp_buy_info->message_type)
		{
			gui_get_string(IDS_CA_POINT, uni_str, 32);
			sprintf((char*)asc_str, "%d", (int)p_ipp_buy_info->ipp_charge);
			str_asc2uni(asc_str, uni_str2);
			uni_strcat(uni_str2, uni_str, 32);
			sprintf((char*)asc_str, "/");
			str_asc2uni(asc_str, uni_str);
			uni_strcat(uni_str2, uni_str, 32);
			sprintf((char*)asc_str, "%d", (int)(p_ipp_buy_info->ipp_unit_time/60));
			str_asc2uni(asc_str, uni_str);
			uni_strcat(uni_str2, uni_str, 32);
			gui_get_string(IDS_CA_MINUTE, uni_str, 32);
			uni_strcat(uni_str2, uni_str, 32);
		}
		else
		{
			gui_get_string(IDS_CA_POINT, uni_str, 32);
			sprintf((char*)asc_str, "%d", (int)p_ipp_buy_info->ipp_charge);
			str_asc2uni(asc_str, uni_str2);
			uni_strcat(uni_str2, uni_str, 32);
		}
		ui_comm_static_set_content_by_unistr(p_price,uni_str2);

		sprintf((char*)asc_str,"%04d.%02d.%02d-%02d:%02d:%02d--%04d.%02d.%02d-%02d:%02d:%02d",
						p_ipp_buy_info->start_time[0] * 100 +
						p_ipp_buy_info->start_time[1],
						p_ipp_buy_info->start_time[2],
						p_ipp_buy_info->start_time[3],
						p_ipp_buy_info->start_time[4],
						p_ipp_buy_info->start_time[5],
						p_ipp_buy_info->start_time[6],
						p_ipp_buy_info->expired_time[0] * 100 +
						p_ipp_buy_info->expired_time[1],
						p_ipp_buy_info->expired_time[2],
						p_ipp_buy_info->expired_time[3],
						p_ipp_buy_info->expired_time[4],
						p_ipp_buy_info->expired_time[5],
						p_ipp_buy_info->expired_time[6]);
		ui_comm_static_set_content_by_ascstr(p_distance,(u8 *)asc_str);

		sprintf((char*)asc_str, "%s", p_ipp_buy_info->note);
		ui_comm_static_set_content_by_ascstr(p_info,(u8 *)asc_str);
	}

	if(p_ipp_buy_info == NULL)
	{
		memset(&g_ipp_buy_info, 0, sizeof(ipp_buy_info_t));
	}
	else
	{
		memcpy(&g_ipp_buy_info, p_ipp_buy_info, sizeof(ipp_buy_info_t));
	}

	OS_PRINTF("[ipp_info_set_content]ipp_buy_info pin %d\n",g_ipp_buy_info.pin);
	OS_PRINTF("[ipp_info_set_content]ipp_buy_info channel id %d\n",g_ipp_buy_info.channel_id);
	OS_PRINTF("[ipp_info_set_content]ipp_buy_info ipp_charge %d\n",g_ipp_buy_info.ipp_charge);
	OS_PRINTF("[ipp_info_set_content]ipp_buy_info info %d\n",g_ipp_buy_info.note);
	return SUCCESS;
}

RET_CODE open_ca_ipp(u32 para1, u32 para2)
{
	control_t *p_cont, *p_ctrl, *p_frm, *p_pwd = NULL, *p_head;
	u16 string_ca_ipp[] = {IDS_IPP_TYPE,IDS_IPP_PRICE,IDS_IPP_DISTANCE,IDS_IPP_INFOR2,IDS_CA_INPUT_PIN_2};
	u16 i,y;
	u16 prog_id = sys_status_get_curn_group_curn_prog_id();
	ipp_buy_info_t *buy_info = (ipp_buy_info_t *)para2;
	DEBUG(CAS,INFO,"prog_id=%x channel_id=%x\n",prog_id,buy_info->channel_id);
	if(buy_info->channel_id != prog_id)
	{
		DEBUG(CAS,INFO,"\n");
		return SUCCESS;
	}

	p_cont = fw_create_mainwin(ROOT_ID_CA_IPP,
			                 CA_IPP_X, CA_IPP_Y,
			                 CA_IPP_W, CA_IPP_H, 
			                 ROOT_ID_INVALID, 0,OBJ_ATTR_ACTIVE,0);
	ctrl_set_keymap(p_cont, ca_ipp_root_keymap);
	ctrl_set_proc(p_cont, ca_ipp_root_proc);
	ctrl_set_rstyle(p_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);

	//head
	p_head = ctrl_create_ctrl(CTRL_TEXT, IDC_IPP_HEAD,
	                		CA_IPP_HEAD_X, CA_IPP_HEAD_Y,
	                		CA_IPP_HEAD_W, CA_IPP_HEAD_H, p_cont, 0);  
	ctrl_set_rstyle(p_head, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_head, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_offset(p_head,100,0);
	text_set_align_type(p_head, STL_RIGHT| STL_BOTTOM);
	text_set_content_type(p_head, TEXT_STRTYPE_UNICODE);

	//frm
	p_frm = ctrl_create_ctrl(CTRL_CONT, IDC_IPP_FRM,
	               			 CA_IPP_FRM_X, CA_IPP_FRM_Y,
	               			 CA_IPP_FRM_W, CA_IPP_FRM_H, p_cont, 0);  
	ctrl_set_rstyle(p_frm, RSI_PBACK, RSI_PBACK, RSI_PBACK);



	y = CA_IPP_INFO_ITEM_Y;
	for(i=0; i < CA_IPP_INFO_ITEM_CNT; i++)
	{
		switch(i)
		{
			case 0:
			case 1:
			case 2:
			case 3:
				p_ctrl = ui_comm_static_create(p_frm, (u8)(IDC_IPP_TYPE + i),
											  CA_IPP_INFO_ITEM_X, y,
											  CA_IPP_INFO_ITEM_LW,
											  CA_IPP_INFO_ITEM_RW);
				ui_comm_static_set_static_txt(p_ctrl, string_ca_ipp[i]);
				ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
				ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
				break;
			case 4:
				p_pwd = ui_comm_pwdedit_create(p_frm, (u8)(IDC_IPP_TYPE + i),
										  CA_IPP_INFO_ITEM_X, y,
										  CA_IPP_INFO_ITEM_LW,
										  CA_IPP_INFO_ITEM_RW);
				ui_comm_pwdedit_set_static_txt(p_pwd, string_ca_ipp[i]);
				ui_comm_pwdedit_set_param(p_pwd, 6);
				ui_comm_ctrl_set_proc(p_pwd, ca_ipp_pwdedit_proc);
				break;
			default:
				break;
		}
		DEBUG(CAS,INFO,"%d\n",i);
		y += COMM_CTRL_H;
	}

	//ipp result content
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_IPP_RESULT_CONTENT),
	                 CA_IPP_INFO_RESULT_X, CA_IPP_INFO_RESULT_Y,
	                 CA_IPP_INFO_RESULT_W, CA_IPP_INFO_RESULT_H,p_frm, 0);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_BOTTOM);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
	ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
	DEBUG(CAS,INFO,"\n");

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_IPP_OTHER_HELP,
					CA_IPP_INFO_HELP_X,CA_IPP_INFO_HELP_Y,
					CA_IPP_INFO_HELP_W,CA_IPP_INFO_HELP_H, p_frm, 0);
	text_set_align_type(p_ctrl, STL_CENTER|STL_VCENTER);
	text_set_font_style(p_ctrl, FSI_WHITE,FSI_WHITE,FSI_WHITE);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_IPP_HELP);
	DEBUG(CAS,INFO,"\n");
	
	ipp_info_set_content(p_cont, (ipp_buy_info_t *)para2);

	{
		dvbs_prog_node_t pg;
		u16 uni_buf[32] = {0};
		
		if (db_dvbs_get_pg_by_id(g_ipp_buy_info.channel_id, &pg) == DB_DVBS_OK)
		{
			ui_dbase_get_full_prog_name(&pg, uni_buf, 31);
			text_set_content_by_unistr(p_head, uni_buf);
		}
	}
	DEBUG(CAS,INFO,"\n");

	ctrl_default_proc(p_pwd, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);

	return SUCCESS;
}

static RET_CODE on_ca_ipp_exit(control_t *p_cont,u16 msg,u32 para1,u32 para2)
{
	g_ipp_pwdlg_idata.input_bit = 0;
	g_ipp_pwdlg_idata.input_value = 0;
	g_ipp_buy_info.state_flag = 0;

	return ERR_NOFEATURE;
}

static RET_CODE on_ca_ipp_destroy(control_t *p_cont,u16 msg,u32 para1,u32 para2)
{
	g_ipp_pwdlg_idata.input_bit = 0;
	g_ipp_pwdlg_idata.input_value = 0;
	
	return ERR_NOFEATURE;
}

static RET_CODE on_ca_ipp_exit_updown(control_t *p_cont,u16 msg,u32 para1,u32 para2)
{
	on_ca_ipp_exit(p_cont, msg, para1, para2);

	manage_close_menu(ROOT_ID_CA_IPP, 0, 0);
	if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
	{
		OS_PRINTF("on_ca_ipp_exit_updown\n");
		manage_notify_root(ROOT_ID_PROG_BAR, msg, para1, para2);
	}

	return SUCCESS;
}

static RET_CODE on_ca_ipp_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	ipp_buy_info_t *info = (ipp_buy_info_t *)para2;
	u16 prog_id = sys_status_get_curn_group_curn_prog_id();
	DEBUG(CAS,INFO,"prog_id=%x channel_id=%x\n",prog_id,info->channel_id);
	if(info->channel_id != prog_id)
	{
		ctrl_process_msg(fw_find_root_by_id(ROOT_ID_CA_IPP), MSG_CA_EVT_NOTIFY, 0, MSG_EXIT);
		DEBUG(CAS,INFO,"\n");
		return SUCCESS;
	}

	ipp_info_set_content(p_cont, (ipp_buy_info_t *)para2);
	ctrl_paint_ctrl(p_cont, TRUE);
	return SUCCESS;
}

static RET_CODE on_ca_ipp_buy_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	s16 i =0;
	u8 pin = 0;
	u32 card_pin = 0;
	
	g_ipp_buy_info.pin_len = 6;
	card_pin = ui_comm_pwdedit_get_realValue(ctrl_get_parent(p_ctrl));

	for (i = g_ipp_buy_info.pin_len - 1; i >= 0; i--)
	{
		pin = card_pin%10;
		g_ipp_buy_info.pin[i] = pin;
		card_pin = card_pin/10;
	}

	DEBUG(CAS,INFO,"ipp_buy_info pin %s\n",g_ipp_buy_info.pin);
	DEBUG(CAS,INFO,"ipp_buy_info channel id %d\n",g_ipp_buy_info.channel_id);
	DEBUG(CAS,INFO,"ipp_buy_info ipp_charge %d\n",g_ipp_buy_info.ipp_charge);
	DEBUG(CAS,INFO,"ipp_buy_info price %d\n",g_ipp_buy_info.ipp_price[0].price);
	DEBUG(CAS,INFO,"ipp_buy_info price type %d\n",g_ipp_buy_info.ipp_price[0].price_type);

	if(ipp_price_type == 0)
		g_ipp_buy_info.purchase_num = 0;
	else
		g_ipp_buy_info.purchase_num = 1;

	ui_ca_do_cmd(CAS_CMD_IPP_BUY_SET, (u32)&g_ipp_buy_info, 0);
	return SUCCESS;
}

static RET_CODE on_ca_ipp_buy_result(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_frm=NULL, *p_result=NULL,*p_pin=NULL;

	p_frm = ctrl_get_child_by_id(p_cont, IDC_IPP_FRM);
	p_result = ctrl_get_child_by_id(p_frm, IDC_IPP_RESULT_CONTENT);
	p_pin = ctrl_get_child_by_id(p_frm, IDC_IPP_INPUT_PIN);

	OS_PRINTF("on_ca_ipp_buy_result %d",para1);

	if(ipp_price_type == 0)
		ipp_price_type = 1;
	else
		ipp_price_type = 0;

	switch(para1)
	{
		case SUCCESS:
			manage_close_menu( ROOT_ID_CA_IPP,  0, 0);
			return SUCCESS; 

		case CAS_E_PARAM_ERR: /* parameter error */
			text_set_content_by_ascstr(p_result, (u8*)"param error");
			ctrl_set_sts(p_result, OBJ_STS_SHOW);
			ctrl_paint_ctrl(p_result,  TRUE);
			break;

		case CAS_E_PIN_INVALID:  /* invalid pin */
			text_set_content_by_ascstr(p_result, (u8*)"pin error");
			ctrl_set_sts(p_result, OBJ_STS_SHOW);
			ctrl_paint_ctrl(p_result,  TRUE);
			break;

		case CAS_E_NVRAM_ERROR: /* Nvram error from smart card */
			text_set_content_by_ascstr(p_result, (u8*)"nvram error");
			ctrl_set_sts(p_result, OBJ_STS_SHOW);
			ctrl_paint_ctrl(p_result,  TRUE);
			break;

		case CAS_E_MEM_ALLOC_FAIL: /* No enough money */
			text_set_content_by_ascstr(p_result, (u8*)"no enough money");
			ctrl_set_sts(p_result, OBJ_STS_SHOW);
			ctrl_paint_ctrl(p_result,  TRUE);
			break;

		case CAS_E_CARD_NO_ROOM: /* No space save product */
			text_set_content_by_ascstr(p_result, (u8*)"space full");
			ctrl_set_sts(p_result, OBJ_STS_SHOW);
			ctrl_paint_ctrl(p_result,  TRUE);
			break;

		default: /* CAS_E_IOCMD_NOT_OK */
			text_set_content_by_ascstr(p_result, (u8*)"unknown error");
			ctrl_set_sts(p_result, OBJ_STS_SHOW);
			ctrl_paint_ctrl(p_result,  TRUE);
			break;

	}
	ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
	ctrl_paint_ctrl(p_cont, TRUE);
	return SUCCESS; 
}

BEGIN_KEYMAP(ca_ipp_root_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_GREEN, MSG_BUY_NO_TAPING)
	ON_EVENT(V_KEY_YELLOW, MSG_BUY_TAPING)
END_KEYMAP(ca_ipp_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_ipp_root_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_EXIT, on_ca_ipp_exit)
	ON_COMMAND(MSG_EXIT_ALL, on_ca_ipp_exit)
	ON_COMMAND(MSG_CA_IPP_BUY_INFO, on_ca_ipp_info_update)
	ON_COMMAND(MSG_CA_IPP_BUY_OK, on_ca_ipp_buy_result)
	ON_COMMAND(MSG_FOCUS_UP, on_ca_ipp_exit_updown)
	ON_COMMAND(MSG_FOCUS_DOWN, on_ca_ipp_exit_updown)
	ON_COMMAND(MSG_DESTROY, on_ca_ipp_destroy)
END_MSGPROC(ca_ipp_root_proc, ui_comm_root_proc)

BEGIN_MSGPROC(ca_ipp_pwdedit_proc, ui_comm_edit_proc)
	ON_COMMAND(MSG_MAXTEXT, on_ca_ipp_buy_ok)
END_MSGPROC(ca_ipp_pwdedit_proc, ui_comm_edit_proc)


