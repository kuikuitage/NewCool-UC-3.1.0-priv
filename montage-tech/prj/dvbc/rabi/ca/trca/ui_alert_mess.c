/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_alert_mess.h"
#include "ui_ca_public.h"
#include "ui_signal.h"

static cas_announce_headers_t alert_header;
static u8 g_del_alert = 0; 

enum control_id
{
  IDC_INVALID = 0,
  IDC_ALERT_ITEMS,
  IDC_ALERT_PRO,
  IDC_ALERT_BAR,
  IDC_ALERT_RECEIVED_HEAD,
  IDC_ALERT_RECEIVED,
  IDC_ALERT_RESET_HEAD,
  IDC_ALERT_RESET,
  IDC_ALERT_DEL_ICON,
  IDC_ALERT_DEL_TEXT,
  IDC_ALERT_DEL_ALL_ICON,
  IDC_ALERT_DEL_ALL_TEXT,
};

enum alert_mess_local_msg
{
  MSG_DELETE_ONE = MSG_LOCAL_BEGIN + 150,
  MSG_DELETE_ALL,
};

enum read_alert_id
{
  READY_TO_READ_MAIL = 0,
};

#ifdef WIN32
static cas_mail_headers_t test_cas_mail_head = 
{
      {
        {0, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {20, 12, 7, 12, 0x11, 0x58}, 4, 5, 6},
        {1, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {2, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {3, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {4, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {5, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
        {0, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
        {1, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {2, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {3, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {4, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {5, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
        {0, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
        {1, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {2, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {3, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {4, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {5, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
      },
      18,
      9,
      18,
      8,
};
#endif

u16 alert_list_keymap(u16 key);
RET_CODE alert_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE alert_plist_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(ctrl);
  u8 asc_str[32];
  u16 uni_str[64];
  control_t *p_ctrl= NULL;
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char *)asc_str, "%d ", i + start + 1);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 
      
      /* Is new */
      list_set_field_content_by_strid(ctrl, (u16)(start + i), 1, alert_header.p_mail_head[i + start].new_email ? IDS_YES : IDS_NO);

      /* NAME */
      gb2312_to_unicode(alert_header.p_mail_head[i + start].subject, 80, 
                      uni_str, 41);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 2, (u16 *)uni_str);
      
      //importance
      list_set_field_content_by_strid(ctrl, (u16)(start + i), 3,
        alert_header.p_mail_head[i + start].priority ? IDS_YES : IDS_NO);

      //delete
      //if(del_flag[start + i] == STATUS_DEL_TRUE)
      //{
        //list_set_field_content_by_icon(ctrl, start + i, 4, IM_TV_DEL);
      //}
      //else if(del_flag[start + i] == STATUS_DEL_FALSE)
      //{
     //list_set_field_content_by_icon(ctrl, start + i, 4, 0);
      //}
    }

  }
  
  if(0xFFFF != alert_header.max_num)
  {
     p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_ALERT_RECEIVED);
     sprintf((char*)asc_str,"%d",alert_header.max_num);
     text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
     p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_ALERT_RESET);
     sprintf((char*)asc_str,"%d",ALERT_MAX_NUMB - alert_header.max_num);
     text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
  }
  return SUCCESS;
}

static RET_CODE plist_update_alert(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  u16 size = ALERT_LIST_PAGE;
  memset(&alert_header, 0, sizeof(cas_announce_headers_t));
  #ifndef WIN32
  memcpy(&alert_header, (void*)para2, sizeof(cas_announce_headers_t));
  OS_PRINTF("plist_update_alert alert_header.max_num= %d\n",alert_header.max_num);

  #else
  memcpy(&alert_header, &test_cas_mail_head, sizeof(cas_announce_headers_t));
  #endif

  list_set_count(p_ctrl, alert_header.max_num, ALERT_LIST_PAGE);  
  list_set_focus_pos(p_ctrl, 0);
  alert_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), size, 0);
  ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);

 return SUCCESS;

}

RET_CODE open_alert_mess(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_mbox = NULL;
  control_t *p_sbar = NULL;
  control_t *p_list = NULL;
  //control_t *p_ctrl_cont = NULL;
  control_t *p_ctrl = NULL;
  u8 asc_str[8];
  u16 i = 0;
  
  u16 itemmenu_btn_str[ALERT_ITME_COUNT] =
  {
    IDS_CA_NUMBER, IDS_CA_NEW_NOT, IDS_CA_TITLE, IDS_CA_IMPORT, IDS_DELETE,  
  };
  
  list_xstyle_t alert_item_rstyle =
  {
  	RSI_PBACK,
    RSI_COMM_CONT_SH,
    RSI_SELECT_F,
    RSI_SELECT_F,
    RSI_SELECT_F,
  };
  
  list_xstyle_t alert_plist_field_fstyle =
  {
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
  };
  list_xstyle_t alert_plist_field_rstyle =
  {
    RSI_PBACK,
    RSI_PBACK,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };

  list_field_attr_t alert_plist_attr[ALERT_LIST_FIELD] =
  {
    { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
        (ALERT_MBOX_W / 5 - 20), 0, 0, &alert_plist_field_rstyle,  &alert_plist_field_fstyle },
        
    { LISTFIELD_TYPE_STRID | STL_CENTER| STL_VCENTER,
        (ALERT_MBOX_W / 5), (ALERT_MBOX_W / 5 - 10), 0, &alert_plist_field_rstyle,  &alert_plist_field_fstyle },

    { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
        (ALERT_MBOX_W / 5), (ALERT_MBOX_W / 5) * 2 - 10, 0, &alert_plist_field_rstyle,  &alert_plist_field_fstyle},

    { LISTFIELD_TYPE_STRID,
        (ALERT_MBOX_W / 5), (ALERT_MBOX_W / 5) * 3 - 10, 0, &alert_plist_field_rstyle,  &alert_plist_field_fstyle },
    
    { LISTFIELD_TYPE_ICON ,
        (ALERT_MBOX_W / 5 - 30), ((ALERT_MBOX_W / 5) * 4 - 10), 0, &alert_plist_field_rstyle,  &alert_plist_field_fstyle },
  };

  p_cont = ui_comm_right_root_create(ROOT_ID_ALERT_MESS, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);

  //mbox item title
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_ALERT_ITEMS,
                            ALERT_MBOX_X, ALERT_MBOX_Y, 
                            ALERT_MBOX_W, ALERT_MBOX_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_PBACK,RSI_PBACK, RSI_PBACK);
  ctrl_set_attr(p_mbox, OBJ_ATTR_ACTIVE);
  mbox_set_focus(p_mbox, 1);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox, ALERT_ITME_COUNT, ALERT_ITME_COUNT, 1);
  //mbox_set_item_rect(p_mbox, CA_MBOX_MID_X, 0, CA_MBOX_MID_W, CA_MBOX_MID_H, 0, 0);
  mbox_set_string_fstyle(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_offset(p_mbox, ALERT_ITEM_OFFSET, 0);
  mbox_set_string_align_type(p_mbox, STL_CENTER| STL_VCENTER);
  for (i = 0; i < ALERT_ITME_COUNT; i++)
  {
    mbox_set_content_by_strid(p_mbox, i, itemmenu_btn_str[i]);
  }
  //ALERT list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_ALERT_PRO,
                           ALERT_LIST_X, ALERT_LIST_Y, ALERT_LIST_W,ALERT_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_list, alert_list_keymap);
  ctrl_set_proc(p_list, alert_list_proc);

  ctrl_set_mrect(p_list, ALERT_LIST_MIDL, ALERT_LIST_MIDT,
           ALERT_LIST_MIDL + ALERT_LIST_MIDW, ALERT_LIST_MIDT + ALERT_LIST_MIDH);
  list_set_item_rstyle(p_list, &alert_item_rstyle);
  list_set_count(p_list, 0, ALERT_LIST_PAGE);

  list_set_field_count(p_list, ALERT_LIST_FIELD, ALERT_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, alert_plist_update, 0);

  for (i = 0; i < ALERT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(alert_plist_attr[i].attr), (u16)(alert_plist_attr[i].width),
                        (u16)(alert_plist_attr[i].left), (u8)(alert_plist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, alert_plist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, alert_plist_attr[i].fstyle);
  }
  
  //list scroll bar
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_ALERT_BAR, 
                              ALERT_SBAR_X, ALERT_SBAR_Y, ALERT_SBAR_W, ALERT_SBAR_H, 
                              p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
  ctrl_set_mrect(p_sbar, 0, 0, ALERT_SBAR_W,  ALERT_SBAR_H);
  list_set_scrollbar(p_list, p_sbar);
  //ALERT_plist_update(p_list, list_get_valid_pos(p_list), ALERT_LIST_PAGE, 0);

  //received head
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_ALERT_RECEIVED_HEAD,
                           ALERT_RECEIVED_HEAD_X, ALERT_RECEIVED_HEAD_Y,
                           ALERT_RECEIVED_HEAD_W,ALERT_RECEIVED_HEAD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_RECEIVED);

  //received
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_ALERT_RECEIVED,
                           ALERT_RECEIVED_X, ALERT_RECEIVED_Y,
                           ALERT_RECEIVED_W,ALERT_RECEIVED_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_ctrl, (u8*)"0");

  //rest space head
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_ALERT_RESET_HEAD,
                           ALERT_RESET_HEAD_X, ALERT_RESET_HEAD_Y,
                           ALERT_RESET_HEAD_W, ALERT_RESET_HEAD_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_REST_SPACE);

  //rest space
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_ALERT_RESET,
                           ALERT_RESET_X, ALERT_RESET_Y,
                           ALERT_RESET_W, ALERT_RESET_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  sprintf((char*)asc_str,"%d",ALERT_MAX_NUMB);
  text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);

  //del one
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ALERT_DEL_ICON,
							ALERT_DEL_ICON_X, ALERT_DEL_ICON_Y,
							ALERT_DEL_ICON_W, ALERT_DEL_ICON_H,
							p_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_KEY_IC_DEL);
  mtos_printk("[debug] %s %d \n",__FUNCTION__,__LINE__);

  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_ALERT_DEL_TEXT,
                           ALERT_DEL_TEXT_X, ALERT_DEL_TEXT_Y,
                           ALERT_DEL_TEXT_W, ALERT_DEL_TEXT_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_DELETE);

  //del all
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ALERT_DEL_ALL_ICON,
							ALERT_DEL_ALL_ICON_X, ALERT_DEL_ALL_ICON_Y,
							ALERT_DEL_ALL_ICON_W, ALERT_DEL_ALL_ICON_H,
							p_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_KEY_IC_FN);
  mtos_printk("[debug] %s %d \n",__FUNCTION__,__LINE__);

  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_ALERT_DEL_ALL_TEXT,
                           ALERT_DEL_ALL_TEXT_X, ALERT_DEL_ALL_TEXT_Y,
                           ALERT_DEL_ALL_TEXT_W, ALERT_DEL_ALL_TEXT_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_DELETE_ALL);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifdef WIN32 
  plist_update_alert(p_list, 0, 0, 0);
  #else
  ui_ca_do_cmd(CAS_CMD_ANNOUNCE_HEADER_GET, 0 ,0);
  #endif
  return SUCCESS;
}

