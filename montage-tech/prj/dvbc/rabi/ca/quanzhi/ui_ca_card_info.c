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
  IDC_CA_CARD_TYPE ,
  IDC_CA_MOTHER_CARD_NUM,
};

u16 card_info_root_keymap(u16 key);
RET_CODE card_info_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl;
  u16 stxt_card_info[]=
  {
    IDS_CA_SMART_CARD_NUMBER,
    IDS_QZ_CA_AUTH_TIME_EDN,
    IDS_CARD_TYPE,
    IDS_MOTHER_CARD_NUM,
  };
  u16 y=0, i=0;

  DEBUG(CAS, INFO, "\n");
  p_cont = ui_comm_right_root_create(ROOT_ID_CA_CARD_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(NULL == p_cont)
  {
	return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, card_info_root_keymap);
  ctrl_set_proc(p_cont, card_info_root_proc);
  ctrl_set_rstyle(p_cont, RSI_PBACK,RSI_PBACK, RSI_PBACK);

  y = ACCEPT_INFO_CA_INFO_ITEM_Y;
  for(i=0; i < ACCEPT_INFO_CARD_INFO_ITEM_CNT; i++)
  {
  	switch(i)
  	{
  	  case 0:
	  case 1:
        p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_SMART_CARD_NUMBER + i),
                                               ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                               ACCEPT_INFO_CA_INFO_ITEM_LW,
                                               ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
		break;
	 case 2:
        p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_SMART_CARD_NUMBER + i),
                                               ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                               ACCEPT_INFO_CA_INFO_ITEM_LW,
                                               ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
		break;
	case 3:
        p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_SMART_CARD_NUMBER + i),
                                               ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                               ACCEPT_INFO_CA_INFO_ITEM_LW,
                                               ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
		ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);
		break;
	default:
		break;
     
  	}
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

static void conditional_accept_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_card_id = ctrl_get_child_by_id(p_cont, IDC_CA_SMART_CARD_NUMBER);
  control_t *p_cas_ver = ctrl_get_child_by_id(p_cont, IDC_CA_CAS_VER);
  control_t *p_cas_card_type = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_TYPE);
  control_t *p_cas_mother_card_num = ctrl_get_child_by_id(p_cont, IDC_CA_MOTHER_CARD_NUM);

  u8 asc_str[64] = {0};
  MT_ASSERT(p_card_id != NULL);
  MT_ASSERT(p_cas_ver != NULL);
  
  if(p_card_info == NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_card_id,(u8 *) " ");
	ui_comm_static_set_content_by_ascstr(p_cas_ver,(u8 *) " ");
	ui_comm_static_set_content_by_ascstr(p_cas_card_type,(u8 *) " ");
	ui_comm_static_set_content_by_ascstr(p_cas_mother_card_num,(u8 *) " ");
  }
  else
  {

  	if(p_card_info->card_state == 1)
		return;
    //tbd
    sprintf((char *)asc_str, "%s", p_card_info->sn);
    //str_asc2uni(asc_str, uni_num);
    ui_comm_static_set_content_by_ascstr(p_card_id, asc_str);
    
    //tbd
    sprintf((char *)asc_str, "%04d-%02d-%02d", (p_card_info->expire_date[0]<<8) |(p_card_info->expire_date[1]),p_card_info->expire_date[2],
        p_card_info->expire_date[3]);
    ui_comm_static_set_content_by_ascstr(p_cas_ver, asc_str);

	if(p_card_info->card_type == 0)
	{
		ui_comm_static_set_content_by_strid(p_cas_card_type, IDS_CA_MOTHER_CARD);
		ctrl_set_sts(p_cas_mother_card_num,OBJ_STS_HIDE);
		ctrl_paint_ctrl(p_cas_mother_card_num,TRUE);
	}
	else
	{
		ui_comm_static_set_content_by_strid(p_cas_card_type, IDS_CA_CHILD_CARD);

		//sprintf((char *)asc_str, "%s", p_card_info->mother_card_id);
    	//ui_comm_static_set_content_by_ascstr(p_cas_mother_card_num, asc_str);
		//ctrl_set_sts(p_cas_mother_card_num,OBJ_STS_SHOW);
		//ctrl_paint_ctrl(p_cas_mother_card_num,TRUE);
	}
    
  }
}

static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_info_set_content(p_cont, (cas_card_info_t *)para2);
  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

RET_CODE on_exit_card_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  fw_destroy_mainwin_by_id(ROOT_ID_CA_CARD_INFO);
  return SUCCESS;
}

BEGIN_KEYMAP(card_info_root_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(card_info_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(card_info_root_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_info_update)
  ON_COMMAND(MSG_EXIT, on_exit_card_info)
END_MSGPROC(card_info_root_proc, ui_comm_root_proc)


