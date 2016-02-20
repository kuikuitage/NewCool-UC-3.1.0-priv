/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_public.h"
#include "ui_ca_notify.h"

enum ca_notify_cont_id
{
  IDC_NOTIFY_FRM = 1,
};

enum ca_notify_ctrl_id
{
  IDC_NOTIFY_CONT = 1, 
  IDC_NOTIFY_TYPE,
  IDC_NOTIFY_TITLE,
  IDC_NOTIFY_CONTENT,  

  IDC_NOTIFY_RECEIVE_TIME_TITLE,
  IDC_NOTIFY_RECEIVE_TIME_CONTENT,

  IDC_NOTIFY_NEW_TITLE,
  IDC_NOTIFY_NEW_CONTENT,

  IDC_NOTIFY_IMPORT_TITLE,
  IDC_NOTIFY_IMPORT_CONTENT,
};

u16 ca_notify_root_keymap(u16 key);
RET_CODE ca_notify_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

void set_ca_notify_content(control_t *p_cont, msg_info_t * p_cas_msg)
{
	u8 asc_str[64];
	u16 uni_str[1024] = {0};
	control_t *p_ctrl = NULL;

	DEBUG(CAS, INFO,"type = %d \n",p_cas_msg->type);
	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NOTIFY_TYPE);
	if(p_cas_msg->type == 0)/*CAS_MSG_MAIL*/
	{
		text_set_content_by_strid(p_ctrl, IDS_CA_EMAIL);	
	}
	else if(p_cas_msg->type == 1/*CAS_MSG_ANNOUNCE*/)
	{	
		text_set_content_by_strid(p_ctrl, IDS_TR_ANNOUNCE);	
	}
	else if(p_cas_msg->type == 2/*CAS_MSG_NOTIFICATION*/)
	{
		text_set_content_by_strid(p_ctrl, IDS_ALERT_MESSAGE);
	}

	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NOTIFY_TITLE);
	gb2312_to_unicode(p_cas_msg->title,80,uni_str,41);
	text_set_content_by_unistr(p_ctrl, (u16 *)uni_str);

	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NOTIFY_CONTENT);
	gb2312_to_unicode(p_cas_msg->data,1023,uni_str,1023);
	text_set_content_by_unistr(p_ctrl, uni_str);

	p_ctrl  = ctrl_get_child_by_id(p_cont, IDC_NOTIFY_RECEIVE_TIME_CONTENT);
	sprintf((char*)asc_str, "%04d.%02d.%02d-%02d:%02d:%02d", 
					p_cas_msg->create_time[0] * 100 + 
					p_cas_msg->create_time[1],
					p_cas_msg->create_time[2],
					p_cas_msg->create_time[3],
					p_cas_msg->create_time[4],
					p_cas_msg->create_time[5],
					p_cas_msg->create_time[6]);
	text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);

	p_ctrl= ctrl_get_child_by_id(p_cont, IDC_NOTIFY_NEW_CONTENT);
	text_set_content_by_strid(p_ctrl, IDS_YES);

	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NOTIFY_IMPORT_CONTENT);
	text_set_content_by_strid(p_ctrl, p_cas_msg->priority?IDS_YES:IDS_NO);

}

