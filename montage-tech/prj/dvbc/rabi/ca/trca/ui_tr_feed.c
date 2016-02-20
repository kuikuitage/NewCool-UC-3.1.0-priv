/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ui_tr_feed.h"
#include "ui_ca_public.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CA_CARD_FEED_FRM1,
  IDC_CA_CARD_FEED_FRM2,
};

enum ca_feed_frm1_info_id
{
  IDC_CA_CARD_PROMPT_1 = 1,
  IDC_CA_CARD_PROMPT_2,
};

enum ca_feed_frm2_info_id
{
  IDC_CA_CARD_FEED_TIP = 1,
  IDC_CA_CARD_FEED_OK,
  IDC_CA_CARD_FEED_CANCEL,
  IDC_CA_CARD_FEED_HELP_ICON,
  IDC_CA_CARD_FEED_HELP_TEXT,
};

enum ca_feed_status
{
  FEED_STATUS_NULL = 0,
  FEED_STATUS_PROMPT,
  FEED_STATUS_INSERT,
  FEED_STATUS_READY,
  FEED_STATUS_MOTHER_SUCCESS,
  FEED_STATUS_SON_SUCCESS,
  FEED_STATUS_PARAM_ERR,
  FEED_STATUS_FEED_DATA_ERR,
  FEED_STATUS_CARD_NOTSUPPORT,
  FEED_STATUS_PARTNER_FAILED,
  FEED_STATUS_UNKNOW_ERR,
}ca_feed_status_t;

enum ctrl_id
{
  IDC_TITLE_MBOX = 1,
  IDC_CAS_MOTHER_LIST,
  IDC_CAS_MOTHER_LIST_SBAR,
};

static u8 g_feed_status = FEED_STATUS_NULL;
static chlid_card_status_info g_feed_info;

u16 tr_feed_cont_keymap(u16 key);
RET_CODE tr_feed_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 tr_feed_btn_keymap(u16 key);
RET_CODE tr_feed_btn_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);


static RET_CODE on_tr_feed_state_update(control_t *p_cont, u16 msg, u32 para1, u32 para2, BOOL is_force)
{
  control_t *p_ctrl_tip = NULL;
  control_t *p_ctrl_frm2 = NULL;
  
  p_ctrl_frm2 = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_FEED_FRM2);
  p_ctrl_tip = ctrl_get_child_by_id(p_ctrl_frm2, IDC_CA_CARD_FEED_TIP);
  
  switch(g_feed_status)
  {
    case FEED_STATUS_MOTHER_SUCCESS:
      text_set_content_by_strid(p_ctrl_tip, IDS_CA_FEED_INSERT_CHILD_CARD);
      break;

    case FEED_STATUS_SON_SUCCESS:
      text_set_content_by_strid(p_ctrl_tip, IDS_CA_FEED_SUCCESS);
      break;

    case FEED_STATUS_PARAM_ERR:
      text_set_content_by_strid(p_ctrl_tip, IDS_FEED_PARAM_ERROR);
      break;

    case FEED_STATUS_FEED_DATA_ERR:
      text_set_content_by_strid(p_ctrl_tip, IDS_CA_FEED_DATA_ERROR);
      break;

    case FEED_STATUS_CARD_NOTSUPPORT:
      text_set_content_by_strid(p_ctrl_tip, IDS_NOT_SUPPORT_FEED);
      break;

    case FEED_STATUS_PARTNER_FAILED:
      text_set_content_by_strid(p_ctrl_tip, IDS_FEED_PARTNER_FAILED);
      break;

    case FEED_STATUS_UNKNOW_ERR:
      text_set_content_by_strid(p_ctrl_tip, IDS_CAS_E_UNKNOWN_ERROR);
      break;

    default:
      break;
  }

  ctrl_paint_ctrl(p_ctrl_tip, is_force);

  return ERR_NOFEATURE;
}

