/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_do_search.h"

#include "ui_ca_finger.h"
#include "ui_ca_public.h"
#include "cas_manager.h"
#include "ui_new_mail.h"
#include "config_cas.h"
#include "cas_ware.h"

#define MAX_ON_SCREEN_MSG_LENGTH 320

static u32 g_ca_last_msg = CAS_S_CLEAR_DISPLAY;
static BOOL g_is_smart_card_insert = FALSE;

static cas_sid_t g_ca_sid;
static BOOL g_ca_check_start = FALSE;
static BOOL g_is_smart_card_status = FALSE;
static BOOL g_is_finger_show = FALSE;
static finger_msg_t g_ca_finger_msg = {{0,},0,};
static BOOL g_osd_stop_msg_unsend = FALSE;
static BOOL g_is_force_channel = FALSE;
static BOOL g_ca_msg_rolling_over = TRUE;
static u8 g_is_roll_bottom_show = 0;
static u8 g_is_roll_top_show = 0;
extern rsc_rstyle_t c_tab[MAX_RSTYLE_CNT];
extern rsc_fstyle_t f_tab[FSTYLE_CNT];

enum background_menu_ctrl_id
{
  /* value 1 for TITLE_ROLL*/
  IDC_BG_MENU_CA_ROLL_TOP = 2,
  IDC_BG_MENU_CA_ROLL_BOTTOM = 3,
};

extern void ui_desktop_start(void);
RET_CODE ui_ca_rolling_proc(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);

u32 ui_get_ca_last_msg(void)
{
    return g_ca_last_msg;
}

void ui_reset_ca_last_msg(void)
{
    g_ca_last_msg = 0;
}

BOOL ui_set_smart_card_insert(BOOL status)
{
    g_is_smart_card_insert = status;
    return 0;
}

BOOL ui_is_smart_card_insert(void)
{
  return g_is_smart_card_insert;
}

void ui_set_smart_card_state(BOOL status)
{
	g_is_smart_card_status = status;
}

BOOL ui_get_smart_card_state(void)
{
	return g_is_smart_card_status;
}

void ui_ca_set_sid(cas_sid_t *p_ca_sid)
{
	cmd_t cmd = {CAS_CMD_PLAY_INFO_SET_ANSYNC};

	DEBUG(MAIN,INFO,"CA: set sid pgid: %d\n", p_ca_sid->pgid);
	memcpy(&g_ca_sid, p_ca_sid, sizeof(cas_sid_t));
	cmd.data1 = (u32)(&g_ca_sid);
	ap_frm_do_command(APP_CA, &cmd);
}

static void ui_release_ap(void)
{
  ui_time_release();
  ui_epg_stop();
  ui_epg_release();
  ui_release_signal();
  ui_enable_uio(FALSE);
}

static void ui_activate_ap(void)
{
  ui_time_init();
  ui_epg_init();
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
  ui_init_signal();
  ui_enable_uio(TRUE);
}

void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2)
{
	cmd_t cmd = {0};
	dvbc_lock_t tp_set = {0};
	static nim_channel_info_t nim_channel_info = {0};

	sys_status_get_main_tp1(&tp_set);
	nim_channel_info.frequency = tp_set.tp_freq;
	nim_channel_info.param.dvbc.symbol_rate = tp_set.tp_sym;
	nim_channel_info.param.dvbc.modulation = tp_set.nim_modulate;

	ui_release_ap();
	g_ca_check_start = TRUE;

	cmd.id = cmd_id;
	cmd.data1 = (u32)(&nim_channel_info);
	cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_GS);
	DEBUG(MAIN,INFO,"\r\n***ui_ca_zone_check_start cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",cmd.id,cmd.data1,cmd.data2);
	ap_frm_do_command(APP_CA, &cmd);
}

void on_ca_zone_check_end(void)
{
	/*restart the ap task*/
	ui_activate_ap();
	g_ca_check_start = FALSE;
	ui_desktop_start();
}

void on_ca_zone_check_stop(void)
{
    /**no zone check funtion,it will set msg self**/
    /*todo: need dsca zone check end msg to active ap*/
#if 0
    control_t *p_ctrl = NULL;
    p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
    if(NULL != p_ctrl)
    {
        /*simulator msg :ca zone check end*/
        ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_ZONE_CHECK_END);
    }
#endif
}

RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	u32 event_id = para2;

	DEBUG(MAIN,INFO,"\r\n*** ca_message_update  g_ca_last_msg[0x%x],event_id[0x%x]\r\n",g_ca_last_msg,event_id);
	if(g_ca_last_msg == event_id) //same msg
	{
		return SUCCESS;
	}

	if(event_id != CAS_S_ZONE_CHECK_END &&
			event_id != CAS_C_IEXIGENT_PROG_STOP &&
			event_id != CAS_C_IEXIGENT_PROG)
	{
		g_ca_last_msg = event_id;
	}
	
	switch(event_id)
	{
		case CAS_S_ZONE_CHECK_END:
			DEBUG(MAIN,INFO,"\r\n*** %s :CAS_S_ZONE_CHECK_END ",__FUNCTION__);
			on_ca_zone_check_end();
			break;
		case CAS_C_SHOW_NEW_EMAIL:
			DEBUG(MAIN,INFO,"@@@CAS_C_SHOW_NEW_EMAIL @@@ \n");
			if(ui_is_fullscreen_menu( fw_get_focus_id()))
			{
				ui_set_new_mail(TRUE);
				update_email_picture(EMAIL_PIC_SHOW);
			}
			break;
		case CAS_C_HIDE_NEW_EMAIL:
			DEBUG(MAIN,INFO,"@@@CAS_C_HIDE_NEW_EMAIL @@@ \n");
			ui_set_new_mail(FALSE);
			update_email_picture(EMAIL_PIC_HIDE);
			break;
		case CAS_C_EMAIL_FULL:
			DEBUG(MAIN,INFO,"@@@CAS_C_SHOW_SPACE_EXHAUST @@@ \n");
			if(ui_is_fullscreen_menu( fw_get_focus_id()))
			{
				update_email_picture(EMAIL_PIC_FLASH);
			}
			break;
		case CAS_S_ADPT_CARD_REMOVE:
			DEBUG(CAS, ERR, "加密频道, 请插入智能卡\n");
			ui_set_smart_card_state(FALSE);
			update_ca_message(IDS_CRYPTED_PROG_INSERT_SMC);
			break;
		case CAS_S_ADPT_CARD_INSERT:
			g_ca_last_msg = CAS_S_CLEAR_DISPLAY;
			ui_set_smart_card_state(TRUE);
			update_ca_message(RSC_INVALID_ID);
			break;
		case CAS_S_CLEAR_DISPLAY:
			ui_set_playpg_scrambled(FALSE);
			update_ca_message(RSC_INVALID_ID);
			break;

        case CAS_E_SW1SW2_MEMORY_ERROR:
            DEBUG(MAIN,INFO,"\n智能卡内存出错\n");
            update_ca_message(IDS_QZ_CARD_MEMORY_ERROR);
            break;
			
        case CAS_E_SW1SW2_WRONG_LC_LENGTH:
		case CAS_E_SW1SW2_CLA_NOT_SUPPORTED:
		case CAS_E_SW1SW2_INS_NOT_SUPPORTED:
		case CAS_E_SW1SW2_WRONG_LE_LENGTH:
		case CAS_E_SW1SW2_INCORRECT_PARA:
        case CAS_E_SW1SW2_INCORRECT_DATA:
            DEBUG(MAIN,INFO,"\nAPDU 数据错误\n");
            update_ca_message(IDS_CA_IC_CMD_FAIL);
            break;
        case CAS_E_UNKNOW_ERR:
            DEBUG(MAIN,INFO,"\n未知类型错误\n");
            update_ca_message(IDS_CAS_E_UNKNOWN_ERROR);
            break;
        case CAS_E_PROG_UNAUTH:
            DEBUG(MAIN,INFO,"\n频道未授权\n");
            update_ca_message(IDS_NO_AUTHORIZED);
            break;
        case CAS_E_CARD_DIS_PARTNER:
            DEBUG(MAIN,INFO,"\n机卡不配对\n");
            update_ca_message(IDS_CA_NEED_SMC_STB_PAIR);
            break;
        case CAS_E_CARD_INIT_FAIL:
            DEBUG(MAIN,INFO,"\n智能卡未初始化\n");
            update_ca_message(IDS_CA_CARD_INIT_FAIL);
            break;
        case CAS_E_SW1SW2_INCORRECT_SUPER_CAS_ID:
        case CAS_E_SW1SW2_INCORRECT_SMARTCARD_ID:
            DEBUG(MAIN,INFO,"\n错误的智能卡号\n");
            update_ca_message(IDS_QZ_WRONG_SMART_CARD);
            break;
        case CAS_E_IPPV_NO_BOOK:
            DEBUG(MAIN,INFO,"\nPPV过期\n");
            update_ca_message(IDS_CA_CARD_IPPV_OUT_TIME);
            break;
        case CAS_E_ZONE_CODE_ERR:
            DEBUG(MAIN,INFO,"\n区域限播\n");
            update_ca_message(IDS_CA_AREA_ERROR);
            break;
        case CAS_C_CARD_ZONE_INVALID:
            DEBUG(MAIN,INFO,"\n卡限播\n");
            update_ca_message(IDS_CA_CARD_ZONE_INVALID);
            break;
        case CAS_E_SW1SW2_NOT_CHILD_MODE:
            DEBUG(MAIN,INFO,"\n智能卡没有设置为子卡模式\n");
            update_ca_message(IDS_QZ_NSET_SUB_MODE);
            break;
        case CAS_C_MASTERSLAVE_NEEDPAIRED:
            DEBUG(MAIN,INFO,"\n子卡未授权\n");
            update_ca_message(IDS_QZ_UNAUTH_SUB);
            break;
        case CAS_E_SW1SW2_CHILD_AUTH_TIMEOUT:
            DEBUG(MAIN,INFO,"\n设置子卡授权超时\n");
			update_ca_message(IDS_QZ_SUB_AUTH_TIMEOUT);
            break;
            
		default:
			//g_ca_last_msg = CAS_S_CLEAR_DISPLAY;
			break;
	}

	return SUCCESS;
}

