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
#include "ui_nprog_bar.h"
#include "ui_entitle_info.h"
#include "ui_ca_finger.h"
#include "ui_ca_public.h"
#include "cas_manager.h"
#include "ui_new_mail.h"
#include "config_cas.h"
#include "cas_ware.h"

#define MAX_ON_SCREEN_MSG_LENGTH 320

static u32 g_ca_last_msg = CAS_S_CLEAR_DISPLAY;
static BOOL g_is_smart_card_insert = FALSE;
static BOOL g_is_smart_card_status = FALSE;

static cas_sid_t g_ca_sid;
static BOOL g_ca_check_start = FALSE;
static BOOL g_is_finger_show = FALSE;
static finger_msg_t g_ca_finger_msg = {{0,},0,};
static BOOL g_osd_stop_msg_unsend = FALSE;
static BOOL g_is_force_channel = FALSE;
static BOOL g_ca_msg_rolling_over = TRUE;
static u8 g_is_roll_bottom_show = 0;
static u8 g_is_roll_top_show = 0;
static msg_info_t g_ca_msg_info;
extern rsc_rstyle_t c_tab[MAX_RSTYLE_CNT];
extern rsc_fstyle_t f_tab[FSTYLE_CNT];
enum background_menu_ctrl_id
{
  /* value 1 for TITLE_ROLL*/
  IDC_BG_MENU_CA_ROLL_TOP = 2,
  IDC_BG_MENU_CA_ROLL_BOTTOM = 3,
};

extern void ui_desktop_start(void);

 RET_CODE ui_ca_rolling_proc(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);

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

  OS_PRINTF("CA: set sid pgid: %d\n", p_ca_sid->pgid);

  memcpy(&g_ca_sid, p_ca_sid, sizeof(cas_sid_t));

  cmd.data1 = (u32)(&g_ca_sid);
  
  ap_frm_do_command(APP_CA, &cmd);
}

static void ui_release_ap(void)
{
  ui_time_release();
  ui_epg_stop();
  ui_release_signal();
  ui_enable_uio(FALSE);
}

static void ui_activate_ap(void)
{
  ui_time_init();
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
  ui_init_signal();
  ui_enable_uio(TRUE);
}

void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2)
{
    /*stop all ap task,wait ca zone check end*/

    cmd_t cmd = {0};
    dvbc_lock_t tp_set = {0};
    static nim_channel_info_t nim_channel_info = {0};
    control_t *p_ctrl = NULL;

    ui_desktop_start();
    return;

    
    p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
    sys_status_get_main_tp1(&tp_set);
    nim_channel_info.frequency = tp_set.tp_freq;
    nim_channel_info.param.dvbc.symbol_rate = tp_set.tp_sym;
    nim_channel_info.param.dvbc.modulation = tp_set.nim_modulate;

    ui_release_ap();
    g_ca_check_start = TRUE;

    cmd.id = cmd_id;
    cmd.data1 = (u32)(&nim_channel_info);
    cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_DVBCA);
    OS_PRINTF("\r\n***ui_ca_zone_check_start cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",cmd.id,cmd.data1,cmd.data2);
    ap_frm_do_command(APP_CA, &cmd);
}

void on_ca_zone_check_end(void)
{
    return;

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
void ui_cas_factory_set(u32 cmd_id, u32 para1, u32 para2)
{
  #if 0
  cmd_t cmd = {0};

  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_BY);
  OS_PRINTF("\r\n***ui_cas_factory_set cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",
                    cmd.id,cmd.data1,cmd.data2);
  ap_frm_do_command(APP_CA, &cmd);
  #endif
}

static RET_CODE on_ipp_buy_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 focus_id = 0;
  u16 pg_id = INVALIDID;
  dvbs_prog_node_t prog = {0};
  ipp_buy_info_t *p_ipp_buy_info = NULL;

  focus_id = fw_get_focus_id();
  p_ipp_buy_info = (ipp_buy_info_t *)para2;

  pg_id = sys_status_get_curn_group_curn_prog_id();
  db_dvbs_get_pg_by_id(pg_id, &prog);
  #ifndef WIN32
  UI_PRINTF("on_ipp_buy_info s_id[%d], channel_id[%d] pmt:%d pcr:%d tp:%d video:%d pg:%d\n", prog.s_id, p_ipp_buy_info->channel_id,
     prog.pmt_pid, prog.pcr_pid, prog.tp_id, prog.video_pid, pg_id);
  #endif

  if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
  {
    fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
  }
  
  if((ui_is_fullscreen_menu(focus_id)) && (focus_id != ROOT_ID_CA_IPPV_PPT_DLG))
  {
    manage_open_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, para2);
  }

  return SUCCESS;
}

static RET_CODE on_ipp_hide_buy_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  UI_PRINTF("on_ipp_hide_buy_info\n");
  manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG, 0, 0);
  return SUCCESS;
}