static RET_CODE on_alert_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  u16 old_focus, new_focus;
  old_focus = list_get_focus_pos(p_list);
  list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);
  OS_PRINTF("m_id %d\n",alert_header.p_mail_head[new_focus].m_id);
  OS_PRINTF("sender %d\n",alert_header.p_mail_head[new_focus].sender);
  OS_PRINTF("subject %d\n",alert_header.p_mail_head[new_focus].subject);
  
  OS_PRINTF("on_ALERT_list_change_focus : old focus : %d,  new focus : %d\n",old_focus, new_focus);
  return SUCCESS;
}

//extern u16 get_message_strid();
static RET_CODE on_alert_del_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  //RET_CODE ret = SUCCESS;
  u16 new_focus;
  control_t *p_ctrl = NULL;
  p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);

  new_focus = list_get_focus_pos(p_list);
  OS_PRINTF("--------on_ALERT_del_mail   :focus :%d\n",new_focus);
  switch(msg)
  {
      case MSG_DELETE_ONE:
        ui_ca_do_cmd(CAS_CMD_ANNOUNCE_DEL_ONE, alert_header.p_mail_head[new_focus].m_id,0);  
        break;
      case MSG_DELETE_ALL:
        ui_ca_do_cmd(CAS_CMD_ANNOUNCE_DEL_ALL, 0 ,0);  
        break;
      default:
        return ERR_FAILURE;
  }
  if((get_message_strid() == IDS_CA_EMAIL_NO_SPACE) && (NULL != p_ctrl))
  {
    ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_CLEAR_DISPLAY);
  }
  ui_ca_do_cmd(CAS_CMD_ANNOUNCE_HEADER_GET, 0 ,0);
  return SUCCESS;
}

