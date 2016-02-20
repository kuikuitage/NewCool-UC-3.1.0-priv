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
#include "ui_nprog_bar.h"
#include "ui_new_mail.h"
#include "ui_ca_public.h"
#include "ui_ca_super_osd.h"
#include "ui_ca_card_update.h"
#include "ui_finger_print.h"
#include "cas_manager.h"
#include "ui_nprog_bar.h"
#include "cas_ware.h"
#include "ui_ca_curtain.h"
#include "config_cas.h"

#include "nim_ctrl_svc.h"


#define MAX_ON_SCREEN_MSG_LENGTH 320
#define CA_MOVE_FINGER_TMOUT 10000  //ms
#define CA_INVISBLE_FINGER_TMOUT 60000  //ms

typedef struct
{
  BOOL is_msg_need_show;
  BOOL is_cur_msg_showing;
  u16 roll_uni_str[CAS_MAX_MSG_DATA_LEN + 1];
  u8 roll_mode;
  u8 display_time;
  u8 location;
}on_screen_msg_t;

static BOOL g_is_smart_card_insert = FALSE;
static BOOL g_is_smart_card_removed = FALSE;
static u32 g_smart_card_rate = 0;
static u8 finger_print_flag = 0;
card_info_t ca_card_info = {FALSE, FALSE, {0,}, {0,}};

on_screen_msg_t g_ca_save_osd = {0};
static msg_info_t g_ca_super_osd_info = {0};

static finger_msg_t g_ca_finger_msg = {{0,},0,};
static BOOL g_ca_force_super_osd = FALSE;

static BOOL g_is_finger_show = FALSE;
static BOOL g_ca_force_finger = FALSE;
static BOOL g_ca_flash_finger = FALSE;
static BOOL g_ca_finger_is_show_time = TRUE;
static BOOL g_super_osd_show = FALSE;

static cas_operators_info_t *p_operator = NULL;
static  cas_card_work_time_t *g_worktime_info = NULL;
static cas_sid_t g_ca_sid;

/*!
  The type of ca continue watch limit
  */
typedef enum 
{
  CONTIBUS_WATCH_LIMIT_DISABLE,
  CONTIBUS_WATCH_LIMIT_ENABLE,
  CONTIBUS_WATCH_LIMIT_CANCEL,
}CA_CONTINUE_WATCH_TYPE;

/*!
  ActionRequest
  */
typedef enum 
{
	CA_ACTIONREQUEST_RESTARTSTB = 0,			/* 重启机顶盒 */
	CA_ACTIONREQUEST_FREEZESTB,				/* 冻结机顶盒 */
	CA_ACTIONREQUEST_SEARCHCHANNEL,			/* 重新搜索节目 */
	CA_ACTIONREQUEST_STBUPGRADE,				/* 机顶盒程序升?*/
	CA_ACTIONREQUEST_UNFREEZESTB,				/* 解除冻结机顶盒 */
	CA_ACTIONREQUEST_INITIALIZESTB,			/* 初始化机顶盒 */
	CA_ACTIONREQUEST_SHOWSYSTEMINFO,			/* 显示系统信息 */
	CA_ACTIONREQUEST_CARDINSERTFINISH = 255,	/* 插卡处理完成 */

}CA_ACTIONREQUEST_TYPE;

//extern void restore_to_factory(void);
extern void set_uio_key_status(BOOL is_disable);
RET_CODE ui_ca_rolling_proc(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);

static u8 g_card_upg_is_open = 0;


BOOL ui_set_smart_card_insert(BOOL status)
{
	g_is_smart_card_insert = status;
	return 0;
}

BOOL ui_is_smart_card_insert(void)
{
	return g_is_smart_card_insert;
}

RET_CODE on_ca_level_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	if (para1 != SUCCESS)
	{
		return ERR_FAILURE;
	}
	g_smart_card_rate = para2;

	return SUCCESS;
}

u32 ui_get_smart_card_rate(void)
{
	return g_smart_card_rate;
}

void ui_set_smart_card_rate(u32 level)
{
	g_smart_card_rate = level;
}


void g_set_operator_info(cas_operators_info_t *p)
{
	p_operator = p;
}

cas_operators_info_t *g_get_operator_info()
{
	return p_operator;
}

void g_set_worktime_info(cas_card_work_time_t *p)
{
	g_worktime_info = p;
}

cas_card_work_time_t *g_get_worktime_info()
{
	return g_worktime_info;
}

static void ui_clear_ca_card_info()
{
  //card removed, clear card info
  ca_card_info.is_got_ca_card_num = FALSE;
  memset(ca_card_info.card_sn, 0, (CAS_CARD_SN_MAX_LEN + 1));
  update_signal();
}

RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	u32 event_id = para2;
	cas_module_id_t ca_module =(cas_module_id_t) para1;

	rect_t ca_notify_rect =
	{
		210, 125,
		210 + NOTIFY_CONT_W + 80,
		125 + NOTIFY_CONT_H,
	};

	switch(ca_module)
	{
		case CAS_ID_TF:
			DEBUG(CAS,INFO,"on_ca_message_update CAS_ID_TF    event_id[%d]\n",event_id);
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
		case CAS_C_SHOW_NEW_EMAIL:
			DEBUG(CAS,INFO,"CAS_C_SHOW_NEW_EMAIL\n");
			ui_set_new_mail(TRUE);
			if(ui_is_fullscreen_menu( fw_get_focus_id()))
			{
				update_email_picture(EMAIL_PIC_SHOW);
			}
			break;
		case CAS_C_HIDE_NEW_EMAIL:
			DEBUG(CAS,INFO,"CAS_C_HIDE_NEW_EMAIL\n");
			ui_set_new_mail(FALSE);
			close_new_mail();
			break;
		case CAS_C_SHOW_SPACE_EXHAUST:
			DEBUG(CAS,INFO,"CAS_C_EMAIL_FULL \n");
			ui_set_full_mail(TRUE);
			if(ui_is_fullscreen_menu( fw_get_focus_id()))
			{
			update_email_picture(EMAIL_PIC_FLASH);
			}
			break;


		case CAS_S_ADPT_CARD_REMOVE:
			DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_REMOVE\n");
			g_is_smart_card_insert = FALSE;
			g_is_smart_card_removed = TRUE;
			ui_clear_ca_card_info();
			{
				#if 0
				watch_limit_t curn_watch_limit = sys_status_get()->watch_limit;
				if(curn_watch_limit.type == CONTIBUS_WATCH_LIMIT_ENABLE)
				{
					fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK);
				}
				#endif
			}
			break;

		case CAS_E_ZONE_CODE_ERR:
			DEBUG(CAS,INFO,"CAS_E_ZONE_CODE_ERR\n");
			update_ca_message(IDS_AREA_ERROR);
			break;

		case CAS_S_MOTHER_CARD_REGU_INSERT:
			DEBUG(CAS,INFO,"CAS_S_MOTHER_CARD_REGU_INSERT\n");
			{
			update_ca_message(IDS_CA_SLAVE_NEED_PAIR);
			break;
			}
		case CAS_S_SON_CARD_REGU_INSERT:
			DEBUG(CAS,INFO,"CAS_S_SON_CARD_REGU_INSERT\n");
			if(ui_is_fullscreen_menu(fw_get_focus_id()))
			ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID, IDS_CA_INSERT_CHILD_CARD);
			//update_ca_message(IDS_CA_INSERT_CHILD_CARD);
			break;
		case CAS_E_FEED_DATA_ERROR:
			DEBUG(CAS,INFO,"CAS_E_FEED_DATA_ERROR\n");
			ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID, IDS_CA_INVALID_PARENT_CARD);
			break;


		//CDSTBCA_ShowBuyMessage
		case CAS_E_ILLEGAL_CARD:
			DEBUG(CAS,INFO,"无法识别卡\n");
			g_is_smart_card_insert = FALSE;
			update_ca_message(IDS_CA_UNIDENTIFIED_CARD);
			break;

		case CAS_E_CARD_OVERDUE:
			DEBUG(CAS,INFO,"智能卡过期，请更换新卡\n");
			update_ca_message(IDS_CA_CARD_OVER_DATE);
			break;

		case CAS_E_CARD_REGU_INSERT:
			DEBUG(CAS,INFO,"加扰节目，请插入智能卡 \n");
			g_is_smart_card_insert = FALSE;
			g_is_smart_card_removed = TRUE;
			update_ca_message(IDS_CRYPTED_PROG_INSERT_SMC);
			ui_clear_ca_card_info();
			{
				#if 0
				watch_limit_t curn_watch_limit = sys_status_get()->watch_limit;
				if(curn_watch_limit.type == CONTIBUS_WATCH_LIMIT_ENABLE)
				{
					fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK);
				}
				#endif
			}
			break;
			
		case CAS_E_NO_OPTRATER_INFO:
			DEBUG(CAS,INFO,"卡中不存在节目运营商\n");
			update_ca_message(IDS_CA_NO_SP_IN_SMC);
			break;

		case CAS_E_BLACKOUT:
			DEBUG(CAS,INFO," 条件禁播 \n");
			update_ca_message(IDS_CONDITIAN_BAN);
			break;

		case CAS_E_CARD_OUT_WORK_TIME:
			DEBUG(CAS,INFO," 当前时段被设定为不能观看\n");
			update_ca_message(IDS_CA_OUT_WORKING_HOURS);
			break;
		case CAS_E_CARD_CHILD_UNLOCK:
			DEBUG(CAS,INFO," 节目级别高于设定观看级别 \n");
			update_ca_message(IDS_CA_OUT_RATING);
			break;

		case CAS_E_CARD_DIS_PARTNER:
			DEBUG(CAS,INFO,"智能卡与本机顶盒不对应\n");
			update_ca_message(IDS_CA_NOT_PAIRED);
			break;
		case CAS_E_PROG_UNAUTH:
			DEBUG(CAS,INFO,"没有授权\n");
			update_ca_message(IDS_NO_ENTITLEMENT);
			break;	
		case CAS_E_CW_INVALID:
			DEBUG(CAS,INFO,"节目解密失败\n");
			update_ca_message(IDS_CA_DECRYPTION_FAIL);
			break;
		case CAS_E_CADR_NO_MONEY:
			DEBUG(CAS,INFO,"卡内金额不足\n");
			update_ca_message(IDS_NO_ENOUGH_MONEY);
			break;

			
		case CAS_E_UNKNOW_ERR:
			DEBUG(CAS,INFO,"CAS_E_UNKNOW_ERR\n");
			update_ca_message(IDS_CAS_E_UNKNOWN_ERROR);
			break;
		case CAS_E_CARD_INIT_FAIL:
			DEBUG(CAS,INFO,"CAS_E_CARD_INIT_FAIL\n");
			g_is_smart_card_insert = FALSE;
			update_ca_message(IDS_CA_CHECK_FAILED);
			break;


		case CAS_S_CARD_UPGRADING:
			DEBUG(CAS,INFO,"CAS_S_CARD_UPGRADING\n");
			update_ca_message(IDS_CA_CARD_UPGRADING);
			break;
		case CAS_S_CARD_NEED_UPG:
			DEBUG(CAS,INFO,"CAS_S_CARD_NEED_UPG\n");
			update_ca_message(IDS_CA_CARD_NEED_UPG);
			break;
		case CAS_E_VIEWLOCK:
			DEBUG(CAS,INFO,"CAS_E_VIEWLOCK\n");
			update_ca_message(IDS_CA_SWITCH_FREQ);
			break;
		case CAS_E_CARD_SLEEP:
			DEBUG(CAS,INFO,"CAS_E_CARD_SLEEP\n");
			update_ca_message(IDS_CA_CARD_SLEEP);
			break;
		case CAS_E_CARD_FREEZE:
			DEBUG(CAS,INFO,"CAS_E_CARD_FREEZE\n");
			update_ca_message(IDS_BLACKLIST);
			break;
		case CAS_E_CARD_PAUSE:
			DEBUG(CAS,INFO,"CAS_E_CARD_PAUSE\n");
			update_ca_message(IDS_CA_CARD_STOPED);
			break;
		case CAS_E_CARD_STBLOCKED:
			DEBUG(CAS,INFO,"CAS_E_CARD_STBLOCKED\n");
			update_ca_message(IDS_CA_STB_REBOOT);
			break; 
		case CAS_S_CLEAR_DISPLAY:
			{
#if 0
			watch_limit_t *curn_watch_limit = &sys_status_get()->watch_limit;
			DEBUG(CAS,INFO,"CAS_S_CLEAR_DISPLAY\n");
			if(curn_watch_limit->type != CONTIBUS_WATCH_LIMIT_ENABLE
			|| (curn_watch_limit->is_work_time && curn_watch_limit->type == CONTIBUS_WATCH_LIMIT_ENABLE))
			{
			update_ca_message(RSC_INVALID_ID);
			}
#endif
			update_ca_message(RSC_INVALID_ID);
			break;  
			}

			/*
			case CAS_S_ADPT_CARD_REMOVE:
			DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_REMOVE\n");
			g_is_smart_card_insert = FALSE;
			on_adpt_card_remove();
			*/
		case CAS_S_ADPT_CARD_INSERT:
			//update_ca_message(RSC_INVALID_ID);
			break;
		case CAS_E_CARD_CURTAIN:
			DEBUG(CAS,INFO,"CAS_E_CARD_CURTAIN\n");
			update_ca_message(IDS_CA_SENIOR_PREVIEW_INFO);
			break;
		case CAS_E_CARD_TESTSTART:
			update_ca_message(IDS_CA_CARD_UPG_TEST);
			OS_PRINTF(" 升级测试卡测试中...  \n");
			break;
		case CAS_E_CARD_TESTFAILD:
			update_ca_message(IDS_CA_CARD_UPG_TEST_FAIL);
			OS_PRINTF("  升级测试卡测试失败，请检查机卡通讯模块 \n");
			break;
		case CAS_E_CARD_TESTSUCC:
			update_ca_message(IDS_CA_CARD_UPG_TEST_SUCCESS);
			OS_PRINTF("  升级测试卡测试成功 \n");
			break;
		case CAS_E_CARD_NOCALIBOPER:
			update_ca_message(IDS_CA_CARD_CUSTOM_OPER);
			OS_PRINTF(" 卡中不存在移植库定制运营商 \n");
			break;
		case CAS_E_CARD_STBFREEZE:
			update_ca_message(IDS_CA_STB_FROZEN);
			OS_PRINTF(" 机顶盒被冻结 \n");
			break;

		case CAS_E_CARD_CURTAIN_OK:
			OS_PRINTF("打开窗帘 \n");
			manage_open_menu(ROOT_ID_CA_CARD_CURTAIN, 0, 0);
			break;
		case CAS_E_CARD_CURTAIN_CANCLE:
			OS_PRINTF("关闭窗帘\n");
			close_ca_card_curtain();
			break;

		//fuxl add
		case CAS_S_PROG_AUTH_CHANGE:
		  DEBUG(CAS,INFO,"CAS_S_PROG_AUTH_CHANGE\n");
		  //update_ca_message(IDS_CA_E15);
		  ui_set_notify(&ca_notify_rect, NOTIFY_TYPE_STRID, IDS_CA_E15);
		  break;

		case CAS_E_IPPV_CANNTDEL:
			update_ca_message(IDS_TF_IPPV_CANNTDEL);
			break;
		default:
			DEBUG(CAS,INFO,"default CAS_EVENT\n");
			break;
	}
   return SUCCESS;
}

