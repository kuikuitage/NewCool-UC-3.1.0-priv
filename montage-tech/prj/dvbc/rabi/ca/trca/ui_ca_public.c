/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_do_search.h"
#include "ui_new_mail.h"
#include "ui_ca_public.h"
#include "ui_finger_print.h"
#include "cas_manager.h"

#define MAX_ON_SCREEN_MSG_LENGTH 320

typedef struct
{
  BOOL is_msg_need_show;
  BOOL is_cur_msg_showing;
  u16 roll_uni_str[CAS_MAX_MSG_DATA_LEN + 1];
  u8 roll_mode;
  u8 display_time;
  u8 location;
}on_screen_msg_t;


u8 g_ca_pin[6];
static on_screen_msg_t g_ca_save_osd = {0,};
static BOOL g_is_smart_card_insert = FALSE;
static u8 finger_print_flag = 0;
static BOOL g_is_smart_card_status = FALSE;

#ifndef WIN32
static finger_msg_t *g_p_finger_info = NULL;
#else
static finger_msg_t g_p_finger_info = {0,};
static msg_info_t g_p_super_osd_info = {0,};
static cas_mail_headers_t g_p_new_email_info = {0,};
#endif

static u32 start_ticks = 0;

u16 pwdlg_keymap_in_ca(u16 key);
RET_CODE pwdlg_proc_in_ca(control_t *ctrl, u16 msg, u32 para1, u32 para2);

comm_pwdlg_data_t pwdlg_data =
{
    ROOT_ID_BACKGROUND,
    ((SCREEN_WIDTH - PWDLG_W)/2),
    (SIGNAL_CONT_FULL_Y + SIGNAL_CONT_FULL_H + 30),
    IDS_CA_INPUT_PIN,
    pwdlg_keymap_in_ca,
    pwdlg_proc_in_ca,
    PWDLG_T_CA,
};

enum auto_feed_local_msg
{
  MSG_INSERT_MON_CARD = MSG_LOCAL_BEGIN + 625,
  MSG_INSERT_CHILD_CARD
};

enum email_local_msg
{
  MSG_SHOW_EMAIL = MSG_LOCAL_BEGIN + 500,
  //MSG_HIDE_EMAIL
};
enum ca_feed_status
{
  FEED_STATUS_NULL = 0,
  FEED_STATUS_OPID_SUCCESS,
  FEED_STATUS_MOTHER_CARD_INSEERT,
  FEED_STATUS_CHILD_CARD_INSERT,
  FEED_STATUS_GET_MOTHER_FEED_INFO_SUCCESS,
  FEED_STATUS_SET_CHILD_FEED_INFO_SUCCESS,
}ca_feed_status_t;

RET_CODE ui_ca_rolling_proc(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);

static comm_dlg_data_t auto_feed_dlg_data =
{
  ROOT_ID_INVALID,
  DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
  (SCREEN_WIDTH - COMM_DLG_W)/2, (SCREEN_HEIGHT- COMM_DLG_H)/2,
  COMM_DLG_W, COMM_DLG_H,
  RSC_INVALID_ID,
  RSC_INVALID_ID,
  0,
};

static chlid_card_status_info g_child_feed_info;

u8 g_auto_feed_status = FEED_STATUS_NULL;

void ui_set_smart_card_state(BOOL status)
{
	g_is_smart_card_status = status;
}

BOOL ui_get_smart_card_state(void)
{
	return g_is_smart_card_status;
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

extern void ui_desktop_start(void);
extern void zone_check(u32 freq);
void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2)
{
	dvbc_lock_t tp_set = {0};
	sys_status_get_main_tp1(&tp_set);

	ui_release_ap();
	zone_check(tp_set.tp_freq);
	DEBUG(CAS,INFO,"\n");
}


RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
  u32 event_id = para2;
  cas_module_id_t ca_module = (cas_module_id_t)para1;

  switch(ca_module)
   {
     case CAS_ID_TR:
       DEBUG(CAS,INFO,"on_ca_message_update CAS_ID_TR    event_id[%d]\n",event_id);
       break;

     case CAS_UNKNOWN:
       DEBUG(CAS,INFO,"on_ca_message_update CAS_UNKNOWN  event_id[%d]\n",event_id);
       break;

     default:
       DEBUG(CAS,INFO,"on_ca_message_update CAS_OTHER    event_id[%d]\n",event_id);
       break;
   }

  switch(event_id)
  {
     case CAS_S_ZONE_CHECK_END:
	   ui_activate_ap();
	   ui_desktop_start();
       break;
     case CAS_C_NOTIFY_EXPIRY_STATE:
       DEBUG(CAS,INFO,"CAS_C_NOTIFY_EXPIRY_STATE\n");
       update_ca_message(IDS_CA_CARD_BECOME_DUE);
       break;
     case CAS_E_CARD_UPG_FAILED:
       DEBUG(CAS,INFO,"CAS_E_CARD_UPG_FAILED\n");
       update_ca_message(IDS_CA_CARD_UPG_FAILED);
       break;
     case CAS_E_CARD_RST_FAIL:
       DEBUG(CAS,INFO,"CAS_E_CARD_RST_FAIL\n");
       g_is_smart_card_insert = FALSE;
       update_ca_message(IDS_CA_CARD_RST_FAIL);
       break;
     case CAS_E_CADR_NO_MONEY:
       DEBUG(CAS,INFO,"CAS_E_CADR_NO_MONEY\n");
       update_ca_message(IDS_CA_CADR_NO_MONEY);
       break;
     case CAS_E_SERVICE_LOCKED:
       DEBUG(CAS,INFO,"CAS_E_SERVICE_LOCKED\n");
       update_ca_message(IDS_CA_SERVICE_LOCKED);
       break;
     case CAS_E_CADR_NOT_ACT:
       DEBUG(CAS,INFO,"CAS_E_CADR_NOT_ACT\n");
       update_ca_message(IDS_CA_CADR_NOT_ACT);
       break;
     case CAS_E_ZONE_CODE_ERR:
       DEBUG(CAS,INFO,"CAS_E_ZONE_CODE_ERR\n");
       update_ca_message(IDS_CA_NOT_ALLOWED_REGION);
       break;
     case CAS_E_NOT_RECEIVED_SPID:
       DEBUG(CAS,INFO,"CAS_E_NOT_RECEIVED_SPID\n");
       update_ca_message(IDS_CA_NOT_RECEIVED_SPID);
       break;
     case CAS_E_PIN_INVALID:
       DEBUG(CAS,INFO,"CAS_E_PIN_INVALID\n");
       update_ca_message(IDS_CA_CARD_PIN_INVALID);
       break;
     case CAS_ERROR_CARD_NOTSUPPORT:
       DEBUG(CAS,INFO,"CAS_ERROR_CARD_NOTSUPPORT\n");
       update_ca_message(IDS_CA_ERROR_CARD_NOTSUPPORT);
       break;
     case CAS_E_IPP_INFO_ERROR:
       DEBUG(CAS,INFO,"CAS_E_IPP_INFO_ERROR\n");
       update_ca_message(IDS_CA_IPP_INFO_ERROR);
       break;
     case CAS_E_FEED_DATA_ERROR:
       DEBUG(CAS,INFO,"CAS_E_FEED_DATA_ERROR\n");
       update_ca_message(IDS_CA_FEED_DATA_ERROR);
       break;
     case CAS_S_CARD_RST_SUCCESS:
       DEBUG(CAS,INFO,"CAS_S_CARD_RST_SUCCESS\n");
       update_ca_message(IDS_CA_CARD_RST_SUCCESS);
       break;
     case CAS_S_CARD_UPG_SUCCESS:
       DEBUG(CAS,INFO,"CAS_S_CARD_UPG_SUCCESS\n");
       //update_ca_message(IDS_CAS_S_CARD_UPG_SUCCESS);
       break;
     case CAS_S_ADPT_CARD_REMOVE:
       DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_REMOVE\n");
       g_is_smart_card_insert = FALSE;
 	ui_set_smart_card_state(FALSE);
       //update_ca_message(IDS_CAS_S_CARD_REMOVE);
       if(ui_is_playpg_scrambled())
       {
         update_ca_message(IDS_CA_E04);
       }
       break;
     case CAS_S_ADPT_CARD_INSERT:
       DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_INSERT\n");
       g_is_smart_card_insert = TRUE;
	ui_set_smart_card_state(TRUE);
       break;
     case CAS_S_MODIFY_ZONECODE:
       DEBUG(CAS,INFO,"CAS_S_MODIFY_ZONECODE\n");
       update_ca_message(IDS_CA_MODIFY_ZONECODE);
       break;
     case CAS_S_SEVICE_NOT_SCRAMBLED:
       DEBUG(CAS,INFO,"CAS_S_SEVICE_NOT_SCRAMBLED\n");
        update_ca_message(IDS_CA_E00);
       break;
     case CAS_S_CARD_MODULE_REMOVE:
        DEBUG(CAS,INFO,"CAS_S_CARD_MODULE_REMOVE\n");
        if(ui_is_smart_card_insert() != TRUE)
          update_ca_message(IDS_CA_E01);
       break;
     case CAS_E_CARD_MODULE_EEPROM_FAILED:
       DEBUG(CAS,INFO,"CAS_E_CARD_MODULE_EEPROM_FAILED\n");
        update_ca_message(IDS_CA_E02);
       break;
       case CAS_E_CARD_MODULE_FAIL:
       DEBUG(CAS,INFO,"CAS_E_CARD_MODULE_FAIL\n");
        update_ca_message(IDS_CA_E03);
       break;
     case CAS_S_CARD_REMOVE:
        DEBUG(CAS,INFO,"CAS_S_CARD_REMOVE\n");
        update_ca_message(IDS_CA_E04);
        break;
     case CAS_E_ILLEGAL_CARD:
       DEBUG(CAS,INFO,"CAS_E_ILLEGAL_CARD\n");
       g_is_smart_card_insert = FALSE;
       update_ca_message(IDS_CA_E05);
       break;
     case CAS_E_CARD_INIT_FAIL:
       DEBUG(CAS,INFO,"CAS_E_CARD_INIT_FAIL\n");
       g_is_smart_card_insert = FALSE;
       update_ca_message(IDS_CA_E06);
       break;
     case CAS_S_CARD_CHECKING:
       DEBUG(CAS,INFO,"CAS_S_CARD_CHECKING\n");
       update_ca_message(IDS_CA_E07);
       break;
     case CAS_E_CARD_EEPROM_FAILED:
       DEBUG(CAS,INFO,"CAS_E_CARD_EEPROM_FAILED\n");
       update_ca_message(IDS_CA_E09);
       break;
     case CAS_E_PMT_FORMAT_WRONG:
       DEBUG(CAS,INFO,"CAS_E_PMT_FORMAT_WRONG\n");
       update_ca_message(IDS_CA_E10);
       break;
     case CAS_E_CARD_DIS_PARTNER:
       DEBUG(CAS,INFO,"CAS_E_CARD_DIS_PARTNER\n");
       update_ca_message(IDS_CA_E11);
       break;
     case CAS_C_SON_CARD_NEED_FEED:
       DEBUG(CAS,INFO,"CAS_C_SON_CARD_NEED_FEED\n");
       update_ca_message(IDS_CA_E12);
       g_auto_feed_status = FEED_STATUS_OPID_SUCCESS;
       break;
     case CAS_E_SEVICE_NOT_AVAILABLE:
       DEBUG(CAS,INFO,"CAS_E_SEVICE_NOT_AVAILABLE\n");
       update_ca_message(IDS_CA_E13);
       break;
     case CAS_E_PROG_UNAUTH:
       DEBUG(CAS,INFO,"CAS_E_PROG_UNAUTH\n");
       update_ca_message(IDS_CA_E14);
       break;
     case CAS_S_PROG_AUTH_CHANGE:
       DEBUG(CAS,INFO,"CAS_S_PROG_AUTH_CHANGE\n");
       update_ca_message(IDS_CA_E15);
       break;
     case CAS_E_SEVICE_SCRAMBLED:
       DEBUG(CAS,INFO,"CAS_E_SEVICE_SCRAMBLED\n");
       update_ca_message(IDS_CA_E16);
       break;
     case CAS_E_SEVICE_SCRAMBLED2:
       DEBUG(CAS,INFO,"CAS_E_SEVICE_SCRAMBLED2\n");
       update_ca_message(IDS_CA_E17);
       break;
     case CAS_E_ERROR_E18:
       DEBUG(CAS,INFO,"CAS_E_ERROR_E18\n");
       update_ca_message(IDS_CA_E18);
       break;
     case CAS_E_ERROR_E19:
       DEBUG(CAS,INFO,"CAS_E_ERROR_E19\n");
       update_ca_message(IDS_CA_E19);
       break;
     case CAS_E_NOT_ALLOWED_BROADCASTER:
       DEBUG(CAS,INFO,"CAS_E_NOT_ALLOWED_BROADCASTER\n");
       update_ca_message(IDS_CA_E20);
       break;
     case CAS_E_ERROR_E21:
       DEBUG(CAS,INFO,"CAS_E_ERROR_E21\n");
       update_ca_message(IDS_CA_E21);
       break;
     case CAS_E_ERROR_E22:
       DEBUG(CAS,INFO,"CAS_E_ERROR_E22\n");
       update_ca_message(IDS_CA_E22);
       break;
     case CAS_S_SERVICE_DISCRAMBLING:
       DEBUG(CAS,INFO,"CAS_S_SERVICE_DISCRAMBLING\n");
       update_ca_message(IDS_CA_E23);
       break;
     case CAS_E_NOT_ALLOWED_REGION:
       DEBUG(CAS,INFO,"CAS_E_NOT_ALLOWED_REGION\n");
       update_ca_message(IDS_CA_E24);
       break;
     case CAS_E_CARD_PARTNER_FAILED:
       DEBUG(CAS,INFO,"CAS_E_CARD_PARTNER_FAILED\n");
       update_ca_message(IDS_CA_E25);
       break;
     case CAS_E_SEVICE_UNKNOWN:
       DEBUG(CAS,INFO,"CAS_E_SEVICE_UNKNOWN\n");
       update_ca_message(IDS_CA_E26);
       break;
     case CAS_E_SEVICE_NOT_RUNNING:
       DEBUG(CAS,INFO,"CAS_E_SEVICE_NOT_RUNNING\n");
       update_ca_message(IDS_CA_E27);
       break;
     case CAS_E_CARD_LOCKED:
       DEBUG(CAS,INFO,"CAS_E_CARD_LOCKED\n");
       update_ca_message(IDS_CA_E28);
       break;
     case CAS_E_ERROR_E29:
       DEBUG(CAS,INFO,"CAS_E_ERROR_E29\n");
       update_ca_message(IDS_CA_E29);
       break;
     case CAS_E_ERROR_E30:
       DEBUG(CAS,INFO,"CAS_E_ERROR_E30\n");
       update_ca_message(IDS_CA_E30);
       break;
     case CAS_S_ADPT_CARD_RST_SUCCESS:
       DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_RST_SUCCESS\n");
       ui_ca_do_cmd(CAS_CMD_RATING_GET, 0, 0);
       break;
     case CAS_E_PARENTAL_CTRL_LOCKED:
       DEBUG(CAS,INFO,"CAS_E_PARENTAL_CTRL_LOCKED\n");
       update_ca_message(IDS_CA_E31);
       if(ui_is_fullscreen_menu(fw_get_focus_id()))
       {
       	  if(fw_get_focus_id() == ROOT_ID_PROG_BAR)
		  	manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
		  
          if(fw_find_root_by_id(ROOT_ID_PASSWORD) == NULL)
            ui_comm_pwdlg_open(&pwdlg_data);
       }
       break;
     case CAS_E_NOT_ALLOWED_COUNTRY:
       DEBUG(CAS,INFO,"CAS_E_NOT_ALLOWED_COUNTRY\n");
       update_ca_message(IDS_CA_E32);
       break;
     case CAS_E_NO_AUTH_DATA:
       DEBUG(CAS,INFO,"CAS_E_NO_AUTH_DATA\n");
       update_ca_message(IDS_CA_E33);
       break;
      case CAS_E_NO_AUTH_STB:
       DEBUG(CAS,INFO,"CAS_E_NO_AUTH_STB\n");
       update_ca_message(IDS_CA_E34);
       break;
      case CAS_E_NO_SIGNAL:
       DEBUG(CAS,INFO,"CAS_E_NO_SIGNAL\n");
       update_ca_message(IDS_CA_E35);
       break;
     case CAS_E_ADPT_ILLEGAL_CARD:
       DEBUG(CAS,INFO,"CAS_E_ADPT_ILLEGAL_CARD\n");
       g_is_smart_card_insert = FALSE;
       if(ui_is_playpg_scrambled())
       {
         update_ca_message(IDS_CA_E05);
       }
       break;
     case CAS_C_SHOW_NEW_EMAIL:
       DEBUG(CAS,INFO,"CAS_C_SHOW_NEW_EMAIL\n");
       ui_set_new_mail(TRUE);
       open_ui_new_mail(0, 0);
       break;
	 case CAS_C_SHOW_SPACE_EXHAUST:
	 	DEBUG(CAS,INFO,"邮件空间已满\n");
	 	break;
	 case CAS_ANNOUNCE_SHOW_SPACE_EXHAUST:
	 	DEBUG(CAS,INFO,"通知空间已满\n");
	 	break;
     default:
       DEBUG(CAS,INFO,"default CAS_EVENT\n");
       break;
   }

   return SUCCESS;
}