RET_CODE open_ca_rolling_menu(u32 para1, u32 para2)
{
#define CA_ROLLING_X			(400)
#define CA_ROLLING_W			(SCREEN_WIDTH - CA_ROLLING_X)
#define CA_ROLLING_H			(56)
#define CA_ROLLING_TOP_Y		(38)
#define CA_ROLLING_BUTTON_Y		(SCREEN_HEIGHT - CA_ROLLING_H - CA_ROLLING_TOP_Y)

  control_t *p_cont = NULL;
  control_t *p_txt_top = NULL;
  control_t *p_txt_bottom = NULL;
  
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  
  // top roll control
  p_txt_top = ctrl_create_ctrl(CTRL_TEXT, IDC_BG_MENU_CA_ROLL_TOP,
                           CA_ROLLING_X, CA_ROLLING_TOP_Y,
                           CA_ROLLING_W,CA_ROLLING_H,
                           p_cont, 0);
  ctrl_set_proc(p_txt_top, ui_ca_rolling_proc);
  ctrl_set_sts(p_txt_top,OBJ_STS_HIDE);
  //ctrl_set_rstyle(p_txt_top, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  text_set_font_style(p_txt_top, FSI_ROLL_OSD, FSI_ROLL_OSD, FSI_ROLL_OSD);
  text_set_align_type(p_txt_top,STL_LEFT|STL_VCENTER);
  text_set_content_type(p_txt_top, TEXT_STRTYPE_UNICODE);

  //bottom roll control
  p_txt_bottom = ctrl_create_ctrl(CTRL_TEXT, IDC_BG_MENU_CA_ROLL_BOTTOM,
                           CA_ROLLING_X, CA_ROLLING_BUTTON_Y,
                           CA_ROLLING_W,CA_ROLLING_H,
                           p_cont, 0);
  ctrl_set_proc(p_txt_bottom, ui_ca_rolling_proc);
  ctrl_set_sts(p_txt_bottom,OBJ_STS_HIDE);
  //ctrl_set_rstyle(p_txt_bottom, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  text_set_font_style(p_txt_bottom, FSI_ROLL_OSD, FSI_ROLL_OSD, FSI_ROLL_OSD);
  text_set_align_type(p_txt_bottom,STL_LEFT|STL_VCENTER);
  text_set_content_type(p_txt_bottom, TEXT_STRTYPE_UNICODE);

  return SUCCESS;
}
 
static RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	DEBUG(MAIN,INFO,"on_ca_init_ok\n");
	ui_set_smart_card_insert(TRUE);
	//fw_tmr_create(ROOT_ID_BACKGROUND, MSG_TIMER_CARD_OVERDUE, 10*1000, FALSE);
	return SUCCESS;
}
static RET_CODE on_ca_clear_OSD(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	DEBUG(CAS_OSD, INFO,"\n");
	ui_set_playpg_scrambled(FALSE);
	update_signal();
	return SUCCESS;
}

BOOL get_force_channel_status(void)
{
  return g_is_force_channel;
}

static RET_CODE on_ca_force_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = ERR_FAILURE;
  u16 view_count = 0, pg_id = 0;
  u16 cur_pg_id = 0;
  static u16 pre_pg_id = INVALID_PG_ID;
  u8 view_id;
  u8 i = 0;
  u8 focus_id = 0;
  
  dvbs_prog_node_t pg = {0};
  cas_force_channel_t *p_force_channel_info = NULL;
  p_force_channel_info = (cas_force_channel_t *) para2;
  
  switch(msg)
  {
    case MSG_CA_FORCE_CHANNEL_INFO:
    if(NULL == p_force_channel_info)
    {
      return ERR_FAILURE;
    }
   
    view_id = ui_dbase_get_pg_view_id();
    view_count = db_dvbs_get_count(view_id);
    cur_pg_id = sys_status_get_curn_group_curn_prog_id();

    for(i=0; i<view_count; i++)
    {
        pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
        if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
       {
          if(pg.s_id == p_force_channel_info->serv_id
            && pg.ts_id == p_force_channel_info->ts_id
            && pg.orig_net_id == p_force_channel_info->netwrok_id)
          {
              focus_id = fw_get_focus_id();
              if(!ui_is_fullscreen_menu(focus_id))
              {
                ui_close_all_mennus();
              }
              if(ui_is_mute())
              {
                ui_set_mute(FALSE);
              }
              ui_play_prog(pg_id, FALSE);
              g_is_force_channel = TRUE;
              #ifdef TEMP_SUPPORT_DS_AD

              if(TRUE == ui_get_ads_osd_status())
              {
                ui_ads_osd_roll_stop();
              }
              #endif
              //fill_prog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), TRUE, pg_id);
              DEBUG(MAIN,INFO,"pg_id [%d] , cur_pg_id [%d]\n", pg_id, cur_pg_id);
              if(pg_id != cur_pg_id)
              {
                pre_pg_id = cur_pg_id;
                manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR, 0, 0);
              }
              else
              {
                pre_pg_id = INVALID_PG_ID;
              }
              ret = SUCCESS;
              break;
          }
       }
    }
    break;
   case MSG_CA_UNFORCE_CHANNEL_INFO:
      g_is_force_channel = FALSE;
      if(pre_pg_id != INVALID_PG_ID)
      {
        ui_play_prog(pre_pg_id, FALSE);
        pre_pg_id = INVALID_PG_ID;
       }
      break;
    default:
      break;
  }
 
  return ret;
}

