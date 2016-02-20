/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_ca_mother.h"
#include "ui_notify.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

enum card_manage_id
{
  IDC_TITLE1 = 1, 
  IDC_TITLE2,
  IDC_TITLE3,
  IDC_TITLE4,
  SN_CA_MOTHER_TITLE1,
  SN_CA_MOTHER_TITLE2,
  SN_CA_MOTHER_TITLE3,
  SN_CA_MOTHER_TITLE4,  
};

#define PARENT_CARD_SN_MAX_LEN         8
static u16 ca_mother_cont_keymap(u16 key);
RET_CODE ca_mother_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE open_ca_mother(u32 para1, u32 para2)
{
//CA frame
#define SN_ACCEPT_CA_MOTHER_FRM_X                    80//106
#define SN_ACCEPT_CA_MOTHER_FRM_Y                    24
#define SN_ACCEPT_CA_MOTHER_FRM_W                    500
#define SN_ACCEPT_CA_MOTHER_FRM_H                    360 

  control_t *p_cont, *p_ctrl = NULL;
  control_t *p_ca_frm;
  u8 i;
  u16 y;
  u16 str_id[ACCEPT_CA_MOTHER_TITLE_CNT] =
  { 
    IDS_CARD_TYPE, 
    IDS_CA_MASTER_CARD_ID,
    IDS_CA_WATCH_TIME,
    IDS_CA_REMAIN_TIME
    };

  // create container
  p_cont = ui_comm_right_root_create(ROOT_ID_CA_MOTHER, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ca_mother_cont_keymap);
  ctrl_set_proc(p_cont, ca_mother_cont_proc);
  
  //CA frm
  p_ca_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_FRM_BUTTONS,
                              SN_ACCEPT_CA_MOTHER_FRM_X, SN_ACCEPT_CA_MOTHER_FRM_Y,
                              SN_ACCEPT_CA_MOTHER_FRM_W, SN_ACCEPT_CA_MOTHER_FRM_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  //card manage
   y = ACCEPT_CA_MOTHER_ITEM_Y;
  for (i = 0; i<ACCEPT_CA_MOTHER_TITLE_CNT; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_TITLE1 + i),
                                          ACCEPT_CA_MOTHER_ITEM_X,y,
                                          ACCEPT_CA_MOTHER_ITEM_LW0,
                                          ACCEPT_CA_MOTHER_ITEM_RW0);
        ui_comm_static_set_static_txt(p_ctrl, str_id[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
		
      case 1:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_TITLE1 + i),
                                           ACCEPT_CA_MOTHER_ITEM_X, y,
                                           ACCEPT_CA_MOTHER_ITEM_LW1,
                                           ACCEPT_CA_MOTHER_ITEM_RW1);
        ui_comm_static_set_static_txt(p_ctrl, str_id[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
		
      case 2:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_TITLE1 + i),
                                           ACCEPT_CA_MOTHER_ITEM_X, y,
                                           ACCEPT_CA_MOTHER_ITEM_LW2,
                                           ACCEPT_CA_MOTHER_ITEM_RW2);
        ui_comm_static_set_static_txt(p_ctrl, str_id[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
		
     case 3:
        p_ctrl = ui_comm_static_create(p_ca_frm, (u8)(IDC_TITLE1 + i),
                                           ACCEPT_CA_MOTHER_ITEM_X, y,
                                           ACCEPT_CA_MOTHER_ITEM_LW3,
                                           ACCEPT_CA_MOTHER_ITEM_RW3);
        ui_comm_static_set_static_txt(p_ctrl, str_id[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
		
      default:
        break;    
     }
    ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              ACCEPT_CA_MOTHER_TITLE_CNT) %
                             ACCEPT_CA_MOTHER_TITLE_CNT + IDC_TITLE1),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % ACCEPT_CA_MOTHER_TITLE_CNT + IDC_TITLE1));/* down */
    y += ACCEPT_CA_MOTHER_ITEM_H + ACCEPT_CA_MOTHER_ITEM_V_GAP;
  }

  ctrl_default_proc(p_ca_frm, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifndef WIN32
  ui_ca_do_cmd((u32)CAS_CMD_MON_CHILD_STATUS_GET, 0 ,0);
  #endif
  return SUCCESS;
}

static void set_ca_child_mother_info(control_t *p_cont, chlid_card_status_info *p_card)
{
  #if 1
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_BUTTONS);
  control_t *p_card_style = ctrl_get_child_by_id(p_frm, IDC_TITLE1);
  control_t *p_mon_code = ctrl_get_child_by_id(p_frm, IDC_TITLE2);
  control_t *p_watch_time = ctrl_get_child_by_id(p_frm, IDC_TITLE3);
  control_t *p_over_time = ctrl_get_child_by_id(p_frm, IDC_TITLE4);
  u8 asc_str[64],i = 0,j = 0;
  if(p_card == NULL)
  {
    mtos_printk("%s,    %d\n",__FUNCTION__,__LINE__);
    ui_comm_static_set_content_by_ascstr(p_card_style, (u8 *)" ");
    ui_comm_static_set_content_by_ascstr(p_mon_code, (u8 *)" ");
  }
  else
  {
    switch(p_card->is_child)
    {
      case 0:
        ui_comm_static_set_content_by_strid(p_card_style, IDS_CA_MOTHER_CARD);
        break;
      case 1:
        ui_comm_static_set_content_by_strid(p_card_style, IDS_CA_CHILD_CARD);
        break;
      default:
        break;
    }
    for(i = 0, j = 0; i < PARENT_CARD_SN_MAX_LEN; i += 2, j++)
    {
      sprintf((char *)(&asc_str[i]), "%02x", p_card->parent_card_sn[j]);
    }
    
    ui_comm_static_set_content_by_ascstr(p_mon_code, asc_str);
    sprintf((char *)asc_str, "%d", p_card->child_card_authed_time[0]<<8 | p_card->child_card_authed_time[1]);
    ui_comm_static_set_content_by_ascstr(p_watch_time, asc_str);
		
    sprintf((char *)asc_str, "%d", p_card->child_card_remain_time[0]<<8 | p_card->child_card_remain_time[1]);
    ui_comm_static_set_content_by_ascstr(p_over_time, asc_str);
    
  }
  #endif
  return;
}

static RET_CODE on_ca_get_mon_child_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  set_ca_child_mother_info(p_cont, (chlid_card_status_info *)para2);
  ctrl_paint_ctrl(p_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_is_card_move(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u32 event_id = para2;

  switch(event_id)
  {
    case CAS_S_ADPT_CARD_REMOVE:
      manage_close_menu(ROOT_ID_CA_MOTHER,0,0);
      break;
    default:
      break;
  }
    
  return SUCCESS;
}


BEGIN_KEYMAP(ca_mother_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ca_mother_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_mother_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_ca_get_mon_child_info)
  ON_COMMAND(MSG_CA_EVT_NOTIFY, on_is_card_move)
END_MSGPROC(ca_mother_cont_proc, ui_comm_root_proc)