static RET_CODE on_finger_print(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u16 i, j;
	s8 asc_num[32];

	g_p_finger_info = (finger_msg_t *)para2;
	for(i = 0, j = 0; i < 16; i += 2, j++)
	{
		sprintf(&asc_num[i], "%02x", g_p_finger_info->data[j]);
	}
	finger_print_flag = 1;
	fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
	#ifndef WIN32
	open_finger_print(0, (u32)(u32)asc_num);
	#else
	open_finger_print(0, (u32)(&g_p_finger_info));
	#endif

	DEBUG(MAIN,INFO,"ecm finger = %s \n",asc_num);
	return SUCCESS;
}

static RET_CODE on_ca_hide_finger(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"@@@@@ on_ca_hide_finger\n");
	if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
	{
		ui_close_finger_menu();
	}

	return SUCCESS;
}

static RET_CODE on_ipp_buy_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u8 focus_id = 0;

	focus_id = fw_get_focus_id();

	if((ui_is_fullscreen_menu(focus_id)||(focus_id == ROOT_ID_FINGER_PRINT ))&& (focus_id != ROOT_ID_CA_IPP ) )
	{
		if(focus_id == ROOT_ID_PROG_BAR)
		{
			manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
		}
		manage_open_menu(ROOT_ID_CA_IPP, para1, para2);
	}

	return SUCCESS;
}

