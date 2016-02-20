/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_ca_antiauth_info.h"
#include "ui_ca_public.h"

enum control_id
{
    IDC_INVALID = 0,
    IDC_STB_CARD_BURSE1,
    IDC_STB_CARD_BURSE2,
    IDC_STB_CARD_BURSE3,
    IDC_STB_CARD_BURSE4,
};

u16 ca_burse_cont_keymap(u16 key);
RET_CODE ca_burse_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

static void ca_burse_set_content(control_t *p_cont, burses_info_t * burses)
{
    u8 asc_str[32];
    control_t *p_burse1 = ctrl_get_child_by_id(p_cont, IDC_STB_CARD_BURSE1);
    control_t *p_burse2 = ctrl_get_child_by_id(p_cont, IDC_STB_CARD_BURSE2);
    control_t *p_burse3 = ctrl_get_child_by_id(p_cont, IDC_STB_CARD_BURSE3);
    control_t *p_burse4 = ctrl_get_child_by_id(p_cont, IDC_STB_CARD_BURSE4);

    sprintf((char *)asc_str, "%s", burses->p_burse_info[1].burse_value);
    ui_comm_static_set_content_by_ascstr(p_burse1, asc_str);

    sprintf((char *)asc_str, "%s", burses->p_burse_info[2].burse_value);
    ui_comm_static_set_content_by_ascstr(p_burse2, asc_str);

    sprintf((char *)asc_str, "%s", burses->p_burse_info[3].burse_value);
    ui_comm_static_set_content_by_ascstr(p_burse3, asc_str);

    sprintf((char *)asc_str, "%s", burses->p_burse_info[4].burse_value);
    ui_comm_static_set_content_by_ascstr(p_burse4, asc_str);
}

static RET_CODE on_ca_burse_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    ca_burse_set_content(p_cont, (burses_info_t *)para2);
    ctrl_paint_ctrl(p_cont, TRUE);

    return SUCCESS;
}

RET_CODE open_ca_antiauth_info(u32 para1, u32 para2)
{
    control_t *p_cont = NULL, *p_ctrl = NULL;
    u8 i;
    u16 y;
    u16 stxt_ca_info[CA_BURSE_INFO_ITEM_CNT] =
    { 
        IDS_CA_PURSE_BALANCE1, IDS_CA_PURSE_BALANCE2,IDS_CA_PURSE_BALANCE3,IDS_CA_PURSE_BALANCE4
    };

    p_cont = ui_comm_right_root_create(ROOT_ID_CA_ANTIAUTH_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
    if (p_cont == NULL)
    {
        return ERR_FAILURE;
    }
    ctrl_set_keymap(p_cont, ca_burse_cont_keymap);
    ctrl_set_proc(p_cont, ca_burse_cont_proc);
  
    //pair info
    y = CA_BURSE_INFO_ITEM_Y;
    for (i = 0; i<CA_BURSE_INFO_ITEM_CNT; i++)
    {
        switch(i)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_STB_CARD_BURSE1 + i),
                                                   CA_BURSE_INFO_ITEM_X, y,
                                                   CA_BURSE_INFO_ITEM_LW,
                                                   CA_BURSE_INFO_ITEM_RW);
                ui_comm_static_set_static_txt(p_ctrl, stxt_ca_info[i]);
                ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
                ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
                ui_comm_static_set_content_by_ascstr(p_ctrl, (u8 *)" ");
                break;

            default:
                break;
        }

        ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

        ctrl_set_related_id(p_ctrl,
                                0,                                     /* left */
                                (u8)((i - 1 +
                                      CA_BURSE_INFO_ITEM_CNT) %
                                     CA_BURSE_INFO_ITEM_CNT + IDC_STB_CARD_BURSE1),           /* up */
                                0,                                     /* right */
                                (u8)((i + 1) % CA_BURSE_INFO_ITEM_CNT + IDC_STB_CARD_BURSE1));/* down */

        y += CA_BURSE_INFO_ITEM_H + CA_BURSE_INFO_ITEM_V_GAP;
    }

    ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
    ui_ca_do_cmd(CAS_CMD_BURSE_INFO_GET, 0, 0);
    return SUCCESS;
}


BEGIN_KEYMAP(ca_burse_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ca_burse_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_burse_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_CA_BUR_INFO, on_ca_burse_update)
END_MSGPROC(ca_burse_cont_proc, ui_comm_root_proc)


