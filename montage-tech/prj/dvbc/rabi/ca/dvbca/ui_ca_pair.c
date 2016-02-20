/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_ca_pair.h"
#include "sys_dbg.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_PAIR,
};

enum ca_pair_control_id
{
  IDC_CA_PAIR_CONTROL_PIN = 1,
  IDC_CA_PAIR_CHANGE_OK,
  IDC_CA_PAIR_CHANGE_CANCEL,
  IDC_CA_PAIR_CHANGE_RESULT,
};


static u32 pairPin;

u16 conditional_accept_pair_cont_keymap(u16 key);
RET_CODE conditional_accept_pair_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
u16 conditional_accept_pair_ca_frm_keymap(u16 key);
RET_CODE conditional_accept_pair_ca_frm_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
u16 conditional_accept_pair_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_pair_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_stb_card_pair_info(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl = NULL;
  control_t *p_ca_frm;
  u8 i;
  u16 y;

  // create container
  p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_pair_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_pair_cont_proc);

  //CA frm
  p_ca_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_FRM_PAIR,
                              0, 0,
                              RIGHT_ROOT_W, RIGHT_ROOT_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_ca_frm, conditional_accept_pair_ca_frm_keymap);
  ctrl_set_proc(p_ca_frm, conditional_accept_pair_ca_frm_proc);

  //work duration
  y = SN_CONDITIONAL_ACCEPT_PAIR_ITEM_Y;
  for (i = 0; i<SN_CONDITIONAL_ACCEPT_PAIR_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl = ui_comm_pwdedit_create(p_ca_frm, (u8)(IDC_CA_PAIR_CONTROL_PIN + i),
                                           SN_CONDITIONAL_ACCEPT_PAIR_ITEM_X, y,
                                           SN_CONDITIONAL_ACCEPT_PAIR_ITEM_LW,
                                           SN_CONDITIONAL_ACCEPT_PAIR_ITEM_RW);
		
        ui_comm_pwdedit_set_static_txt(p_ctrl, IDS_CA_INPUT_PIN_2);
        ui_comm_pwdedit_set_param(p_ctrl, 6);
        ui_comm_ctrl_set_keymap(p_ctrl,conditional_accept_pair_pwdedit_keymap);
        ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_pair_pwdedit_proc);
        break;

     case 1:
        p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_PAIR_CONTROL_PIN + i),
                            SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_X, 
                            SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_Y,
                            SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_W, 
                            SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_H,
                            p_ca_frm, 0);
		
        ctrl_set_rstyle(p_ctrl,RSI_PBACK, RSI_SELECT_F, RSI_PBACK);//RSI_MEDIO_LEFT_BG
        text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, IDS_OK);  
        break;
	 case 2:
		p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_PAIR_CONTROL_PIN + i),
                            SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_X+SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_W*2, 
                            SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_Y,
                            SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_W, 
                            SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_H,
                            p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl,RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
        text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, IDS_CANCEL);
		break;
      default:
        p_ctrl = NULL;
        break;
    }
    if(NULL == p_ctrl)
    {
        break;
    }
   if(i == 0){
    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              SN_CONDITIONAL_ACCEPT_PAIR_ITEM_CNT) %
                             SN_CONDITIONAL_ACCEPT_PAIR_ITEM_CNT + IDC_CA_PAIR_CONTROL_PIN),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % SN_CONDITIONAL_ACCEPT_PAIR_ITEM_CNT + IDC_CA_PAIR_CONTROL_PIN));/* down */
	
   	}
	else
	{
		ctrl_set_related_id(p_ctrl,
                       (u8) ((i - 2) % 2 + IDC_CA_PAIR_CHANGE_OK),                                     /* left */
                        (u8)((i - 1 +
                              SN_CONDITIONAL_ACCEPT_PAIR_ITEM_CNT) %
                             SN_CONDITIONAL_ACCEPT_PAIR_ITEM_CNT + IDC_CA_PAIR_CONTROL_PIN),           /* up */
                        (u8) ((i + 2) % 2 + IDC_CA_PAIR_CHANGE_OK),                                     /* right */
                        (u8)((i + 1) % SN_CONDITIONAL_ACCEPT_PAIR_ITEM_CNT + IDC_CA_PAIR_CONTROL_PIN));/* down */ 
	}
 
  }

  //change result
  
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_PAIR_CHANGE_RESULT,
                              CONDITIONAL_ACCEPT_PAIR_RESULT_X, 
                              CONDITIONAL_ACCEPT_PAIR_RESULT_Y,
                              CONDITIONAL_ACCEPT_PAIR_RESULT_W, 
                              CONDITIONAL_ACCEPT_PAIR_RESULT_H,
                              p_ca_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
  text_set_align_type(p_ctrl, STL_CENTER | STL_LEFT);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  ctrl_set_attr(p_ctrl,OBJ_STS_HIDE);

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  
  ctrl_default_proc(ctrl_get_child_by_id(p_ca_frm, IDC_CA_PAIR_CONTROL_PIN), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  return SUCCESS;
}

