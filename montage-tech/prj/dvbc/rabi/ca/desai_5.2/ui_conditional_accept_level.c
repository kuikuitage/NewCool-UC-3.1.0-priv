/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_level.h"

enum control_id
{
    IDC_INVALID = 0,
    IDC_FRM_CA,
};

enum ca_level_control_id
{
    IDC_CA_LEVEL_CONTROL_PIN = 1,
    IDC_CA_LEVEL_CONTROL_LEVEL,
    IDC_CA_LEVEL_CHANGE_OK,
    IDC_CA_LEVEL_CHANGE_CANCEL,
    IDC_CA_LEVEL_CHANGE_RESULT,
    IDC_CA_LEVEL_CHANGE_RESULT2,
};

static cas_rating_set_t rate_info;

u16 conditional_accept_level_cont_keymap(u16 key);
RET_CODE conditional_accept_level_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_level_ca_frm_keymap(u16 key);
RET_CODE conditional_accept_level_ca_frm_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE conditional_accept_level_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 conditional_accept_level_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_level_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static u8 get_level_rating(u16 focus_num)
{
    u8 level;
    
    switch(focus_num)
    {
        case 0: level = 1; break;
        case 1: level = 2; break;
        case 2: level = 3;break;
        case 3: level = 4; break;
        case 4: level = 5;break;
        case 5: level = 6;break;
        case 6: level = 7;break;
        case 7: level = 8;break;
        case 8: level = 9;break;
        default:level = 1;break;
    }
    return level;
}

static void conditional_accept_level_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
    control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_CA);
    control_t *p_level = ctrl_get_child_by_id(p_frm, IDC_CA_LEVEL_CONTROL_LEVEL);
    u16 focus = 0;
    
    if(p_card_info == NULL)
    {
        ui_comm_select_set_focus(p_level, 0);
    }
    else
    {
        switch(p_card_info->card_work_level)
        {
            case 1: focus = 0;break;
            case 2: focus = 1;break;
            case 3: focus = 2;break;
            case 4: focus = 3;break;
            case 5: focus = 4;break;
            case 6: focus = 5;break;
            case 7: focus = 6;break;
            case 8: focus = 7;break;
            case 9: focus = 8;break;
            default: focus = 0;break;
        }
        DEBUG(MAIN,INFO,"***********card_work_level=%d******focus=%d********\n",p_card_info->card_work_level,focus);
    }

    ui_comm_select_set_focus(p_level, focus);
  }