static RET_CODE on_ipp_hide_buy_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"on_ipp_hide_buy_info\n");
	manage_close_menu(ROOT_ID_CA_IPP, 0, 0);

	return SUCCESS;
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

	if(MSG_CA_FORCE_CHANNEL_INFO == msg)
	{
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
				if((pg.s_id == p_force_channel_info->serv_id)
							&& (pg.ts_id == p_force_channel_info->ts_id)
							&& (pg.orig_net_id == p_force_channel_info->netwrok_id)
							&&(pg_id != cur_pg_id))
				{
					focus_id = fw_get_focus_id();
					if(!ui_is_fullscreen_menu(focus_id))
					{
						ui_close_all_mennus();
					}
					set_uio_status(FALSE);
					ui_play_prog(pg_id, FALSE);
					pre_pg_id = cur_pg_id;
					manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR, 0, 0);
					ret = SUCCESS;
					break;
				}
				else
				{
					pre_pg_id = INVALID_PG_ID;
				}
			}
		}
		DEBUG(MAIN,INFO,"MSG_CA_FORCE_CHANNEL_INFO\n");
	}
	else
	{
		set_uio_status(TRUE);
		if(pre_pg_id != INVALID_PG_ID)
		{
			ui_play_prog(pre_pg_id, FALSE);
			pre_pg_id = INVALID_PG_ID;
		}
		ret = SUCCESS;
		DEBUG(MAIN,INFO,"MSG_CA_UNFORCE_CHANNEL_INFO\n");
	}
	
	return ret;
}

static RET_CODE on_ca_show_senior_preview(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont, *p_senior_preview;
	u32 *p_ca_msg_info = NULL;
	u16 ret_code = 0;
	p_ca_msg_info = (u32 *)para2;

	DEBUG(CAS,INFO,"on_ca_show_senior_preview addr[0x%x]\n", para2);
	if(p_ca_msg_info == NULL)
	{
		return ERR_FAILURE;
	}

	if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
	{
		fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
	}
	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_senior_preview = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_SENIOR_PREVIEW);

	ret_code = (*p_ca_msg_info) & (0xffff);
	DEBUG(CAS,INFO,"@@@@@ ret_code:%d\n", ret_code);
	if(ret_code == 1)
	{
		text_set_content_by_strid(p_senior_preview, RSC_INVALID_ID);
	}
	else
	{
		text_set_content_by_strid(p_senior_preview, IDS_CA_SENIOR_PREVIEW_INFO);
	}

	ctrl_set_attr(p_senior_preview, OBJ_ATTR_ACTIVE);
	ctrl_paint_ctrl(p_cont,TRUE);
	return SUCCESS;
}

static RET_CODE on_ca_hide_senior_preview(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont, *p_senior_preview;
	//rect_t frame = {0};
	DEBUG(CAS,INFO," on_ca_hide_preview\n");
	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_senior_preview = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_SENIOR_PREVIEW);
	MT_ASSERT(p_senior_preview != NULL);
	ctrl_set_sts(p_senior_preview ,OBJ_STS_HIDE);
	//ctrl_get_frame(p_senior_preview, &frame);
	// ctrl_add_rect_to_invrgn(p_cont, &frame);
	ctrl_paint_ctrl(p_cont, TRUE);
	return SUCCESS;
}