static dlg_ret_t  pair_show_dlg(u32 content, u8 style, u32 dlg_tmout)
{
    comm_dlg_data_t dlg_data;

    dlg_data.parent_root = ROOT_ID_CONDITIONAL_ACCEPT_PAIR;
    dlg_data.x = (RIGHT_ROOT_W - PWDLG_W)/2 + 300;
    dlg_data.y = SYS_DLG_FOR_CHK_Y;
    dlg_data.w = PWDLG_W;
    dlg_data.h = PWDLG_H;
    dlg_data.content = content;
    dlg_data.style = style;
    dlg_data.dlg_tmout = dlg_tmout;

    return ui_comm_dlg_open(&dlg_data);
}

static RET_CODE on_conditional_accept_pair_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_pin,*p_result,*p_child;
	u32 card_pin = 0;
	BOOL card_pin_is_full = FALSE;
	//dlg_ret_t ret=FALSE;
	u8 ctrl_id;
	DEBUG(MAIN,INFO,"*********************\n");
	p_pin = ctrl_get_child_by_id(p_ctrl, IDC_CA_PAIR_CONTROL_PIN);
	p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PAIR_CHANGE_RESULT);
	card_pin = ui_comm_pwdedit_get_realValue(p_pin);
	card_pin_is_full = ui_comm_pwdedit_is_full(p_pin);

	p_child = ctrl_get_active_ctrl(p_ctrl);
	ctrl_id = ctrl_get_ctrl_id(p_child);
	if(ctrl_id == IDC_CA_PAIR_CHANGE_CANCEL){
		manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, 0, 0);
	}
	else if(ctrl_id == IDC_CA_PAIR_CHANGE_OK)
	{
	#if 0
            ret = pair_show_dlg(IDS_MSG_ASK_FOR_CLEAR_SAT_POSITION, DLG_FOR_ASK | DLG_STR_MODE_STATIC,0);
	
		if(DLG_RET_YES==ret)
       #endif
		{
			DEBUG(MAIN,INFO,"*********************\n");
			if(card_pin_is_full)
			{
#ifndef WIN32
                          pairPin = card_pin;
				DEBUG(MAIN,INFO,"card_pin = %d *********************\n",card_pin);
				ui_ca_do_cmd((u32)CAS_CMD_PAIRE_SET,  (u32)&pairPin ,0);
#endif
			}
			else
			{//PIN invalid
				pair_show_dlg(IDS_CA_CARD_PIN_INVALID, DLG_FOR_SHOW | DLG_STR_MODE_STATIC,1000);
			}
			ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
			ctrl_paint_ctrl(p_pin, TRUE);

		}
       #if 0
		else
		{
			manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, 0, 0);
		}
        #endif
	}
	return SUCCESS;

}

