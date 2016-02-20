/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_ca_character_code.h"
#include "ui_notify.h"
#include "cas_ware.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

enum card_manage_id
{
  IDC_CHARACTER_NUMB = 1, 
  IDC_CHARACTER_CODE,
  IDC_CHARACTER_CONT,
};

static u16 ca_character_code_keymap(u16 key);
RET_CODE ca_character_code_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_character_code(u32 para1, u32 para2)
{
	//CA frm
#define SN_ACCEPT_CHARACTER_CODE_X                56
#define SN_ACCEPT_CHARACTER_CODE_Y                24
#define SN_ACCEPT_CHARACTER_CODE_W               400// 505
#define SN_ACCEPT_CHARACTER_CODE_H               200// 360

	//LXD	create container 
#define SN_CHARACTER_CODE_CONT_X				  120
#define SN_CHARACTER_CODE_CONT_Y				  150
#define SN_CHARACTER_CODE_CONT_W				  430
#define SN_CHARACTER_CODE_CONT_H				  200

	//LXD    TEXT
#define SN_CHARACTER_CODE_LXD_X				      168
#define SN_CHARACTER_CODE_LXD_Y					  20
#define SN_CHARACTER_CODE_LXD_W					  100// 505
#define SN_CHARACTER_CODE_LXD_H					  30 // 360


	control_t *p_cont, *p_ctrl = NULL;
	control_t *p_ca_frm;
	control_t *p_cont1;

	p_cont = fw_create_mainwin(ROOT_ID_CHARACTER_CODE,
	           SN_CHARACTER_CODE_CONT_X,SN_CHARACTER_CODE_CONT_Y, 
	           SN_CHARACTER_CODE_CONT_W,SN_CHARACTER_CODE_CONT_H-30,
	           0, 0, OBJ_ATTR_ACTIVE, STL_EX_TOPMOST);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_rstyle(p_cont,RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);

	p_cont1 = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_CHARACTER_CONT,
					    0, 0,
					    SN_CHARACTER_CODE_CONT_W, SN_CHARACTER_CODE_CONT_H-30,
					    p_cont, 0);
	ctrl_set_keymap(p_cont1, ca_character_code_keymap);
	ctrl_set_proc(p_cont1, ca_character_code_proc);

	//card character code
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_CHARACTER_NUMB,
					            ACCEPT_CHARACTER_CODE_NUB_X+15, ACCEPT_CHARACTER_CODE_NUB_Y+20, 
					            ACCEPT_CHARACTER_CODE_NUB_W+50, ACCEPT_CHARACTER_CODE_NUB_H+30, 
					            p_cont1, 0);
	ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
	text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

	//TEXT
	p_ca_frm= ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CHARACTER_CODE),
											SN_CHARACTER_CODE_LXD_X+20, SN_CHARACTER_CODE_LXD_Y,
											SN_CHARACTER_CODE_LXD_W, SN_CHARACTER_CODE_LXD_H,
											p_cont1, 0);

	ctrl_set_rstyle(p_ca_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_font_style(p_ca_frm, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
	text_set_align_type(p_ca_frm, STL_LEFT | STL_TOP);
	text_set_content_type(p_ca_frm, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ca_frm, IDS_CA_EIGENVALUE);

	ctrl_default_proc(p_cont1, MSG_GETFOCUS, 0, 0);//p_ca_frm
	ctrl_paint_ctrl(ctrl_get_root(p_cont1), FALSE);      
	ui_ca_do_cmd((u32)CAS_CMD_CARD_INFO_GET, 0 ,0);
	return SUCCESS;
}

static RET_CODE on_is_card_move(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u32 event_id = para2;

  switch(event_id)
  {
    case CAS_S_ADPT_CARD_REMOVE:
      manage_close_menu(ROOT_ID_CA_CARD_INFO,0,0);
      ctrl_paint_ctrl(p_ctrl, FALSE);	  
      break;
    default:
      break;
  }
      ctrl_paint_ctrl(p_ctrl, FALSE);	    
  return SUCCESS;
}

static void set_ca_character_code(control_t *p_cont, cas_card_info_t *p_card_info)
{
	u16 i = 0,j = 0;
	u8 asc_str[128];
	control_t *p_character_code = ctrl_get_child_by_id(p_cont, IDC_CHARACTER_NUMB);

#ifndef WIN32
	for(i = 0, j = 0; i < 2 * CARD_EIGEN_VALU_MAX_LEN; i += 2, j++)
	{
		sprintf((char *)(&asc_str[i]), "%02x", p_card_info->card_eigen_valu[j]);
	}
#endif

	text_set_content_by_ascstr(p_character_code, asc_str);

}

static RET_CODE on_ca_get_character_code(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	set_ca_character_code(p_cont, (cas_card_info_t *)para2);
	ctrl_paint_ctrl(p_cont, TRUE);
	return SUCCESS;
}


static RET_CODE back_to_ca_ca(control_t *p_ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
		manage_close_menu(ROOT_ID_CHARACTER_CODE, 0, 0); 
	  return SUCCESS;
}

BEGIN_KEYMAP(ca_character_code_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_BACK,MSG_BACKSPACE) 
END_KEYMAP(ca_character_code_keymap, ui_comm_root_keymap)


BEGIN_MSGPROC(ca_character_code_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_ca_get_character_code)
  ON_COMMAND(MSG_CA_EVT_NOTIFY, on_is_card_move)
  ON_COMMAND(MSG_BACKSPACE,back_to_ca_ca)
END_MSGPROC(ca_character_code_proc, ui_comm_root_proc)