RET_CODE open_conditional_accept_level(u32 para1, u32 para2)
{
    control_t *p_cont = NULL;
    control_t *p_ctrl = NULL;
    control_t *p_ca_frm;
    u8 i;
    u16 y;
    u16 stxt_level_control[CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT] =
    {
        IDS_CA_INPUT_PIN, IDS_CA_SELECT_LEVEL,
    };

    // create container
    p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
    if(p_cont == NULL)
    {
        return ERR_FAILURE;
    }
    ctrl_set_keymap(p_cont, conditional_accept_level_cont_keymap);
    ctrl_set_proc(p_cont, conditional_accept_level_cont_proc);

    //CA frm
    p_ca_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_FRM_CA,
                              CONDITIONAL_ACCEPT_LEVEL_CA_FRM_X, CONDITIONAL_ACCEPT_LEVEL_CA_FRM_Y,
                              CONDITIONAL_ACCEPT_LEVEL_CA_FRM_W, CONDITIONAL_ACCEPT_LEVEL_CA_FRM_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ca_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    ctrl_set_keymap(p_ca_frm, conditional_accept_level_ca_frm_keymap);
    ctrl_set_proc(p_ca_frm, conditional_accept_level_ca_frm_proc);

    //level control
    y = CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_Y;
    for (i = 0; i<CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT; i++)
    {
        switch(i)
        {
            case 0:
                p_ctrl = ui_comm_pwdedit_create(p_ca_frm, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X, y,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW);
                ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_level_control[i]);
                ui_comm_pwdedit_set_param(p_ctrl, 6);
                //ui_comm_pwdedit_set_mask(p_ctrl, '-', '*');
                ui_comm_ctrl_set_keymap(p_ctrl, conditional_accept_level_pwdedit_keymap);
                ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_level_pwdedit_proc);
                break;
            case 1:
                p_ctrl = ui_comm_select_create(p_ca_frm, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X, y+CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW);
                ui_comm_ctrl_set_ctrl_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
                ui_comm_ctrl_set_txt_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
                ui_comm_select_set_static_txt(p_ctrl, stxt_level_control[i]);
                ui_comm_select_set_param(p_ctrl, TRUE,
                                           CBOX_WORKMODE_STATIC, CONDITIONAL_ACCEPT_WATCH_LEVEL_TOTAL,
                                           CBOX_ITEM_STRTYPE_STRID,
                                           NULL);
                ui_comm_select_set_content(p_ctrl, 0, IDS_CA_LEVEL1);
                ui_comm_select_set_content(p_ctrl, 1, IDS_CA_LEVEL2);
                ui_comm_select_set_content(p_ctrl, 2, IDS_CA_LEVEL3);
                ui_comm_select_set_content(p_ctrl, 3, IDS_CA_LEVEL4);
                ui_comm_select_set_content(p_ctrl, 4, IDS_CA_LEVEL5);
                ui_comm_select_set_content(p_ctrl, 5, IDS_CA_LEVEL6);
                ui_comm_select_set_content(p_ctrl, 6, IDS_CA_LEVEL7);
                ui_comm_select_set_content(p_ctrl, 7, IDS_CA_LEVEL8);
                ui_comm_select_set_content(p_ctrl, 8, IDS_CA_LEVEL9);	
                break;
            case 2:
                p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_X,
                                          y+(CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP)*2+20,
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W,
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_H,
                                          p_ca_frm, 0);
                ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
                text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
                text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
                text_set_content_by_strid(p_ctrl, IDS_OK);
                break;
            case 3: 
                p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_X+CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W+170,
                                          y+(CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP)*2 + 20,
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W,
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_H,
                                          p_ca_frm, 0);
                ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
                text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
                text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
                text_set_content_by_strid(p_ctrl, IDS_CANCEL);
                break;
            default:
                p_ctrl = NULL;
            break;
        }
        
        if(NULL  == p_ctrl)
        {
            break;
        }
        
        if(i >= 0 && i < 2){
            ctrl_set_related_id(p_ctrl,
                            0,                                     /* left */
                            (u8)((i - 1 +
                                  CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT) %
                                 CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN),           /* up */
                            0,                                     /* right */
                            (u8)((i + 1) % CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN));/* down */
        }
        else{
            ctrl_set_related_id(p_ctrl,
                            (u8) ((i - 1) % 2 + IDC_CA_LEVEL_CHANGE_OK),                                     /* left */
                            (u8)((i - 1 +
                                  CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT) %
                                 CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN),           /* up */
                            (u8) ((i + 1) % 2 + IDC_CA_LEVEL_CHANGE_OK),                                     /* right */
                            (u8)((i + 1) % CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN));/* down */

        }
    }

  //change result
  
    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_LEVEL_CHANGE_RESULT,
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X,
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_Y,
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W,
                                          CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H,
                                          p_ca_frm, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
    text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
    ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_LEVEL_CHANGE_RESULT2,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_Y,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H,
                              p_ca_frm, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
    text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);   
    ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);

    #ifdef SPT_DUMP_DATA
    spt_dump_menu_data(p_cont);
    #endif

    ctrl_default_proc(ctrl_get_child_by_id(p_ca_frm, IDC_CA_LEVEL_CONTROL_PIN), MSG_GETFOCUS, 0, 0);

    ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
    #ifndef WIN32
    ui_ca_do_cmd((u32)CAS_CMD_CARD_INFO_GET, 0 ,0);
    #endif
  return SUCCESS;
}

static RET_CODE on_conditional_accept_level_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    conditional_accept_level_set_content(p_cont, (cas_card_info_t *)para2);
    ctrl_paint_ctrl(p_cont, TRUE);

    return SUCCESS;
}

static void get_pin_error_str(u16 * uni_str, u16 str_id1,u16 str_id2, u32 content)
{
  u16 str[30] = {0}; 
  u16 len = 0;
  
  gui_get_string(str_id1, uni_str, 64);
  convert_i_to_dec_str(str, (s32)content); 
  uni_strcat(uni_str, str,64);
  
  len = (u16)uni_strlen(uni_str);
  gui_get_string(str_id2, str, 64 - len); 
  uni_strcat(uni_str, str, 64); 

  return;
}