static RET_CODE on_new_mail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  cas_mail_headers_t *p_mail_header = (cas_mail_headers_t*)para2;
  u32 new_mail = 0;
  s8 i =0;
  menu_attr_t *p_attr;

  p_attr = manage_get_curn_menu_attr();

  if((0 != p_mail_header->max_num) && (0xFFFF != p_mail_header->max_num))
  {
    for(i = 0; i < p_mail_header->max_num; i++)
    {
      if(p_mail_header->p_mail_head[i].new_email)
        new_mail++;
    }
    if(new_mail > 0)
    {
      ui_set_new_mail(TRUE);
      if(!ui_is_fullscreen_menu(p_attr->root_id)
      && !ui_is_popup_menu(p_attr->root_id))
      {
        return SUCCESS;
      }
      open_ui_new_mail(0, 0);
    }
  }
  return SUCCESS;
}

BOOL osd_roll_stop_msg_unsend(void)
{
  return g_osd_stop_msg_unsend;
}

BOOL get_ca_msg_rolling_status(void)
{
  return g_ca_msg_rolling_over;
}

void set_ca_msg_rolling_status(BOOL is_over)
{
  g_ca_msg_rolling_over = is_over;
}

void ui_ca_send_rolling_over(u32 cmd_id, u32 para1, u32 para2)
{
	cmd_t cmd = {0};
	cmd.id = cmd_id;
	cmd.data1 = para1;
	cmd.data2 = ((CAS_INVALID_SLOT << 16) | AP_CAS_ID);
	ap_frm_do_command(APP_CA, &cmd);
	return;
}

static void on_desktop_roll_set_font_size_and_ctrl_h(u8 *font_size, u8 *display_pos)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl_top = NULL;
	control_t *p_ctrl_bottom = NULL;
	rect_t frm = {0};
	u16 ctrl_h = 0;

	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_ctrl_top = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
	p_ctrl_bottom = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
	switch(*font_size)
	{
		case 1:
			f_tab[FSI_ROLL_OSD1].width = 18;
			f_tab[FSI_ROLL_OSD1].height = 18;
			ctrl_h = 28;
			break;
		case 2:
			f_tab[FSI_ROLL_OSD1].width = 22;
			f_tab[FSI_ROLL_OSD1].height = 22;
			ctrl_h = 32;
			break;
		case 3:
			f_tab[FSI_ROLL_OSD1].width = 26;
			f_tab[FSI_ROLL_OSD1].height = 26;
			ctrl_h = 36;
			break;
		case 4:
			f_tab[FSI_ROLL_OSD1].width = 30;
			f_tab[FSI_ROLL_OSD1].height = 30;
			ctrl_h = 40;
			break;
		case 5:
			f_tab[FSI_ROLL_OSD1].width = 34;
			f_tab[FSI_ROLL_OSD1].height = 34;
			ctrl_h = 44;
			break;
		case 6:
			f_tab[FSI_ROLL_OSD1].width = 38;
			f_tab[FSI_ROLL_OSD1].height = 38;
			ctrl_h = 48;
			break;
		case 7:
			f_tab[FSI_ROLL_OSD1].width = 42;
			f_tab[FSI_ROLL_OSD1].height = 42;
			ctrl_h = 52;
			break;
		case 8:
			f_tab[FSI_ROLL_OSD1].width = 46;
			f_tab[FSI_ROLL_OSD1].height = 46;
			ctrl_h = 56;
			break;
		case 9:
			f_tab[FSI_ROLL_OSD1].width = 50;
			f_tab[FSI_ROLL_OSD1].height = 50;
			ctrl_h = 60;
			break;
		case 10:
			f_tab[FSI_ROLL_OSD1].width = 54;
			f_tab[FSI_ROLL_OSD1].height = 54;
			ctrl_h = 64;
			break;
		case 11:
			f_tab[FSI_ROLL_OSD1].width = 58;
			f_tab[FSI_ROLL_OSD1].height = 58;
			ctrl_h = 68;
			break;
		case 12:
			f_tab[FSI_ROLL_OSD1].width = 62;
			f_tab[FSI_ROLL_OSD1].height = 62;
			ctrl_h = 72;
			break;
		default:
			f_tab[FSI_ROLL_OSD1].width = 38;
			f_tab[FSI_ROLL_OSD1].height = 38;
			ctrl_h = 48;
			break;
	}

	switch(*display_pos)
	{
		case 0://top
			ctrl_get_frame(p_ctrl_top, &frm);
			frm.bottom = frm.top + ctrl_h;
			ctrl_set_frame(p_ctrl_top, &frm);
			ctrl_set_mrect(p_ctrl_top, 0, 0, (u16)(frm.right - frm.left), (u16)(frm.bottom - frm.top));
			break;
		case 1://bottom
			ctrl_get_frame(p_ctrl_bottom, &frm);
			frm.top = frm.bottom - ctrl_h;
			ctrl_set_frame(p_ctrl_bottom, &frm);
			ctrl_set_mrect(p_ctrl_bottom, 0, 0, (u16)(frm.right - frm.left), (u16)(frm.bottom - frm.top));
			break;
		case 2://top + bottom
			ctrl_get_frame(p_ctrl_top, &frm);
			frm.bottom = frm.top + ctrl_h;
			ctrl_set_frame(p_ctrl_top, &frm);
			ctrl_set_mrect(p_ctrl_top, 0, 0, (u16)(frm.right - frm.left), (u16)(frm.bottom - frm.top));

			ctrl_get_frame(p_ctrl_bottom, &frm);
			frm.top = frm.bottom - ctrl_h;
			ctrl_set_frame(p_ctrl_bottom, &frm);
			ctrl_set_mrect(p_ctrl_bottom, 0, 0, (u16)(frm.right - frm.left), (u16)(frm.bottom - frm.top));
			break;
		default:
			DEBUG(MAIN,INFO,"error display_pos , do nothing!!!!!\n");
			break;
	}
}

