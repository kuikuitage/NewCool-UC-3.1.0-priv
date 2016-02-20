/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_ca_card_info.h"
#include "ui_ca_public.h"
#include "desai_ca5.2_api.h"

enum prog_info_cont_ctrl_id
{
    IDC_CA_SP_ID = 1,
    IDC_CA_SMART_CARD_NUMBER,
    IDC_CA_CARD_NUMBER2,
    IDC_CA_CA_STATUS,
    IDC_CA_CAS_VER,
    IDC_CA_WATCH_LEVE,
    IDC_CA_START_TIME3,
    IDC_CA_MASTER_ID,
    IDC_CA_LCO_ID,
    IDC_CA_GROUP_ID,
    IDC_CA_CARD_VERSION
};

u16 card_info_root_keymap(u16 key);
RET_CODE card_info_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl;
  u16 stxt_card_info[]=
  {
    IDS_CA_SP_OPERATOR_INFO, 
    IDS_CA_SMART_CARD_NUMBER,
    IDS_CA_ADDRES_INFO,
    IDS_CA_CARD_STATE,
    IDS_CA_CARD_VER, 
    IDS_CA_WATCH_LEVEL,
    IDS_CA_WORK_TIME,
    IDS_CA_MASTER_CARD_ID,
    IDS_CA_LOCAL_DIVISION_ID,
    IDS_CA_GROUP_ID,
    IDS_CA_CARD_VERSION
  };
  u16 y=0, i=0;
  
  p_cont = ui_comm_right_root_create(ROOT_ID_CA_CARD_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, card_info_root_keymap);
  ctrl_set_proc(p_cont, card_info_root_proc);

  y = ACCEPT_INFO_CA_INFO_ITEM_Y;
  for(i=0; i < ACCEPT_INFO_CARD_INFO_ITEM_CNT; i++)
  {
    p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_SP_ID + i),
                                           ACCEPT_INFO_CA_INFO_ITEM_X, y,
                                           ACCEPT_INFO_CA_INFO_ITEM_LW,
                                           ACCEPT_INFO_CA_INFO_ITEM_RW);
    ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
    if(IDC_CA_CA_STATUS == (IDC_CA_SP_ID + i))
    {
       ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
    }
    else
    {
      ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
    }
    ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              ACCEPT_INFO_CARD_INFO_ITEM_CNT) %
                             ACCEPT_INFO_CARD_INFO_ITEM_CNT + IDC_CA_SP_ID),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % ACCEPT_INFO_CARD_INFO_ITEM_CNT + IDC_CA_SP_ID));/* down */

    y += COMM_CTRL_H + ACCEPT_CA_INFO_ITEM_V_GAP;
  }
  
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);
#ifndef WIN32
  ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
#endif
  return SUCCESS;
}

