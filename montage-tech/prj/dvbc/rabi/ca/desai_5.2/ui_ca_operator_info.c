/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_ca_operator_info.h"
#include "ui_ca_public.h"

enum ca_ca_info_id
{
    IDC_CA_CARD_SP_ID = 1,
    IDC_CA_CARD_SP_NAME,
    IDC_CA_CARD_SP_TEL,
    IDC_CA_CARD_SP_ADDR,
    IDC_CA_CARD_SP_BUTTON,
};


u16 operator_info_cont_keymap(u16 key);
RET_CODE operator_info_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 operator_info_button_keymap(u16 key);
RET_CODE operator_info_button_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

static void operator_info_get_ascstr(u8 * asc_str, u8 * str_addr)
{
    sprintf((char *)asc_str, "%s", str_addr);
}

static void operator_info_set_content(control_t *p_cont, cas_operators_info_t *p_card_info)
{
    u8 asc_str[OPERATOR_INFO_CA_INFO_MAX_LENGTH];
    u16 uni_str[OPERATOR_INFO_CA_INFO_MAX_LENGTH + 1] = {0};
    control_t *p_card_sp_id = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_SP_ID);
    control_t *p_card_name = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_SP_NAME);
    control_t *p_card_sp_tel = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_SP_TEL);
    control_t *p_card_sp_addr = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_SP_ADDR);

    if(p_card_info == NULL)
    {
        ui_comm_static_set_content_by_ascstr(p_card_sp_id, (u8 *)" ");
        ui_comm_static_set_content_by_ascstr(p_card_name,(u8 *) " ");
        ui_comm_static_set_content_by_ascstr(p_card_sp_tel, (u8 *)" ");
        ui_comm_static_set_content_by_ascstr(p_card_sp_addr, (u8 *)" ");
    }
    else
    {
    //sp_id
    operator_info_get_ascstr(asc_str, p_card_info->p_operator_info[0].operator_id_ascii);
    ui_comm_static_set_content_by_ascstr(p_card_sp_id, p_card_info->p_operator_info[0].operator_id_ascii);

    //SP-name
    //operator_info_get_ascstr(asc_str, p_card_info->p_operator_info[0].operator_name);
    gb2312_to_unicode(p_card_info->p_operator_info[0].operator_name, 
                                        OPERATOR_INFO_CA_INFO_MAX_LENGTH, 
                                        uni_str, OPERATOR_INFO_CA_INFO_MAX_LENGTH);
    ui_comm_static_set_content_by_unistr(p_card_name, uni_str);

    //sp_tel
    //operator_info_get_ascstr(asc_str, p_card_info->p_operator_info[0].operator_phone);
    //OS_PRINTF("operator_phone=%s\n",asc_str);
    gb2312_to_unicode(p_card_info->p_operator_info[0].operator_phone, 
                                    OPERATOR_INFO_CA_INFO_MAX_LENGTH, 
                                    uni_str, OPERATOR_INFO_CA_INFO_MAX_LENGTH);
    //ui_comm_static_set_content_by_ascstr(p_card_sp_tel, asc_str);
    ui_comm_static_set_content_by_unistr(p_card_sp_tel, uni_str);

    //sp_addr
    //operator_info_get_ascstr(asc_str, p_card_info->p_operator_info[0].operator_address);
    gb2312_to_unicode(p_card_info->p_operator_info[0].operator_address, 
                                        OPERATOR_INFO_CA_INFO_MAX_LENGTH, 
                                        uni_str, OPERATOR_INFO_CA_INFO_MAX_LENGTH);
    ui_comm_static_set_content_by_unistr(p_card_sp_addr, uni_str);
    }
}