void on_desktop_start_roll_position(u16 *uni_str, u8 *display_back_color, u8 *display_color, u8 *display_front_size, u8 *display_pos)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl_top = NULL;
	control_t *p_ctrl_bottom = NULL;
	roll_param_t p_param;
	u32 color = 0, color_bg = 0;
	u8 i = 0;
	BOOL bcolor = FALSE;

	memset(&p_param, 0, sizeof(roll_param_t));

	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	for(i=0;i<4;i++)
	{
		if(display_back_color[i] != display_color[i])
		{
			bcolor = TRUE;
			break;
		}
	}
	color_bg = ((u32)(display_back_color[3]) <<24) + ((u32)(display_back_color[2]) << 16)
				+ ((u32)(display_back_color[1]) << 8) + ((u32)(display_back_color[0]));  
	c_tab[RSI_ROLL_OSD].bg.value = color_bg;  

	if(!bcolor)
	{
		color = 0xffffffff;/*background color is the same as font color ;default font color is white*/
	}
	else
	{
		color = ((u32)(display_color[3]) <<24) +(u32)( (display_color[2]) << 16)
					+ ((u32)(display_color[1]) << 8) + (u32)((display_color[0]));
	}

	f_tab[FSI_ROLL_OSD1].color = color;

	on_desktop_roll_set_font_size_and_ctrl_h(display_front_size, display_pos);

	if(*display_pos == 0)
	{
		p_ctrl_top = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
		text_set_font_style(p_ctrl_top, FSI_ROLL_OSD1, FSI_ROLL_OSD1, FSI_ROLL_OSD1);
		ctrl_set_rstyle(p_ctrl_top, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
		gui_stop_roll(p_ctrl_top);
		ctrl_set_sts(p_ctrl_top, OBJ_STS_SHOW);
		text_set_content_by_unistr(p_ctrl_top, uni_str);
		p_param.pace = ROLL_SINGLE;
		p_param.style = ROLL_LR;
		p_param.repeats= 1;
		p_param.is_force = 1;
		p_param.use_bg = TRUE;
		p_param.bg_color = color_bg;
		gui_start_roll(p_ctrl_top, &p_param);
		ctrl_paint_ctrl(p_ctrl_top,TRUE);
		g_is_roll_top_show = 1;
		g_ca_msg_rolling_over = FALSE;
	}
	else if(*display_pos == 1)
	{
		p_ctrl_bottom = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
		text_set_font_style(p_ctrl_bottom, FSI_ROLL_OSD1, FSI_ROLL_OSD1, FSI_ROLL_OSD1);
		ctrl_set_rstyle(p_ctrl_bottom, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);

		gui_stop_roll(p_ctrl_bottom);
		ctrl_set_sts(p_ctrl_bottom, OBJ_STS_SHOW);
		text_set_content_by_unistr(p_ctrl_bottom, uni_str);
		p_param.pace = ROLL_SINGLE;
		p_param.style = ROLL_LR;
		p_param.repeats= 1;
		p_param.is_force = 1;
		p_param.use_bg = TRUE;
		p_param.bg_color = color_bg;
		gui_start_roll(p_ctrl_bottom, &p_param);
		ctrl_paint_ctrl(p_ctrl_bottom,TRUE);
		g_is_roll_bottom_show = 1;
		g_ca_msg_rolling_over = FALSE;
	}
	else if(*display_pos == 2)
	{
		p_ctrl_top = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
		p_ctrl_bottom = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
		text_set_font_style(p_ctrl_top, FSI_ROLL_OSD1, FSI_ROLL_OSD1, FSI_ROLL_OSD1);
		text_set_font_style(p_ctrl_bottom, FSI_ROLL_OSD1, FSI_ROLL_OSD1, FSI_ROLL_OSD1); 
		ctrl_set_rstyle(p_ctrl_top, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
		ctrl_set_rstyle(p_ctrl_bottom, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
		gui_stop_roll(p_ctrl_top);
		gui_stop_roll(p_ctrl_bottom);
		ctrl_set_sts(p_ctrl_top, OBJ_STS_SHOW);
		ctrl_set_sts(p_ctrl_bottom, OBJ_STS_SHOW);
		text_set_content_by_unistr(p_ctrl_top, uni_str);
		text_set_content_by_unistr(p_ctrl_bottom, uni_str);

		p_param.pace = ROLL_SINGLE;
		p_param.style = ROLL_LR;
		p_param.repeats= 1;
		p_param.is_force = 1;
		p_param.use_bg = TRUE;
		p_param.bg_color = color_bg;
		gui_start_roll(p_ctrl_top, &p_param);
		gui_start_roll(p_ctrl_bottom, &p_param);
		ctrl_paint_ctrl(p_ctrl_top,TRUE);
		ctrl_paint_ctrl(p_ctrl_bottom,TRUE);

		g_is_roll_top_show = 1;
		g_is_roll_bottom_show = 1;
		g_ca_msg_rolling_over = FALSE;

	}
}

static void on_desktop_start_roll(msg_info_t *ca_osd_msg)
{
	msg_info_t *p_ca_msg_info = NULL;
	u16 uni_str[MAX_ON_SCREEN_MSG_LENGTH] = {0};
	p_ca_msg_info = (msg_info_t *)ca_osd_msg;

	gb2312_to_unicode(p_ca_msg_info->data,
	                           MAX_ON_SCREEN_MSG_LENGTH -1, uni_str, MAX_ON_SCREEN_MSG_LENGTH - 1);

	on_desktop_start_roll_position(uni_str, p_ca_msg_info->display_back_color,
	                          p_ca_msg_info->display_color, &p_ca_msg_info->display_front_size, &p_ca_msg_info->display_pos);
}

RET_CODE on_ca_show_osd(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  msg_info_t *p_ca_msg_info = NULL;
  control_t *p_cont = NULL;
  p_ca_msg_info = (msg_info_t *)para2;
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  UI_PRINTF("\r\n*** rolling_msg_update  p_ca_msg_info addr[0x%x], msg:[%d]***\r\n", para2, msg);
  if(p_ca_msg_info->type == 1)
  {

     UI_PRINTF("[BYUI] title = %s content = %s \n",p_ca_msg_info->title,p_ca_msg_info->data);
	 comm_dlg_data_t2 overdue_data = //popup dialog data
     {
             ROOT_ID_INVALID,
             DLG_FOR_CONFIRM| DLG_STR_MODE_EXTSTR,
             DLG_FOR_CONFIRM | DLG_STR_MODE_EXTSTR,
             COMM_DLG_X, (COMM_DLG_Y - 40),
             COMM_DLG_W, 200,
             0,(u32)p_ca_msg_info->title,
             0,(u32)p_ca_msg_info->data,
             0,
     };
	 ui_comm_dlg_open3(&overdue_data);
	 return SUCCESS;
  	}
   
#ifdef WIN32
  {
    msg_info_t osd_info;
    u8 screen_ratio_t[4] = {80,1,0,1};
    u8 roll_mode_t[13] = {1,2,6,6,6,6,6,6,6,6,6,6,6};
  //  u8 color_type[4] = {2,0,5,20};
    u8 bg_color_t[4] = {0xff,0x77,0xff,0x66};
    u8 font_color_t[4] = {0x22,0xff,0x11,0x55};
    u8 font_size_t[13] = {0,1,2,3,4,5,6,7,8,9,10,11,12};

    static u8 pos_t = 1;

    memset(&osd_info,0,sizeof(osd_info));
    memcpy(osd_info.data,"welcome,this is test",25);
    osd_info.data_len = 22;
	osd_info.content_len = 22;
    //osd_info.screen_ratio= screen_ratio_t[0];
    osd_info.display_pos = roll_mode_t[1];
    //osd_info.osd_display.roll_value = 3;
    //osd_info.display_time = dispaly_time_t[pos_t];
    //osd_info.display_front_size = 20;
    memcpy(osd_info.display_color, font_color_t, 4);
    memcpy(osd_info.display_back_color, bg_color_t, 4);
    //osd_info.display_color = font_color_t;
    //osd_info.display_back_color= bg_color_t;
 //   osd_info.color_type = 0;
 //   osd_info.display_front = 0;
 //   osd_info.transparency = 0;
    osd_info.display_front_size = font_size_t[pos_t];

    p_ca_msg_info = &osd_info;

    pos_t ++;
    if(pos_t >12)
      pos_t = 1;
   }
 #endif
  	if(p_ca_msg_info == NULL)
  	{
    		return ERR_FAILURE;
  	}
	if((p_ca_msg_info->display_pos == 0) ||(p_ca_msg_info->display_pos == 1) || (p_ca_msg_info->display_pos == 2))
  	{
  		  if(TRUE == g_ca_msg_rolling_over)
  		 {
			DEBUG(MAIN,INFO,"SHOW OSD \n");
			DEBUG(MAIN,INFO,"display_pos: %d!\n",p_ca_msg_info->display_pos);
			DEBUG(MAIN,INFO,"display_front: %d!\n",p_ca_msg_info->display_front);
			DEBUG(MAIN,INFO,"display_front_size: %d!\n",p_ca_msg_info->display_front_size);
			DEBUG(MAIN,INFO,"display_back_color: 0x%x-%x-%x-%x !\n",p_ca_msg_info->display_back_color[0],p_ca_msg_info->display_back_color[1],p_ca_msg_info->display_back_color[2],p_ca_msg_info->display_back_color[3]);
			DEBUG(MAIN,INFO,"display_color: 0x%x-%x-%x-%x!\n",p_ca_msg_info->display_color[0],p_ca_msg_info->display_color[1],p_ca_msg_info->display_color[2],p_ca_msg_info->display_color[3]);
			DEBUG(MAIN,INFO,"content_len: %d!\n",p_ca_msg_info->content_len);
			DEBUG(MAIN,INFO,"OSD data len:%d\n",p_ca_msg_info->data_len);
			DEBUG(MAIN,INFO,"OSD data:%s\n",p_ca_msg_info->data);
			on_desktop_start_roll(p_ca_msg_info);
			if(ui_is_fullscreen_menu(fw_get_focus_id()))
				ui_time_enable_heart_beat(TRUE);
   		 }
  }
  return SUCCESS;
}

static void ui_ca_rolling_post_stop(void)
{
	DEBUG(CAS_OSD, INFO, "\n");
	if(do_search_is_finish())
	{
		ui_ca_do_cmd(CAS_CMD_OSD_ROLL_OVER, 0, 0);
		g_osd_stop_msg_unsend = FALSE;
	}
	else
	{
		g_osd_stop_msg_unsend = TRUE;
	}
	g_ca_msg_rolling_over = TRUE;
	ui_time_enable_heart_beat(FALSE);
}

static RET_CODE on_ca_rolling_stop(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  rect_t frame = {0};
  control_t *p_parent = NULL;

  DEBUG(CAS_OSD, INFO, "\n");
  MT_ASSERT(p_ctrl != NULL);
  ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);
  ctrl_get_frame(p_ctrl, &frame);

  gui_stop_roll(p_ctrl);
  p_parent = ctrl_get_parent(p_ctrl);
  MT_ASSERT(p_parent != NULL);

  ctrl_add_rect_to_invrgn(p_parent, &frame);
  ctrl_paint_ctrl(p_parent, FALSE);

  ui_ca_rolling_post_stop();
  return SUCCESS;
}

RET_CODE on_ca_rolling_show(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t * p_ctrl_top = NULL;
	control_t * p_ctrl_bottom = NULL;
	control_t *p_background = NULL;

	DEBUG(CAS_OSD,INFO," on_ca_rolling_show \n");
	p_background = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_ctrl_top = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_TOP);
	p_ctrl_bottom = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_BOTTOM);

	if((NULL != p_ctrl_top) && (ctrl_is_rolling(p_ctrl_top))&&(g_is_roll_top_show))
	{
		DEBUG(CAS_OSD,INFO," on_ca_rolling_show \n");
		ctrl_set_sts(p_ctrl_top,OBJ_STS_SHOW);
		gui_resume_roll(p_ctrl_top);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(TRUE);
	}
	if((NULL != p_ctrl_bottom) && (ctrl_is_rolling(p_ctrl_bottom))&&(g_is_roll_bottom_show))
	{
		DEBUG(CAS_OSD,INFO," on_ca_rolling_show \n");
		ctrl_set_sts(p_ctrl_bottom,OBJ_STS_SHOW);
		gui_resume_roll(p_ctrl_bottom);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(TRUE);
	}
	return SUCCESS;
}