static void conditional_accept_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
    control_t *p_cas_sp_id = ctrl_get_child_by_id(p_cont, IDC_CA_SP_ID);
    control_t *p_card_id = ctrl_get_child_by_id(p_cont, IDC_CA_SMART_CARD_NUMBER);
    control_t *p_arear_code = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_NUMBER2);
    control_t *p_cas_satue = ctrl_get_child_by_id(p_cont, IDC_CA_CA_STATUS);

    control_t *p_cas_ver = ctrl_get_child_by_id(p_cont, IDC_CA_CAS_VER);
    control_t *p_cas_time = ctrl_get_child_by_id(p_cont, IDC_CA_START_TIME3);
    control_t *p_cas_leve = ctrl_get_child_by_id(p_cont, IDC_CA_WATCH_LEVE);

    control_t *p_cas_mater_id = ctrl_get_child_by_id(p_cont, IDC_CA_MASTER_ID);
    control_t *p_cas_lco_id = ctrl_get_child_by_id(p_cont, IDC_CA_LCO_ID);
    control_t *p_cas_group_id = ctrl_get_child_by_id(p_cont, IDC_CA_GROUP_ID);
    control_t *p_cas_card_version = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_VERSION);

    u8 asc_str[64] = {0};
    u16 uni_str[64] = {0};
    u8 card_state = 0;
  
    if(p_card_info == NULL)
    {
        ui_comm_static_set_content_by_ascstr(p_cas_sp_id, (u8 *)" ");
        ui_comm_static_set_content_by_ascstr(p_card_id,(u8 *) " ");
        ui_comm_static_set_content_by_ascstr(p_arear_code, (u8 *)" ");
    }
    else
    {
        sprintf((char *)asc_str, "%s", p_card_info->cas_id);
        ui_comm_static_set_content_by_ascstr(p_cas_sp_id, asc_str);

        //tbd
        sprintf((char *)asc_str, "%s", p_card_info->sn);
        //str_asc2uni(asc_str, uni_num);
        ui_comm_static_set_content_by_ascstr(p_card_id, asc_str);

        sprintf((char *)asc_str, "%s", p_card_info->area_code);
        ui_comm_static_set_content_by_ascstr(p_arear_code, asc_str);

        sprintf((char *)asc_str, "%02d:%02d:%02d~%02d:%02d:%02d ", p_card_info->work_time.start_hour,p_card_info->work_time.start_minute,p_card_info->work_time.start_second,
                                    p_card_info->work_time.end_hour,p_card_info->work_time.end_minute,p_card_info->work_time.end_second);
        ui_comm_static_set_content_by_ascstr(p_cas_time, asc_str);

        sprintf((char *)asc_str, "%s", p_card_info->cas_ver);
        ui_comm_static_set_content_by_ascstr(p_cas_ver, asc_str);

        sprintf((char *)asc_str, "%d", p_card_info->card_work_level);
        ui_comm_static_set_content_by_ascstr(p_cas_leve, asc_str);

        //master id
        sprintf((char *)asc_str, "%s", p_card_info->mother_card_id);
        ui_comm_static_set_content_by_ascstr(p_cas_mater_id, asc_str);

        //lco id
        sprintf((char *)asc_str, "%s", p_card_info->cas_loc);
        ui_comm_static_set_content_by_ascstr(p_cas_lco_id, asc_str);

        //group id
        sprintf((char *)asc_str, "%s", p_card_info->cas_group);
        ui_comm_static_set_content_by_ascstr(p_cas_group_id, asc_str);

        //card_version
        sprintf((char *)asc_str, "%s", p_card_info->card_version);
        gb2312_to_unicode(asc_str, sizeof(asc_str), uni_str, 64);
        ui_comm_static_set_content_by_unistr(p_cas_card_version, uni_str);

        sprintf((char *)asc_str, "%d", p_card_info->card_state);
        card_state = p_card_info->card_state;
        //	DEBUG(DSCA, INFO, "card_state:%d\n", card_state);
        switch(card_state){
            case CA_ERROR_CARD_CANCELED:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_SMC_OVERDUE);
                break;

            case CA_ERROR_CARD_PENDING:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_NO_ENABLE1);
                break;

            case CA_ERROR_CARD_OUTTIME:
            	    ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_SMC_OVERDUE);
                break;

            case CA_ERROR_IC_INVALID:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_INSERT_SMC);
                break;

            case CA_ERROR_CARD_STOP:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_CARD_STOP);
                break;

            case CA_ERROR_CARD_NOPAIR:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_NO_NEED_PAIR);
                break;

            case CA_ERROR_OK:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_CARD_PARTNER_SUCCESS);
                break;

            case CA_ERROR_CARD_PAIROTHER:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_STB_PAIR_OTHER);
                break;

            case CA_ERROR_NET_ERR:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_CARD_NET_ERR_LOCK);
                break;

            case CA_ERROR_SYS_ERR:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_CARD_SYS_ERR_LOCK);
                break;

            case CA_ERROR_CARD_UNWONTED:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_CARD_DATA_ERROR);
                break;

            case CA_ERROR_CARD_NEED_UPDATE:
                ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_CARD_NEED_UPG);
                break;

            default:
                break;
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


