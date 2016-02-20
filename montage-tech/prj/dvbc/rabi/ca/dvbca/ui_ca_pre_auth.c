/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_ca_pre_auth.h"

enum pre_auth_ctrl_id
{
  IDC_PRE_AUTH_CONT1 = 1,
  
};

RET_CODE ca_pre_auth_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_pre_auth(u32 para1, u32 para2)
{
  control_t *p_cont = NULL,*p_ctrl = NULL;

  if(!ui_is_fullscreen_menu(fw_get_focus_id()))
    return ERR_FAILURE;
  if(fw_find_root_by_id(ROOT_ID_CA_PRE_AUTH) != NULL)
    return ERR_FAILURE;
  p_cont = fw_create_mainwin(ROOT_ID_CA_PRE_AUTH,
                                  PRE_AUTH_CONT_X, PRE_AUTH_CONT_Y, 
                                  PRE_AUTH_CONT_W, PRE_AUTH_CONT_H,
                                  0, 0,
                                  OBJ_ATTR_INACTIVE, 0);
  ctrl_set_proc(p_cont, ca_pre_auth_cont_proc);
  ctrl_set_rstyle(p_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  if (p_cont == NULL)
  {
    return FALSE;
  }
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PRE_AUTH_CONT1, 
                                        PRE_AUTH_TXT_X, PRE_AUTH_TXT_Y, 
                                        PRE_AUTH_TXT_W, PRE_AUTH_TXT_H, 
                                        p_cont, 0);
  text_set_font_style(p_ctrl, FSI_PRE_AUTH_TXT, FSI_PRE_AUTH_TXT, FSI_PRE_AUTH_TXT);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_by_strid(p_ctrl, IDS_PRE_AUTH);

  
  fw_tmr_create(ROOT_ID_CA_PRE_AUTH, MSG_CA_CANCEL_PRE_AUTH, PRE_AUTH_AUTOCLOSE_MS, FALSE);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  return SUCCESS;
}

void close_ca_pre_auth(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_CA_PRE_AUTH);
}

static RET_CODE on_pre_auth_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_ca_pre_auth();

  fw_tmr_destroy(ROOT_ID_CA_PRE_AUTH, MSG_CA_CANCEL_PRE_AUTH);
  return SUCCESS;
}

BEGIN_MSGPROC(ca_pre_auth_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_CA_CANCEL_PRE_AUTH, on_pre_auth_exit)
END_MSGPROC(ca_pre_auth_cont_proc, cont_class_proc)



