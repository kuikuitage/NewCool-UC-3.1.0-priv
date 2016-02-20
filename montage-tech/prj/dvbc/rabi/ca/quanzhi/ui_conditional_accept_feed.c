/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_xsys_set.h"

#include "ui_conditional_accept_feed.h"
#include "sys_dbg.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CA_CARD_FEED_FRM1,
  IDC_CA_CARD_PROMPT_1,
};

u16 card_feed_root_keymap(u16 key);
RET_CODE card_feed_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


RET_CODE open_conditional_accept_feed(u32 para1, u32 para2)
{

  control_t *p_cont, *p_ctrl,*p_frm1;

  p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_FEED, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, card_feed_root_keymap);
  ctrl_set_proc(p_cont, card_feed_root_proc);
  ctrl_set_rstyle(p_cont, RSI_PBACK,RSI_PBACK,RSI_PBACK);
 
  
  //CA FEED FRAME1
  p_frm1 = ctrl_create_ctrl(CTRL_CONT, (u16)(IDC_CA_CARD_FEED_FRM1),
                            ((SYS_RIGHT_CONT_W - 600)/2), 30, 600, 160, p_cont, 0);
  //ctrl_set_rstyle(p_frm1, RSI_RECT,RSI_HD_RECT,RSI_HD_RECT);
 
  //PROMPT
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u16)(IDC_CA_CARD_PROMPT_1),
                            50, 0, 500, 160, p_frm1, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_HD_RECT,RSI_HD_RECT,RSI_HD_RECT);
  text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_COMM_TXT_GRAY);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_QZ_FEED_CHILD_CARD_PROMPT);


  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  

  return SUCCESS;
}

RET_CODE on_exit_card_feed(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  fw_destroy_mainwin_by_id(ROOT_ID_CONDITIONAL_ACCEPT_FEED);
  return SUCCESS;
}


BEGIN_KEYMAP(card_feed_root_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(card_feed_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(card_feed_root_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_exit_card_feed)
END_MSGPROC(card_feed_root_proc, ui_comm_root_proc)




