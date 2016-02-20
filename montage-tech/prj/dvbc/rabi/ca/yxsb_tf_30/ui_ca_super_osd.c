/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ui_ca_super_osd.h"

#include "ui_ca_public.h"
//lint -save -e19
#include "math.h"
//lint -restore
enum control_id
{
  IDC_INVALID = 0,
  IDC_CONTENT,
  IDC_SN_CARD,
};

void on_ca_super_osd_exit()
{
  fw_destroy_mainwin_by_id(ROOT_ID_CA_SUPER_OSD);
}

RET_CODE open_super_osd(u32 para1, u32 para2)
{
  control_t *p_cont, *p_txt, *p_card;
  msg_info_t *osd_msg_info = (msg_info_t *)para2;
  u8 display_front_size = 0;
  u8 background_area = 0;

  u16 width = 0;
  u16 height = 0;
  u16 sn_card_height = 0;
  u16 txt_height = 0;
  u16 x = 0;
  u16 y = 0; 
  u8 asc_str[360] = {0};
  u16 uni_str[360] = {0}; 

  if(NULL != fw_find_root_by_id(ROOT_ID_CA_SUPER_OSD))
  {
      on_ca_super_osd_exit();
  }

  //color_t p_colors;
  if(osd_msg_info)
  {
    background_area = osd_msg_info->background_area;
    display_front_size = osd_msg_info->display_front_size;
  }
  else
  {
     //background_area = 80;
     //display_front_size = 0;
	 return ERR_FAILURE;
  }

  UI_PRINTF("@@@@@ super osd background_area:%d, display_front_size:%d\n", background_area, display_front_size);
  //lint -e{734}
  width = (u16)(sqrt((background_area * 1.0)/100) * (SCREEN_WIDTH -60));
  //lint -e{734}
  height = (u16)(sqrt((background_area * 1.0)/100) * (SCREEN_HEIGHT - 50));
  x = (SCREEN_WIDTH - width)/2;
  y = (SCREEN_HEIGHT - height)/2;  
  
  p_cont = fw_create_mainwin(ROOT_ID_CA_SUPER_OSD,
                             x, y,
                             width, height,
                             RSC_INVALID_ID, 0,OBJ_ATTR_INACTIVE, 0);
                             
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }  
  
  //ctrl_set_keymap(p_cont, ca_super_osd_keymap);
  //ctrl_set_proc(p_cont, ca_super_osd_proc);
  g_rsc_config.p_rstyle_tab[RSI_PBACK].bg.value =osd_msg_info->background_color;    
  ctrl_set_rstyle(p_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  sn_card_height = 40;
  txt_height = height - sn_card_height;
  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           0, 0,
                           width,txt_height,
                           p_cont, 0);
  text_set_align_type(p_txt, STL_VCENTER | STL_CENTER);

  //set txt font 
  if(display_front_size == 0)
  {
    g_rsc_config.p_fstyle_tab[FSI_OSD1].width = 24;
    g_rsc_config.p_fstyle_tab[FSI_OSD1].height = 24;
  }
  else if(display_front_size == 1)
  {
    g_rsc_config.p_fstyle_tab[FSI_OSD1].width = 48;
    g_rsc_config.p_fstyle_tab[FSI_OSD1].height = 48;

  }
  else if(display_front_size == 2)
  {
    g_rsc_config.p_fstyle_tab[FSI_OSD1].width = 12;
    g_rsc_config.p_fstyle_tab[FSI_OSD1].height = 12;
  }
  else
  {
  /*
    if(display_front_size > 56)
    {
      display_front_size = 56;
    }*/
    //lint -e{734}
    display_front_size = (display_front_size * 56/255);
    g_rsc_config.p_fstyle_tab[FSI_OSD1].width = display_front_size;
    g_rsc_config.p_fstyle_tab[FSI_OSD1].height = display_front_size;
  }
  g_rsc_config.p_fstyle_tab[FSI_OSD1].color =osd_msg_info->font_color;
  text_set_font_style(p_txt, FSI_OSD1, FSI_OSD1, FSI_OSD1);

  text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
  memcpy((void *)asc_str, (void *)osd_msg_info->data, 360);
  //gb2312_to_unicode(osd_msg_info->data, osd_msg_info->data_len, uni_str, 1024);
  OS_PRINTF("osd_msg_info->data === === %s, data_len ======= %d\n",osd_msg_info->data, osd_msg_info->data_len);
  str_asc2uni(asc_str, uni_str);
  text_set_content_by_unistr(p_txt, uni_str);
  if(ui_is_smart_card_insert())
  {
    p_card = ctrl_create_ctrl(CTRL_TEXT, IDC_SN_CARD,
                             10, txt_height,
                             width - 10,sn_card_height,
                             p_cont, 0);
    text_set_align_type(p_card, STL_VCENTER | STL_LEFT);
    
    g_rsc_config.p_fstyle_tab[FSI_OSD4].color =osd_msg_info->font_color;
    g_rsc_config.p_fstyle_tab[FSI_OSD4].height =18;
    g_rsc_config.p_fstyle_tab[FSI_OSD4].width =18;
    text_set_font_style(p_card, FSI_OSD4, FSI_OSD4, FSI_OSD4);
    
    text_set_content_type(p_card, TEXT_STRTYPE_UNICODE);
    if(osd_msg_info->display_front)
    {
      text_set_content_by_ascstr(p_card, osd_msg_info->title);
    }
  }
   
  ctrl_default_proc(p_txt, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

/*
BEGIN_KEYMAP(ca_super_osd_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  //ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(ca_super_osd_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_super_osd_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_ca_super_osd_exit_updown)
  ON_COMMAND(MSG_FOCUS_DOWN, on_ca_super_osd_exit_updown)
  ON_COMMAND(MSG_EXIT, on_ca_super_osd_exit)
  ON_COMMAND(MSG_CANCEL, on_ca_super_osd_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_ca_super_osd_exit_all)
END_MSGPROC(ca_super_osd_proc, ui_comm_root_proc)
*/
