/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_worktime.h"
#include "ui_comm_root.h"


enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_WORKTIME,
  IDC_DATE,
  IDC_TIME,
};

enum ca_work_duration_id
{
  IDC_CA_WORK_DURATION_PIN = 1,
  IDC_CA_WORK_DURATION_START_TIME,
  IDC_CA_WORK_DURATION_END_TIME,
  IDC_CA_WORK_DURATION_OK,
  IDC_CA_WORK_DURATION_CANCEL,
  IDC_CA_WORK_DURATION_CHANGE_RESULT,
};

static cas_card_work_time_t card_word_time;

u16 conditional_accept_worktime_cont_keymap(u16 key);
RET_CODE conditional_accept_worktime_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_worktime_ca_frm_keymap(u16 key);
RET_CODE conditional_accept_worktime_ca_frm_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
u16 conditional_accept_worktime_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_worktime_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE conditional_accept_worktime_timedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void conditional_accept_worktime_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_WORKTIME);
  control_t *p_start = ctrl_get_child_by_id(p_frm, IDC_CA_WORK_DURATION_START_TIME);
  control_t *p_end = ctrl_get_child_by_id(p_frm, IDC_CA_WORK_DURATION_END_TIME);
  utc_time_t time = {0};
  
  if(p_card_info == NULL)
  {
    ui_comm_timedit_set_time(p_start, &time);
    ui_comm_timedit_set_time(p_end, &time);
  }
  else
  {
    time.hour = p_card_info->work_time.start_hour;
    time.minute = p_card_info->work_time.start_minute;
   // time.second = p_card_info->work_time.start_second;
    ui_comm_timedit_set_time(p_start, &time);

    time.hour = p_card_info->end_time.end_hour;
    time.minute = p_card_info->end_time.end_minute;
   // time.second = p_card_info->end_time.end_second;
    ui_comm_timedit_set_time(p_end, &time);
  }
}
 
