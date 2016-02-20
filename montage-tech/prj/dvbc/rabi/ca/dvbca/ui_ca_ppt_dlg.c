/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_ca_ppt_dlg.h"
#include "ui_notify.h"
#include "ui_ca_ppv_icon.h"
#include "ui_signal.h"
#include "ui_ca_public.h"

enum ppv_ctrl_id
{
  IDC_PPV_CONT1 = 1,
  IDC_TEXT_BIT_YES,
  IDC_TEXT_BIT_NO,  
  IDC_PPV_CONT2,
  IDC_TEXT_TITLE1,
  IDC_TEXT_TITLE2,
  IDC_PPV_TITLE1 ,
  IDC_PPV_TITLE2 ,
  IDC_PPV_TITLE3 ,
  
};

enum local_msg
{
  MSG_PPV_ORDE = MSG_LOCAL_BEGIN + 720,

};

static u16 ca_ppv_info_keymap(u16 key);
RET_CODE ca_ppv_info_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
static u16 ca_ppv_cont1_keymap(u16 key);
RET_CODE ca_ppv_cont1_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
static u16 ca_ppv_cont2_keymap(u16 key);
RET_CODE ca_ppv_cont2_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

static ipp_buy_info_t *p_buy_info = NULL;
static u8 p_buy_flag = 0;
static burses_info_t *p_burse = NULL;
static ipps_info_t *p_ipp_info = NULL;
//for x86 test
#ifdef WIN32
ipp_buy_info_t g_buy_info;
burses_info_t g_burse;
ipps_info_t g_ipp_info;
static void ui_set_ppv_info_test()
{
  p_buy_info = &g_buy_info;
  p_burse = &g_burse;
  p_ipp_info = &g_ipp_info;
  g_burse.p_burse_info[p_burse->index].cash_value = 102030405;
  g_ipp_info.p_ipp_info[0].book_state_flag = 0;
  g_buy_info.ipp_charge = 20;
}
#endif

void ui_ppv_dlg_close(void)
{
	manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG,0,0);
}


static void get_ppv_str2(u16 *uni_str, u16 str_id, u8 *ansstr)
{
  u16 str[20] = {0}; 
  u16 len = 0;

  str_asc2uni((u8 *)ansstr, str);
  uni_strcat(uni_str, str,64);
  len = (u16)uni_strlen(uni_str);
  gui_get_string(str_id, str, 64 - len); 
  uni_strcat(uni_str, str, 64); 

  return;
}

#if 0 
static void get_ppv_str(u16 *uni_str, u16 str_id, u32 content)
{
  u16 str[10] = {0}; 
  u16 len = 0;
  
  convert_i_to_dec_str(str, content);
  uni_strcat(uni_str, str,64);
  len = (u16)uni_strlen(uni_str);
  gui_get_string(str_id, str, 64 - len); 
  uni_strcat(uni_str, str, 64); 

  return;
}

static RET_CODE on_ca_dlg_cancel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;
  
  p_btn = ctrl_get_child_by_id(p_ctrl, IDC_TEXT_BIT_NO);

  if(p_btn != NULL)
  {
    ctrl_set_active_ctrl(p_ctrl, p_btn);
  }
  
  ui_ppv_dlg_close();
  return SUCCESS;
}
#endif
static RET_CODE on_ppv_ca_orde_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;
  p_btn = ctrl_get_active_ctrl(p_ctrl);
  if (p_btn != NULL)
  {
    if(ctrl_get_ctrl_id(p_btn) == IDC_TEXT_BIT_YES)
    {
      p_buy_info->state_flag = 1;
	  p_buy_flag = 1;//when click YES
      ui_ca_do_cmd((u32)CAS_CMD_IPP_BUY_SET, (u32)p_buy_info,0);
    }
    else
    {
      p_buy_info->state_flag = 0;
      ui_ca_do_cmd((u32)CAS_CMD_IPP_BUY_SET, (u32)p_buy_info,0);
    }
  }
  ui_ppv_dlg_close();
  update_signal();
  return SUCCESS;
}