RET_CODE open_tr_feed(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl, *p_frm1,*p_frm2;

  p_cont = ui_comm_right_root_create(ROOT_ID_TR_FEED, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, tr_feed_cont_keymap);
  ctrl_set_proc(p_cont, tr_feed_cont_proc);


   //CA FEED FRAME1
  p_frm1 = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_CA_CARD_FEED_FRM1),
                            TR_CA_FEED_CONT_PROMPT_X,
                            TR_CA_FEED_CONT_PROMPT_Y,
                            TR_CA_FEED_CONT_PROMPT_W,
                            TR_CA_FEED_CONT_PROMPT_H, p_cont, 0);
  ctrl_set_rstyle(p_frm1, RSI_PBACK,RSI_PBACK, RSI_PBACK);

    //PROMPT
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_CARD_PROMPT_1),
                            TR_CA_FEED_PROMPT_X,
                            TR_CA_FEED_PROMPT_Y,
                            TR_CA_FEED_PROMPT_W,
                            TR_CA_FEED_PROMPT_H, p_frm1, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK,RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_PROMPT);
  
  //SUPPORT
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_CARD_PROMPT_2),
                            TR_CA_FEED_SUPPORT_X,
                            TR_CA_FEED_SUPPORT_Y,
                            TR_CA_FEED_SUPPORT_W,
                            TR_CA_FEED_SUPPORT_H, p_frm1, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK,RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);

  //CA FEED FRAME2
  p_frm2 = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_CA_CARD_FEED_FRM2),
                            TR_CA_FEED_CONT_OPERATE_X,
                            TR_CA_FEED_CONT_OPERATE_Y,
                            TR_CA_FEED_CONT_OPERATE_W,
                            TR_CA_FEED_CONT_OPERATE_H, p_cont, 0);
  ctrl_set_rstyle(p_frm2, RSI_PBACK,RSI_PBACK, RSI_PBACK);
  ctrl_set_sts(p_frm2,OBJ_STS_HIDE);
  
  //feed tip
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_TIP),
                           TR_CA_FEED_TIP_X,
                           TR_CA_FEED_TIP_Y,
                           TR_CA_FEED_TIP_W,
                           TR_CA_FEED_TIP_H, p_frm2, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK,RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CA_INSERT_PARENT_CARD);

  //OK
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_OK),
                            TR_CA_FEED_OK_X,
                            TR_CA_FEED_OK_Y,
                            TR_CA_FEED_OK_W,
                            TR_CA_FEED_OK_H, p_frm2, 0);
  ctrl_set_keymap(p_ctrl, tr_feed_btn_keymap);
  ctrl_set_proc(p_ctrl, tr_feed_btn_proc);
  ctrl_set_rstyle(p_ctrl, RSI_ITEM_8_BG, RSI_OPT_BTN_H, RSI_ITEM_8_BG);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_SH, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_OK);
  ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);

  //Cancel
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_CARD_FEED_CANCEL),
                            TR_CA_FEED_CANCEL_X,
                            TR_CA_FEED_CANCEL_Y,
                            TR_CA_FEED_CANCEL_W,
                            TR_CA_FEED_CANCEL_H, p_frm2, 0);
  ctrl_set_keymap(p_ctrl, tr_feed_btn_keymap);
  ctrl_set_proc(p_ctrl, tr_feed_btn_proc);
  ctrl_set_rstyle(p_ctrl, RSI_ITEM_8_BG, RSI_OPT_BTN_H, RSI_ITEM_8_BG);
  text_set_font_style(p_ctrl, FSI_COMM_TXT_SH, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CANCEL);

  //help
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_CA_CARD_FEED_HELP_ICON,
							TR_CA_FEED_HELP_ICON_X, TR_CA_FEED_HELP_ICON_Y,
							TR_CA_FEED_HELP_ICON_W, TR_CA_FEED_HELP_ICON_H,p_frm1, 0);
  bmap_set_content_by_id(p_ctrl, IM_HELP_OK);

  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_CA_CARD_FEED_HELP_TEXT,
                           TR_CA_FEED_HELP_TEXT_X, TR_CA_FEED_HELP_TEXT_Y,
                           TR_CA_FEED_HELP_TEXT_W, TR_CA_FEED_HELP_TEXT_H,
                           p_frm1, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_TR_CHILD_FEED);

  ctrl_default_proc(ctrl_get_child_by_id(p_frm2, IDC_CA_CARD_FEED_OK), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_ca_do_cmd(CAS_CMD_MON_CHILD_STATUS_GET, 0 ,0);
  g_feed_status = FEED_STATUS_NULL;

  return SUCCESS;
}

static RET_CODE on_tr_feed_btn_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 next_id = 0;
  control_t *p_next_ctrl = NULL;

  if(g_feed_status !=FEED_STATUS_NULL && g_feed_status != FEED_STATUS_PROMPT)
  {
    switch(p_ctrl->id)
    {
      case IDC_CA_CARD_FEED_OK:
        if(msg == MSG_FOCUS_DOWN)
        {
          next_id = IDC_CA_CARD_FEED_CANCEL;
        }
        else if(msg == MSG_FOCUS_UP)
        {
          next_id = IDC_CA_CARD_FEED_CANCEL;
        }
        break;

      case IDC_CA_CARD_FEED_CANCEL:
        if(msg == MSG_FOCUS_DOWN)
        {
          next_id = IDC_CA_CARD_FEED_OK;
        }
        else if(msg == MSG_FOCUS_UP)
        {
          next_id = IDC_CA_CARD_FEED_OK;
        }
        break;

	default:
        break;
    }

    p_next_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, next_id);

    ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0);
    ctrl_default_proc(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    
    ctrl_paint_ctrl(p_ctrl, TRUE);
    ctrl_paint_ctrl(p_next_ctrl, TRUE);
  }
  return ERR_NOFEATURE;
}