static RET_CODE on_new_mail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	cas_mail_headers_t *p_mail_header = (cas_mail_headers_t*)para2;
	u32 new_mail = 0;
	s8 i =0;
	u8 index;
	static BOOL full_email_flag = FALSE;

	DEBUG(CAS,INFO,"\n");
	index = fw_get_focus_id();
	if(0 == p_mail_header->max_num)
	{
		ui_set_new_mail(FALSE);
		return SUCCESS;
	}

	if(index != ROOT_ID_DO_SEARCH&& do_search_is_finish()&& index != ROOT_ID_EMAIL_MESS)
	{
		//if up to max num mail, new mail icon flash
		if(p_mail_header->max_num == 20)
		{
			if(FALSE == full_email_flag)
				fw_tmr_create(ROOT_ID_BACKGROUND, MSG_SHOW_EMAIL, 1000, TRUE);
			start_ticks = mtos_ticks_get();
		}
		else if(TRUE == full_email_flag)
		{
			fw_tmr_destroy(ROOT_ID_BACKGROUND,MSG_SHOW_EMAIL);
		}

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
				open_ui_new_mail(0, 0);
			}		
			else
			{
				ui_set_new_mail(FALSE);
			}
		}
	}

	return SUCCESS;
}

static RET_CODE on_mail_full(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"\n");
	if(ui_is_fullscreen_menu(fw_get_focus_id()))
	{
		if(((mtos_ticks_get() - start_ticks)/100) % 2 ==0 )
		{
			close_new_mail();
		}
		else
		{
			open_ui_new_mail(0, 0);
		}
	}

	return SUCCESS;
}

static RET_CODE on_auto_feed_insert_card(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	cas_card_info_t *p_card_info = (cas_card_info_t *)para2;
	if((g_auto_feed_status == FEED_STATUS_OPID_SUCCESS))
	{
		if(p_card_info->card_type == 0)//from mon card get feed data 
		{
			g_auto_feed_status = FEED_STATUS_MOTHER_CARD_INSEERT;
			ui_ca_do_cmd(CAS_CMD_MON_CHILD_STATUS_GET, 0, 0);
		}
	}
	else if((g_auto_feed_status == FEED_STATUS_CHILD_CARD_INSERT))
	{
		if(p_card_info->card_type != 0)//set feed data to sub card
		{
			g_auto_feed_status = FEED_STATUS_CHILD_CARD_INSERT;
			ui_ca_do_cmd(CAS_CMD_MON_CHILD_FEED, (u32)&g_child_feed_info, 0);
		}
	}

	return SUCCESS;
}

static RET_CODE on_auto_feed_mon_child_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	u32 ret = para1;
	if(ui_is_fullscreen_menu(fw_get_focus_id()))
	{
		switch(ret)
		{
			case SUCCESS:
				auto_feed_dlg_data.content = IDS_CA_FEED_INSERT_CHILD_CARD;
				auto_feed_dlg_data.dlg_tmout = 5000;
				g_auto_feed_status = FEED_STATUS_GET_MOTHER_FEED_INFO_SUCCESS;
				memcpy(&g_child_feed_info, (void*)para2, sizeof(chlid_card_status_info));
				break;

			case CAS_ERROR_CARD_NOTSUPPORT:
			case CAS_E_FEED_DATA_ERROR:
			case CAS_E_UNKNOW_ERR:
			case CAS_E_IOCMD_NOT_OK:
			default:
				auto_feed_dlg_data.content = IDS_CA_MOTHER_CARD_ERR;
				auto_feed_dlg_data.dlg_tmout = 5000;
				g_auto_feed_status = FEED_STATUS_NULL;
				memset(&g_child_feed_info, 0, sizeof(chlid_card_status_info));
				break;
		}
		ui_comm_dlg_open(&auto_feed_dlg_data);
	}

	return SUCCESS;
}

static RET_CODE on_auto_feed_result(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	u32 ret = para1;
	if(ui_is_fullscreen_menu(fw_get_focus_id()))
	{
		switch(ret)
		{
			case SUCCESS:
				auto_feed_dlg_data.content = IDS_CA_FEED_INSERT_CHILD_CARD;
				auto_feed_dlg_data.dlg_tmout = 5000;
				g_auto_feed_status = FEED_STATUS_SET_CHILD_FEED_INFO_SUCCESS;
				memcpy(&g_child_feed_info, (void*)para2, sizeof(chlid_card_status_info));
				break;

			case CAS_E_CARD_PARTNER_FAILED:
				auto_feed_dlg_data.content = IDS_CA_WRITE_CHILD_CARD_FAILURE;
				auto_feed_dlg_data.dlg_tmout = 5000;
				g_auto_feed_status = FEED_STATUS_NULL;
				memset(&g_child_feed_info, 0, sizeof(chlid_card_status_info));
				break;
				
			case CAS_E_PARAM_ERR:
			case CAS_ERROR_CARD_NOTSUPPORT:
			case CAS_E_FEED_DATA_ERROR:
			case CAS_E_UNKNOW_ERR:
			case CAS_E_IOCMD_NOT_OK:
			default:
				auto_feed_dlg_data.content = IDS_TR_CHILD_CARD_ERR;
				auto_feed_dlg_data.dlg_tmout = 5000;
				g_auto_feed_status = FEED_STATUS_NULL;
				memset(&g_child_feed_info, 0, sizeof(chlid_card_status_info));
				break;
		}
		ui_comm_dlg_open(&auto_feed_dlg_data);
	}
	
	return SUCCESS;
}