RET_CODE on_ca_rolling_hide(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t * p_ctrl_top = NULL;
	control_t * p_ctrl_bottom = NULL;
	control_t *p_background = NULL;

	DEBUG(CAS_OSD,INFO," on_ca_rolling_hide \n");
	p_background = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_ctrl_top = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_TOP);
	p_ctrl_bottom = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_BOTTOM);

	if((NULL != p_ctrl_top) && (ctrl_is_rolling(p_ctrl_top)))
	{
		DEBUG(CAS_OSD,INFO," on_ca_rolling_hide \n");
		ctrl_set_sts(p_ctrl_top,OBJ_STS_HIDE);
		gui_pause_roll(p_ctrl_top);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(FALSE);
	}
	if((NULL != p_ctrl_bottom) && (ctrl_is_rolling(p_ctrl_bottom)))
	{
		DEBUG(CAS_OSD,INFO," on_ca_rolling_hide \n");
		ctrl_set_sts(p_ctrl_bottom,OBJ_STS_HIDE);
		gui_pause_roll(p_ctrl_bottom);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(FALSE);
	}
	return SUCCESS;
}

RET_CODE on_ca_hide_osd(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl_top = NULL;
	control_t *p_ctrl_bottom = NULL;

	DEBUG(CAS_OSD,INFO,"msg = %d \n",msg);
	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);

	if(g_is_roll_top_show)
	{
		g_is_roll_top_show = 0;
		p_ctrl_top = ctrl_get_child_by_id(p_cont, IDC_BG_MENU_CA_ROLL_TOP);
		gui_pause_roll(p_ctrl_top);
		ctrl_set_attr(p_ctrl_top ,OBJ_STS_HIDE);
		ctrl_paint_ctrl(p_cont, TRUE);
		ui_time_enable_heart_beat(FALSE);
	}

	if(g_is_roll_bottom_show)
	{
		g_is_roll_bottom_show =0;
		p_ctrl_bottom = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
		gui_pause_roll(p_ctrl_bottom);
		ctrl_set_attr(p_ctrl_bottom ,OBJ_STS_HIDE);
		ctrl_paint_ctrl(p_cont, FALSE);
		ui_time_enable_heart_beat(FALSE);
	}
	g_ca_msg_rolling_over = TRUE;
	return SUCCESS;
}