static RET_CODE on_ca_prog_oper(u32 event_id)
{
  switch(event_id)
  {
    case CAS_C_IEXIGENT_PROG:
      UI_PRINTF("[cas play]..... play current programe \n");
	  OS_PRINTF("g_ca_check_start = %d \n", g_ca_check_start);
      if(!g_ca_check_start&& is_ap_playback_active())
      	{
      		  ui_play_curn_pg();
    

      	}
	break;
    /*
    case CAS_C_IEXIGENT_PROG_STOP:
      UI_PRINTF("[cas stop play]..... stop current programe \n");
      ui_stop_play(STOP_PLAY_BLACK, TRUE);
    break;
    */
    default:
      UI_PRINTF("[cas stop play].....do nothing \n");
    break;
}
  return SUCCESS;
}

RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  u32 event_id = para2;

  DEBUG(CAS, INFO, "\r\n*** ca_message_update  g_ca_last_msg[0x%x],event_id[0x%x]\r\n",
                    g_ca_last_msg,event_id);

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
  	case CAS_E_ZONE_DISPLAY:
		update_ca_message(IDS_CA_ZONECDN_LIMIT_CTRL_ENABLE);
		break;
	case CAS_E_PASSWORD_ERROR:
		update_ca_message(IDS_DVB_OPERATOR_PASSWORD_ERROR);
		break;
    case CAS_S_ZONE_CHECK_END:
      DEBUG(CAS, INFO, "\r\n*** %s :CAS_S_ZONE_CHECK_END ",__FUNCTION__);
      on_ca_zone_check_end();
      break;
    case CAS_C_SHOW_NEW_EMAIL:
      DEBUG(CAS, INFO, "@@@CAS_C_SHOW_NEW_EMAIL @@@ \n");
      if(ui_is_fullscreen_menu( fw_get_focus_id()))
      {
        ui_set_new_mail(TRUE);
        update_email_picture(EMAIL_PIC_SHOW);
      }
      break;
    case CAS_C_HIDE_NEW_EMAIL:
      DEBUG(CAS, INFO, "@@@CAS_C_HIDE_NEW_EMAIL @@@ \n");
      ui_set_new_mail(FALSE);
      update_email_picture(EMAIL_PIC_HIDE);
      break;
    case CAS_C_EMAIL_FULL:
      OS_PRINTF("@@@CAS_C_SHOW_SPACE_EXHAUST @@@ \n");
      if(ui_is_fullscreen_menu( fw_get_focus_id()))
      {
         update_email_picture(EMAIL_PIC_FLASH);
      }
      break;
    case CAS_E_CARD_REGU_INSERT:
      DEBUG(CAS, INFO, "加密频道, 请插入智能卡\n");
      update_ca_message(IDS_CA_CARD_REGU_INSERT);
      break;
    case CAS_E_ILLEGAL_CARD:
	  DEBUG(CAS, INFO, "无法识别卡,不能使用\n");
      update_ca_message(IDS_CA_UNKNOW_SMC);
      break;
    case CAS_E_CARD_OVERDUE:
      update_ca_message(IDS_CA_CARD_OVER_DATE);
      break;
    case CAS_E_CARD_INIT_FAIL:
      DEBUG(CAS, INFO, "智能卡初始化失败\n");
      update_ca_message(IDS_CA_CARD_INIT_FAIL);
      break;
    case CAS_E_NO_OPTRATER_INFO:
      update_ca_message(IDS_CA_NO_SP_IN_SMC);
      break;
    case CAS_E_CARD_FREEZE:
      update_ca_message(IDS_CA_CARD_STOP);
      break;
    case CAS_E_CARD_OUT_WORK_TIME:
      update_ca_message(IDS_CA_CARD_OUT_WORK_TIME);
      break;
    case CAS_E_CARD_CHILD_UNLOCK:
      update_ca_message(IDS_CA_PROG_HIGH_WATCH_LEVEL);
      break;
    case CAS_E_CARD_MODULE_FAIL:
      update_ca_message(IDS_CA_STB_SN_NOT_MATCH);
      break;
    case CAS_E_PROG_UNAUTH:
      update_ca_message(IDS_CA_NO_PURCH_PROG);
      break;  
    case CAS_E_CW_INVALID:
      update_ca_message(IDS_CA_DECRYPTION_FAIL);   
      break; 
    case CAS_E_CADR_NO_MONEY:
      update_ca_message(IDS_CA_NO_MONEY);
      break; 
    case CAS_E_CARD_SLEEP:
      update_ca_message(IDS_CA_NO_ENABLE);
      break; 
    case CAS_E_CARD_REG_FAILED:
      update_ca_message(IDS_CA_DELETE_CARD);
      break; 
    case CAS_S_CARD_PARTNER_SUCCESS:
      update_ca_message(IDS_CA_MASTERSLAVE_PAIRED_OK);
      break; 
    case CAS_E_CARD_PARTNER_FAILED:
      update_ca_message(IDS_CA_CARD_PARTNER_FAILED);
      break; 
    case CAS_E_CARD_DIS_PARTNER:
      update_ca_message(IDS_CA_SLAVE_NEED_MASTER);
      break; 
    case CAS_E_CARD_MATCH_OK:
      update_ca_message(IDS_CA_CARD_PARTNER_SUCCESS);
      break; 
    case CAS_E_CARD_REMATCH:
      update_ca_message(IDS_CA_REPAIR_STB);
      break;   
    case CAS_E_CARD_REMATCH_CANCEL:
      update_ca_message(IDS_CA_CARD_REMATCH_CANCEL);
      break; 
   // case CAS_C_EMAIL_FULL:
   //   update_ca_message(IDS_CA_EMAIL_NO_SPACE);
    //  break; 
    case CAS_C_CARD_UNLOCK_OK:
      update_ca_message(IDS_CA_SMART_UNLOCK);
      break; 
    case CAS_C_PURSER_UPDATE:
      update_ca_message(IDS_CA_WALLET_RENEW);
      break; 
    case CAS_E_CARD_AUTHEN_OVERDUE:
      {
         comm_dlg_data_t overdue_data = //popup dialog data
        {
          ROOT_ID_INVALID,
          DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
          COMM_DLG_X, COMM_DLG_Y,
          COMM_DLG_W, COMM_DLG_H,
          IDS_CA_CARD_NO_MONEY,
          0,
        };
        ui_comm_dlg_open(&overdue_data);
      }
      //update_ca_message(IDS_CA_CARD_NO_MONEY);
      break; 
    case CAS_C_IPP_OVERDUE:
      update_ca_message(IDS_CA_CARD_IPPV_OUT_TIME);
      break; 
     case CAS_S_CLEAR_DISPLAY:
      ui_set_playpg_scrambled(FALSE);
      update_ca_message(RSC_INVALID_ID);
      #ifdef TEMP_SUPPORT_DS_AD
        UI_PRINTF("CAS_S_CLEAR_DISPLAY,set unauth ad status to false!! \n");
        set_unauth_ad_status(FALSE);
      #endif
      break;
    case CAS_S_ADPT_CARD_INSERT:
      g_ca_last_msg = CAS_S_CLEAR_DISPLAY;
      ui_set_smart_card_state(TRUE);
      update_ca_message(RSC_INVALID_ID);
      break;
    case CAS_C_IEXIGENT_PROG:
    //case CAS_C_IEXIGENT_PROG_STOP:
      on_ca_prog_oper(event_id);
      break;
    case CAS_S_CARD_UPG_START:
      update_ca_message(IDS_CA_CARD_INIT);
      break;
    case CAS_S_ADPT_CARD_REMOVE:
      DEBUG(CAS, ERR, "加密频道, 请插入智能卡\n");
      ui_set_smart_card_state(FALSE);
      update_ca_message(IDS_CA_CARD_REGU_INSERT);
      break;
    case CAS_C_CARD_NEED_UPDATE:
      update_ca_message(IDS_CA_CARD_NEED_UPDATE);
      break;	
    case CAS_E_CARD_NOT_FOUND:
      update_ca_message(IDS_CA_CARD_NOT_INSERT);
      break;
    case CAS_E_CARD_PAIROTHER:
      update_ca_message(IDS_CA_STB_SN_NOT_MATCH);
      break;
    case CAS_E_AUTHEN_UPDATE:
      update_ca_message(IDS_CA_CARD_ENTITLE_UPDATE);
      break;
    case CAS_E_NO_AUTH_DATA:
      update_ca_message(IDS_CA_CARD_CONDITIONAL_LIMIT);
      break;
    case CAS_C_PNET_ERR_LOCK:
      update_ca_message(IDS_CA_CARD_NET_ERR_LOCK);
      break;
    case  CAS_C_SYSTEM_ERR_LOCK:
      update_ca_message(IDS_CA_CARD_SYS_ERR_LOCK);
      break;
    case  CAS_C_PNET_ERR_UNLOCK:
      update_ca_message(IDS_CA_CARD_NET_ERR_UNLOCK);
      break;
    case CAS_C_SYSTEM_ERR_UNLOCK:
      update_ca_message(IDS_CA_CARD_SYS_ERR_UNLOCK);
      break;
    case  CAS_E_AUTHEN_EXPIRED:
      update_ca_message(IDS_CA_E_CARD_ENTITLE_EXPIRED);
      break; 
    case CAS_C_CARD_ADDR_CTL_OPEN:
      update_ca_message(IDS_CA_CARD_ADDR_CTL_OPEN);
      break;
    case CAS_C_CARD_ADDR_CTL_CLOSE:
      update_ca_message(IDS_CA_CARD_ADDR_CTL_CLOSE);
      break;
    case CAS_E_INVALID_PRICE:
      update_ca_message(IDS_CA_CARD_WRITEPURSER_ERROR);
      break;
    case CAS_E_CARD_MONEY_LACK:
      update_ca_message(IDS_CA_CARD_IPPV_NO_MONEY);
      break;         
    case CAS_C_CARD_CHANGEPRO_TOO_FAST:
      UI_PRINTF("on_ca_message_update CAS_C_CARD_CHANGEPRO_TOO_FAST\n");
      update_ca_message(IDS_CA_VIEWLOCK);
      break;
	case CAS_C_CARD_ZONE_INVALID:
	  OS_PRINTF("%s, %d 您的智能卡不在此服务区\n", __FUNCTION__, __LINE__);
      update_ca_message(IDS_CA_CARD_ZONE_INVALID);
	  break;
	case CAS_C_CARD_ZONE_CHECK_ENTER:
	  OS_PRINTF("%s, %d 正在寻找网络，请等待\n", __FUNCTION__, __LINE__);
      update_ca_message(IDS_CA_CARD_ZONE_CHECK_ENTER);
	  break;
	case CAS_C_CARD_ZONE_CHECK_OK:
	  OS_PRINTF("%s, %d 漫游功能卡检测正常\n", __FUNCTION__, __LINE__);
      update_ca_message(IDS_CA_CARD_ZONE_CHEKC_OK);
	  break;
	case CAS_E_ZONE_CODE_ERR:
	  OS_PRINTF("%s, %d 该节目被区域限播\n", __FUNCTION__, __LINE__);
      update_ca_message(IDS_CA_PROG_AREA_LIMIT);
	  break;
	case CAS_C_USERCDN_LIMIT_CTRL_ENABLE:
	  OS_PRINTF("%s, %d 该用户被条件限播\n", __FUNCTION__, __LINE__);
      update_ca_message(IDS_CA_USERCDN_LIMIT_CTRL_ENABLE);
	  break;
	case CAS_C_USERCDN_LIMIT_CTRL_DISABLE:
	  OS_PRINTF("%s, %d 该用户被取消条件限播\n", __FUNCTION__, __LINE__);
      update_ca_message(IDS_CA_USERCDN_LIMIT_CTRL_DISABLE);
	  break;
	case CAS_C_ZONECDN_LIMIT_CTRL_ENABLE:
	  OS_PRINTF("%s, %d 该区域被条件限播\n", __FUNCTION__, __LINE__);
      update_ca_message(IDS_CA_ZONECDN_LIMIT_CTRL_ENABLE);
	  break;
	case CAS_C_ZONECDN_LIMIT_CTRL_DISABLE:
	  OS_PRINTF("%s, %d 该区域被取消条件限播\n", __FUNCTION__, __LINE__);
      update_ca_message(IDS_CA_ZONECDN_LIMIT_CTRL_DISABLE);
	  break;
    //add 2014.11.30
    case CAS_E_CARD_DATA_GET:// 智能卡数据错误
	  OS_PRINTF("智能卡数据错误!\n");      
	  update_ca_message(IDS_CA_CARD_DATA_ERROR);
	  break;
    case CAS_E_NO_AUTH_STB:
	   OS_PRINTF("CAS_E_NO_AUTH_STB\n");
	   update_ca_message(IDS_CA_NO_AUTH_STB);
	   break;
   case CAS_E_UNKNOWN_ERROR://未知错误
       OS_PRINTF("！！！！！未知错误\n");
	   update_ca_message(IDS_CAS_E_UNKNOWN_ERROR);
	   break;

   case CAS_E_PAIRED_OTHERCARD_ERROR:
   case CAS_E_PAIRED_OTHERSTB_ERROR:
   	   OS_PRINTF("\n机卡不配对\n");
	   update_ca_message(IDS_CA_NEED_SMC_STB_PAIR);
	   break;
   case CAS_S_CARD_STB_PARTNER:
       OS_PRINTF("\n节目机卡配对成n");
	   update_ca_message(IDS_CA_STB_CADR_MATCH_SUCCESS);
	   break;
   case CAS_C_CANCEL_PAIRED_OK:
	   update_ca_message(IDS_CA_STB_CADR_DISMATCH_SUCCESS);
	   break;  
   case CAS_C_MASTERSLAVE_PAIRED_OK://子母卡配对成功
	   OS_PRINTF("\n子母卡配对成功\n");
	   update_ca_message(IDS_CA_CARD_PAIR_OK);
	   break;
   case CAS_C_MASTERSLAVE_PAIRED_TIMEUPDATE://子母卡配对时间更新
	   OS_PRINTF("\n子母卡配对时间更新\n");
	   update_ca_message(IDS_CA_CARD_PAIR_TIME_UPDATE);
	   break;
   case CAS_C_MASTERSLAVE_NEEDPAIRED://子母卡需要配对
	   OS_PRINTF("\n子母卡需要配对\n");
	   update_ca_message(IDS_CA_CARD_NEED_PAIR);
	   break;
   case CAS_C_PPC_LESS:
   	   OS_PRINTF("\n授权即将到期\n");
	   update_ca_message(IDS_DVB_ENTITLE_EXPIRING);
	   break;	
    default:
      DEBUG(CAS, INFO, "\n");
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
 
static RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  DEBUG(MAIN,INFO,"on_ca_init_ok\n");
  ui_set_smart_card_insert(TRUE);
  fw_tmr_create(ROOT_ID_BACKGROUND, MSG_TIMER_CARD_OVERDUE, 10*1000, FALSE);
  return SUCCESS;
}
 
static RET_CODE on_ca_clear_OSD(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
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

RET_CODE on_ca_force_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
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

  if(0 == p_mail_header->max_num)
  {
    ui_set_new_mail(FALSE);
    return SUCCESS;
  }

  if(0xFFFF != p_mail_header->max_num)
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
    else
    {
        ui_set_new_mail(FALSE);
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
      ctrl_set_mrect(p_ctrl_top, 0, 0, (frm.right - frm.left), (frm.bottom - frm.top));
      break;
    case 1://bottom
      ctrl_get_frame(p_ctrl_bottom, &frm);
      frm.top = frm.bottom - ctrl_h;
      ctrl_set_frame(p_ctrl_bottom, &frm);
      ctrl_set_mrect(p_ctrl_bottom, 0, 0, (frm.right - frm.left), (frm.bottom - frm.top));
      break;
    case 2://top + bottom
      ctrl_get_frame(p_ctrl_top, &frm);
      frm.bottom = frm.top + ctrl_h;
      ctrl_set_frame(p_ctrl_top, &frm);
      ctrl_set_mrect(p_ctrl_top, 0, 0, (frm.right - frm.left), (frm.bottom - frm.top));
      
      ctrl_get_frame(p_ctrl_bottom, &frm);
      frm.top = frm.bottom - ctrl_h;
      ctrl_set_frame(p_ctrl_bottom, &frm);
      ctrl_set_mrect(p_ctrl_bottom, 0, 0, (frm.right - frm.left), (frm.bottom - frm.top));
      break;
    default:
      DEBUG(CAS_OSD, INFO,"error display_pos , do nothing!!!!!\n");
      break;
  }
}