static RET_CODE on_ipp_buy_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	ipp_buy_info_t *p_ipp_info = NULL;
	dvbs_prog_node_t pg = {0};
	u16 prog_id = 0;
	u32 i = 0; 
	#ifdef WIN32
	static ipp_buy_info_t p_ipp_buy_info = {0};
	p_ipp_buy_info.message_type = 1;
	para2=(u32)&p_ipp_buy_info;
	#endif

	p_ipp_info = (ipp_buy_info_t *)para2;
	prog_id = sys_status_get_curn_group_curn_prog_id();

	#ifndef WIN32
	db_dvbs_get_pg_by_id(prog_id, &pg);
	DEBUG(CAS,INFO,"pg.ecm_num:%d\n", pg.ecm_num);
	if(pg.ecm_num == 0)
	{
		return SUCCESS;
	}
	else
	{//lint --e{574,737}
		for(i = 0; i < pg.ecm_num; i ++)
		{
			DEBUG(CAS,INFO,"ecm_pid:0x%x, p_ipp_info->ecm_pid:0x%x\n", pg.cas_ecm[i].ecm_pid, p_ipp_info->ecm_pid);
			if(pg.cas_ecm[i].ecm_pid == p_ipp_info->ecm_pid)
			{
				break;
			}
		}
		if(i >= pg.ecm_num)
		{
			return SUCCESS;
		}
	}
	#endif

	if(ui_is_fullscreen_menu(fw_get_focus_id()))
	{
		update_ca_message(RSC_INVALID_ID);
		if(NULL != fw_find_root_by_id(ROOT_ID_PROG_BAR))
		{
			manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
		}
		DEBUG(CAS,INFO,"open ipp_buy\n");
		manage_open_menu(ROOT_ID_CA_BOOK_IPP_REAL_TIME,0,(u32)para2);
	}
	return SUCCESS;
}


static RET_CODE on_ipp_hide_buy_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"on_ipp_hide_buy_info\n");
	if(fw_find_root_by_id(ROOT_ID_CA_BOOK_IPP_REAL_TIME))
	{
		fw_destroy_mainwin_by_id(ROOT_ID_CA_BOOK_IPP_REAL_TIME);
	}
	return SUCCESS;
}

void ui_ca_set_sid(cas_sid_t *p_ca_sid)
{
	cmd_t cmd = {CAS_CMD_SID_SET_ANSYNC};
	DEBUG(CAS,INFO,"CA: set sid pgid: %d\n", p_ca_sid->pgid);

	memcpy(&g_ca_sid, p_ca_sid, sizeof(cas_sid_t));

	cmd.data1 = (u32)(&g_ca_sid);

	ap_frm_do_command(APP_CA, &cmd);
}


static RET_CODE ui_force_play_by_ca(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	cmd_t cmd;
	//lint -e{64}
	static play_param_t g_pb_info = {0};
	cas_force_channel_t *p_force_channel_info = NULL;
	u16 video_pid = 0;
	u16 audio_pid = 0;
	

	p_force_channel_info = (cas_force_channel_t *) para2;

	DEBUG(CAS,INFO,"@@@@@ ui_force_play_by_ca\n");
	if(p_force_channel_info->CompArr[0].comp_type == 1)
	{
		video_pid = p_force_channel_info->CompArr[0].comp_PID;
	}
	else if(p_force_channel_info->CompArr[0].comp_type == 3)
	{
		audio_pid = p_force_channel_info->CompArr[0].comp_PID;
	}

	if(p_force_channel_info->CompArr[1].comp_type == 1)
	{
		video_pid = p_force_channel_info->CompArr[1].comp_PID;
	}
	else if(p_force_channel_info->CompArr[1].comp_type == 3)
	{
		audio_pid = p_force_channel_info->CompArr[1].comp_PID;
	}
	ui_stop_play(STOP_PLAY_BLACK,TRUE);
	
#ifdef DVBC_PROJECT
	g_pb_info.lock_mode = SYS_DVBC;
	g_pb_info.inner.dvbc_lock_info.tp_freq=  p_force_channel_info->frequency*100;
	g_pb_info.inner.dvbc_lock_info.tp_sym= (u32)(p_force_channel_info->symbol_rate/10);
	g_pb_info.inner.dvbc_lock_info.nim_modulate =(nim_modulation_t)p_force_channel_info->modulation;

	g_pb_info.pg_info.s_id = 0x1fff;
	g_pb_info.pg_info.tv_mode= sys_status_get()->av_set.tv_mode;
	g_pb_info.pg_info.v_pid = video_pid;
	g_pb_info.pg_info.a_pid = audio_pid;
	g_pb_info.pg_info.pcr_pid = p_force_channel_info->pcr_pid;
	g_pb_info.pg_info.pmt_pid = 0x1fff;
	g_pb_info.pg_info.audio_track = 0x1;
	g_pb_info.pg_info.audio_volume = 31;
	g_pb_info.pg_info.audio_type = 0x2;
	g_pb_info.pg_info.is_scrambled = TRUE;
	g_pb_info.pg_info.video_type = 0x0;
	g_pb_info.pg_info.aspect_ratio = 0x1;
	g_pb_info.pg_info.context1 = 0x1;
	g_pb_info.pg_info.context2 = 0x0;

	DEBUG(CAS,INFO,"@@@@@ frequency:%d, symbol_rate:%d, modulation:%d, serv_id:%d, video_id:%d, audio_id:%d, pcr_pid:%d\n",
	          g_pb_info.inner.dvbc_lock_info.tp_freq, g_pb_info.inner.dvbc_lock_info.tp_sym, p_force_channel_info->modulation, g_pb_info.pg_info.s_id,
	          video_pid, audio_pid, p_force_channel_info->pcr_pid);
#else
	g_pb_info.lock_mode = SYS_DTMB;
	g_pb_info.inner.dvbt_lock_info.tp_freq=  p_force_channel_info->frequency*100;
//g_pb_info.inner.dvbc_lock_info.tp_sym= (u32)(p_force_channel_info->symbol_rate/10);
	//g_pb_info.inner.dvbt_lock_info.nim_modulate =(nim_modulation_t)p_force_channel_info->modulation;

	g_pb_info.pg_info.s_id = 0x1fff;
	g_pb_info.pg_info.tv_mode= sys_status_get()->av_set.tv_mode;
	g_pb_info.pg_info.v_pid = video_pid;
	g_pb_info.pg_info.a_pid = audio_pid;
	g_pb_info.pg_info.pcr_pid = p_force_channel_info->pcr_pid;
	g_pb_info.pg_info.pmt_pid = 0x1fff;
	g_pb_info.pg_info.audio_track = 0x1;
	g_pb_info.pg_info.audio_volume = 31;
	g_pb_info.pg_info.audio_type = 0x2;
	g_pb_info.pg_info.is_scrambled = TRUE;
	g_pb_info.pg_info.video_type = 0x0;
	g_pb_info.pg_info.aspect_ratio = 0x1;
	g_pb_info.pg_info.context1 = 0x1;
	g_pb_info.pg_info.context2 = 0x0;
#endif


	//===============================
	cmd.id = PB_CMD_PLAY;
	cmd.data1 = (u32)&g_pb_info;
	cmd.data2 = 0;
	ap_frm_do_command(APP_PLAYBACK, &cmd);
	avc_setvolume_1(class_get_handle_by_id(AVC_CLASS_ID),(u8)g_pb_info.pg_info.audio_volume);	

	//cas_manager_set_sid(para1);
	{
		cas_sid_t ca_sid;
		memset(&ca_sid, 0, sizeof(cas_sid_t));   
		ca_sid.num = 1;
		ca_sid.pg_sid_list[0] = g_pb_info.pg_info.s_id;
		ca_sid.tp_freq = g_pb_info.inner.dvbc_lock_info.tp_freq;
		ca_sid.a_pid = g_pb_info.pg_info.a_pid;
		ca_sid.v_pid = g_pb_info.pg_info.v_pid;
		ca_sid.pmt_pid = g_pb_info.pg_info.pmt_pid;
		ca_sid.ca_sys_id = 0x1FFF;
		ui_ca_set_sid(&ca_sid);
	}
	update_ca_message(RSC_INVALID_ID);

	return SUCCESS;
}