static RET_CODE on_ppv_ca_burse_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_attr, *p_text;
  u16 uni_str[32];
  u8 burse_str[64]={0};
  u16 ipp_burse_i = 0; ////整数部分，元
  u16 ipp_burse_f = 0;////小数部分，元
  u32 ipp_val = 0;////余额

  #ifndef WIN32
  p_burse = (burses_info_t*)para2;
  #endif
  p_attr = ctrl_get_child_by_id(p_ctrl, IDC_PPV_CONT1);
  p_text = ctrl_get_child_by_id(p_attr, IDC_PPV_TITLE3);
  if(p_burse->p_burse_info[p_burse->index].cash_value >= 0)
  {
    ipp_val = p_burse->p_burse_info[p_burse->index].cash_value - p_burse->p_burse_info[p_burse->index].balance;
    ipp_burse_i = ipp_val / 10000;
    ipp_burse_f = (ipp_val - ipp_burse_i * 10000) / 100;
    sprintf((char *)burse_str, "%d.%02d", ipp_burse_i, ipp_burse_f);
    memset(uni_str, 0, 32 * sizeof(u16));
    get_ppv_str2(uni_str, IDS_CA_RMB, burse_str);
    
    ui_comm_static_set_content_by_unistr(p_text, uni_str);
  }
  else
  {
    ipp_val =  p_burse->p_burse_info[p_burse->index].balance - p_burse->p_burse_info[p_burse->index].cash_value;
    ipp_burse_i = ipp_val / 10000;
    ipp_burse_f = (ipp_val - ipp_burse_i * 10000) / 100;
    sprintf((char *)burse_str, "-%d.%02d", ipp_burse_i, ipp_burse_f);
    memset(uni_str, 0, 32 * sizeof(u16));
    get_ppv_str2(uni_str, IDS_CA_RMB, burse_str);
    
    ui_comm_static_set_content_by_unistr(p_text, uni_str);
  }
  ctrl_paint_ctrl(p_ctrl,TRUE);

  return SUCCESS;
}

static RET_CODE on_ppv_ca_ppv_bug_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_attr, *p_text;
  u16 uni_str[64] = {0,};
  u8 ipp_str[64]={0};
  u16 ipp_charge_i = 0; ////整数部分，元
  u16 ipp_charge_f = 0;////小数部分，元

  #ifndef WIN32
  p_buy_info = (ipp_buy_info_t *)para2;
  #endif
  p_attr = ctrl_get_child_by_id(p_ctrl, IDC_PPV_CONT1); 
  p_text = ctrl_get_child_by_id(p_attr, IDC_PPV_TITLE2);
  OS_PRINTF("p_buy_info->ipp_charge ====== : %d\n",p_buy_info->ipp_charge);
  
  ipp_charge_i = p_buy_info->ipp_charge / 10000;
  ipp_charge_f = (p_buy_info->ipp_charge - ipp_charge_i * 10000) / 100;
  sprintf((char *)ipp_str, "%d.%02d", ipp_charge_i, ipp_charge_f);
  memset(uni_str, 0, 64 * sizeof(u16));
  get_ppv_str2(uni_str, IDS_CA_RMB, ipp_str);
  ui_comm_static_set_content_by_unistr(p_text, uni_str);
  ctrl_paint_ctrl(p_ctrl,TRUE);
  return SUCCESS;
}

static RET_CODE on_ppv_ca_ppv_show_status(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_attr, *p_text;
    if (ui_get_ca_last_msg() == 0)
	{
		manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);
		return SUCCESS;
	}
  #ifndef WIN32
  p_ipp_info = (ipps_info_t *)para2;
  #endif
  p_attr = ctrl_get_child_by_id(p_ctrl, IDC_PPV_CONT1);
  p_text = ctrl_get_child_by_id(p_attr, IDC_PPV_TITLE1);
  OS_PRINTF("p_ipp_info->p_ipp_info[0].book_state_flag ====== : %d\n",p_ipp_info->p_ipp_info[0].book_state_flag);

  if(p_ipp_info->p_ipp_info[0].book_state_flag)
  {
    ui_comm_static_set_content_by_strid(p_text, IDS_HAS_ORRDERED);
  }
  else
  {
    ui_comm_static_set_content_by_strid(p_text, IDS_NOT_ORDER);
  }
  ctrl_paint_ctrl(p_ctrl, TRUE);
  return SUCCESS;
}

