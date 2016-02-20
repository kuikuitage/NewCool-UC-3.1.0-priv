/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_pin.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_PIN,
};

enum ca_pin_modify_id
{
  IDC_CA_PIN_MODIFY_PSW_OLD = 1,
  IDC_CA_PIN_MODIFY_PSW_NEW,
  IDC_CA_PIN_MODIFY_PSW_CONFIRM,
  IDC_CA_PIN_MODIFY_PSW_OK,
  IDC_CA_PIN_MODIFY_PSW_CANCEL,
  IDC_CA_PIN_CHANGE_RESULT,
};

static cas_pin_modify_t pin_modify;
u16 conditional_accept_pin_cont_keymap(u16 key);
RET_CODE conditional_accept_pin_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_pin_ca_frm_keymap(u16 key);
RET_CODE conditional_accept_pin_ca_frm_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_pin_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_pin_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);



RET_CODE open_cas_accept_pin(u32 para1, u32 para2)
{
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT      5
  #define SN_CONDITIONAL_ACCEPT_PIN_HELP_RSC_CNT	  15
  //CA frm
  #define SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_X          (0)
  #define SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_Y          (0)
  #define SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_W         (RIGHT_ROOT_W)
  #define SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_H          (RIGHT_ROOT_H)

  //pin modify
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X         (COMM_ITEM_OX_IN_ROOT/2)
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_Y         (COMM_ITEM_OY_IN_ROOT/2)
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW       (COMM_ITEM_LW)
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW       (RIGHT_ROOT_W - COMM_ITEM_LW - COMM_ITEM_OX_IN_ROOT)
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H         45
  #define SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP     20

  #define SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_X           (250)
  #define SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_W          (160)
  #define SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_H           (COMM_CTRL_H)

  //change pin result
  #define SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_X         (COMM_ITEM_OX_IN_ROOT/2)
  #define SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_Y         (360)
  #define SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_W        (300)
  #define SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_H         (COMM_CTRL_H)


  control_t *p_ctrl=NULL;
  control_t *p_cont;
  control_t *p_ca_frm;
  u8 i;
  u16 y;
  u16 stxt_pin[SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT] =
  { 
    IDS_CA_INPUT_OLD_PIN, IDS_CA_INPUT_NEW_PIN, IDS_CA_CONFIRM_NEW_PIN,
  };

  p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_PIN, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_pin_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_pin_cont_proc);
  
  //CA frm
  p_ca_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_FRM_PIN,
                              SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_X, SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_Y,
                              SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_W, SN_CONDITIONAL_ACCEPT_PIN_CA_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_ca_frm, conditional_accept_pin_ca_frm_keymap);
  ctrl_set_proc(p_ca_frm, conditional_accept_pin_ca_frm_proc);

  //pin modify
  y = SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_Y;
  for (i = 0; i<SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
      case 1:
      case 2:
        p_ctrl = ui_comm_pwdedit_create(p_ca_frm, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
                                             SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X, y+(SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H + SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP)*i,
                                             SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW,
                                             SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW);
        ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_pin[i]);
        ui_comm_pwdedit_set_param(p_ctrl, 6);
        //ui_comm_pwdedit_set_mask(p_ctrl, '-', '*');
        ui_comm_ctrl_set_keymap(p_ctrl,conditional_accept_pin_pwdedit_keymap);
        ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_pin_pwdedit_proc);
        break;

       case 3:
        p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
                              SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_X,
                              y+(SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H + SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP)*(3),
                              SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_W,
                              SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_H,
                              p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
        text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, IDS_OK);
        break;
      case 4: 
	  	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
                              SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_X+SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_W*2,
                              y+(SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H + SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP)*(3),
                              SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_W,
                              SN_CONDITIONAL_ACCEPT_PIN_OK_ITEM_H,
                              p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
        text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, IDS_CANCEL);
		break;
      default:  
        break;
      }
    if(i>=0 && i<3 )
    {
      ctrl_set_related_id(p_ctrl,
                          0,                                     /* left */
                          (u8)((i - 1 +
                                SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT) %
                               SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD),           /* up */
                          0,                                     /* right */
                          (u8)((i + 1) % SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD));/* down */
        }
    else
    {
      ctrl_set_related_id(p_ctrl,
                          (u8) ((i - 2) % 2 + IDC_CA_PIN_MODIFY_PSW_OK),                                     /* left */
                          (u8)((i - 1 +
                                SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT) %
                               SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD),           /* up */
                          (u8) ((i + 2) % 2 + IDC_CA_PIN_MODIFY_PSW_OK),                                     /* right */
                          (u8)((i + 1) % SN_CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD));/* down */        
        }
  }

  //change result
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_PIN_CHANGE_RESULT,
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_X, 
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_Y,
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_W, 
                              SN_CONDITIONAL_ACCEPT_CHANGE_RESULT_H,
                              p_ca_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  ctrl_set_attr(p_ctrl,OBJ_STS_HIDE);
  
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(ctrl_get_child_by_id(p_ca_frm, IDC_CA_PIN_MODIFY_PSW_OLD), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_pwdedit_maxtext(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_active;
  u8 ctrl_id;

  p_active = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_active);

  ctrl_id = ctrl_get_ctrl_id(p_active);

  ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);
  ctrl_paint_ctrl(ui_comm_ctrl_get_ctrl(p_cont),TRUE);
  return SUCCESS;
}