static RET_CODE on_ca_rolling_over(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	on_ca_rolling_stop(p_ctrl, msg, para1, para2);
	return SUCCESS;
}
static RET_CODE on_ca_stop_cur_pg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u16 pg_id = 0;
	u32 index = 0;
	dvbs_prog_node_t pg = {0};
	u32 pid_param = 0; // (u32)*(u32*)para2;
	u16 ecm_pid = 0; 
	u16 service_id = 0;
	u16 temp = 0;

	if(para2 == 0)
	{
		DEBUG(MAIN,INFO,"@@ PARA2 error, return;");
		return SUCCESS;
	}
	pid_param = (u32)*(u32*)para2;
	ecm_pid = (u16)(pid_param >> 16);
	service_id = (u16)(pid_param & 0xFFFF);
	if(is_ap_playback_active())
	{
		pg_id = sys_status_get_curn_group_curn_prog_id();
		db_dvbs_get_pg_by_id(pg_id, &pg);

		//undescramble pg ecm_pid 0x1fff
		if((0x1FFF == ecm_pid) && (pg.s_id == service_id))
		{
			DEBUG(MAIN,INFO,"@@@@  current pg ecm_pid = %d ,ca_sys ecm_pid = %d ,current pg service_id = %d, cas service_id = %d \n",
			      temp, ecm_pid,pg.s_id,service_id);
			//ui_stop_play(STOP_PLAY_BLACK, TRUE);
			return SUCCESS;
		}

		//descramble pg
		for(index = 0; index < (u32)(int)pg.ecm_num; index++)
		{
			temp = pg.cas_ecm[index].ecm_pid;
			DEBUG(MAIN,INFO,"@@@@ index = %d, current pg ecm_pid = %d ,ca_sys ecm_pid = %d ,current pg service_id = %d, cas service_id = %d \n",
			index, temp, ecm_pid,pg.s_id,service_id);
			if(((pg.cas_ecm[index].ecm_pid == ecm_pid) && ( pg.s_id == service_id) ))
			{
				ui_stop_play(STOP_PLAY_BLACK, TRUE);
			}
		}
	}
	return SUCCESS;
}