static RET_CODE on_conditional_accept_level_set(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_result = NULL, *p_result2 = NULL,*p_frm = NULL;
    cas_rating_set_t *ca_pin_info = NULL;
    ca_pin_info = (cas_rating_set_t *)para2;
    u16 uni_num[64];
    u16 pin_spare_num;
    
    p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_CA);
    p_result = ctrl_get_child_by_id(p_frm, IDC_CA_LEVEL_CHANGE_RESULT);
    p_result2 = ctrl_get_child_by_id(p_frm, IDC_CA_LEVEL_CHANGE_RESULT2);
    DEBUG(MAIN,INFO,"***********para1=%d******pin_spare_num = %d********\n",para1,ca_pin_info->pin_spare_num);

  if(7 == para1)
  {
    ctrl_set_sts(p_result2,OBJ_STS_SHOW);
    ctrl_set_sts(p_result,OBJ_STS_HIDE);
  }
  else
  {
    ctrl_set_sts(p_result,OBJ_STS_SHOW);
    ctrl_set_sts(p_result2,OBJ_STS_HIDE);
  }
  if(para1 == 2)
  {
    text_set_content_by_strid(p_result, IDS_CA_SET_SUCCESS);
  }
  else if(para1 == 18)
  {
    text_set_content_by_strid(p_result, IDS_CA_PARAME_SET_ERROR);
  }
  else if(para1 == 1)
  {
    text_set_content_by_strid(p_result, IDS_CA_IC_CMD_FAIL);
  }
  else if(para1 == 9)
  {
    text_set_content_by_strid(p_result, IDS_DS_CA_CARD_LOCK);
  }
  else if(para1 == 7)
  {
    pin_spare_num = (u16)ca_pin_info->pin_spare_num;
    get_pin_error_str(uni_num, IDS_CA_PASSWORD_ERROR, IDS_CA_PIN_CHANCES_LEFT, pin_spare_num);
    text_set_content_by_unistr(p_result2,uni_num);
  }
  else
  {
    text_set_content_by_strid(p_result, IDS_CA_PASSWORD_ERROR);
  }
  ctrl_paint_ctrl(p_result,TRUE);
  ctrl_paint_ctrl(p_result2,TRUE);

  return SUCCESS;
}


static RET_CODE on_conditional_accept_level_pwdedit_maxtext(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
    control_t *p_cont;
    control_t *p_active;

    p_active = ctrl_get_parent(p_ctrl);
    p_cont = ctrl_get_parent(p_active);
    ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);

    return SUCCESS;
}

static RET_CODE on_conditional_accept_level_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_pin,*p_result,*p_child, *p_level = NULL;
    u32 card_pin = 0;
    u16 focus_num = 0;
    BOOL card_pin_is_full = FALSE;
    u8 ctrl_id;
    p_pin = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_PIN);
    p_level = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CONTROL_LEVEL);
    p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_LEVEL_CHANGE_RESULT);
    card_pin = ui_comm_pwdedit_get_realValue(p_pin);
    card_pin_is_full = ui_comm_pwdedit_is_full(p_pin);

    p_child = ctrl_get_active_ctrl(p_ctrl);
    ctrl_id = ctrl_get_ctrl_id(p_child);
    
    if(ctrl_id == IDC_CA_LEVEL_CHANGE_CANCEL){
        manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 0, 0);
    }
    else if(ctrl_id == IDC_CA_LEVEL_CHANGE_OK)
    {
        rate_info.pin[0]=(u8)((card_pin & 0x00ff0000)>>16);
        rate_info.pin[1]=(u8)((card_pin & 0x0000ff00)>>8);
        rate_info.pin[2]=(u8)(card_pin & 0x000000ff);

        focus_num = ui_comm_select_get_focus(p_level);
        rate_info.rate = get_level_rating(focus_num);
        DEBUG(MAIN,INFO,"[ca_accept_level]rate_info.rate= %d \n",rate_info.rate);
        if(card_pin_is_full)
        {
            #ifndef WIN32
            ui_ca_do_cmd((u32)CAS_CMD_RATING_SET, (u32)&rate_info ,0);
            #endif
        }
        else
        {
            text_set_content_by_strid(p_result, IDS_CA_CARD_PIN_INVALID);
            ctrl_paint_ctrl(p_result, TRUE); 
        }
        ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
        ctrl_paint_ctrl(p_pin, TRUE);
    }
	return SUCCESS;
}



BEGIN_KEYMAP(conditional_accept_level_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(conditional_accept_level_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_level_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_level_update)
    ON_COMMAND(MSG_CA_RATING_SET, on_conditional_accept_level_set)
END_MSGPROC(conditional_accept_level_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_level_ca_frm_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_level_ca_frm_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_level_ca_frm_proc, cont_class_proc)
    ON_COMMAND(MSG_SELECT, on_conditional_accept_level_ca_frm_ok)
END_MSGPROC(conditional_accept_level_ca_frm_proc, cont_class_proc)

BEGIN_KEYMAP(conditional_accept_level_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_level_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_level_pwdedit_proc, ui_comm_edit_proc)
    ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_level_pwdedit_maxtext)
END_MSGPROC(conditional_accept_level_pwdedit_proc, ui_comm_edit_proc)