static dlg_ret_t  pin_show_dlg(u32 content, u8 style, u32 dlg_tmout)
{
    comm_dlg_data_t dlg_data;

    dlg_data.parent_root = ROOT_ID_CONDITIONAL_ACCEPT_PIN;
    dlg_data.x = (RIGHT_ROOT_W - PWDLG_W)/2 + 300;
    dlg_data.y = SYS_DLG_FOR_CHK_Y;
    dlg_data.w = PWDLG_W;
    dlg_data.h = PWDLG_H;
    dlg_data.content = content;
    dlg_data.style = style;
    dlg_data.dlg_tmout = dlg_tmout;

    return ui_comm_dlg_open(&dlg_data);
}

static RET_CODE on_conditional_accept_pin_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
       //u32 psw_len_confirm_all;
       control_t *p_result, *p_ctrl_temp, *p_child;
       u32 pin_new_psw,pin_old_psw;
       u32 pin_new_psw_confirm;
       u8 i = 0;
       u8 ctrl_id;
       u8 enable_psw_pinold, enable_psw_pinnew,enable_psw_pinnew_confirm;
       //dlg_ret_t ret=FALSE;
 
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
 
       DEBUG(DBG,INFO,"*old pin: %#x\n",pin_old_psw);
       DEBUG(DBG,INFO,"*new pin: %#x\n",pin_new_psw);
       DEBUG(DBG,INFO,"*cfm pin: %#x\n",pin_new_psw_confirm);

	p_child = ctrl_get_active_ctrl(p_ctrl);
	ctrl_id = ctrl_get_ctrl_id(p_child);
	if(ctrl_id == IDC_CA_PIN_MODIFY_PSW_CANCEL)
      {
		manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, 0, 0);
	}
	else if(ctrl_id == IDC_CA_PIN_MODIFY_PSW_OK)
	{
            {

                if(TRUE != enable_psw_pinold || TRUE != enable_psw_pinnew || TRUE != enable_psw_pinnew_confirm)
                {
                    pin_show_dlg(IDS_CA_PIN_INVALID, DLG_FOR_SHOW | DLG_STR_MODE_STATIC,1000);
                }
                if(pin_new_psw == pin_new_psw_confirm)
                {
                    DEBUG(DBG,INFO,"***********ui new pin: %#x\n",pin_new_psw);
                    pin_modify.new_pin[0]=(u8)(pin_new_psw/100000);
                    pin_modify.new_pin[1]=(u8)((pin_new_psw/10000)%10);
                    pin_modify.new_pin[2]=(u8)(((pin_new_psw/1000)%100)%10);
                    pin_modify.new_pin[3]=(u8)((((pin_new_psw/100)%1000)%100)%10);
                    pin_modify.new_pin[4]=(u8)(((((pin_new_psw/10)%10000)%1000)%100)%10);
                    pin_modify.new_pin[5]=(u8)(((((pin_new_psw%100000)%10000)%1000)%100)%10);

                    DEBUG(DBG,INFO,"***********ui new pin = %d,%d,%d \n",pin_modify.new_pin[0],pin_modify.new_pin[1],pin_modify.new_pin[2]);
                    pin_modify.old_pin[0]=(u8)(pin_old_psw/100000);
                    pin_modify.old_pin[1]=(u8)((pin_old_psw/10000)%10);
                    pin_modify.old_pin[2]=(u8)(((pin_old_psw/1000)%100)%10);
                    pin_modify.old_pin[3]=(u8)((((pin_old_psw/100)%1000)%100)%10);
                    pin_modify.old_pin[4]=(u8)(((((pin_old_psw/10)%10000)%1000)%100)%10);
                    pin_modify.old_pin[5]=(u8)(((((pin_old_psw%100000)%10000)%1000)%100)%10);
                    DEBUG(DBG,INFO,"***********ui old pin = %d,%d,%d \n",pin_modify.old_pin[0],pin_modify.old_pin[1],pin_modify.old_pin[2]);
#ifndef WIN32
                    ui_ca_do_cmd((u32)CAS_CMD_PIN_SET, (u32)&pin_modify ,0);
#endif
                }
                else
                {
                    pin_show_dlg(IDS_CA_CARD_PIN_INVALID, DLG_FOR_SHOW | DLG_STR_MODE_STATIC,1000);
                }
                for(i=0; i<3; i++)
                {
                    p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD+i));
                    ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_ctrl_temp), MSG_EMPTY, 0, 0);
                    ctrl_paint_ctrl(ctrl_get_root(p_ctrl_temp), TRUE);
                }

            }
       }
  return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_ca_modify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_result;
  RET_CODE ret;
  cas_pin_modify_t *ca_pin_info = NULL;
  ca_pin_info = (cas_pin_modify_t *)para2;
  ret = (RET_CODE)para1;
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT);
  
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


BEGIN_KEYMAP(conditional_accept_pin_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_pin_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_pin_cont_proc, ui_comm_root_proc)

END_MSGPROC(conditional_accept_pin_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_pin_ca_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_pin_ca_frm_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_pin_ca_frm_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, on_conditional_accept_pin_ca_frm_ok)  
  ON_COMMAND(MSG_CA_PIN_SET_INFO, on_conditional_accept_pin_ca_modify)
END_MSGPROC(conditional_accept_pin_ca_frm_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_pin_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_pin_pwdedit_maxtext)
END_MSGPROC(conditional_accept_pin_pwdedit_proc, ui_comm_edit_proc)