BOOL ui_is_finger_show(void)
{
	return g_is_finger_show;
}

void redraw_finger(void)
{
	if(ui_is_finger_show())
	{
		DEBUG(MAIN,INFO,"\r\n *** finger_msg:data = %s***\n",g_ca_finger_msg.data);
		DEBUG(MAIN,INFO,"\r\n *** finger_msg:postion =%d  :x = %d,y =                 %d***\n",
		                               g_ca_finger_msg.show_postion,
		                               g_ca_finger_msg.show_postion_x,
		                               g_ca_finger_msg.show_postion_y);
		open_ca_finger_menu(&g_ca_finger_msg);
	}
}

static RET_CODE on_ca_finger_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	finger_msg_t *p_ca_finger_msg = NULL;
	DEBUG(MAIN,INFO,"\r\n*** on_ca_finger_update msg[0x%x],  para2 addr[0x%x] ***\r\n",
	msg, para2);
	switch(msg)
	{
		case MSG_CA_FINGER_INFO:
			p_ca_finger_msg = (finger_msg_t *)para2;

			if(NULL == p_ca_finger_msg)
			{
				return ERR_FAILURE;
			}

			memcpy(&g_ca_finger_msg, p_ca_finger_msg, sizeof(g_ca_finger_msg));  //don't remove!!

			DEBUG(MAIN,INFO,"\r\n *** finger_msg:data = %s***\n",p_ca_finger_msg->data);
			DEBUG(MAIN,INFO,"\r\n *** finger_msg:postion =%d  :x = %d,y = %d***\n",
								                   p_ca_finger_msg->show_postion,
								                   p_ca_finger_msg->show_postion_x,
								                   p_ca_finger_msg->show_postion_y);

			open_ca_finger_menu(p_ca_finger_msg);
			g_is_finger_show = TRUE;
			break;
		case MSG_CA_HIDE_FINGER:
			close_ca_finger_menu();
			g_is_finger_show = FALSE;
			break;
		default:
			break;
	}

	return SUCCESS;
}

BEGIN_MSGPROC(ui_desktop_proc_cas, cont_class_proc)
	ON_COMMAND(MSG_CA_INIT_OK, on_ca_init_ok)
	ON_COMMAND(MSG_CA_CLEAR_DISPLAY, on_ca_clear_OSD)
	ON_COMMAND(MSG_CA_EVT_NOTIFY, on_ca_message_update)
	ON_COMMAND(MSG_CA_FINGER_INFO, on_ca_finger_update)
	ON_COMMAND(MSG_CA_HIDE_FINGER, on_ca_finger_update)
	ON_COMMAND(MSG_CA_FORCE_CHANNEL_INFO, on_ca_force_channel)
	ON_COMMAND(MSG_CA_UNFORCE_CHANNEL_INFO, on_ca_force_channel)

	ON_COMMAND(MSG_CA_SHOW_OSD, on_ca_show_osd)
	ON_COMMAND(MSG_CA_HIDE_OSD, on_ca_hide_osd)
	ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, on_new_mail)
	ON_COMMAND(MSG_CA_STOP_CUR_PG, on_ca_stop_cur_pg)
END_MSGPROC(ui_desktop_proc_cas, cont_class_proc)

BEGIN_KEYMAP(ui_desktop_keymap_cas, NULL)
	ON_EVENT(V_KEY_MAIL,MSG_OPEN_MENU_IN_TAB | ROOT_ID_EMAIL_MESS)
END_KEYMAP(ui_desktop_keymap_cas, NULL)

BEGIN_MSGPROC(ui_ca_rolling_proc, text_class_proc)
	ON_COMMAND(MSG_ROLL_STOPPED, on_ca_rolling_over)
END_MSGPROC(ui_ca_rolling_proc, text_class_proc)