RET_CODE open_ca_operator_info(u32 para1, u32 para2)
{
    control_t *p_cont, *p_ctrl = NULL;
    u8 i;
    u16 y;
    u16 stxt_ca_info[OPERATOR_INFO_ITEM_CNT] =
    { 
        IDS_CA_SP_ID, IDS_CA_SP_NAME, IDS_CA_OPERATOR_TEL,IDS_CA_OPERATOR_ADDRES
    };
  
    p_cont = ui_comm_right_root_create(ROOT_ID_CA_OPERATOR_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
    if (p_cont == NULL)
    {
        return ERR_FAILURE;
    }
    ctrl_set_keymap(p_cont, operator_info_cont_keymap);
    ctrl_set_proc(p_cont, operator_info_cont_proc);

    //CA info
    y = OPERATOR_INFO_ITEM_Y;
    for (i = 0; i<OPERATOR_INFO_ITEM_CNT; i++)
    {
        p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_CARD_SP_ID + i),
                                               OPERATOR_INFO_ITEM_X, y,
                                               OPERATOR_INFO_ITEM_LW,
                                               OPERATOR_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, stxt_ca_info[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

        ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_COMM_CONT_SH, RSI_COMM_CONT_SH, RSI_COMM_CONT_SH);

        ctrl_set_related_id(p_ctrl,
                            0,                                     /* left */
                            (u8)((i - 1 +
                                  OPERATOR_INFO_ITEM_CNT) %
                                 OPERATOR_INFO_ITEM_CNT + IDC_CA_CARD_SP_ID),           /* up */
                            0,                                     /* right */
                            (u8)((i + 1) % OPERATOR_INFO_ITEM_CNT + IDC_CA_CARD_SP_ID));/* down */

        y += OPERATOR_INFO_ITEM_H + ACCEPT_CA_OPER_INFO_ITEM_V_GAP;
    }

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_CA_CARD_SP_BUTTON, OPERATOR_INFO_ITEM_X, y,
    						(OPERATOR_INFO_ITEM_LW + OPERATOR_INFO_ITEM_RW), OPERATOR_INFO_ITEM_H,p_cont, 0);
    ctrl_set_keymap(p_ctrl, operator_info_button_keymap);
    ctrl_set_proc(p_ctrl, operator_info_button_proc);
    ctrl_set_rstyle(p_ctrl,RSI_COMM_CONT_SH,RSI_SELECT_F,RSI_COMM_CONT_GRAY);
    text_set_font_style(p_ctrl,FSI_COMM_TXT_SH,FSI_COMM_TXT_HL,FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_offset(p_ctrl, COMM_CTRL_OX, 0);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl, IDS_CA_ACCREDIT_INFO);
    
    ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);  
    ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

    #ifndef WIN32
    ui_ca_do_cmd((u32)CAS_CMD_OPERATOR_INFO_GET, 0 ,0);
    #else
    on_operator_info_update(p_cont, 0, 0, para1);
    #endif

    return SUCCESS;
}

static RET_CODE on_operator_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    operator_info_set_content(p_cont, (cas_operators_info_t *)para2);
    ctrl_paint_ctrl(p_cont, TRUE);

    return SUCCESS;
}

static RET_CODE on_operator_info_inter_entitle(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    manage_open_menu(ROOT_ID_CA_ENTITLE_INFO, 0, 0);
    return SUCCESS;
}

BEGIN_KEYMAP(operator_info_button_keymap, NULL)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(operator_info_button_keymap, NULL)

BEGIN_MSGPROC(operator_info_button_proc, text_class_proc)
    ON_COMMAND(MSG_SELECT, on_operator_info_inter_entitle)
END_MSGPROC(operator_info_button_proc, text_class_proc)

BEGIN_KEYMAP(operator_info_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(operator_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(operator_info_cont_proc, ui_comm_root_proc)
    //ON_COMMAND(MSG_FOCUS_UP, on_operator_cont_focus_change)
    //ON_COMMAND(MSG_FOCUS_DOWN, on_operator_cont_focus_change)
    ON_COMMAND(MSG_CA_OPE_INFO, on_operator_info_update)
END_MSGPROC(operator_info_cont_proc, ui_comm_root_proc)