void on_desktop_start_roll_position(u16 *uni_str, u8 *display_back_color, u8 *display_color, u8 *display_front_size, u8 *display_pos)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl_top = NULL;
  control_t *p_ctrl_bottom = NULL;
  roll_param_t p_param = {0};
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  u32 color = 0;
  u8 i = 0;
  BOOL bcolor = FALSE;
  for(i=0;i<4;i++)
  {
      if(display_back_color[i] != display_color[i])
      {
          bcolor = TRUE;
          break;
      }
  }
  color = (display_back_color[3] <<24) + (display_back_color[0] << 16)
          + (display_back_color[1] << 8) + (display_back_color[2]);  
  
  c_tab[RSI_ROLL_OSD].bg.value = color;  

  if(!bcolor)
  {
      color = 0xffffffff;/*background color is the same as font color ;default font color is white*/
  }
  else
  {
      color = (display_color[3] <<24) + (display_color[0] << 16)
                + (display_color[1] << 8) + (display_color[2]); 
  }

  f_tab[FSI_ROLL_OSD1].color = color;	
  
  on_desktop_roll_set_font_size_and_ctrl_h(display_front_size, display_pos);

  if(*display_pos == 0)
  {
    p_ctrl_top = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
    text_set_font_style(p_ctrl_top, FSI_ROLL_OSD1, FSI_ROLL_OSD1, FSI_ROLL_OSD1);
    ctrl_set_rstyle(p_ctrl_top, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
    gui_stop_roll(p_ctrl_top);
    ctrl_set_attr(p_ctrl_top, OBJ_STS_SHOW);
    text_set_content_by_unistr(p_ctrl_top, uni_str);
    p_param.pace = ROLL_SINGLE;
    p_param.style = ROLL_LR;
    p_param.repeats= 1;
    p_param.is_force = 1;
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
    ctrl_set_attr(p_ctrl_bottom, OBJ_STS_SHOW);
    text_set_content_by_unistr(p_ctrl_bottom, uni_str);
    p_param.pace = ROLL_SINGLE;
    p_param.style = ROLL_LR;
    p_param.repeats= 1;
    p_param.is_force = 1;
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
    ctrl_set_attr(p_ctrl_top, OBJ_STS_SHOW);
    ctrl_set_attr(p_ctrl_bottom, OBJ_STS_SHOW);
    text_set_content_by_unistr(p_ctrl_top, uni_str);
    text_set_content_by_unistr(p_ctrl_bottom, uni_str);

    p_param.pace = ROLL_SINGLE;
    p_param.style = ROLL_LR;
    p_param.repeats= 1;
    p_param.is_force = 1;
    gui_start_roll(p_ctrl_top, &p_param);
    gui_start_roll(p_ctrl_bottom, &p_param);
    ctrl_paint_ctrl(p_ctrl_top,TRUE);
    ctrl_paint_ctrl(p_ctrl_bottom,TRUE);

    g_is_roll_top_show = 1;
    g_is_roll_bottom_show = 1;
    g_ca_msg_rolling_over = FALSE;

  }
}