void on_alert_select_del(void)
{
  g_del_alert = 1;
}

void on_alert_cancel_select_del(void)
{
  g_del_alert = 0;
}

static RET_CODE on_alert_dlg_alert_del_one_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  if(alert_header.max_num)
  {
	ui_comm_ask_for_dodlg_open(NULL, IDS_CA_DEL_CURRENT_MAIL, 
	                     on_alert_select_del, on_alert_cancel_select_del, 0);

	if(g_del_alert == 1)
	  on_alert_del_mail(p_list,msg,para1,para2);

	if(list_get_focus_pos(p_list) == (alert_header.max_num - 1))
	  list_set_focus_pos(p_list, (alert_header.max_num - 2));
  }
  return SUCCESS;
}

static RET_CODE on_dlg_alert_del_all_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  if(alert_header.max_num)
  {
    ui_comm_ask_for_dodlg_open(NULL, IDS_CA_DEL_ALL_MAIL, 
                                 on_alert_select_del, on_alert_cancel_select_del, 0);
    if(g_del_alert == 1)
      on_alert_del_mail(p_list,msg,para1,para2);
  }
  return SUCCESS;
}

static RET_CODE on_alert_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u32 ret = READY_TO_READ_MAIL;
  //cas_mail_body_t *mail_body = NULL;
  u16 index = 0;
  prompt_type_t prompt_type;
  index = list_get_focus_pos(p_list);

  prompt_type.index = index;
  prompt_type.message_type = 1;

  OS_PRINTF("MMMMMMMMMMMp_mail_data_buf.total_email_num = %d, index = %d\n",alert_header.total_email_num,index);

  switch(ret)
  {
    case READY_TO_READ_MAIL:
      if((0 != alert_header.max_num) && (0xFFFF != alert_header.max_num))
      {
#ifndef WIN32
        manage_open_menu(ROOT_ID_CA_PROMPT, (u32)&prompt_type, (u32)&alert_header);
#else
        manage_open_menu(ROOT_ID_CA_PROMPT, (u32)&prompt_type, (u32)&test_cas_mail_head);
#endif
      }
      break;
    case CAS_E_MAIL_CAN_NOT_READ:
      ui_comm_cfmdlg_open(NULL,IDS_CA_CARD_EMAIL_ERR,NULL,0);
      OS_PRINTF("ret  = CAS_E_MAIL_CAN_NOT_READ\n");
      break;
    default:
      ui_comm_cfmdlg_open(NULL,IDS_CA_RECEIVED_DATA_FAIL, NULL,0);
    break;
  }
  return SUCCESS;
}