static RET_CODE on_ca_clear_OSD(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	DEBUG(CAS_OSD, INFO,"\n");
	ui_set_playpg_scrambled(FALSE);
	update_signal();
	return SUCCESS;
}

RET_CODE open_ca_rolling_menu(u32 para1, u32 para2)
{
#define CA_ROLLING_TOP_X			(400)
#define CA_ROLLING_TOP_W			(SCREEN_WIDTH - CA_ROLLING_TOP_X)
#define CA_ROLLING_TOP_H			(56)
#define CA_ROLLING_TOP_Y			(38)
	
#define CA_ROLLING_BUTTON_X			(0)
#define CA_ROLLING_BUTTON_W			(SCREEN_WIDTH - CA_ROLLING_BUTTON_X)
#define CA_ROLLING_BUTTON_H			(56)
#define CA_ROLLING_BUTTON_Y			(SCREEN_HEIGHT - CA_ROLLING_BUTTON_H - CA_ROLLING_TOP_Y)

	
	control_t *p_cont = NULL;
	control_t *p_txt_top = NULL;
	control_t *p_txt_bottom = NULL;

	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);

	// top roll control
	p_txt_top = ctrl_create_ctrl(CTRL_TEXT, IDC_BG_MENU_CA_ROLL_TOP,
						   CA_ROLLING_TOP_X, CA_ROLLING_TOP_Y,
						   CA_ROLLING_TOP_W, CA_ROLLING_TOP_H,
						   p_cont, 0);
	ctrl_set_proc(p_txt_top, ui_ca_rolling_proc);
	ctrl_set_sts(p_txt_top,OBJ_STS_HIDE);
	ctrl_set_rstyle(p_txt_top,RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
	text_set_font_style(p_txt_top, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
	text_set_align_type(p_txt_top,STL_LEFT|STL_VCENTER);
	text_set_content_type(p_txt_top, TEXT_STRTYPE_UNICODE);

	//bottom roll control
	p_txt_bottom = ctrl_create_ctrl(CTRL_TEXT, IDC_BG_MENU_CA_ROLL_BOTTOM,
						   CA_ROLLING_BUTTON_X, CA_ROLLING_BUTTON_Y,
						   CA_ROLLING_BUTTON_W, CA_ROLLING_BUTTON_H,
						   p_cont, 0);
	ctrl_set_proc(p_txt_bottom, ui_ca_rolling_proc);
	ctrl_set_sts(p_txt_bottom,OBJ_STS_HIDE);
	ctrl_set_rstyle(p_txt_bottom,RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
	text_set_font_style(p_txt_bottom, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
	text_set_align_type(p_txt_bottom,STL_LEFT|STL_VCENTER);
	text_set_content_type(p_txt_bottom, TEXT_STRTYPE_UNICODE);

	return SUCCESS;
}


static void on_ca_msg_start_roll(u8 location, u32 roll_mode, u8 display_time, u16 *p_uni_str)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl =NULL;
	roll_param_t p_param;

	memset(&p_param,0x0,sizeof(p_param));

	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	if(location == 0)
	{
		p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
	}
	else
	{
		p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
	}

	if(p_ctrl == NULL)
	{
		DEBUG(CAS,INFO,"@@@ got rolling control fail, creat rolltitle control.  \n");
		open_ca_rolling_menu(0,0);
		if(location == 0)
		{
			p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
		}
		else
		{
			p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
		}
	}

	gui_stop_roll(p_ctrl);
	ctrl_set_sts(p_ctrl,OBJ_STS_SHOW);
	text_set_content_by_unistr(p_ctrl,p_uni_str);

	p_param.pace = ROLL_SINGLE;
	p_param.style = ROLL_LR;
	p_param.is_force = TRUE;
	p_param.repeats= 0; //value;
	g_ca_save_osd.is_cur_msg_showing = 1;

	if(display_time == 0)
		fw_tmr_create(ROOT_ID_BACKGROUND, MSG_ROLL_STOPPED, (u32)200000, FALSE);
	else
		fw_tmr_create(ROOT_ID_BACKGROUND, MSG_ROLL_STOPPED, (u32)((display_time * 60) * 1000), FALSE);

	gui_start_roll(p_ctrl, &p_param);
	ctrl_paint_ctrl(p_ctrl,TRUE);
	DEBUG(CAS,INFO,"display_time = %d\n",display_time);
}

static RET_CODE on_ca_rolling_stop(control_t *p_parent, u16 msg, u32 para1, u32 para2)
{
	rect_t frame = {0};
	control_t *p_ctrl = NULL;
	//only1 ca's osd stop is according to time
	fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_ROLL_STOPPED);
	//if use child ctrl ,can't receiver MSG_ROLL_STOPPED msg,so use ROOT_ID_BACKGROUND
	p_ctrl = ctrl_get_child_by_id(p_parent, IDC_BG_MENU_CA_ROLL_TOP);
	ctrl_get_sts(p_ctrl);

	if(ctrl_get_sts(p_ctrl) != OBJ_STS_SHOW)
	{
		p_ctrl = ctrl_get_child_by_id(p_parent, IDC_BG_MENU_CA_ROLL_BOTTOM);
	}

	MT_ASSERT(p_ctrl != NULL);

	ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);
	ctrl_get_frame(p_ctrl, &frame);

	gui_stop_roll(p_ctrl);

	MT_ASSERT(p_parent != NULL);

	ctrl_add_rect_to_invrgn(p_parent, &frame);
	ctrl_paint_ctrl(p_parent, FALSE);
	ui_time_enable_heart_beat(FALSE);
	g_ca_save_osd.is_cur_msg_showing = 0;
	if(g_ca_save_osd.is_msg_need_show)
	{
		g_ca_save_osd.is_msg_need_show = 0;
		on_ca_msg_start_roll(g_ca_save_osd.location, g_ca_save_osd.roll_mode, g_ca_save_osd.display_time, g_ca_save_osd.roll_uni_str);
		if(ui_is_fullscreen_menu(fw_get_focus_id()))
			ui_time_enable_heart_beat(TRUE);		
	}
	else
	{
		//ui_ca_set_roll_status(FALSE);
	}
	DEBUG(CAS,INFO,"\n");
	return SUCCESS;
}