static RET_CODE on_ca_force_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 view_count = 0, pg_id = 0;
  u16 cur_pg_id = 0;
  u8 view_id;
  u8 i = 0;
  u8 focus_id = 0;
  u8 find_pg_flag = 0;
  u32 video_pid = 0;
  u16 audio_pid = 0;
  dvbs_tp_node_t tp = {0};
  RET_CODE ret = ERR_FAILURE;
  dvbs_prog_node_t pg = {0};
  cas_force_channel_t *p_force_channel_info = NULL;
  p_force_channel_info = (cas_force_channel_t *) para2;
  DEBUG(CAS,INFO,"@@@@@ msg:%d \n", msg);
  //enable uio
  if(msg == MSG_CA_UNFORCE_CHANNEL_INFO)
  {
    set_uio_status(TRUE);
    return SUCCESS;
  }
 
  //if(TRUE != ui_get_app_playback_status())
 // {
 //   OS_PRINTF("@@@@ App playback is deactived, undo msg:%d \n", msg);
 //   return SUCCESS;
 // }
  view_id = ui_dbase_get_pg_view_id();
  view_count = db_dvbs_get_count(view_id);
  cur_pg_id = sys_status_get_curn_group_curn_prog_id();
  DEBUG(CAS,INFO,"@@@@@ cur_pg_id:%d, view_count:%d\n", cur_pg_id, view_count);
  for(i=0; i<view_count; i++)
  {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
      if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
     {
        
        if(p_force_channel_info->CompArr[0].comp_type == 1)
        {
          video_pid = p_force_channel_info->CompArr[0].comp_PID;
        }
        else if(p_force_channel_info->CompArr[0].comp_type == 3)
        {
          audio_pid = p_force_channel_info->CompArr[0].comp_PID;
        }
        
        if(p_force_channel_info->CompArr[1].comp_type == 1)
        {
          video_pid = p_force_channel_info->CompArr[1].comp_PID;
        }
        else if(p_force_channel_info->CompArr[1].comp_type == 3)
        {
          audio_pid = p_force_channel_info->CompArr[1].comp_PID;
        }
        
        ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
        //MT_ASSERT(ret == SUCCESS);
        DEBUG(CAS,INFO,"@@@@@ freq1:%d, sym1:%d, freq1:%d, sym1:%d\n", 
                       tp.freq, tp.sym, p_force_channel_info->frequency, p_force_channel_info->symbol_rate);
	
		OS_PRINTF("%s ,%d video_pid=%d audio_pid =%d\n",__FUNCTION__,__LINE__,video_pid,audio_pid);
		OS_PRINTF("%s ,%d pg.video_pid=%d pg.audio[0].p_id =%d\n",__FUNCTION__,__LINE__,
			pg.video_pid,pg.audio[0].p_id);
		OS_PRINTF("%s ,%d pg.pcr_pid=%d p_force_channel_info->pcr_pid =%d\n",__FUNCTION__,__LINE__,
			pg.pcr_pid,p_force_channel_info->pcr_pid);
		OS_PRINTF("%s ,%d tp.freq=%d p_force_channel_info->frequency =%d\n",__FUNCTION__,__LINE__,
			tp.freq,p_force_channel_info->frequency*100);
		OS_PRINTF("%s ,%d tp.sym=%d p_force_channel_info->symbol_rate =%d\n",__FUNCTION__,__LINE__,
			tp.sym,p_force_channel_info->symbol_rate/10);
	
		//lint -e{737}
	#ifdef DVBC_PROJECT
        if(//pg.pcr_pid== p_force_channel_info->pcr_pid &&
			pg.video_pid== video_pid &&
			pg.audio[0].p_id == audio_pid&& 
			tp.freq == p_force_channel_info->frequency*100 
			&& tp.sym== p_force_channel_info->symbol_rate/10)
	#else
		if(//pg.pcr_pid== p_force_channel_info->pcr_pid &&
			pg.video_pid== video_pid &&
			pg.audio[0].p_id == audio_pid
			//&& tp.freq == p_force_channel_info->frequency*100 
			)
	#endif
        {  
            find_pg_flag = 1;
            focus_id = fw_get_focus_id();
            if(!ui_is_fullscreen_menu(focus_id))
            {
              ui_close_all_mennus();
            }
			set_uio_status(FALSE);
            ui_play_prog(pg_id, FALSE);
            //fill_prog_name_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), pg_id);
            if(pg_id != cur_pg_id)
              manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR, 0, 0);
            ret = SUCCESS;
            break;
        }
     }
  }

  if(find_pg_flag == 0)
  {  
    ui_force_play_by_ca(p_ctrl, msg, (u32)cur_pg_id, para2);
  }
  //disable uio
  if(msg == MSG_CA_FORCE_CHANNEL_INFO)
  {
    
  }
  return ret;
}

/*
void lockservice_without_psisi_test()
{
    memset(&LockService, 0 ,sizeof(SCDCALockService)); 

	LockService.m_fec_inner = LockService.m_fec_outer = 0;
	LockService.m_Modulation = 3;
	LockService.m_symbol_rate = 0x00068750;
	LockService.m_dwFrequency = 0x04900000;
	LockService.m_wPcrPid = 33;
	LockService.m_ComponentNum = 2;
	LockService.m_CompArr[0].m_CompType = 1;
	LockService.m_CompArr[0].m_wCompPID = 33;
	LockService.m_CompArr[0].m_wECMPID = 0;
	LockService.m_CompArr[1].m_CompType = 3;
	LockService.m_CompArr[1].m_wCompPID = 0;
	LockService.m_CompArr[1].m_wECMPID = 0;	
	OS_PRINTF("$$time: to prog  without psisi\n");
	CDSTBCA_LockService(&LockService);		

	CDSTBCA_UNLockService();	
}
*/

static BOOL check_cur_program_finger(finger_msg_t *p_ca_finger_msg)
{
	dvbs_prog_node_t pg = {0};
	u16 prog_id = 0;
	u32 i = 0; 

	prog_id = sys_status_get_curn_group_curn_prog_id();

	db_dvbs_get_pg_by_id(prog_id, &pg);
	DEBUG(CAS,INFO,"pg.ecm_num:%d\n", pg.ecm_num);
	if(pg.ecm_num == 0)
	{
		return FALSE;
	}
	else
	{
		for(i = 0; i < pg.ecm_num; i ++)
		{
			DEBUG(CAS,INFO,"ecm_pid:0x%x, p_ca_finger_msg->ecm_pid:0x%x\n", pg.cas_ecm[i].ecm_pid, p_ca_finger_msg->ecm_pid);
			if(pg.cas_ecm[i].ecm_pid == p_ca_finger_msg->ecm_pid)
			{
				return SUCCESS;
			}
		}
	}

	return FALSE;
}
void check_finger(void)
{  
	if(g_ca_flash_finger)
	{
		fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_FINGER_BEAT);
		fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_FINGER_BEAT2);
		g_ca_flash_finger = FALSE;
		g_ca_finger_is_show_time = TRUE;
	}

	if(g_ca_force_finger && !g_ca_force_super_osd)
	{
		g_ca_force_finger = FALSE;
	}
}

static RET_CODE on_ca_finger_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	finger_msg_t *p_ca_finger_msg = NULL;
	
	DEBUG(CAS,INFO,"msg[0x%x], para2 addr[0x%x] \n",msg, para2);
	switch(msg)
	{
		case MSG_CA_FINGER_INFO:
			p_ca_finger_msg = (finger_msg_t *)para2;
			if(NULL == p_ca_finger_msg)
			{
				return ERR_FAILURE;
			}

			if(FALSE == check_cur_program_finger(p_ca_finger_msg))
			{
				return ERR_FAILURE;
			}
			
			check_finger();

			//bIsVisible finger print
			if(p_ca_finger_msg->bIsVisible == 1) 
			{
				check_finger(); 
				close_finger_print();
				g_is_finger_show = FALSE;
				return ERR_FAILURE;
			}

			//flash show
			if(p_ca_finger_msg->during != 0 && p_ca_finger_msg->period != 0)
			{
				fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_FINGER_BEAT, p_ca_finger_msg->during * 1000, FALSE);
				fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_FINGER_BEAT2, (p_ca_finger_msg->period + p_ca_finger_msg->during)* 1000, FALSE);
				g_ca_flash_finger = TRUE;
			}

			memcpy(&g_ca_finger_msg, p_ca_finger_msg, sizeof(g_ca_finger_msg));  //don't remove!!

			//force to show
			DEBUG(CAS,INFO,"is_force:%d, finger_property.m_byFontType:%d\n", p_ca_finger_msg->show_front, p_ca_finger_msg->finger_property.m_byFontType);
			if(p_ca_finger_msg->show_front)
			{
				g_ca_force_finger = TRUE;
			}
			open_finger_print(0, (u32)(&g_ca_finger_msg));
			g_is_finger_show = TRUE;
			break;

      case MSG_CA_HIDE_FINGER:
        check_finger(); 
        close_finger_print();
        g_is_finger_show = FALSE;

        break;
      default:
        break;
  }

   return SUCCESS;
}

