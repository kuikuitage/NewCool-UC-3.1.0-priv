/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "ui_ca_finger.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CONTENT,
};

RET_CODE ca_finger_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

void open_ca_finger_menu(finger_msg_t *p_ca_finger)
{
  control_t *p_txt, *p_cont = NULL;
  u16 finger_x = 0;
  u16 finger_y = 0;
  u8 asc_str[64];


  if(!ui_is_fullscreen_menu(fw_get_focus_id()))
  {
    return;
  }

  if(ROOT_ID_PROG_BAR == fw_get_focus_id())
  {
	manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  }

  if(NULL != fw_find_root_by_id(ROOT_ID_FINGER_PRINT))
  {
  	fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
      //return;
  }
  #ifndef WIN32
  DEBUG(DBG, INFO, "show_postion:%d\n", p_ca_finger->show_postion);
  if(p_ca_finger->show_postion == 0)
  {
    finger_x = 70;
    finger_y = 102;
  }
  else if(p_ca_finger->show_postion == 1)
  {
    finger_x = 1020;
    finger_y = 102;
  }
  else if(p_ca_finger->show_postion == 2)
  {
    finger_x = 70;
    finger_y = 580;
  }
  else if(p_ca_finger->show_postion == 3)
  {
    finger_x = 1020;
    finger_y = 580;
  }
  else
  {
    finger_x = p_ca_finger->show_postion_x;
    finger_y = p_ca_finger->show_postion_y;
  } 
  #endif
  
  p_cont = fw_create_mainwin(ROOT_ID_FINGER_PRINT,
                             finger_x, finger_y,
                             200, 31,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
    return;
  }
  ctrl_set_proc(p_cont, ca_finger_proc);
  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);


  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           0, 0, 200, 31,
                           p_cont, 0);
  ctrl_set_rstyle(p_txt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  text_set_align_type(p_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_txt, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
  text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
  #ifndef WIN32
  sprintf((char *)asc_str, "%s", p_ca_finger->data);
  #endif
  text_set_content_by_ascstr(p_txt, asc_str);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_paint_ctrl(p_cont, FALSE);

  fw_tmr_create(ROOT_ID_FINGER_PRINT, MSG_EXIT, FINGER_AUTOCLOSE_MS, FALSE);

}

void close_ca_finger_menu(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
}

static RET_CODE on_ca_finger_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_ca_finger_menu();
//  ui_set_finger_show(FALSE);
  fw_tmr_destroy(ROOT_ID_FINGER_PRINT, MSG_EXIT);
  return SUCCESS;
}

BEGIN_MSGPROC(ca_finger_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_ca_finger_exit)
END_MSGPROC(ca_finger_proc, cont_class_proc)
