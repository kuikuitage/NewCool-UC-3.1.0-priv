/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_ca_card_info.h"
#include "ui_ca_public.h"

enum prog_info_cont_ctrl_id
{
  IDC_CA_SMART_CARD_NUMBER = 1,
  IDC_CA_CAS_VER,
  IDC_CA_SP_ID ,
  IDC_CA_CARD_VERSION
};

u16 card_info_root_keymap(u16 key);
RET_CODE card_info_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl;
  u16 stxt_card_info[]=
  {
    IDS_CA_SMART_CARD_NUMBER,
    IDS_CA_CARD_VER, 
    IDS_CA_SP_NAME, 
    IDS_CA_SP_INFO,
  };
  u16 y=0, i=0;

  DEBUG(CAS, INFO, "\n");
  p_cont = ui_comm_right_root_create(ROOT_ID_CA_CARD_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(NULL!=p_cont);
  ctrl_set_keymap(p_cont, card_info_root_keymap);
  ctrl_set_proc(p_cont, card_info_root_proc);

  y = ACCEPT_INFO_CA_INFO_ITEM_Y;
  for(i=0; i < ACCEPT_INFO_CARD_INFO_ITEM_CNT; i++)
  {
    p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_SMART_CARD_NUMBER + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW);
	MT_ASSERT(NULL!=p_ctrl);
    ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
    ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
    ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    y += ACCEPT_INFO_CA_INFO_ITEM_H + ACCEPT_CA_INFO_ITEM_V_GAP;
  }

  //ui_comm_help_create(&card_info_help_data, p_cont);
  
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, TRUE);
#ifndef WIN32
  ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
#endif
  return SUCCESS;
}
#if 1
static void conditional_accept_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_cas_sp_id = ctrl_get_child_by_id(p_cont, IDC_CA_SP_ID);
  control_t *p_card_id = ctrl_get_child_by_id(p_cont, IDC_CA_SMART_CARD_NUMBER);
  control_t *p_cas_ver = ctrl_get_child_by_id(p_cont, IDC_CA_CAS_VER);
  control_t *p_cas_card_version = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_VERSION);

  u8 asc_str[64] = {0};
  u16 uni_str[64] = {0};
  MT_ASSERT(p_cas_sp_id != NULL);
  MT_ASSERT(p_card_id != NULL);
  MT_ASSERT(p_cas_ver != NULL);
  MT_ASSERT(p_cas_card_version != NULL);
  if(p_card_info == NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_cas_sp_id, (u8 *)" ");
    ui_comm_static_set_content_by_ascstr(p_card_id,(u8 *) " ");
	ui_comm_static_set_content_by_ascstr(p_cas_card_version,(u8 *) " ");
	ui_comm_static_set_content_by_ascstr(p_cas_ver,(u8 *) " ");
  }
  else
  {
  	if(p_card_info->cas_manu_name != NULL)
  	{
    	sprintf((char *)asc_str, "%s", p_card_info->cas_manu_name);
		gb2312_to_unicode(asc_str, sizeof(asc_str), uni_str, (sizeof(uni_str)/sizeof(uni_str[0]) - 1));
    	//ui_comm_static_set_content_by_ascstr(p_cas_sp_id, asc_str);
		ui_comm_static_set_content_by_unistr(p_cas_sp_id, uni_str);
  	}
	
    //tbd
    sprintf((char *)asc_str, "%s", p_card_info->sn);
    //str_asc2uni(asc_str, uni_num);
    ui_comm_static_set_content_by_ascstr(p_card_id, asc_str);
    
    sprintf((char *)asc_str, "%s", p_card_info->cas_ver);
    ui_comm_static_set_content_by_ascstr(p_cas_ver, asc_str);
    
    //card_version
    if(p_card_info->desc != NULL)
  	{
    	sprintf((char *)asc_str, "%s", p_card_info->desc);
		memset(uni_str,0,(sizeof(uni_str)/sizeof(uni_str[0])));
    	gb2312_to_unicode(asc_str, sizeof(asc_str), uni_str, (sizeof(uni_str)/sizeof(uni_str[0]) - 1));
    	ui_comm_static_set_content_by_unistr(p_cas_card_version, uni_str);
    }
  }
}
#endif
static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS, INFO, "get card info in ui\n");
  conditional_accept_info_set_content(p_cont, (cas_card_info_t *)para2);
  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

BEGIN_KEYMAP(card_info_root_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(card_info_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(card_info_root_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_info_update)
END_MSGPROC(card_info_root_proc, ui_comm_root_proc)