RET_CODE reopen_finger(u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"reopen_finger **** ******\n");
	if(g_is_finger_show)
	{
		DEBUG(CAS,INFO,"\r\n ***reopen_finger:data = %s***\n",g_ca_finger_msg.data);
		open_finger_print(0, (u32)(&g_ca_finger_msg));
	}
	else
	{
		DEBUG(CAS,INFO,"finger is hide,don't need reopen!!!!\n");
	}

	return SUCCESS;
}

static RET_CODE reclose_finger(u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"reclose_finger **** ******\n");
	if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
	{
		fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
	}

	return SUCCESS;
}

static RET_CODE on_finger_flash(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	if(g_ca_flash_finger == TRUE)
	{
		DEBUG(CAS,INFO,"&*******on_finger_flash!*************** !!\n");
		if(msg == MSG_CA_FINGER_BEAT)
		{
			reclose_finger(0, 0);
			g_ca_finger_is_show_time = FALSE;
			if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CA_FINGER_BEAT2, g_ca_finger_msg.period * 1000) != SUCCESS)
			{
				fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_FINGER_BEAT2, g_ca_finger_msg.period * 1000, FALSE);
			}
		}
		else
		{
			reopen_finger(0, 0);
			g_ca_finger_is_show_time = TRUE;
			if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CA_FINGER_BEAT, g_ca_finger_msg.during * 1000) != SUCCESS)
			{
				fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_FINGER_BEAT, g_ca_finger_msg.during * 1000, FALSE);
			}
		}

	}
  return SUCCESS;
}




/*
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
*/
static RET_CODE on_conditional_accept_unlock_parental(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = (RET_CODE)para1;
  if(ret == SUCCESS)
  {
    close_signal();
    ui_comm_pwdlg_close();
  }
  return SUCCESS;
}


static RET_CODE on_new_mail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  cas_mail_headers_t *p_mail_header = (cas_mail_headers_t*)para2;
  u32 new_mail = 0;
  s8 i =0;
  
  if(ui_is_fullscreen_menu( fw_get_focus_id()))
  {
    //if up to max num mail, new mail icon flash
    DEBUG(CAS,INFO,"p_mail_header->max_num:%d\n", p_mail_header->max_num);
    if(p_mail_header->max_num == 100)
    {
      //g_email_full_flag = TRUE;
      update_email_picture(EMAIL_PIC_FLASH);
      return SUCCESS;
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
        update_email_picture(EMAIL_PIC_SHOW);
      }
    }
  }
  return SUCCESS;
}

RET_CODE on_ca_card_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	RET_CODE ret = ERR_FAILURE;
	u8 percent = ((para2 & 0XFF));
	u8 title_type = ((para2 >> 8) & (0XFF));
	
	utc_time_t card_upg_start_time = {0};
	utc_time_t card_current_upg_time = {0};
	sys_status_get_card_upg_time(&card_current_upg_time);
  
	DEBUG(CAS,INFO,"@@@@@ on_ca_card_update percent:%d\n", percent);

	//receive packet
	if(title_type == 1 && percent == 0)
	{
		time_get(&card_upg_start_time, FALSE);
		DEBUG(CAS,INFO,"@@@@@ on_ca_card_update %.4d.%.2d.%.2d %.2d:%.2d\n", 
					     card_upg_start_time.year, card_upg_start_time.month, card_upg_start_time.day,
					     card_upg_start_time.hour, card_upg_start_time.minute);
		sys_status_set_card_upg_time(card_upg_start_time);
		sys_status_set_card_upg_state(0);
	}

	//card upg
	if(title_type == 2)
	{
		if(percent == 0)
		{
			time_get(&card_upg_start_time, FALSE);
			DEBUG(CAS,INFO,"@@@@@ on_ca_card_update %.4d.%.2d.%.2d %.2d:%.2d\n", 
			     card_upg_start_time.year, card_upg_start_time.month, card_upg_start_time.day,
			     card_upg_start_time.hour, card_upg_start_time.minute);

			sys_status_set_card_upg_time(card_upg_start_time);
			sys_status_set_card_upg_state(0);
		}

		if(percent == 100)
		{
			time_get(&card_upg_start_time, FALSE);
			DEBUG(CAS,INFO,"@@@@@ on_ca_card_update %.4d.%.2d.%.2d %.2d:%.2d\n", 
			         card_upg_start_time.year, card_upg_start_time.month, card_upg_start_time.day,
			         card_upg_start_time.hour, card_upg_start_time.minute);
			sys_status_set_card_upg_time(card_upg_start_time);
			sys_status_set_card_upg_state(1);
		}
	}

	if((percent == 0) || (percent == 1))
	{
		if(!g_card_upg_is_open)
		{
			ui_clean_all_menus_do_nothing();
			manage_open_menu(ROOT_ID_CA_CARD_UPDATE, 0, para2);
			g_card_upg_is_open = 1;
		}
	}
	else if(percent > 100)
	{
		if(fw_find_root_by_id(ROOT_ID_CA_CARD_UPDATE) != NULL)
		{
			fw_destroy_mainwin_by_id(ROOT_ID_CA_CARD_UPDATE);
			g_card_upg_is_open = 0;
		}
	}
	return ret;
}

static RET_CODE on_ca_show_super_osd(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  //u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
  DEBUG(CAS,INFO,"@@@@@ on_ca_show_super_osd\n");
  g_super_osd_show = TRUE;
  //if(ui_is_fullscreen_menu(curn_root_id))
  {
    if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
    {
      fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
    }
    
    if(fw_find_root_by_id(ROOT_ID_CA_SUPER_OSD))
    {
      on_ca_super_osd_exit();
    }
    open_super_osd(0, para2);
    
  }
  return SUCCESS;
}

static RET_CODE on_ca_hide_super_osd(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  DEBUG(CAS,INFO,"@@@@@ on_ca_hide_super_osd\n");
  g_super_osd_show = FALSE;
  g_ca_force_super_osd = FALSE;

  if((g_ca_force_finger && !g_ca_finger_is_show_time) || (!g_ca_force_finger))
  {
    
  }
  if(fw_find_root_by_id(ROOT_ID_CA_SUPER_OSD) != NULL)
  {
    fw_destroy_mainwin_by_id(ROOT_ID_CA_SUPER_OSD);
  }
  return SUCCESS;
}

void redraw_super_osd(void)
{
  //u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
  //if(ui_is_fullscreen_menu(curn_root_id))
  {
	if(g_super_osd_show)
	{
	  open_super_osd(0, (u32)&g_ca_super_osd_info);
	}
  }
}

 
static RET_CODE on_ca_hide_osd(control_t *p_parent, u16 msg, u32 para1, u32 para2)
{
	control_t *p_ctrl_roll = NULL;
	rect_t frame = {0};
	u8 close_all_osd_flag = 0;

	if(msg == MSG_CA_HIDE_OSD_UP)
	{
		p_ctrl_roll = ctrl_get_child_by_id(p_parent, IDC_BG_MENU_CA_ROLL_TOP);
		if(ctrl_get_sts(ctrl_get_child_by_id(p_parent, IDC_BG_MENU_CA_ROLL_BOTTOM)) != OBJ_STS_SHOW)
		{
			close_all_osd_flag = 1;
		}
	}
	else if(msg == MSG_CA_HIDE_OSD_DOWN)
	{
		p_ctrl_roll = ctrl_get_child_by_id(p_parent, IDC_BG_MENU_CA_ROLL_BOTTOM);
		if(ctrl_get_sts(ctrl_get_child_by_id(p_parent, IDC_BG_MENU_CA_ROLL_TOP)) != OBJ_STS_SHOW)
		{
			close_all_osd_flag = 1;
		}
	}
 
	MT_ASSERT(p_ctrl_roll != NULL);

	ctrl_set_sts(p_ctrl_roll,OBJ_STS_HIDE);
	ctrl_get_frame(p_ctrl_roll, &frame);

	gui_stop_roll(p_ctrl_roll);

	ctrl_add_rect_to_invrgn(p_parent, &frame);
	ctrl_paint_ctrl(p_parent, FALSE);

	if(close_all_osd_flag)
	{
		ui_time_enable_heart_beat(FALSE);
	}
	return SUCCESS;
}