static RET_CODE on_tr_feed_btn_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_frm1, *p_frm2, *p_button;
  p_frm2 = p_ctrl->p_parent;
  p_frm1 = ctrl_get_child_by_id(p_frm2->p_parent, IDC_CA_CARD_FEED_FRM1);
  p_button = ctrl_get_child_by_id(p_frm2, IDC_CA_CARD_FEED_OK);
    
  if(g_feed_status == FEED_STATUS_PROMPT)
  {
    ctrl_set_sts(p_frm1,OBJ_STS_HIDE);
    ctrl_set_sts(p_frm2, OBJ_STS_SHOW);
    ctrl_default_proc(p_button, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_frm1->p_parent, TRUE);
    g_feed_status = FEED_STATUS_INSERT;
  }
  else
  {
    switch(p_ctrl->id)
    {
      case IDC_CA_CARD_FEED_OK:
        if(g_feed_status == FEED_STATUS_INSERT)
        {
         g_feed_status = FEED_STATUS_READY;
         ui_ca_do_cmd(CAS_CMD_MON_CHILD_STATUS_GET, 0 ,0);
        }
        else if(g_feed_status == FEED_STATUS_MOTHER_SUCCESS)
        {
          //ui_ca_do_cmd(CAS_CMD_MON_CHILD_FEED, g_info_addr, 0);
          ui_ca_do_cmd(CAS_CMD_MON_CHILD_FEED, (u32)&g_feed_info, 0);
        }
        else if(g_feed_status == FEED_STATUS_SON_SUCCESS)
        {
          manage_close_menu(ROOT_ID_TR_FEED, 0, 0);
        }
        break;

      case IDC_CA_CARD_FEED_CANCEL:
        g_feed_status = FEED_STATUS_NULL;
	 	ctrl_set_sts(p_frm2,OBJ_STS_HIDE);
        ctrl_set_sts(p_frm1, OBJ_STS_SHOW);
        ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0);
        ctrl_default_proc(p_button, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_frm2->p_parent, TRUE);
        ui_ca_do_cmd(CAS_CMD_MON_CHILD_STATUS_GET, 0 ,0);
        break;
        
      default:
        break;
    }
  }
  return ERR_NOFEATURE;
}

static RET_CODE on_tr_cont_ca_msg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{

  control_t *p_frm1, *p_support;
  RET_CODE ret = (RET_CODE)para1;

  p_frm1 = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_FEED_FRM1);
  p_support = ctrl_get_child_by_id(p_frm1, IDC_CA_CARD_PROMPT_2);

  DEBUG(CAS,INFO,"ret = %x \n",ret);
  if(g_feed_status == FEED_STATUS_NULL)
  {
    g_feed_status = FEED_STATUS_PROMPT;
    if(ret == SUCCESS)
    {
      text_set_content_by_strid(p_support, IDS_CURRENT_IS_MOTHER_CARD);
    }
    else if(ret == CAS_ERROR_CARD_NOTSUPPORT)
    {
       text_set_content_by_strid(p_support, IDS_CURRENT_IS_SON_CARD);
    }
    else
    {
      if(ui_is_smart_card_insert() == FALSE)
        text_set_content_by_strid(p_support, IDS_CA_CARD_NOT_INSERT);
      else
        text_set_content_by_strid(p_support, IDS_NOT_SUPPORT_FEED);
    }
    ctrl_paint_ctrl(p_cont, TRUE);
    return ERR_NOFEATURE;
  }

  switch(ret)
  {
    case SUCCESS:
      if(g_feed_status == FEED_STATUS_READY)
      {
        //g_info_addr = para2;
        memcpy(&g_feed_info, (void*)para2, sizeof(chlid_card_status_info));
        g_feed_status = FEED_STATUS_MOTHER_SUCCESS;  
      }
      else if(g_feed_status == FEED_STATUS_MOTHER_SUCCESS)
      {
        g_feed_status = FEED_STATUS_SON_SUCCESS;          
      }
      break;

    case CAS_E_PARAM_ERR:
      g_feed_status = FEED_STATUS_PARAM_ERR; 
      break;
      
    case CAS_E_FEED_DATA_ERROR:
      g_feed_status = FEED_STATUS_FEED_DATA_ERR; 
      break;
      
    case CAS_ERROR_CARD_NOTSUPPORT:
      g_feed_status = FEED_STATUS_CARD_NOTSUPPORT; 
      break;
      
    case CAS_E_CARD_PARTNER_FAILED:
      g_feed_status = FEED_STATUS_PARTNER_FAILED; 
      break;
      
    case CAS_E_UNKNOW_ERR:
      g_feed_status = FEED_STATUS_UNKNOW_ERR; 
      break;
      
    default:
      break;
  }

  on_tr_feed_state_update(p_cont, 0, 0, 0, TRUE);

  return ERR_NOFEATURE;
}


BEGIN_KEYMAP(tr_feed_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(tr_feed_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tr_feed_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_tr_cont_ca_msg)
  ON_COMMAND(MSG_CA_MON_CHILD_FEED, on_tr_cont_ca_msg)
END_MSGPROC(tr_feed_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(tr_feed_btn_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(tr_feed_btn_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tr_feed_btn_proc, text_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_tr_feed_btn_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_tr_feed_btn_change)
  ON_COMMAND(MSG_SELECT, on_tr_feed_btn_select) 
END_MSGPROC(tr_feed_btn_proc, text_class_proc)