RET_CODE open_cas_accept_worktime(u32 para1, u32 para2)
{
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT	5
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_HELP_RSC_CNT	15
  //CA frame
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_X       106
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_Y       24
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_W       505
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_H       420

  //work duration items
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_X        10
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_Y        20
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_LW       260
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_RW       140
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_H        35
  
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_X        250
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_Y        160
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_W        160
  #define SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_H        COMM_CTRL_H

  control_t *p_cont = NULL;
  control_t *p_ctrl = NULL;
  control_t *p_ca_frm;
  u8 i;
  u16 y;
  u16 stxt_work[SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT] =
  { 
  	IDS_CA_INPUT_PIN, IDS_START_TIME4, IDS_END_TIME4,
  };

  // create container
  p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_worktime_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_worktime_cont_proc);

  //CA frm
  p_ca_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_FRM_WORKTIME,
                              0, 0,
                              RIGHT_ROOT_W, RIGHT_ROOT_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ca_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_ca_frm, conditional_accept_worktime_ca_frm_keymap);
  ctrl_set_proc(p_ca_frm, conditional_accept_worktime_ca_frm_proc);

  //work duration
  y = COMM_ITEM_OY_IN_ROOT/2;
  for (i = 0; i<SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl = ui_comm_pwdedit_create(p_ca_frm, (u8)(IDC_CA_WORK_DURATION_PIN + i),
                                           COMM_ITEM_OX_IN_ROOT/2, y,
                                           COMM_ITEM_LW,
                                           RIGHT_ROOT_W - COMM_ITEM_LW - COMM_ITEM_OX_IN_ROOT);
        ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_work[i]);
        ui_comm_pwdedit_set_param(p_ctrl, 6);
        ui_comm_ctrl_set_keymap(p_ctrl,conditional_accept_worktime_pwdedit_keymap);
        ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_worktime_pwdedit_proc);
        break;

      case 1:
      case 2:
        p_ctrl = ui_comm_timedit_create(p_ca_frm, (u8)(IDC_CA_WORK_DURATION_PIN + i),
                                           COMM_ITEM_OX_IN_ROOT/2, y+(SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_H + CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_V_GAP+20)*i,
                                           COMM_ITEM_LW,
                                           RIGHT_ROOT_W - COMM_ITEM_LW - COMM_ITEM_OX_IN_ROOT);
        ui_comm_timedit_set_static_txt(p_ctrl, stxt_work[i]);
        ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_worktime_timedit_proc);
        ui_comm_timedit_set_param(p_ctrl, 0, TBOX_ITEM_HOUR, TBOX_HOUR| TBOX_MIN, 
                                        TBOX_SEPARATOR_TYPE_UNICODE, 18);
        ui_comm_timedit_set_separator_by_ascchar(p_ctrl, TBOX_ITEM_HOUR, ':');
        //ui_comm_timedit_set_separator_by_ascchar(p_ctrl, TBOX_ITEM_MIN, ':');
        break; 
     case 3:
        p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_WORK_DURATION_PIN + i),
                            SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_X, 
                            SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_Y+120,
                            SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_W, 
                            SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_H,
                            p_ca_frm, 0);
         ctrl_set_rstyle(p_ctrl,RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
        text_set_font_style(p_ctrl,FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, IDS_OK);  
        break;
	 case 4:
		p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_WORK_DURATION_PIN + i),
                            SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_X+SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_W*2, 
                            SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_Y+120,
                            SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_W, 
                            SN_CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_H,
                            p_ca_frm, 0);
        ctrl_set_rstyle(p_ctrl,RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
        text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, IDS_CANCEL);
		break;
      default:
        p_ctrl = NULL;
        break;
    }
    if(NULL == p_ctrl)
    {
        break;
    }
   if(i >= 0 && i < 3){
    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT) %
                             SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT + IDC_CA_WORK_DURATION_PIN),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT + IDC_CA_WORK_DURATION_PIN));/* down */
	
   	}
	else
	{
		ctrl_set_related_id(p_ctrl,
                       (u8) ((i - 2) % 2 + IDC_CA_WORK_DURATION_OK),                                     /* left */
                        (u8)((i - 1 +
                              SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT) %
                             SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT + IDC_CA_WORK_DURATION_PIN),           /* up */
                        (u8) ((i + 2) % 2 + IDC_CA_WORK_DURATION_OK),                                     /* right */
                        (u8)((i + 1) % SN_CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT + IDC_CA_WORK_DURATION_PIN));/* down */ 
	}
 
  }

  //change result
  
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_WORK_DURATION_CHANGE_RESULT,
                              CONDITIONAL_ACCEPT_WORKTIME_RESULT_X, 
                              CONDITIONAL_ACCEPT_WORKTIME_RESULT_Y,
                              CONDITIONAL_ACCEPT_WORKTIME_RESULT_W, 
                              CONDITIONAL_ACCEPT_WORKTIME_RESULT_H,
                              p_ca_frm, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_ctrl, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  ctrl_set_attr(p_ctrl,OBJ_STS_HIDE);

  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  conditional_accept_worktime_set_content(p_cont, (cas_card_info_t *)para1);
  
  ctrl_default_proc(ctrl_get_child_by_id(p_ca_frm, IDC_CA_WORK_DURATION_PIN), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifndef WIN32
//  ui_ca_get_info(CAS_CMD_CARD_INFO_GET, 0 ,0);
  ui_ca_do_cmd((u32)CAS_CMD_CARD_INFO_GET, 0 ,0);
  #endif

  return SUCCESS;
}

static RET_CODE on_conditional_accept_worktime_pwdedit_maxtext(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_active;
  u8 ctrl_id;

  p_active = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_active);

  ctrl_id = ctrl_get_ctrl_id(p_active);

  ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);

  return SUCCESS;
}

static RET_CODE on_conditional_accept_worktime_timedit_unselect(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_date, *p_time;
  utc_time_t loc_time = {0};
  utc_time_t gmt_time = {0};

  p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
  p_date = ctrl_get_child_by_id(p_cont, IDC_DATE);
  p_time = ctrl_get_child_by_id(p_cont, IDC_TIME);

  ui_comm_timedit_get_time(p_date, &loc_time);
  ui_comm_timedit_get_time(p_time, &loc_time);

  time_to_gmt(&loc_time, &gmt_time);

  time_set(&gmt_time);
  
  return ERR_NOFEATURE;
}  
static BOOL is_conditional_accept_worktime_valid(utc_time_t p_star_time,utc_time_t p_end_time)
{
  u32 start_time = 0;
  u32 end_time = 0;

  start_time = (u32)((p_star_time.hour<<16) | (p_star_time.minute<<8)); //|(p_star_time.second)
  end_time = (u32)((p_end_time.hour<<16) | (p_end_time.minute<<8) );//| (p_end_time.second)
  
  UI_PRINTF("****** print ca worktime: start time= [%d : %d ], \
                        end time = [%d : %d ], \
                        start_time value = 0x%x, end_time value = 0x%x \n",
                      p_star_time.hour,p_star_time.minute,//p_star_time.second,
                      p_end_time.hour,p_end_time.minute,//p_end_time.second,
                      start_time,end_time);

  if(start_time >= end_time)
    return FALSE;
  else
    return TRUE;
}