static RET_CODE on_ca_clear_OSD(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	ui_set_playpg_scrambled(FALSE);
	update_signal();
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
	ctrl_set_rstyle(p_txt_top,RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
	text_set_font_style(p_txt_top, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
	text_set_align_type(p_txt_top,STL_LEFT|STL_VCENTER);
	text_set_content_type(p_txt_top, TEXT_STRTYPE_UNICODE);

	//bottom roll control
	p_txt_bottom = ctrl_create_ctrl(CTRL_TEXT, IDC_BG_MENU_CA_ROLL_BOTTOM,
						CA_ROLLING_X, CA_ROLLING_BUTTON_Y,
						CA_ROLLING_W,CA_ROLLING_H,
						p_cont, 0);
	ctrl_set_proc(p_txt_bottom, ui_ca_rolling_proc);
	ctrl_set_sts(p_txt_bottom,OBJ_STS_HIDE);
	ctrl_set_rstyle(p_txt_bottom,RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
	text_set_font_style(p_txt_bottom, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
	text_set_align_type(p_txt_bottom,STL_LEFT|STL_VCENTER);
	text_set_content_type(p_txt_bottom, TEXT_STRTYPE_UNICODE);

	return SUCCESS;
}

static void on_ca_msg_start_roll(u8 location, u32 bg_rgb, u32 font_rgb, u16 *p_uni_str)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl =NULL;
	roll_param_t p_param = {0};

	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	if(location == 0)
	{
		p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
	}
	else if(location == 1)
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
		else if(location == 1)
		{
			p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
		}
	}
	
	if(bg_rgb == 0 && font_rgb == 0)
	{
		bg_rgb = 0x0;
		font_rgb = 0xffffffff;
	}
	g_rsc_config.p_rstyle_tab[RSI_ROLL_OSD].bg.value =bg_rgb;
	g_rsc_config.p_fstyle_tab[FSI_ROLL_OSD].color = font_rgb;
	g_rsc_config.p_fstyle_tab[FSI_ROLL_OSD].width = 36;
	g_rsc_config.p_fstyle_tab[FSI_ROLL_OSD].height = 36; 
	ctrl_set_rstyle(p_ctrl, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
	text_set_font_style(p_ctrl, FSI_ROLL_OSD, FSI_ROLL_OSD, FSI_ROLL_OSD);

	ctrl_set_sts(p_ctrl,OBJ_STS_SHOW);
	gui_stop_roll(p_ctrl);
	text_set_content_by_unistr(p_ctrl,p_uni_str);

	p_param.pace = ROLL_SINGLE;
	p_param.style = ROLL_LR;
	p_param.is_force = TRUE;
	p_param.repeats= 1;
	p_param.use_bg = TRUE;
	p_param.bg_color = bg_rgb;
	g_ca_save_osd.is_cur_msg_showing = 1;
	
	gui_start_roll(p_ctrl, &p_param);
	ctrl_paint_ctrl(p_ctrl,TRUE);
}

static RET_CODE on_ca_rolling_stop(control_t *p_parent, u16 msg, u32 para1, u32 para2)
{
	rect_t frame = {0};
	control_t *p_ctrl = NULL;

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

static RET_CODE on_ca_show_osd(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
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

	if((p_ca_msg_info->display_pos == 0) || (p_ca_msg_info->display_pos == 1))
	{
		if(!g_ca_save_osd.is_cur_msg_showing)
		{
			OS_PRINTF("@@@ got roll osd\n");
			on_ca_msg_start_roll(p_ca_msg_info->display_pos, p_ca_msg_info->background_color, p_ca_msg_info->font_color, uni_str);
			if(ui_is_fullscreen_menu(fw_get_focus_id()))
			ui_time_enable_heart_beat(TRUE);
		}
		else
		{
			DEBUG(CAS,INFO,"[enter save function] rolling_msg:%s***\n",p_ca_msg_info->data);
			on_ca_rolling_msg_save(p_ca_msg_info->osd_display.roll_mode, p_ca_msg_info->display_time, p_ca_msg_info->display_pos, uni_str);
			on_ca_rolling_stop(p_ctrl, msg, para1, para2);
		}
	}
	else
	{
		OS_PRINTF("@@@  got super osd background_area:%d\n", p_ca_msg_info->background_area);
		memcpy(&g_ca_super_osd_info, p_ca_msg_info, sizeof(msg_info_t));
		OS_PRINTF("@@@@@  got super osd background_area:%d\n", p_ca_msg_info->background_area);

		if(p_ca_msg_info->class == 1) //force osd set ir disable
		{
			g_ca_force_super_osd = TRUE;
		}
		on_ca_show_super_osd(p_ctrl, msg, para1, para2);
	}
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






RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  //watch_limit_t *curn_watch_limit = &sys_status_get()->watch_limit;
  DEBUG(CAS,INFO,"on_ca_init_ok g_is_smart_card_removed:%d\n", g_is_smart_card_removed);
  ui_set_smart_card_insert(TRUE);
  if(finger_print_flag)
  {
    if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
    {
      fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
    }
    #ifndef WIN32
    reopen_finger(0, 0);
    #endif
    finger_print_flag = 0;
  }
 return SUCCESS;
 // if(curn_watch_limit->type == CONTIBUS_WATCH_LIMIT_ENABLE && g_is_smart_card_removed)
  {
   // if(curn_watch_limit->is_work_time)
    {
     // if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->work_time) != SUCCESS)
      {
     //    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->work_time, FALSE);
      }
    }
//    else
    {
      ui_stop_play(STOP_PLAY_BLACK,TRUE);
      //
      update_ca_message(IDS_CA_WATCH_LIMIT);
    //  if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->stop_time) != SUCCESS)
      {
    //     fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->stop_time, FALSE);
      }
    }
  }

  //add delay for card info require, 3seconds delay after got 'card init ok' msg
  if(CUSTOMER_KINGVON == CUSTOMER_ID)
  {
    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_REQUIRE_CARD_INFO, 3000, FALSE);
  }
  return SUCCESS;
}

void check_watch_limit()
{
 // watch_limit_t *curn_watch_limit = &sys_status_get()->watch_limit;
 // if(curn_watch_limit->type == CONTIBUS_WATCH_LIMIT_ENABLE)
  {
    update_ca_message(RSC_INVALID_ID);
    fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK);
  }
}

static void on_stop_normal_play()
{	
	//watch_limit_t *curn_watch_limit = &sys_status_get()->watch_limit;
	u8 curn_mode = sys_status_get_curn_prog_mode();
	if(curn_mode != CURN_MODE_NONE)
	{
	  DEBUG(CAS,INFO,"on_stop_normal_play\n");
		ui_stop_play(STOP_PLAY_BLACK,TRUE);/*yxsb的io并没有处理CAS_CMD_STOP*/		
		//
		update_ca_message(IDS_CA_WATCH_LIMIT);
		//	curn_watch_limit->is_work_time = 0;	
		sys_status_save();		
		//if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->stop_time) != SUCCESS)
		{
	   //  fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->stop_time, FALSE);
		}
	}	
	OS_PRINTF("[CA Public][%s %d]	 tick= %lu\n", __FUNCTION__, __LINE__,mtos_ticks_get()); 	
}

static void on_start_normal_play()
{		
 // watch_limit_t *curn_watch_limit = &sys_status_get()->watch_limit;
  DEBUG(CAS,INFO,"on_start_normal_play\n");
  //	
  //curn_watch_limit->is_work_time = 1; 
  sys_status_save();  
  update_ca_message(RSC_INVALID_ID);
  ui_play_curn_pg();
  //if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->work_time) != SUCCESS)
  {
   //  fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->work_time, FALSE);
  }

  OS_PRINTF("[CA Public][%s %d]	 tick= %lu\n", __FUNCTION__, __LINE__,mtos_ticks_get()); 	
}


static RET_CODE on_check_continue_watch_limit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{	
	//watch_limit_t *curn_watch_limit = &sys_status_get()->watch_limit;
		
//	OS_PRINTF("[CA Public][%s %d]	sd_watch_limit.type=%d\n", __FUNCTION__, __LINE__, curn_watch_limit->type);  

	if(!g_is_smart_card_insert)
	{
	    OS_PRINTF("[CA Public][%s %d]smart card is removed.\n", __FUNCTION__, __LINE__);  
	    return SUCCESS;
	}
	
	//if(curn_watch_limit->is_work_time)   	//stop play when work time
	{	
		on_stop_normal_play();/* 开始播放限制*/		
	}	
	//else  	//play when stop time
	{
		on_start_normal_play();/* 恢复正常播放*/
	}
	return SUCCESS;
}

#if  0
static RET_CODE on_watch_limit_reboot(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{	
  //watch_limit_t *curn_watch_limit = &sys_status_get()->watch_limit;
  //DEBUG(CAS,INFO,"curn_watch_limit type:%d, is_work_time:%d, work_time:%d, stop_time:%d\n", 
  //                  curn_watch_limit->type, curn_watch_limit->is_work_time, curn_watch_limit->work_time,curn_watch_limit->stop_time);
  //if( (curn_watch_limit->type == CONTIBUS_WATCH_LIMIT_ENABLE) && g_is_smart_card_insert )
  {
  //  if(curn_watch_limit->is_work_time)
    {
   //   if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->work_time) != SUCCESS)
      {
    //     fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->work_time, FALSE);
      }
    }
   // else
    {
      ui_stop_play(STOP_PLAY_BLACK,TRUE);/*yxsb的io并没有处理CAS_CMD_STOP*/   
      //
      update_ca_message(IDS_CA_WATCH_LIMIT);
      //if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->stop_time) != SUCCESS)
      {
      //   fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->stop_time, FALSE);
      }
    }
  }
  return SUCCESS;
}
#endif

static RET_CODE on_continue_watch_limit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	cas_continues_watch_limit_t *watch_limit_info = (cas_continues_watch_limit_t *)para2;	
//	watch_limit_t *curn_watch_limit = &sys_status_get()->watch_limit;
    u8 curn_mode = sys_status_get_curn_prog_mode();
	//OS_PRINTF("[CA Public][%s %d] watch_limit_info->type =%d, work time:%d, stop time:%d\n", 
	//                    __FUNCTION__, __LINE__,watch_limit_info->type, watch_limit_info->work_time, watch_limit_info->stop_time);  
	
	if (watch_limit_info->type == CONTIBUS_WATCH_LIMIT_ENABLE)
	{		
	//	if(curn_watch_limit->type == CONTIBUS_WATCH_LIMIT_ENABLE)
       {
          return SUCCESS;
       }  
    
      //  curn_watch_limit->type	  = CONTIBUS_WATCH_LIMIT_ENABLE;
      //  curn_watch_limit->is_work_time = 1;  
      //  curn_watch_limit->work_time = watch_limit_info->work_time*60 * 60 * 1000;  //ms
      //  curn_watch_limit->stop_time = watch_limit_info->stop_time * 60 * 1000;         //ms
        //curn_watch_limit->work_time = watch_limit_info->work_time * 60 * 1000;  //simulate ms
        //curn_watch_limit->stop_time = watch_limit_info->stop_time * 1000;         //simulate ms

		sys_status_save();
		//有节目才开始计时，如果无节目等到节目播放时候，会重置timer?
		if (curn_mode == CURN_MODE_NONE)
		{     		   	
			OS_PRINTF("[CA Public][%s %d] no channel return.\n", __FUNCTION__, __LINE__);  
			return SUCCESS; 
		}
		else
		{ 
		//    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->work_time, FALSE);
		}	
		OS_PRINTF("[CA Public][%s %d]  tick= %lu\n", __FUNCTION__, __LINE__,mtos_ticks_get());		
	 
	}
	else if(watch_limit_info->type == CONTIBUS_WATCH_LIMIT_CANCEL)
	{
	  // if(curn_watch_limit->type == CONTIBUS_WATCH_LIMIT_ENABLE && !curn_watch_limit->is_work_time)
	   {
         ui_play_curn_pg();
         update_ca_message(RSC_INVALID_ID);
        // if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->work_time) != SUCCESS)
         {
        //    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK, curn_watch_limit->work_time, FALSE);
         }

        // curn_watch_limit->type    = CONTIBUS_WATCH_LIMIT_ENABLE;
        // curn_watch_limit->is_work_time = 1;  
         sys_status_save();
	   }
      OS_PRINTF("[CA Public][%s %d]  cancel watch limit! pos:1\n", __FUNCTION__, __LINE__); 
	}
	else if(watch_limit_info->type == CONTIBUS_WATCH_LIMIT_DISABLE)
	{
     // if(curn_watch_limit->type == CONTIBUS_WATCH_LIMIT_ENABLE && !curn_watch_limit->is_work_time)
      {
        //  
        update_ca_message(RSC_INVALID_ID);
        ui_play_curn_pg();
      } 
      //memset(curn_watch_limit, 0, sizeof(watch_limit_t));   
      sys_status_save();  
      fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_CONTINUE_WATCH_CHECK);
      OS_PRINTF("[CA Public][%s %d]  cancel watch limit! pos:2\n", __FUNCTION__, __LINE__); 
	}	
	OS_PRINTF("[CA Public][%s %d]	 tick= %lu\n", __FUNCTION__, __LINE__,mtos_ticks_get()); 	
	return SUCCESS;
}


//--------关于机顶盒通知消息乃得?
/*
 1.下列功能要看运营商是否打开，认证版本需要有显示系统信息，初始化机顶盒，黑名单
 2.关于黑名单(冻结)其实上来的消息是 CDSTBCA_ShowBuyMessage 0x13 CDCA_MESSAGE_FREEZE_TYPE,
其实是在国内版镜那岸朔⑺偷南ⅲ词乖诠獾陌姹痉⑺拖ⅲ不嵘侠碈DSTBCA_ShowBuyMessage 0x13,
如果需要厥獯硭消?则打开下娴腎NCLUDE_FREEZESTB
3.同样在国外陌姹痉⑺椭仄艋ズ械南ⅲ不嵘侠碈DCA_MESSAGE_STBLOCKED_TYPE 0x20
*/

//#define INCLUDE_RESTART		//同方认证此功能不蚩?
//#define INCLUDE_RESEARCH		//同方认证此功能不打开
//#define INCLUDE_STBUPGRADE	//同方认证此功能不打开
//#define INCLUDE_FREEZESTB	//同方认证此功能不打开

RET_CODE on_stb_notification(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{

	u32  notifyType = para2;	
    OS_PRINTF("[CA Public]%s,line = %d, invoked mtos_ticks_get =%lu notifyType=%d\n", __FUNCTION__, __LINE__,mtos_ticks_get(),notifyType);	

	switch(notifyType)
	{
		case CA_ACTIONREQUEST_INITIALIZESTB:/*初始化*/			
          
		   //restore_to_factory();
			break;
		case CA_ACTIONREQUEST_SHOWSYSTEMINFO:/*显示系统信息*/			
			{
				u8 focus_id = 0;
				focus_id = fw_get_focus_id();
				if(!ui_is_fullscreen_menu(focus_id))
				{
					ui_close_all_mennus();
				}
				manage_open_menu(ROOT_ID_INFO, 0, 0);
			}
			break;
		default:
      		break;
			
	}
	
	return SUCCESS;
}

static RET_CODE on_stb_require_card_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
  
  ui_ca_do_cmd(CAS_CMD_PLATFORM_ID_GET, 0 ,0);
  return SUCCESS;
}

static RET_CODE on_stb_get_card_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  cas_card_info_t * p_card_info = (cas_card_info_t *)para2;

  OS_PRINTF("@@@@@[YXSB]Card SN: %s\n", p_card_info->sn);
  
  memset(ca_card_info.card_sn, 0, (CAS_CARD_SN_MAX_LEN + 1));
  if(memcmp(ca_card_info.card_sn,p_card_info->sn,(CAS_CARD_SN_MAX_LEN + 1)) != 0)
  {
    memcpy(ca_card_info.card_sn, p_card_info->sn, (CAS_CARD_SN_MAX_LEN + 1));
    ca_card_info.is_got_ca_card_num = TRUE;
  }
  else
  {
    return SUCCESS;
  }
  
  if(fw_find_root_by_id(ROOT_ID_SIGNAL) != NULL)
  {
    update_signal();
  }
  return SUCCESS;
}