void on_desktop_start_roll(msg_info_t *ca_osd_msg)
{
  msg_info_t *p_ca_msg_info = NULL;
  u16 uni_str[MAX_ON_SCREEN_MSG_LENGTH] = {0};
  p_ca_msg_info = (msg_info_t *)ca_osd_msg;
  g_ca_msg_info.osd_display.roll_value--;
 
 DEBUG(CAS_OSD, INFO, "display_pos:%d\n", p_ca_msg_info->display_pos);
 DEBUG(CAS_OSD, INFO, "background_area:%d\n", p_ca_msg_info->background_area);
 DEBUG(CAS_OSD, INFO, "background_color:%#x\n", p_ca_msg_info->background_color); 
 DEBUG(CAS_OSD, INFO, "color_type:%d\n", p_ca_msg_info->color_type);
 DEBUG(CAS_OSD, INFO, "content_len:%d\n", p_ca_msg_info->content_len);
 DEBUG(CAS_OSD, INFO, "data_len:%d\n", p_ca_msg_info->data_len);
 DEBUG(CAS_OSD, INFO, "data:%s\n", p_ca_msg_info->data);
 DEBUG(CAS_OSD, INFO, "display_back_color:%d %d %d %d\n", p_ca_msg_info->display_back_color[0],p_ca_msg_info->display_back_color[1],p_ca_msg_info->display_back_color[2],p_ca_msg_info->display_back_color[3]);
 DEBUG(CAS_OSD, INFO, "display_color:%#x\n", p_ca_msg_info->display_color);
 DEBUG(CAS_OSD, INFO, "display_front:%d\n", p_ca_msg_info->display_front);
 DEBUG(CAS_OSD, INFO, "display_front_size:%d\n", p_ca_msg_info->display_front_size);
 DEBUG(CAS_OSD, INFO, "display_status:%d\n", p_ca_msg_info->display_status);
 DEBUG(CAS_OSD, INFO, "font_color:%#x\n", p_ca_msg_info->font_color);
 DEBUG(CAS_OSD, INFO, "display_time:%#x\n", p_ca_msg_info->display_time);
 DEBUG(CAS_OSD, INFO, "period:%#x\n", p_ca_msg_info->period);

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
  DEBUG(CAS_OSD, INFO, "type:%d p_ca_msg_info addr[0x%x], msg:[%d]***\r\n", p_ca_msg_info->type, para2, msg); 
  if(p_ca_msg_info->type == 1)
  {

     DEBUG(CAS_OSD, INFO," title = %s content = %s \n",p_ca_msg_info->title,p_ca_msg_info->data);
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
 	DEBUG(CAS_OSD, INFO, "\n");
  	if(p_ca_msg_info == NULL)
  	{
    		return ERR_FAILURE;
  	}
    g_ca_msg_info = (msg_info_t)*p_ca_msg_info;
	if((p_ca_msg_info->display_pos == 0) ||(p_ca_msg_info->display_pos == 1) || (p_ca_msg_info->display_pos == 2))
  	{
  		  if(TRUE == g_ca_msg_rolling_over)
  		 {
			DEBUG(CAS, INFO, "SHOW OSD \n");
			on_desktop_start_roll(p_ca_msg_info);   
			if(ui_is_fullscreen_menu(fw_get_focus_id()))
			{
				ui_time_enable_heart_beat(TRUE);
				DEBUG(CAS_OSD, INFO, "\n");
			}
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

#ifdef TEMP_SUPPORT_DS_AD
  if((TRUE == ui_get_ads_osd_status()) && (!g_is_force_channel))
  {    
    UI_PRINTF("[ui_desktop] roll title continue roll!!!\n");
    ui_ads_osd_roll();
  }
  else
#endif
  {
    ui_ca_rolling_post_stop();
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

RET_CODE on_ca_rolling_over(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  DEBUG(CAS_OSD, INFO, "SHOW OSD \n");
  on_ca_rolling_stop(p_ctrl, msg, para1, para2);
  if(g_ca_msg_info.osd_display.roll_value > 0)
  {
        if(g_ca_msg_info.display_time> 0)
        {
            fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_PVR_OSD, (g_ca_msg_info.display_time)*10000, FALSE);
        }
        else
        {
            on_desktop_start_roll(&g_ca_msg_info);   
            ui_time_enable_heart_beat(TRUE);
        }
  }
  return SUCCESS;
}

RET_CODE on_ca_delay_show_osd(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS_OSD, INFO, "SHOW OSD \n");
	if((g_ca_msg_info.display_pos == 0) ||(g_ca_msg_info.display_pos == 1) || (g_ca_msg_info.display_pos == 2))
	{
		if(TRUE == g_ca_msg_rolling_over)
		{
			DEBUG(CAS, INFO, "SHOW OSD \n");
			on_desktop_start_roll(&g_ca_msg_info);
			fw_tmr_destroy(ROOT_ID_BACKGROUND , MSG_CA_PVR_OSD);
			if(ui_is_fullscreen_menu(fw_get_focus_id()))
				ui_time_enable_heart_beat(TRUE);
		}
	}
	return SUCCESS;
}
static RET_CODE on_ca_stop_cur_pg(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
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

RET_CODE on_ca_card_update(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  RET_CODE ret = ERR_FAILURE;

  ui_close_all_mennus();
  ret = manage_open_menu(ROOT_ID_CA_CARD_UPDATE, 0, para2);

  return ret;
}
static RET_CODE on_ca_finger_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
  finger_msg_t *p_ca_finger_msg = NULL;
  //u8 asc_str[256] = {'1','2','3','4','5','1','2','3','4','5','1','2','3','4','5','1','2','3','4','5'};
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

extern void ui_clean_all_menus_do_nothing(void);

static RET_CODE on_desai_conditional_search(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{

  u8 curr_id = 0;
  curr_id = fw_get_focus_id();
  
  if((ROOT_ID_DO_SEARCH == curr_id) ||
      (ROOT_ID_MANUAL_SEARCH == curr_id)  ||
      (!do_search_is_finish()))
  {
    return ERR_FAILURE;
  }
   ui_clean_all_menus_do_nothing();
   
   manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);

   return SUCCESS;
}

#if 0
static void ca_dtmb_full_search(void)
{
	dvbs_tp_node_t tp[FULL_SCAN_CNT];
	sat_node_t sat_node = {0};
	u16 i = 0;
	BOOL lcn_enable = FALSE;
	
	memset(tp, 0, sizeof(dvbs_tp_node_t) * FULL_SCAN_CNT);
	ui_scan_param_init();
	ui_scan_add_dvbt_sat(FALSE, &sat_node);
	ui_scan_param_add_sat(&sat_node);

	tp[0].freq = 474000;
	for(i = 0; i < FULL_SCAN_CNT; i++)
	{
		tp[i].freq 	   = tp[0].freq + (i * 8000);
		tp[i].sym      = DTMB_HN_SYMBOL_DEFAULT;
		tp[i].sat_id   = sat_node.id;
		tp[i].nim_type = NIM_DVBT;
		ui_scan_param_add_tp(&tp[i]);
	}

	sys_status_get_status(BS_LCN_RECEIVED, &lcn_enable);
	if(TRUE == lcn_enable)
	{
		ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_ALL_TP);
	}
	else
	{
		ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
	}
	
	manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DTMB_FULL, 0);
	mtos_printk("[debug] %s %d \n",__FUNCTION__,__LINE__);
}

extern s32 g_DVBCFullFreqTable_Chi[] ;
static void ca_dvbc_full_search(void)
{
    ui_scan_param_t scan_param;
    sat_node_t sat_node = {0};
    u8 i;

	memset(&scan_param, 0, sizeof(ui_scan_param_t));

	scan_param.tp.sym = 6875;
	scan_param.tp.nim_modulate = NIM_MODULA_QAM64;
	scan_param.nit_type = NIT_SCAN_ONCE;

	ui_scan_param_init();
	ui_scan_add_dvbc_sat(&sat_node);
	ui_scan_param_add_sat(&sat_node);

	for(i =0; i< 94 ; i++)
	{
		scan_param.tp.freq = g_DVBCFullFreqTable_Chi[i];
		ui_scan_param_add_tp(&scan_param.tp);
	}
	ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
	mtos_printk("[debug] %s %d \n",__FUNCTION__,__LINE__);
	manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DVBC_RANGE, 0);
}
#endif

RET_CODE on_ca_upgrade_freq(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#if 0
    u8 i;
    ui_scan_param_t scan_param;
    sat_node_t sat_node = {0};
    cas_upgrade_freq_t *p_upgrade_freq_msg;
    
    p_upgrade_freq_msg = (cas_upgrade_freq_t *)para2;       
    OS_PRINTF("[debug] %s %d num = %d \n",__FUNCTION__, __LINE__,p_upgrade_freq_msg->upgrade_freq_num);

    ui_scan_param_init();
    ui_scan_add_dvbc_sat(&sat_node);
    ui_scan_param_add_sat(&sat_node);

    for(i =0; i< p_upgrade_freq_msg->upgrade_freq_num; i++)
    {
        memset(&scan_param, 0, sizeof(ui_scan_param_t));
        OS_PRINTF("[debug] sym[%d] modulate[%d] freq[%d] \n", p_upgrade_freq_msg->upgrade_freq_info[i].uiSymbolrate, 
            p_upgrade_freq_msg->upgrade_freq_info[i].ucModunation, p_upgrade_freq_msg->upgrade_freq_info[i].uiFrequency);
        
        scan_param.nit_type = NIM_MODULA_AUTO;
        scan_param.tp.sym = p_upgrade_freq_msg->upgrade_freq_info[i].uiSymbolrate;
        scan_param.tp.nim_modulate = p_upgrade_freq_msg->upgrade_freq_info[i].ucModunation;
        scan_param.tp.freq = p_upgrade_freq_msg->upgrade_freq_info[i].uiFrequency;
        
        ui_scan_param_add_tp(&scan_param.tp);

    }
    ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
    manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DVBC_RANGE, 0);
#else
    dlg_ret_t ret;
	u8 index = 0;
	static BOOL upgrade_freq_flag = FALSE;
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_BACKGROUND,
        DLG_FOR_ASK| DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        IDS_DVB_UPDATE_SEARCH_TITLE,
        0,
    };

	index = fw_get_focus_id();
	if((!ui_is_fullscreen_menu(index))
			||(! do_search_is_finish())
			||(ui_is_vod_menu(index)))
	{

		if(upgrade_freq_flag == FALSE)
		{
			fw_tmr_create(ROOT_ID_BACKGROUND,MSG_CA_UPGRADE_FREQ, 10*1000, TRUE);
			upgrade_freq_flag = TRUE;
		}
		return SUCCESS;
	}
	
	if(upgrade_freq_flag == TRUE)
	{
		fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_UPGRADE_FREQ);
		upgrade_freq_flag = FALSE;
	}

	DEBUG(CAS,INFO,"\n");
    ret = ui_comm_dlg_open(&dlg_data);
    if(ret == DLG_RET_YES )
    {
		index = fw_get_focus_id();
		if(index != ROOT_ID_DO_SEARCH)
		{
			ui_clean_all_menus_do_nothing();
			manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);
		}
    }