static RET_CODE on_ppv_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  return ERR_NOFEATURE;
}

static RET_CODE on_ppv_change_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  u16 prog_id = 0;
  switch (msg)
  {
    case MSG_FOCUS_UP:
      ui_shift_prog(1, TRUE, &prog_id);
      break;
    case MSG_FOCUS_DOWN:
      ui_shift_prog(-1, TRUE, &prog_id);
      break;
    default:
      break;
  }
  
  ui_ppv_dlg_close();
	close_ca_ppv_icon();
	manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
  
  return SUCCESS;
}


RET_CODE open_ca_ippv_ppt_dlg(u32 para1, u32 para2)
{
//attr1 cont
#define SN_PPV_CONT_TITLE_X              50
#define SN_PPV_CONT_TITLE_Y              0
#define SN_PPV_CONT_TITLE_W              480
#define SN_PPV_CONT_TITLE_H              220

//attr1 ctrl
#define SN_PPV_TEXT_TITLE_X              100
#define SN_PPV_TEXT_TITLE_Y              5
#define SN_PPV_TEXT_TITLE_W              280
#define SN_PPV_TEXT_TITLE_H              35

//attr2
#define SN_PPV_CONT_TITLE2_X             50
#define SN_PPV_CONT_TITLE2_Y             60
#define SN_PPV_CONT_TITLE2_W             480
#define SN_PPV_CONT_TITLE2_H             50

//attr2 ctrl
#define SN_PPV_TEXT_TITLE_X              100
#define SN_PPV_TEXT_TITLE_Y              5
#define SN_PPV_TEXT_TITLE_W              280
#define SN_PPV_TEXT_TITLE_H              35




  control_t *p_cont = NULL,*p_title = NULL,*p_bit,*p_attr1,*p_attr2,*p_ctrl2, *p_ctrl = NULL;
  u16 i = 0, y =0, x = 0;
  u16 str_id[PPV_INFO_ITEM_CNT] = 
  {
    IDS_CA_SELECTED_STATUS_1,
    IDS_CA_PROGRAM_PRICE,
    IDS_CA_CARD_BALANCE,
    IDS_IPPV_ORDER,
    IDS_IPPV_RESCING
  };
  //YYF only use to check if ippv has clicked YES to purchase
  if (p_buy_flag == 1)
  {
	p_buy_flag = 0;//reset flag
  	return SUCCESS;
  }
  //end
  if (ui_get_ca_last_msg() == 0)
  {
  	return SUCCESS;
  }
  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
    manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  #ifdef WIN32
  ui_set_ppv_info_test();
  #endif
  open_ca_ppv_icon(0, 0);
  if((p_cont = fw_find_root_by_id(ROOT_ID_CA_IPPV_PPT_DLG)) != NULL)
  {
    return SUCCESS;
  }
  p_cont = fw_create_mainwin(ROOT_ID_CA_IPPV_PPT_DLG,
                                  PPV_INFO_CONT_X, PPV_INFO_CONT_Y+40, 
                                  PPV_INFO_CONT_W, PPV_INFO_CONT_H,
                                  0, 0,
                                  OBJ_ATTR_ACTIVE, STL_EX_TOPMOST);
  if (p_cont == NULL)
  {
    return FALSE;
  }
  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, ca_ppv_info_keymap);
  ctrl_set_proc(p_cont, ca_ppv_info_proc);
  

  p_attr1 = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_PPV_CONT1),
                        PPV_CONT_TITLE_X,
                        PPV_CONT_TITLE_Y,
                        PPV_CONT_TITLE_W, PPV_CONT_TITLE_H,
                        p_cont, 0);
  ctrl_set_rstyle(p_attr1, RSI_PBACK, RSI_PBACK, RSI_PBACK);//RSI_CA_PPV_N
  ctrl_set_keymap(p_attr1, ca_ppv_cont1_keymap);
  ctrl_set_proc(p_attr1, ca_ppv_cont1_proc);
  
  ctrl_set_attr(p_attr1, OBJ_ATTR_ACTIVE);
  
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_TEXT_TITLE1),
                        PPV_TEXT_TITLE_X,
                        PPV_TEXT_TITLE_Y,
                        PPV_TEXT_TITLE_W, PPV_TEXT_TITLE_H,
                        p_attr1, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);//RSI_CA_PPV_N
  text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_CA_PPV_PROG);

  p_attr2 = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_PPV_CONT2),
                        PPV_CONT_TITLE2_X,
                        PPV_CONT_TITLE2_Y,
                        PPV_CONT_TITLE2_W, PPV_CONT_TITLE2_H,
                        p_cont, 0);
  ctrl_set_rstyle(p_attr2, RSI_PBACK, RSI_PBACK, RSI_PBACK);//RSI_CA_PPV_N
  ctrl_set_keymap(p_attr2, ca_ppv_cont2_keymap);
  ctrl_set_proc(p_attr2, ca_ppv_cont2_proc);
    

  p_ctrl2 = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_TEXT_TITLE2),
                        PPV_TEXT_TITLE_X,
                        PPV_TEXT_TITLE_Y,
                        PPV_TEXT_TITLE_W, PPV_TEXT_TITLE_H,
                        p_attr2, 0);
  ctrl_set_rstyle(p_ctrl2, RSI_PBACK, RSI_PBACK, RSI_PBACK);//RSI_CA_PPV_N
  text_set_font_style(p_ctrl2, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
  text_set_align_type(p_ctrl2, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl2, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl2, IDS_IPPV_PROG);
  
  y = PPV_ACCEPT_CA_INFO_ITEM_Y;
  for (i = 0; i<PPV_ACCEPT_CA_INFO_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
        p_title = ui_comm_static_create(p_attr1, (u8)(IDC_PPV_TITLE1 + i),
                                          PPV_ACCEPT_CA_INFO_ITEM_X,y,
                                          PPV_ACCEPT_CA_INFO_ITEM_LW0,
                                          PPV_ACCEPT_CA_INFO_ITEM_RW0);
        ui_comm_static_set_static_txt(p_title, str_id[i]);
        ui_comm_static_set_param(p_title, TEXT_STRTYPE_STRID);
        ui_comm_static_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
        break;
      case 1:
        p_title = ui_comm_static_create(p_attr1, (u8)(IDC_PPV_TITLE1 + i),
                                           PPV_ACCEPT_CA_INFO_ITEM_X, y,
                                           PPV_ACCEPT_CA_INFO_ITEM_LW1,
                                           PPV_ACCEPT_CA_INFO_ITEM_RW1);
        ui_comm_static_set_static_txt(p_title, str_id[i]);
        ui_comm_static_set_param(p_title, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
        break;
      case 2:
        p_title = ui_comm_static_create(p_attr1, (u8)(IDC_PPV_TITLE1 + i),
                                           PPV_ACCEPT_CA_INFO_ITEM_X, y,
                                           PPV_ACCEPT_CA_INFO_ITEM_LW2,
                                           PPV_ACCEPT_CA_INFO_ITEM_RW2);
        ui_comm_static_set_static_txt(p_title, str_id[i]);
        ui_comm_static_set_param(p_title, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
        break;
       default:
        break;
    }
    ui_comm_ctrl_set_cont_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    y += PPV_ACCEPT_CA_INFO_ITEM_H + PPV_ACCEPT_CA_INFO_ITEM_V_GAP;  
        
  }
  for(i=0; i<PPV_SET_BTN_CNT; i++)
  {
    x = PPV_TEXT_BIT_X;
    p_bit = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_TEXT_BIT_YES+i),
                        (PPV_TEXT_BIT_X+(PPV_TEXT_BIT_W+40)*i),
                        PPV_TEXT_BIT_Y,
                        PPV_TEXT_BIT_W, PPV_TEXT_BIT_H,
                        p_attr1, 0);
    ctrl_set_rstyle(p_bit, RSI_PBACK, RSI_PBACK, RSI_PBACK);//RSI_CA_PPV_BTN_SH, RSI_CA_PPV_BTN_HL, RSI_CA_PPV_BTN_SH
    text_set_font_style(p_bit, FSI_CA_PPV_N, FSI_CA_PPV_HL, FSI_CA_PPV_N);
    text_set_align_type(p_bit, STL_CENTER| STL_VCENTER);
    text_set_content_type(p_bit, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_bit, str_id[3+i]);
    ctrl_set_related_id(p_bit,
                        (u8)((i - 1 + PPV_SET_BTN_CNT) % PPV_SET_BTN_CNT + IDC_TEXT_BIT_YES), /* left */
                        0,                             /* up */
                        (u8)((i + 1) % PPV_SET_BTN_CNT + IDC_TEXT_BIT_YES),       /* right */
                        0);                            /* down */
  }

  p_ctrl = ctrl_get_child_by_id(p_attr1, IDC_TEXT_BIT_YES);
  ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  #ifndef WIN32
  ui_ca_do_cmd((u32)CAS_CMD_GET_IPP_BUY_INFO, 0,0);
  ui_ca_do_cmd((u32)CAS_CMD_BURSE_INFO_GET, 0,0);
  ui_ca_do_cmd((u32)CAS_CMD_IPPV_INFO_GET, 0,0);
  #endif
  return SUCCESS;
}