static RET_CODE on_ca_public_stb_id(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 buf[6] = {0};
  //u8 ascstr[14] = {0};
  u16 *plat_id = (u16 *)para2;
  
  DEBUG(CAS,INFO,"on_conditional_accept_info_plat_id plat_id:0x%x\n", *plat_id);
  buf[0] = (*plat_id) >> 8;
  buf[1] = (*plat_id) & (0xff);
  

  machine_serial_get((u8 *)&buf[2], CDCAS_CA_STDID_SIZE/2);
  
  DEBUG(CAS,INFO,"0x%x%x%x%x%x%x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
  memset(ca_card_info.stb_sn, 0, (CAS_STB_SN_MAX_LEN + 1));
  //lint --e{420}
  if(memcmp(ca_card_info.stb_sn, buf, (CAS_STB_SN_MAX_LEN + 1)) != 0)
  {
    DEBUG(CAS,INFO,"save stb in memory!!!!!\n");
    memcpy(ca_card_info.stb_sn, buf, (CAS_STB_SN_MAX_LEN + 1));
    ca_card_info.is_got_stb_num = TRUE;
    DEBUG(CAS,INFO,"save stb in memory : 0x%02x%02x%02x%02x%02x%02x!!!!!\n",
              ca_card_info.stb_sn[0], ca_card_info.stb_sn[1],
              ca_card_info.stb_sn[2], ca_card_info.stb_sn[3],
              ca_card_info.stb_sn[4], ca_card_info.stb_sn[5]);
  }
  else
  {
    return SUCCESS;
  }
  if(fw_find_root_by_id(ROOT_ID_SIGNAL) != NULL)
  {
    update_signal();
  }

  return SUCCESS;
}

//#define TEST_CA
#ifdef  TEST_CA
#include "CDCASS.h"

enum{
	TEST_CA_CARD_UPGRADE,
	TEST_CA_IPPV,
};

static RET_CODE on_ca_test_ca(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"\n");
	switch(TEST_CA_IPPV)
	{
		case TEST_CA_CARD_UPGRADE:
		{
			static u8 progress = 0, type = 1;
			
			CDSTBCA_ShowProgressStrip(progress,type);
			DEBUG(CAS,INFO,"progress[%d]\n",progress);

			if(progress == 100)
			{
				if(type == 1)
					type = 2;
				else
					type = 1;
			}
			progress = (progress + 10)%120;
		}
			break;
				
		case TEST_CA_IPPV:
		{
			static ipp_buy_info_t p_ipp_buy_info = {0};
			static u8 type = 0,price_num = 1;
			u8 i;

			p_ipp_buy_info.price_num = price_num;
			if(price_num++ == 2)
			{
				price_num = 1;
				p_ipp_buy_info.message_type = (type++)%3;
			}

			p_ipp_buy_info.burse_id = 3;
			for(i=0; i<p_ipp_buy_info.price_num; i++)
			{
				if(i == 0)
				{
					p_ipp_buy_info.ipp_price[i].price_type 	= CDCA_IPPVPRICETYPE_TPPVVIEW;
					p_ipp_buy_info.ipp_price[i].price		= 88;
				}
				else
				{
					p_ipp_buy_info.ipp_price[i].price_type 	= CDCA_IPPVPRICETYPE_TPPVVIEW;
					p_ipp_buy_info.ipp_price[i].price		= 102;
				}
			}

			p_ipp_buy_info.product_id 	= 2;
			p_ipp_buy_info.ecm_pid 		= 0xffff;

			p_ipp_buy_info.tvs_id		= 3;
			
			p_ipp_buy_info.expired_time[0] = 20;
			p_ipp_buy_info.expired_time[1] = 15;
			p_ipp_buy_info.expired_time[2] = 12;
			p_ipp_buy_info.expired_time[3] = 10;

			manage_open_menu(ROOT_ID_CA_BOOK_IPP_REAL_TIME,0,(u32)&p_ipp_buy_info);
		}
			
			break;
			
		default:
			DEBUG(CAS,INFO,"\n");
			break;
	}
	DEBUG(CAS,INFO,"\n");
	return SUCCESS;
}
#endif