static RET_CODE on_conditional_accept_pair_set(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_result = NULL,*p_frm = NULL;
  cas_rating_set_t *ca_pin_info = NULL;
  ca_pin_info = (cas_rating_set_t *)para2;
  p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_PAIR);
  p_result = ctrl_get_child_by_id(p_frm, IDC_CA_PAIR_CHANGE_RESULT);
  DEBUG(MAIN,INFO,"***********para1=%d  CAS_SUCESS[%d]**************\n",para1,CAS_SUCESS);
  
   if(para1 == CAS_SUCESS)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_SUCESS[%d] \n",para1, CAS_SUCESS);
        text_set_content_by_strid(p_result, IDS_CA_SET_SUCCESS);
  }
  else if(para1 == CAS_ERR_INVALID_CARD)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_INVALID_CARD[%d] \n",para1, CAS_ERR_INVALID_CARD);
        text_set_content_by_strid(p_result, IDS_CA_CARD_INVALID);
  }
  else if(para1 == CAS_ERR_NOCARD)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_NOCARD[%d] \n",para1, CAS_ERR_NOCARD);
        text_set_content_by_strid(p_result, IDS_CA_CARD_NOT_INSERT);
  }
  else if(para1 == CAS_ERR_INVALID_ECMPID)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_INVALID_ECMPID[%d] \n",para1, CAS_ERR_INVALID_ECMPID);
        text_set_content_by_strid(p_result, IDS_CA_INVALID_ECM_PID);
  }
  else if(para1 == CAS_ERR_INVALID_EMMPID) 
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_INVALID_EMMPID[%d] \n",para1, CAS_ERR_INVALID_EMMPID);
        text_set_content_by_strid(p_result, IDS_CA_INVALID_EMM_PID);
  }
  else if(para1 == CAS_ERR_NOENTITLE)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_NOENTITLE[%d] \n",para1, CAS_ERR_NOENTITLE);
        text_set_content_by_strid(p_result, IDS_CA_NOT_ENTITLE);
  }
  else if(para1 == CAS_ERR_STB_PAIR_FAILED)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_STB_PAIR_FAILED[%d] \n",para1, CAS_ERR_STB_PAIR_FAILED);
        text_set_content_by_strid(p_result, IDS_CA_STB_PAIR_FAILED);
  }
  else if(para1 == CAS_FAILED)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_FAILED[%d] \n",para1, CAS_FAILED);
        text_set_content_by_strid(p_result, IDS_CAS_E_UNKNOWN_ERROR);
  }
  else if(para1 == CAS_ERR_INIT_ERROR)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_INIT_ERROR[%d] \n",para1, CAS_ERR_INIT_ERROR);
        text_set_content_by_strid(p_result, IDS_CA_CARD_INIT_FAIL);
  }
  else if(para1 == CAS_ERR_CARD_VER_ERROR)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_CARD_VER_ERROR[%d] \n",para1, CAS_ERR_CARD_VER_ERROR);
        text_set_content_by_strid(p_result, IDS_CA_CARD_ERR_VERSION);
  }
  else if(para1 == CAS_ERR_PIN_ERROR)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_PIN_ERROR[%d] \n",para1, CAS_ERR_PIN_ERROR);
        text_set_content_by_strid(p_result, IDS_CA_PASSWORD_PIN_ERROR);
  }
  else
  {
        OS_PRINTF("[debug] para1[%d] \n",para1);
        text_set_content_by_strid(p_result, IDS_CA_PASSWORD_PIN_ERROR);
  }
  ctrl_set_sts(p_result, OBJ_STS_SHOW);
  ctrl_paint_ctrl(p_result, TRUE);
  return SUCCESS;
}
static RET_CODE on_conditional_accept_pair_pwdedit_maxtext(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

	control_t *p_cont;
	control_t *p_active;
	u8 ctrl_id;

	p_active = ctrl_get_parent(p_ctrl);
	p_cont = ctrl_get_parent(p_active);

	ctrl_id = ctrl_get_ctrl_id(p_active);

	ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);

	return SUCCESS;
}

BEGIN_KEYMAP(conditional_accept_pair_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(conditional_accept_pair_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_pair_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_CA_PAIRE_SET, on_conditional_accept_pair_set)
END_MSGPROC(conditional_accept_pair_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_pair_ca_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)  
END_KEYMAP(conditional_accept_pair_ca_frm_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_pair_ca_frm_proc, ui_comm_root_proc) 
  ON_COMMAND(MSG_SELECT, on_conditional_accept_pair_ca_frm_ok)  
END_MSGPROC(conditional_accept_pair_ca_frm_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_pair_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_pair_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_pair_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_pair_pwdedit_maxtext)
END_MSGPROC(conditional_accept_pair_pwdedit_proc, ui_comm_edit_proc)

