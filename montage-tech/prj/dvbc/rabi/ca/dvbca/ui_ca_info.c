/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_notify.h"
#include "sys_dbg.h"

#include "ui_xmainmenu.h"

#include "ui_ca_info.h"

// test whm1210

enum osd_set_control_id
{

  IDC_INVALID = 0,

  //wanghm
  IDC_SMARTCARD_INFO,
  IDC_WORKTIME,
  IDC_ENTITLE_INFO,
  IDC_STB_PAIR,
  IDC_CHANGE_PIN_CODE,
  IDC_CA_VIEW_LEVEL,
  IDC_SP_INFO,
  //IDC_LANGUAGE_SET,
  //IDC_ADVANCED_SET,
  //IDC_CHANNEL_INFORMATION,

  //wanghm end

  //xuxin
  
  IDC_SUB_PREV,
  IDC_AD_WINDOW,
  IDC_SECOND_TITLE,
  IDC_SMALL_BG1,
  IDC_SMALL_BG2,

  IDC_BOOK_MANAGE,
  IDC_PASSWORD_CHANGE,
  IDC_AUTO_SEARCH,
  IDC_MANUAL_SEARCH, 
  IDC_FULL_SEARCH,
  IDC_FACTORY_SET,
  IDC_PARAME_SET,
  IDC_SIGNAL_CHECK,
  IDC_CONDITIONAL_ACCEPT,
  IDC_SOFTWARE_UPGRADE,
  IDC_FREQUENCE_SET,
};
enum submenu_local_msg
{
  MSG_OSD_VER_CHANGE = MSG_LOCAL_BEGIN + 600,
};

static u8 g_sub_curn_index = 0;


u16 card_info_item_keymap(u16 key);

RET_CODE card_info_item_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);

u16 card_info_cont_keymap(u16 key);

RET_CODE card_info_cont_proc(control_t *ctrl, u16 msg, 
                            u32 para1, u32 para2);

u16 card_info_factory_pwdlg_keymap(u16 key);

RET_CODE card_info_factory_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 card_info_freq_set_pwdlg_keymap(u16 key);

RET_CODE card_info_freq_set_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 card_info_tp_frm_keymap(u16 key);


extern void restore_to_factory(void);

RET_CODE open_dvb_ca_info(u32 para1, u32 para2)
{
#define SN_SUBMENU_W          640
#define SN_SUBMENU_H          480
#define SN_SUBMENU_TITLE_H	  57
#define SN_HELP_H			  43
#define SN_SUBMENU_BTNICON_X(x) 	(x+(SN_SUBMENU_BTN_W/2)/2)
#define SN_SUBMENU_BTNICON_Y(y)	(y+(SN_SUBMENU_BTN_H/2)/2)
#define SN_SUBMENU_BTNICON_W		30
#define SN_SUBMENU_BTNICON_H	  30
#define SN_SUBMENU_TITILHELP	102
#define SN_SUBMENU_BTN_W 	((SN_SUBMENU_W-20)/2)
#define SN_SUBMENU_BTN_H 	61
#define SN_SUBMENU_BTN_X1 	0
#define SN_SUBMENU_BTN_Y 	0
#define SN_SUB_MENU_HELP_RSC_CNT	15
#define SN_CA_INFO_CNT		7


  control_t *p_cont;//, *p_ctrl_item[5]
  control_t *p_item = NULL;
  //control_t *p_icon = NULL;
 // control_t *p_mw = NULL;
  u16 i,x,y;
  u16 item_str[SN_CA_INFO_CNT] =
  {
  	  IDS_CA_SMARTCARD_INFO,
	  IDS_CA_WOR_DURATION_SET,
	  IDS_CA_ACCREDIT_PRODUCK_INFO,
	  IDS_CA_PAIR_STATE,
      IDS_CA_PWD_MODIFY,
      IDS_CA_LEVEL_CONTROL_SET,
      IDS_3G_OPERATOR,
  };

  DEBUG(DBG,INFO,"open_sn_ca_card_info.....\n");  

  // create container
  p_cont = ui_comm_right_root_create(ROOT_ID_CA_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  // create 
  for(i = 0; i < SN_CA_INFO_CNT; i++)
  {
      if((i <= SN_CA_INFO_CNT/2))
      {
        x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2;
        y = SN_SUBMENU_BTN_Y + i*SN_SUBMENU_BTN_H ;
      }
      else
      {
        x = (SN_SUBMENU_W-(SN_SUBMENU_BTN_W*2))/2+SN_SUBMENU_BTN_W;
        y = SN_SUBMENU_BTN_Y + ((i - SN_CA_INFO_CNT/2 - 1)*SN_SUBMENU_BTN_H);
      }

      //button
      p_item = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_SMARTCARD_INFO + i),
                                x, y, SN_SUBMENU_BTN_W,SN_SUBMENU_BTN_H,
                                p_cont, 0);
      ctrl_set_rstyle(p_item, RSI_PBACK, RSI_PBACK,RSI_PBACK);
      ctrl_set_keymap(p_item, card_info_item_keymap);
      ctrl_set_proc(p_item, card_info_item_proc);
      text_set_font_style(p_item, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
      text_set_align_type(p_item, STL_CENTER | STL_VCENTER);
      text_set_content_type(p_item, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_item, item_str[i]);
  	
   }
  
  p_item = ctrl_get_child_by_id(p_cont, g_sub_curn_index + IDC_SMARTCARD_INFO);
  ctrl_default_proc(p_item, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  DEBUG(DBG,INFO,"paint end\n");
  return SUCCESS;
}


static RET_CODE on_item_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 ctrlID;
  ctrlID = ctrl_get_ctrl_id(ctrl_get_active_ctrl(p_ctrl->p_parent));

  switch(ctrlID)
  {
    case IDC_SMARTCARD_INFO:
//      manage_open_menu(ROOT_ID_CA_CARD_INFO, 0, 0);
      break;
      
    case IDC_WORKTIME:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, 0, 0);
      break;

    case IDC_ENTITLE_INFO:
      manage_open_menu(ROOT_ID_SP_LIST, 0, 0);
      break;

	case IDC_STB_PAIR:
	  manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, 0, 0);
      break;

    case IDC_CHANGE_PIN_CODE:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, 0, 0);
      break;

    case IDC_CA_VIEW_LEVEL: 
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 0, 0);
      break;


    case IDC_SP_INFO:
      manage_open_menu(ROOT_ID_SP_LIST2,0,0);
      break;

     default:
      break;
  }

  return SUCCESS;
}