BEGIN_MSGPROC(ui_desktop_proc_cas, cont_class_proc)
	ON_COMMAND(MSG_CA_INIT_OK, on_ca_init_ok)
	ON_COMMAND(MSG_CA_CLEAR_DISPLAY, on_ca_clear_OSD)
	ON_COMMAND(MSG_CA_EVT_NOTIFY, on_ca_message_update)
	ON_COMMAND(MSG_CA_CLEAR_DISPLAY, on_ca_clear_OSD)
	ON_COMMAND(MSG_CA_FINGER_INFO, on_ca_finger_update)
	ON_COMMAND(MSG_CA_HIDE_FINGER, on_ca_finger_update)
	ON_COMMAND(MSG_CA_FINGER_BEAT, on_finger_flash)
	ON_COMMAND(MSG_CA_FINGER_BEAT2, on_finger_flash)
	ON_COMMAND(MSG_CA_FORCE_CHANNEL_INFO, on_ca_force_channel)
	ON_COMMAND(MSG_CA_UNFORCE_CHANNEL_INFO, on_ca_force_channel)
	ON_COMMAND(MSG_CA_UNLOCK_PARENTAL, on_conditional_accept_unlock_parental)  
	ON_COMMAND(MSG_CA_IPP_BUY_INFO, on_ipp_buy_info)
	ON_COMMAND(MSG_CA_IPP_HIDE_BUY_INFO, on_ipp_hide_buy_info)
	ON_COMMAND(MSG_CA_RATING_INFO, on_ca_level_update)
	ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, on_new_mail)
	ON_COMMAND(MSG_CA_SHOW_OSD, on_ca_show_osd)
	ON_COMMAND(MSG_CA_HIDE_OSD_UP, on_ca_hide_osd)
	ON_COMMAND(MSG_CA_HIDE_OSD_DOWN, on_ca_hide_osd)
	ON_COMMAND(MSG_CA_HIDE_OSD, on_ca_hide_super_osd)
	//ON_COMMAND(MSG_CA_SHOW_SENIOR_PREVIEW, on_ca_show_senior_preview)
	//ON_COMMAND(MSG_CA_HIDE_SENIOR_PREVIEW, on_ca_hide_senior_preview)
	ON_COMMAND(MSG_CA_CARD_UPDATE_PROGRESS, on_ca_card_update)
	ON_COMMAND(MSG_CA_CONTINUE_WATCH_LIMIT, on_continue_watch_limit)
	ON_COMMAND(MSG_CA_CONTINUE_WATCH_CHECK, on_check_continue_watch_limit)  
	//ON_COMMAND(MSG_WATCH_LIMIT_REBOOT, on_watch_limit_reboot)  
	ON_COMMAND(MSG_CA_STB_NOTIFICATION, on_stb_notification)  
	ON_COMMAND(MSG_CA_REQUIRE_CARD_INFO,on_stb_require_card_info)
	ON_COMMAND(MSG_CA_CARD_INFO, on_stb_get_card_info)
	ON_COMMAND(MSG_CA_PLATFORM_ID, on_ca_public_stb_id)

#ifdef  TEST_CA
	ON_COMMAND(MSG_CA_DEBUG, on_ca_test_ca)
#endif
END_MSGPROC(ui_desktop_proc_cas, cont_class_proc)

BEGIN_MSGPROC(ui_ca_rolling_proc, text_class_proc)
	ON_COMMAND(MSG_ROLL_STOPPED, on_ca_rolling_over)
END_MSGPROC(ui_ca_rolling_proc, text_class_proc)

BEGIN_KEYMAP(ui_desktop_keymap_cas, NULL)
	ON_EVENT(V_KEY_HOT_XREFRESH, MSG_CA_DEBUG)
END_KEYMAP(ui_desktop_keymap_cas, NULL)