#endif
    return SUCCESS;
}

RET_CODE on_ca_show_auth_expiring(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#define AUTH_EXPIRING_DLG_X         ((SCREEN_WIDTH - AUTH_EXPIRING_DLG_W) / 2)
#define AUTH_EXPIRING_DLG_Y         ((SCREEN_HEIGHT - AUTH_EXPIRING_DLG_H) / 2)
#define AUTH_EXPIRING_DLG_W        	(420)
#define AUTH_EXPIRING_DLG_H         (140)

	u16 message[64] = {0};
	u16 uni_str[32] = {0};
	u8 asc_str[32] = {0};
	u32 reminds = *(u32*)para2;

	DEBUG(CAS,INFO,"reminds = %d \n",reminds);
    sprintf((char *)asc_str, "您的费用还有");
    gb2312_to_unicode(asc_str,sizeof(asc_str),uni_str,63);
	uni_strcat(message, uni_str,64);
	
	convert_i_to_dec_str(uni_str, reminds); 
	uni_strcat(message, uni_str,64);
	
	sprintf((char *)asc_str, "天即将到期，请及时缴费!");
	gb2312_to_unicode(asc_str,sizeof(asc_str),uni_str,63);
	uni_strcat(message, uni_str,64);
	
	comm_dlg_data_t auth_expiring =
	{
	  ROOT_ID_INVALID,
	  DLG_FOR_SHOW | DLG_STR_MODE_EXTSTR,
	  AUTH_EXPIRING_DLG_X, AUTH_EXPIRING_DLG_Y,
	  AUTH_EXPIRING_DLG_W, AUTH_EXPIRING_DLG_H,
	  (u32)message,
	  3000,
	  0,
	};

	if(ui_is_fullscreen_menu(fw_get_focus_id()))
		ui_comm_dlg_open2(&auth_expiring);

	return SUCCESS;
}