static RET_CODE on_item_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl;
  u8 pre_index = 0;

  pre_index = g_sub_curn_index;
  p_cont = p_ctrl->p_parent;

  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      if((g_sub_curn_index + SN_CARD_ADT_CNT/2) == 7)
      {
    	break;
      }
	  g_sub_curn_index = (g_sub_curn_index + SN_CARD_ADT_CNT/2) % SN_CARD_ADT_CNT;
	  p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_SMARTCARD_INFO);
	  if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
	  {
	  	g_sub_curn_index = pre_index;
	  }   
	  break;  
    case MSG_FOCUS_LEFT:
	  if((g_sub_curn_index + SN_CARD_ADT_CNT/2) == 7)
      {
    	break;
      }
	  g_sub_curn_index = (g_sub_curn_index + SN_CARD_ADT_CNT/2) % SN_CARD_ADT_CNT;
	  p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_SMARTCARD_INFO);
	  if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
	  {
	  	g_sub_curn_index = pre_index;
	  }	 
	  break;
    case MSG_FOCUS_UP:
	  if((g_sub_curn_index + SN_CARD_ADT_CNT - 1) == 7)
      {
    	g_sub_curn_index = (g_sub_curn_index + SN_CARD_ADT_CNT - 2) % SN_CARD_ADT_CNT;
        p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_SMARTCARD_INFO);
        if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
        {
          g_sub_curn_index = (g_sub_curn_index + SN_CARD_ADT_CNT - 1) % SUBMENU_CNT;
        }      
        break;
      }
      g_sub_curn_index = (g_sub_curn_index + SN_CARD_ADT_CNT - 1) % SN_CARD_ADT_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_SMARTCARD_INFO);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
        g_sub_curn_index = (g_sub_curn_index + SN_CARD_ADT_CNT - 1) % SUBMENU_CNT;
      }      
      break;
    case MSG_FOCUS_DOWN:
	  if((g_sub_curn_index + 1) == 7)
      {
    	g_sub_curn_index = g_sub_curn_index + 1;
      }
      g_sub_curn_index = (g_sub_curn_index + 1) % SN_CARD_ADT_CNT;
      p_next_ctrl = ctrl_get_child_by_id(p_cont, g_sub_curn_index+IDC_SMARTCARD_INFO);
      if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_next_ctrl))
      {
          g_sub_curn_index = (g_sub_curn_index + 1) % SN_CARD_ADT_CNT;
      }      
      break;
  }
  p_next_ctrl = ctrl_get_child_by_id(p_cont, IDC_SMARTCARD_INFO + g_sub_curn_index);

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_next_ctrl, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_item_paint(control_t *ctrl, u16 msg, 
                               u32 para1, u32 para2)
{
  return ERR_NOFEATURE;
}

static RET_CODE on_paint_sub_menu(control_t *ctrl, u16 msg, 
                               u32 para1, u32 para2)
{
  cont_class_proc(ctrl, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_factory_pwdlg_cancel(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_factory_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_factory_pwdlg_correct(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_comm_pwdlg_close();
      
  return SUCCESS;
}


static RET_CODE on_freq_set_pwdlg_cancel(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_freq_set_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_freq_set_pwdlg_correct(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_comm_pwdlg_close();
//  manage_open_menu(ROOT_ID_PROGRAM_SEARCH, 0, 0);
  return SUCCESS;
}

BEGIN_KEYMAP(card_info_item_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(card_info_item_keymap, NULL)

BEGIN_MSGPROC(card_info_item_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_item_select)
  ON_COMMAND(MSG_FOCUS_UP, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_LEFT, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_item_change_focus)
  ON_COMMAND(MSG_PAINT, on_item_paint)
END_MSGPROC(card_info_item_proc, text_class_proc)

BEGIN_KEYMAP(card_info_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_EXIT)
END_KEYMAP(card_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(card_info_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PAINT, on_paint_sub_menu)
END_MSGPROC(card_info_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(card_info_factory_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(card_info_factory_pwdlg_keymap, NULL)

BEGIN_MSGPROC(card_info_factory_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_factory_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_factory_pwdlg_exit)
END_MSGPROC(card_info_factory_pwdlg_proc, cont_class_proc)

BEGIN_KEYMAP(card_info_freq_set_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(card_info_freq_set_pwdlg_keymap, NULL)

BEGIN_MSGPROC(card_info_freq_set_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_freq_set_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_freq_set_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_freq_set_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_freq_set_pwdlg_exit)
END_MSGPROC(card_info_freq_set_pwdlg_proc, cont_class_proc)