static dlg_ret_t  worktime_show_dlg(u32 content, u8 style, u32 dlg_tmout)
{
    comm_dlg_data_t dlg_data;

    dlg_data.parent_root = ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME;
    dlg_data.x = (RIGHT_ROOT_W - PWDLG_W)/2 + 300;
    dlg_data.y = SYS_DLG_FOR_CHK_Y;
    dlg_data.w = PWDLG_W;
    dlg_data.h = PWDLG_H;
    dlg_data.content = content;
    dlg_data.style = style;
    dlg_data.dlg_tmout = dlg_tmout;

    return ui_comm_dlg_open(&dlg_data);
}

static RET_CODE on_conditional_accept_worktime_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_pin, *p_start, *p_end,*p_result,*p_child;
    utc_time_t p_star_time, p_end_time ;
    u32 card_pin = 0;
    BOOL card_pin_is_full = FALSE;
    //dlg_ret_t ret=FALSE;
    u8 ctrl_id;

    p_pin = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_PIN);
    p_start = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_START_TIME);
    p_end = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_END_TIME);
    p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_WORK_DURATION_CHANGE_RESULT);
    card_pin = ui_comm_pwdedit_get_realValue(p_pin);
    card_pin_is_full = ui_comm_pwdedit_is_full(p_pin);

    p_child = ctrl_get_active_ctrl(p_ctrl);
    ctrl_id = ctrl_get_ctrl_id(p_child);
    if(ctrl_id == IDC_CA_WORK_DURATION_CANCEL)
    {
        manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, 0, 0);
    }
    else if(ctrl_id == IDC_CA_WORK_DURATION_OK)
    {
        {
            card_word_time.pin[0]=(u8)(card_pin/100000);
            card_word_time.pin[1]=(u8)((card_pin/10000)%10);
            card_word_time.pin[2]=(u8)(((card_pin/1000)%100)%10);
            card_word_time.pin[3]=(u8)((((card_pin/100)%1000)%100)%10);
            card_word_time.pin[4]=(u8)(((((card_pin/10)%10000)%1000)%100)%10);
            card_word_time.pin[5]=(u8)(((((card_pin%100000)%10000)%1000)%100)%10);

            ui_comm_timedit_get_time(p_start,&p_star_time);
            ui_comm_timedit_get_time(p_end,&p_end_time);

            if(FALSE == is_conditional_accept_worktime_valid(p_star_time,p_end_time))
            {
                ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
                ctrl_paint_ctrl(p_pin, TRUE);
                worktime_show_dlg(IDS_CA_WORK_TIME_ERROR, DLG_FOR_SHOW | DLG_STR_MODE_STATIC,1000);
                return ERR_FAILURE;
            }

            card_word_time.start_hour = p_star_time.hour;
            card_word_time.start_minute= p_star_time.minute;
            card_word_time.end_hour= p_end_time.hour;
            card_word_time.end_minute= p_end_time.minute;
#ifndef WIN32
            if(card_pin_is_full)
            {
            	ui_ca_do_cmd((u32)CAS_CMD_WORK_TIME_SET, (u32)&card_word_time ,0);
            }
            else
            {//PIN invalid
                    worktime_show_dlg(IDS_CA_CARD_PIN_INVALID, DLG_FOR_SHOW | DLG_STR_MODE_STATIC,1000);
            }
#endif
            ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);
            ctrl_paint_ctrl(p_pin, TRUE);

        }
    }
  return SUCCESS;
}