BEGIN_MSGPROC(ui_desktop_proc_cas, cont_class_proc)
	ON_COMMAND(MSG_CA_INIT_OK, on_ca_init_ok)
	ON_COMMAND(MSG_CA_CLEAR_DISPLAY, on_ca_clear_OSD)
	ON_COMMAND(MSG_CA_EVT_NOTIFY, on_ca_message_update)
	ON_COMMAND(MSG_CA_FINGER_INFO, on_ca_finger_update)
	ON_COMMAND(MSG_CA_HIDE_FINGER, on_ca_finger_update)
	ON_COMMAND(MSG_CA_IPP_BUY_INFO, on_ipp_buy_info)
	ON_COMMAND(MSG_CA_IPP_HIDE_BUY_INFO, on_ipp_hide_buy_info)
	ON_COMMAND(MSG_CA_FORCE_CHANNEL_INFO, on_ca_force_channel)
	ON_COMMAND(MSG_CA_UNFORCE_CHANNEL_INFO, on_ca_force_channel)
	ON_COMMAND(MSG_CA_UPGRADE_FREQ, on_ca_upgrade_freq)  
	ON_COMMAND(MSG_CA_SHOW_OSD, on_ca_show_osd)
	ON_COMMAND(MSG_CA_PVR_OSD, on_ca_delay_show_osd)
	ON_COMMAND(MSG_CA_HIDE_OSD, on_ca_hide_osd)
	ON_COMMAND(MSG_CA_CARD_UPDATE_BEGIN, on_ca_card_update)
	ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, on_new_mail)
	ON_COMMAND(MSG_CA_STOP_CUR_PG, on_ca_stop_cur_pg)
	ON_COMMAND(MSG_CA_CONDITION_SEARCH, on_desai_conditional_search)

	ON_COMMAND(MSG_CA_AUTHEN_EXPIRE_DAY, on_ca_show_auth_expiring)
END_MSGPROC(ui_desktop_proc_cas, cont_class_proc)

BEGIN_KEYMAP(ui_desktop_keymap_cas, NULL)
	ON_EVENT(V_KEY_MAIL,MSG_OPEN_MENU_IN_TAB | ROOT_ID_EMAIL_MESS)
END_KEYMAP(ui_desktop_keymap_cas, NULL)

BEGIN_MSGPROC(ui_ca_rolling_proc, text_class_proc)
	ON_COMMAND(MSG_ROLL_STOPPED, on_ca_rolling_over)
END_MSGPROC(ui_ca_rolling_proc, text_class_proc)