#if 0
static RET_CODE on_ppv_ca_ok(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl;
  
  ctrl_default_proc(p_cont, MSG_LOSTFOCUS, 0, 0);
  p_ctrl = ctrl_get_child_by_id(p_cont->p_parent,IDC_PPV_CONT1);
  ctrl_set_attr(p_ctrl, OBJ_ATTR_ACTIVE);
  ctrl_set_attr(p_cont, OBJ_ATTR_HIDDEN);
  p_ctrl = ctrl_get_child_by_id(p_ctrl,IDC_TEXT_BIT_YES);
  ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);

  ui_ca_do_cmd((u32)CAS_CMD_GET_IPP_BUY_INFO, 0,0);
  ui_ca_do_cmd((u32)CAS_CMD_BURSE_INFO_GET, 0,0);
  ui_ca_do_cmd((u32)CAS_CMD_IPPV_INFO_GET, 0,0);
  
  #ifdef WIN32
  //on_ppv_ca_burse_info(p_cont->p_parent, 0, 0, 0);
  //on_ppv_ca_ppv_bug_info(p_cont->p_parent, 0, 0, 0);
  //on_ppv_ca_ppv_show_status(p_cont->p_parent, 0, 0, 0);
  #endif
  
  ctrl_paint_ctrl(p_cont->p_parent, TRUE);
  
  return SUCCESS;
}
#endif
BEGIN_KEYMAP(ca_ppv_info_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ca_ppv_info_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_ppv_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_BUR_INFO,on_ppv_ca_burse_info)
  ON_COMMAND(MSG_CA_IPPV_BUY_INFO,on_ppv_ca_ppv_bug_info)
  ON_COMMAND(MSG_CA_IPV_INFO,on_ppv_ca_ppv_show_status)
END_MSGPROC(ca_ppv_info_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ca_ppv_cont1_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_PPV_ORDE)
END_KEYMAP(ca_ppv_cont1_keymap, NULL)

BEGIN_MSGPROC(ca_ppv_cont1_proc, cont_class_proc)
  ON_COMMAND(MSG_PPV_ORDE,on_ppv_ca_orde_select)
  ON_COMMAND(MSG_DESTROY, on_ppv_destroy)
  ON_COMMAND(MSG_FOCUS_UP,on_ppv_change_channel)
  ON_COMMAND(MSG_FOCUS_DOWN,on_ppv_change_channel)
END_MSGPROC(ca_ppv_cont1_proc, cont_class_proc)
 
BEGIN_KEYMAP(ca_ppv_cont2_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ca_ppv_cont2_keymap, NULL)

BEGIN_MSGPROC(ca_ppv_cont2_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP,on_ppv_change_channel)
  ON_COMMAND(MSG_FOCUS_DOWN,on_ppv_change_channel)
END_MSGPROC(ca_ppv_cont2_proc, cont_class_proc)