static RET_CODE on_ca_rolling_over(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS_OSD, INFO, "SHOW OSD \n");
	on_ca_rolling_stop(ctrl_get_parent(p_ctrl), msg, para1, para2);
	return SUCCESS;
}


static RET_CODE on_ca_rolling_msg_save(u32 roll_mode, u8 display_time, u8 location, u16 *p_uni_str)
{
	DEBUG(CAS,INFO,"[on_ca_rolling_msg_save]\n");
	DEBUG(CAS,INFO,"\r\n *** is_cur_msg_showing:[%d]***\n", g_ca_save_osd.is_cur_msg_showing);
	DEBUG(CAS,INFO,"\r\n *** is_msg_need_show:[%d]***\n", g_ca_save_osd.is_msg_need_show);
	g_ca_save_osd.is_msg_need_show = 1;
	g_ca_save_osd.roll_mode = (u8)roll_mode;
	g_ca_save_osd.display_time = display_time;
	memcpy(g_ca_save_osd.roll_uni_str, p_uni_str, CAS_MAX_MSG_DATA_LEN);
	g_ca_save_osd.location = location;

	return SUCCESS;
}

static RET_CODE on_ca_rolling_msg_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	msg_info_t *p_ca_msg_info = NULL;
	u16 uni_str[CAS_MAX_MSG_DATA_LEN + 1] = {0};

	p_ca_msg_info = (msg_info_t *)para2;

	DEBUG(CAS,INFO,"*** rolling_msg:%s***\n",p_ca_msg_info->data);
	DEBUG(CAS,INFO,"*** roll_mode[0x%x], roll_value[%d] location[%d], display_time[%d]**\n",
	              p_ca_msg_info->osd_display.roll_mode, p_ca_msg_info->osd_display.roll_value,
	              p_ca_msg_info->osd_display.location, p_ca_msg_info->display_time);
	DEBUG(CAS,INFO,"*** is_cur_msg_showing:[%d]***\n", g_ca_save_osd.is_cur_msg_showing);
	DEBUG(CAS,INFO,"*** is_msg_need_show:[%d]***\n", g_ca_save_osd.is_msg_need_show);

	gb2312_to_unicode(p_ca_msg_info->data, CAS_MAX_MSG_DATA_LEN, uni_str, CAS_MAX_MSG_DATA_LEN+1);

	if(NULL == p_ca_msg_info)
	{
		DEBUG(CAS,INFO,"\n");
		return ERR_FAILURE;
	}
	if((p_ca_msg_info->osd_display.location == 0) || (p_ca_msg_info->osd_display.location == 1))
	{
		if(!g_ca_save_osd.is_cur_msg_showing)
		{
			DEBUG(CAS,INFO,"\n");
			on_ca_msg_start_roll(p_ca_msg_info->osd_display.location, p_ca_msg_info->osd_display.roll_mode, p_ca_msg_info->display_time, uni_str);  //to do 
			if(ui_is_fullscreen_menu(fw_get_focus_id()))
				ui_time_enable_heart_beat(TRUE);
		}
		else
		{
			DEBUG(CAS,INFO,"[enter save function] rolling_msg:%s***\n",p_ca_msg_info->data);
			on_ca_rolling_msg_save(p_ca_msg_info->osd_display.roll_mode, p_ca_msg_info->display_time, p_ca_msg_info->osd_display.location, uni_str);
			on_ca_rolling_stop(p_ctrl, msg, para1, para2);
		}
	}
	else
		DEBUG(CAS,INFO,"\n");

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
  
  if((NULL != p_ctrl_top) && (ctrl_is_rolling(p_ctrl_top))&&(g_ca_save_osd.is_cur_msg_showing))
  {
    DEBUG(CAS_OSD,INFO," on_ca_rolling_show \n");
    ctrl_set_sts(p_ctrl_top,OBJ_STS_SHOW);
    gui_resume_roll(p_ctrl_top);
    ctrl_paint_ctrl(p_background, TRUE);
    ui_time_enable_heart_beat(TRUE);
  }
  if((NULL != p_ctrl_bottom) && (ctrl_is_rolling(p_ctrl_bottom))&&(g_ca_save_osd.is_cur_msg_showing))
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