static RET_CODE on_conditional_set_worktime_right(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_frm, *p_result;
  cas_card_work_time_t *ca_pin_info = NULL;
  ca_pin_info = (cas_card_work_time_t *)para2;
  p_frm = ctrl_get_child_by_id(p_cont, IDC_FRM_WORKTIME);
  p_result = ctrl_get_child_by_id(p_frm, IDC_CA_WORK_DURATION_CHANGE_RESULT);

  if(para1 == CAS_SUCESS)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_SUCESS[%d] \n",para1, CAS_SUCESS);
        text_set_content_by_strid(p_result, IDS_CA_SET_SUCCESS);
  }
  else if(para1 == CAS_ERR_INVALID_CARD)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_INVALID_CARD[%d] \n",para1, CAS_ERR_INVALID_CARD);
        text_set_content_by_strid(p_result, IDS_CA_CARD_INVALID);
  }
  else if(para1 == CAS_ERR_NOCARD)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_NOCARD[%d] \n",para1, CAS_ERR_NOCARD);
        text_set_content_by_strid(p_result, IDS_CA_CARD_NOT_INSERT);
  }
  else if(para1 == CAS_ERR_INVALID_ECMPID)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_INVALID_ECMPID[%d] \n",para1, CAS_ERR_INVALID_ECMPID);
        text_set_content_by_strid(p_result, IDS_CA_INVALID_ECM_PID);
  }
  else if(para1 == CAS_ERR_INVALID_EMMPID) 
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_INVALID_EMMPID[%d] \n",para1, CAS_ERR_INVALID_EMMPID);
        text_set_content_by_strid(p_result, IDS_CA_INVALID_EMM_PID);
  }
  else if(para1 == CAS_ERR_NOENTITLE)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_NOENTITLE[%d] \n",para1, CAS_ERR_NOENTITLE);
        text_set_content_by_strid(p_result, IDS_CA_NOT_ENTITLE);
  }
  else if(para1 == CAS_ERR_STB_PAIR_FAILED)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_STB_PAIR_FAILED[%d] \n",para1, CAS_ERR_STB_PAIR_FAILED);
        text_set_content_by_strid(p_result, IDS_CA_STB_PAIR_FAILED);
  }
  else if(para1 == CAS_FAILED)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_FAILED[%d] \n",para1, CAS_FAILED);
        text_set_content_by_strid(p_result, IDS_CAS_E_UNKNOWN_ERROR);
  }
  else if(para1 == CAS_ERR_INIT_ERROR)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_INIT_ERROR[%d] \n",para1, CAS_ERR_INIT_ERROR);
        text_set_content_by_strid(p_result, IDS_CA_CARD_INIT_FAIL);
  }
  else if(para1 == CAS_ERR_CARD_VER_ERROR)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_CARD_VER_ERROR[%d] \n",para1, CAS_ERR_CARD_VER_ERROR);
        text_set_content_by_strid(p_result, IDS_CA_CARD_ERR_VERSION);
  }
  else if(para1 == CAS_ERR_PIN_ERROR)
  {
        OS_PRINTF("[debug] para1[%d]  CAS_ERR_PIN_ERROR[%d] \n",para1, CAS_ERR_PIN_ERROR);
        text_set_content_by_strid(p_result, IDS_CA_PASSWORD_PIN_ERROR);
  }
  else
  {
        OS_PRINTF("[debug] para1[%d] \n",para1);
        text_set_content_by_strid(p_result, IDS_CA_PASSWORD_PIN_ERROR);
  }

  ctrl_set_sts(p_result,OBJ_STS_SHOW);
  ctrl_paint_ctrl(p_result, TRUE); 
  return SUCCESS;
}
static RET_CODE on_conditional_accept_worktime_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_worktime_set_content(p_cont, (cas_card_info_t *)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

BEGIN_KEYMAP(conditional_accept_worktime_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(conditional_accept_worktime_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_worktime_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_worktime_update)
  ON_COMMAND(MSG_CA_WORK_TIME_SET, on_conditional_set_worktime_right)
END_MSGPROC(conditional_accept_worktime_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_worktime_ca_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)  
END_KEYMAP(conditional_accept_worktime_ca_frm_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_worktime_ca_frm_proc, ui_comm_root_proc) 
  ON_COMMAND(MSG_SELECT, on_conditional_accept_worktime_ca_frm_ok)  
END_MSGPROC(conditional_accept_worktime_ca_frm_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_worktime_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_worktime_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_worktime_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_worktime_pwdedit_maxtext)
END_MSGPROC(conditional_accept_worktime_pwdedit_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(conditional_accept_worktime_timedit_proc, ui_comm_time_proc)
  ON_COMMAND(MSG_UNSELECT, on_conditional_accept_worktime_timedit_unselect) 
END_MSGPROC(conditional_accept_worktime_timedit_proc, ui_comm_time_proc)