RET_CODE open_ca_notify(u32 para1, u32 para2)
{
	control_t *p_cont,*p_ctrl,*p_win;
  
	if(fw_find_root_by_id(ROOT_ID_CA_NOTIFY) != NULL)
	{
		fw_destroy_mainwin_by_id(ROOT_ID_CA_NOTIFY);
	}
  
	p_win = fw_create_mainwin(ROOT_ID_CA_NOTIFY,CA_NOTIFY_X,CA_NOTIFY_Y,CA_NOTIFY_W,CA_NOTIFY_H,
	                              ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);
    if(NULL == p_win)
		return ERR_FAILURE;
	ctrl_set_rstyle(p_win,RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
	ctrl_set_keymap(p_win, ca_notify_root_keymap);
	ctrl_set_proc(p_win, ca_notify_root_proc);
	
	p_cont = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_NOTIFY_CONT),
	                        CA_NOTIFY_CONT_X, CA_NOTIFY_CONT_Y,
	                        CA_NOTIFY_CONT_W, CA_NOTIFY_CONT_H, p_win, 0);
	//ctrl_set_rstyle(p_cont,RSI_MEDIO_LEFT_BG,RSI_MEDIO_LEFT_BG,RSI_MEDIO_LEFT_BG);
	ctrl_set_rstyle(p_cont,RSI_POPUP_BG,RSI_POPUP_BG,RSI_POPUP_BG);

	//message type         
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NOTIFY_TYPE),
	                        CA_NOTIFY_MES_TYPE_X, CA_NOTIFY_MES_TYPE_Y,
	                        CA_NOTIFY_MES_TYPE_W, CA_NOTIFY_MES_TYPE_H, p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	

	//message title     
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NOTIFY_TITLE),
	                        CA_NOTIFY_MES_TITLE_X, CA_NOTIFY_MES_TITLE_Y,
	                        CA_NOTIFY_MES_TITLE_W, CA_NOTIFY_MES_TITLE_H, p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

	//message content             
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NOTIFY_CONTENT),
	                        CA_NOTIFY_MES_CONTENT_X, CA_NOTIFY_MES_CONTENT_Y,
	                        CA_NOTIFY_MES_CONTENT_W, CA_NOTIFY_MES_CONTENT_H, p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);


	//message receive time title              
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NOTIFY_RECEIVE_TIME_TITLE),
	                      CA_NOTIFY_RECEIVE_TIME_TITLE_X, CA_NOTIFY_RECEIVE_TIME_TITLE_Y,
	                      CA_NOTIFY_RECEIVE_TIME_TITLE_W, CA_NOTIFY_RECEIVE_TIME_TITLE_H,p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_RECEIVE_TIME);

	//message receive time content         
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NOTIFY_RECEIVE_TIME_CONTENT),
	                      CA_NOTIFY_RECEIVE_TIME_CONTENT_X, CA_NOTIFY_RECEIVE_TIME_CONTENT_Y,
	                      CA_NOTIFY_RECEIVE_TIME_CONTENT_W, CA_NOTIFY_RECEIVE_TIME_CONTENT_H, p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

	//message new title    
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NOTIFY_NEW_TITLE),
	                      CA_NOTIFY_NEW_TITLE_X, CA_NOTIFY_NEW_TITLE_Y,
	                      CA_NOTIFY_NEW_TITLE_W, CA_NOTIFY_NEW_TITLE_H, p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_NEW);

	//message new content   
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NOTIFY_NEW_CONTENT),
	                      CA_NOTIFY_NEW_CONTENT_X, CA_NOTIFY_NEW_CONTENT_Y,
	                      CA_NOTIFY_NEW_CONTENT_W, CA_NOTIFY_NEW_CONTENT_H, p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_YES);

	//message import title    
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NOTIFY_IMPORT_TITLE),
	                      CA_NOTIFY_IMPORT_TITLE_X, CA_NOTIFY_IMPORT_TITLE_Y,
	                      CA_NOTIFY_IMPORT_TITLE_W, CA_NOTIFY_IMPORT_TITLE_H, p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_IMPORT);
  
	//message import content   
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NOTIFY_IMPORT_CONTENT),
	                      CA_NOTIFY_IMPORT_CONTENT_X, CA_NOTIFY_IMPORT_CONTENT_Y,
	                      CA_NOTIFY_IMPORT_CONTENT_W, CA_NOTIFY_IMPORT_CONTENT_H, p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_YES);

	set_ca_notify_content(p_cont, (msg_info_t *)para2);
  
	ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);
	return SUCCESS;
}

static RET_CODE on_ca_notify_exit(control_t *p_cont,u16 msg,u32 para1,u32 para2)
{
	fw_tmr_destroy(ROOT_ID_CA_NOTIFY, MSG_CANCEL);
	return ERR_NOFEATURE;
}

BEGIN_KEYMAP(ca_notify_root_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ca_notify_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_notify_root_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_EXIT, on_ca_notify_exit)
END_MSGPROC(ca_notify_root_proc, ui_comm_root_proc)