static RET_CODE on_alert_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
   u8 root_id;
   root_id = ctrl_get_ctrl_id(p_list->p_parent);
    
   fw_destroy_mainwin_by_id(ROOT_ID_ALERT_MESS);

   if(ROOT_ID_XSYS_SET==fw_get_focus_id())
	  swtich_to_sys_set(root_id, 0);
   
   ui_ca_do_cmd(CAS_CMD_ANNOUNCE_HEADER_GET, 0 ,0);
   return SUCCESS;
}

static RET_CODE on_alert_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_close_all_mennus();

  return SUCCESS;
}

BEGIN_KEYMAP(alert_list_keymap, NULL)
  ON_EVENT(V_KEY_F1, MSG_DELETE_ONE)
  ON_EVENT(V_KEY_RED, MSG_DELETE_ONE)
  ON_EVENT(V_KEY_HOT_XDEL, MSG_DELETE_ONE)
  ON_EVENT(V_KEY_F2, MSG_DELETE_ALL)
  ON_EVENT(V_KEY_HOT_XEXTEND, MSG_DELETE_ALL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MAIL, MSG_EXIT_ALL)
END_KEYMAP(alert_list_keymap, NULL)

BEGIN_MSGPROC(alert_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_alert_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_alert_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_alert_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_alert_list_change_focus)
  ON_COMMAND(MSG_CA_ANNOUNCE_HEADER_INFO, plist_update_alert)
  ON_COMMAND(MSG_SELECT, on_alert_list_select)
  ON_COMMAND(MSG_DELETE_ONE, on_alert_dlg_alert_del_one_mail)
  ON_COMMAND(MSG_DELETE_ALL, on_dlg_alert_del_all_mail)
  ON_COMMAND(MSG_EXIT, on_alert_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_alert_exit_all)
END_MSGPROC(alert_list_proc, list_class_proc)