static RET_CODE on_ca_announec_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	msg_info_t cas_msg;
	u8 focus_id = 0;

	memcpy(&cas_msg, (msg_info_t *)para2, sizeof(msg_info_t));

	DEBUG(CAS,INFO,"on_ca_announec_update =========\n");
	DEBUG(CAS,INFO,"on_ca_announec_update cas_msg.type=%d, cas_msg=%s\n",cas_msg.type, cas_msg.title);
	DEBUG(CAS,INFO,"on_ca_announec_update %04d.%02d.%02d-%02d:%02d:%02d\n",cas_msg.create_time[0]*100 + cas_msg.create_time[1], cas_msg.create_time[2], cas_msg.create_time[3], cas_msg.create_time[4], cas_msg.create_time[5], cas_msg.create_time[6]);
	DEBUG(CAS,INFO,"on_ca_announec_update =========\n");

	if(fw_find_root_by_id(ROOT_ID_CA_NOTIFY) != NULL)
	{
		DEBUG(CAS,INFO,"====ROOT_ID_CA_NOTIFY already opened, now close before reopen===\n");
		manage_close_menu(ROOT_ID_CA_NOTIFY, 0, 0);
	}

	focus_id = fw_get_focus_id();
	DEBUG(CAS,INFO,"on_ca_show_osd focus_id[%d]\n",focus_id);

	if(ui_is_fullscreen_menu(focus_id))
	{
		DEBUG(CAS,INFO,"ROOT_ID_CA_NOTIFY no display\n");
		if(fw_find_root_by_id(ROOT_ID_PROG_BAR))
			manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
		manage_open_menu(ROOT_ID_CA_NOTIFY, 0, (u32)&cas_msg);
	}
   
	return SUCCESS;
}

RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"on_ca_init_ok\n");
	ui_set_smart_card_insert(TRUE);
	if(finger_print_flag)
	{
		if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
		{
			fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
		}
		#ifndef WIN32
		on_finger_print(p_ctrl, 0, 0, (u32)g_p_finger_info);
		#endif
		finger_print_flag = 0;
	}
	
	if(g_auto_feed_status != FEED_STATUS_NULL)
	{
		ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
	}

	return SUCCESS;
}

//parent lock
static RET_CODE on_conditional_accept_unlock_parental(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = (RET_CODE)para1;

	DEBUG(CAS,INFO,"\n");
	if(ret == SUCCESS)
	{
		close_signal();
		ui_comm_pwdlg_close();
	}
	return SUCCESS;
}

static RET_CODE on_pwdlg_correct(control_t *p_ctrl,u16 msg,u32 para1,u32 para2)
{
	s8 i =0;
	for (i = 6 -1; i >= 0; i--)
	{
		g_ca_pin[i] = para1%10;
		para1 = para1/10;
	}

	ui_ca_do_cmd(CAS_CMD_UNLOCK_PIN_SET, (u32)g_ca_pin, 0);
	DEBUG(CAS,INFO,"PIN=%06%d \n",para1);
	return SUCCESS;
}

static RET_CODE on_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	ui_comm_pwdlg_close();
	return SUCCESS;
}

BEGIN_MSGPROC(ui_desktop_proc_cas, cont_class_proc)
  ON_COMMAND(MSG_CA_INIT_OK, on_ca_init_ok)
  ON_COMMAND(MSG_CA_CLEAR_DISPLAY, on_ca_clear_OSD)
  ON_COMMAND(MSG_CA_EVT_NOTIFY, on_ca_message_update)
  ON_COMMAND(MSG_CA_IPP_HIDE_BUY_INFO, on_ipp_hide_buy_info)

  ON_COMMAND(MSG_CA_FINGER_INFO, on_finger_print)
  ON_COMMAND(MSG_CA_ECM_FINGER_INFO, on_finger_print)
  ON_COMMAND(MSG_CA_HIDE_FINGER, on_ca_hide_finger)
  ON_COMMAND(MSG_CA_FORCE_CHANNEL_INFO, on_ca_force_channel)
  ON_COMMAND(MSG_CA_UNFORCE_CHANNEL_INFO, on_ca_force_channel)
  ON_COMMAND(MSG_CA_UNLOCK_PARENTAL, on_conditional_accept_unlock_parental)
  ON_COMMAND(MSG_CA_IPP_BUY_INFO, on_ipp_buy_info)
  ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, on_new_mail)
  ON_COMMAND(MSG_CA_SHOW_OSD, on_ca_rolling_msg_update)
  ON_COMMAND(MSG_CA_STB_NOTIFICATION,on_ca_announec_update)
  ON_COMMAND(MSG_CA_SHOW_SENIOR_PREVIEW, on_ca_show_senior_preview)
  ON_COMMAND(MSG_CA_HIDE_SENIOR_PREVIEW, on_ca_hide_senior_preview)
  ON_COMMAND(MSG_CA_CARD_INFO, 		on_auto_feed_insert_card)
  ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_auto_feed_mon_child_info)
  ON_COMMAND(MSG_CA_MON_CHILD_FEED, on_auto_feed_result)
  ON_COMMAND(MSG_SHOW_EMAIL, on_mail_full)
  ON_COMMAND(MSG_ROLL_STOPPED, on_ca_rolling_stop)
END_MSGPROC(ui_desktop_proc_cas, cont_class_proc)

BEGIN_KEYMAP(ui_desktop_keymap_cas, NULL)
  //ON_EVENT(V_KEY_MAIL,MSG_OPEN_MENU_IN_TAB | ROOT_ID_EMAIL_MESS)
  //ON_EVENT(V_KEY_F1, MSG_OPEN_MENU_IN_TAB | ROOT_ID_ALERT_MESS)
END_KEYMAP(ui_desktop_keymap_cas, NULL)

BEGIN_MSGPROC(ui_ca_rolling_proc, text_class_proc)
	ON_COMMAND(MSG_ROLL_STOPPED, on_ca_rolling_over)
END_MSGPROC(ui_ca_rolling_proc, text_class_proc)

BEGIN_KEYMAP(pwdlg_keymap_in_ca, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_EXIT)
  ON_EVENT(V_KEY_DOWN, MSG_EXIT)
END_KEYMAP(pwdlg_keymap_in_ca, NULL)

BEGIN_MSGPROC(pwdlg_proc_in_ca, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_pwdlg_exit)
END_MSGPROC(pwdlg_proc_in_ca, cont_class_proc)
