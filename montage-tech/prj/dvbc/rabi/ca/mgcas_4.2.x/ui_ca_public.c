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
#include "ui_manual_search.h"
#include "ui_new_mail.h"
#include "ui_ca_public.h"
#include "ui_finger_print.h"
#include "cas_manager.h"
#include "ui_ca_ppv_icon.h"
#include "ui_ca_ppv.h"
#include "ui_conditional_entitle_expire.h"
#include "ui_ca_pair.h"
#include "ui_ca_burse_charge.h"

#define MAX_ON_SCREEN_MSG_LENGTH 320

typedef struct
{
  BOOL is_msg_need_show;
  BOOL is_cur_msg_showing;
  u16 roll_uni_str[CAS_MAX_MSG_DATA_LEN + 1];
  u8 roll_mode;
  u8 display_times;
  u8 location;
}on_screen_msg_t;


enum auto_feed_local_msg
{
  MSG_INSERT_MON_CARD = MSG_LOCAL_BEGIN + 625,
  MSG_INSERT_CHILD_CARD
};

RET_CODE ui_ca_rolling_proc(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);

static BOOL g_is_smart_card_insert = FALSE;
static BOOL g_is_smart_card_status = FALSE;

static u8 finger_print_flag = 0;
static finger_msg_t *g_p_finger_info = NULL;

#define MAX_MEMORY_ROLL_MSG  5
typedef struct {
	BOOL ca_rolled_flag;
	u8 ca_roll_mode;
	u8 ca_roll_value;
	u8 ca_roll_pos;
	u16 ca_roll_uni_str[512];
}ca_roll_msg_t;

static ca_roll_msg_t g_ca_roll_msg[MAX_MEMORY_ROLL_MSG];
static BOOL g_ca_msg_rolling_over = TRUE;

typedef struct
{
  u8 root_id;
  u8 state;
  void (*close_func)(void);
}mgcas_notify_menu_attr_t;

static mgcas_notify_menu_attr_t mgcas_popup_root[] =
{
	{ROOT_ID_CA_BURSE_CHARGE,	FALSE,	ui_ppv_dlg_close},
	{ROOT_ID_CA_PAIR,			FALSE,	close_ca_pair_dlg},
	{ROOT_ID_CA_ENTITLE_EXP_DLG,FALSE,	close_ca_entitle_expire_dlg},
	{ROOT_ID_CA_IPPV_PPT_DLG,	FALSE,	close_ca_burse_dlg},
};

void ui_set_smart_card_insert(BOOL status)
{
	g_is_smart_card_insert = status;
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

static void ui_update_mgca_notify(u16 root_id,u32 para1, u32 para2)
{
	u8 root_cnt = sizeof(mgcas_popup_root)/sizeof(mgcas_popup_root[0]);
	u8 index;

	DEBUG(CAS,INFO,"\n");
	if(RSC_INVALID_ID == root_id)
	{
		for(index = 0;index<root_cnt; index++)
		{
			if(mgcas_popup_root[index].state)
			{
				mgcas_popup_root[index].close_func();
				mgcas_popup_root[index].state = FALSE;
			}
		}

	}
	else
	{
		for(index = 0;index<root_cnt; index++)
		{
			if(root_id == mgcas_popup_root[index].root_id)
			{
				mgcas_popup_root[index].state = TRUE;
				manage_open_menu(root_id, para1, para2);
				break;
			}
		}
	}
}

static RET_CODE on_show_mgcas_notify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	ipps_info_t *p_ippv_info = (ipps_info_t *) para2;

	update_ca_message(RSC_INVALID_ID);
	switch(msg)
	{
		case MSG_CA_SHOW_BURSE_CHARGE:
			DEBUG(CAS,INFO,"MSG_CA_SHOW_BURSE_CHARGE\n");
			ui_update_mgca_notify(ROOT_ID_CA_BURSE_CHARGE,0,para2);
			break;

		case MSG_CA_SHOW_ERROR_PIN_CODE:
			DEBUG(CAS,INFO,"MSG_CA_SHOW_ERROR_PIN_CODE\n");		
			ui_update_mgca_notify(ROOT_ID_CA_PAIR,para1,para2);
			break;

		case MSG_CA_AUTHEN_EXPIRE_DAY:
			DEBUG(CAS,INFO,"MSG_CA_AUTHEN_EXPIRE_DAY\n");
			ui_update_mgca_notify(ROOT_ID_CA_ENTITLE_EXP_DLG,msg,para2);
			break;

		case MSG_CA_OPEN_IPPV_PPT:
			DEBUG(CAS,INFO,"MSG_CA_OPEN_IPPV_PPT\n");
			if(p_ippv_info->p_ipp_info[0].type == 1)
			{
				ui_update_mgca_notify(ROOT_ID_CA_IPPV_PPT_DLG,1,(u32)para2);
			}
			break;
		case MSG_CA_HIDE_IPPV_PPT:
			DEBUG(CAS,INFO,"\n");
			manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG,1,(u32)para2);
			break;
	}

	return SUCCESS;
}


RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	u32 event_id = para2;
	control_t *p_cont = NULL;
	rect_t notify ={(SCREEN_WIDTH-350)/2, (SCREEN_HEIGHT-140)/2 -20,(SCREEN_WIDTH-350)/2 + 350, (SCREEN_HEIGHT-140)/2 +120};

	switch(event_id)
	{
		case CAS_S_ADPT_CARD_REMOVE:
			DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_REMOVE\n");
			if(TRUE == ui_is_smart_card_insert())
			{
				ui_set_smart_card_insert(FALSE);
				ui_set_smart_card_state(FALSE);
				ui_update_mgca_notify(RSC_INVALID_ID,para1,para2);
				update_ca_message(IDS_CAS_S_CARD_REMOVE);
			}
			break;
		case CAS_S_ADPT_CARD_INSERT:
			DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_INSERT\n");
			ui_set_smart_card_state(TRUE);
			update_ca_message(RSC_INVALID_ID);
			break;


		case CAS_C_EMM_APP_STOP://���ܿ�ֹͣ����
			DEBUG(CAS,INFO,"CAS_C_EMM_APP_STOP\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID, IDS_CA_E_APP_STOP);
			break;
		case CAS_C_EMM_APP_RESUME://���ܿ��ָ�����
			DEBUG(CAS,INFO,"CAS_C_EMM_APP_RESUME\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_APP_RESUME);
			break;
		case CAS_C_EMM_PROG_UNAUTH://ȡ����Ŀ����
			DEBUG(CAS,INFO,"CAS_C_EMM_PROG_UNAUTH\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_PROG_UNAUTH);
			break;
		case CAS_C_EMM_PROG_AUTH://���ӽ�Ŀ����
			DEBUG(CAS,INFO,"CAS_C_EMM_PROG_AUTH\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_PROG_AUTH);
			break;
		case CAS_C_EMM_UPDATE_GNO://���ܿ���Ÿ���
			DEBUG(CAS,INFO,"CAS_C_EMM_UPDATE_GNO\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_GNO);
			break;
		case CAS_S_CARD_NEED_UPG://���ܿ�������β忨
			DEBUG(CAS,INFO,"CAS_S_CARD_NEED_UPG\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_MG_CA_E_UPDATE_GK);
			break;
		case CAS_S_MODIFY_ZONECODE://���������
			DEBUG(CAS,INFO,"CAS_S_MODIFY_ZONECODE\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_ZONECODE);
			break;
		case CAS_C_EMM_UPDATE_COUT://���������
			DEBUG(CAS,INFO,"CAS_C_EMM_UPDATE_COUT\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_COUT);
			break;			
		case CAS_C_NOTIFY_UPDATE_CTRLCODE://���ܿ��޲������
			DEBUG(CAS,INFO,"���ܿ��޲������!\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_MODIFY_CTRLCODE);
			break;
		case CAS_C_NOTIFY_UPDATE_CRT://��Ȩ����
			DEBUG(CAS,INFO,"��Ȩ����!\n");
			close_notify();
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_CRT);
			break;
		case CAS_C_EN_PARTNER://����ƥ��
			DEBUG(CAS,INFO,"CAS_C_EN_PARTNER\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_EN_PAPTNER);
			break;
		case CAS_C_DIS_PARTNER://ȡ������ƥ��
			DEBUG(CAS,INFO,"CAS_C_DIS_PARTNER\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_DIS_PARTNER);
			break;
		case CAS_C_SET_SON_CARD://������ĸ��
			DEBUG(CAS,INFO,"������ĸ��!\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_SET_MON_SON_CARD);
			break;
		case CAS_C_SHOW_NEW_EMAIL:
			DEBUG(CAS,INFO,"CAS_C_SHOW_NEW_EMAIL\n");
			ui_set_new_mail(TRUE);
			update_email_picture(EMAIL_PIC_SHOW);
			break;
		case CAS_C_HIDE_NEW_EMAIL:
			DEBUG(CAS,INFO,"CAS_C_HIDE_NEW_EMAIL\n");
			ui_set_new_mail(FALSE);
			update_email_picture(EMAIL_PIC_HIDE);
			break;
		case CAS_S_CLEAR_DISPLAY:
			DEBUG(CAS,INFO,"CAS_S_CLEAR_DISPLAY\n");
			update_ca_message(RSC_INVALID_ID);
			{
				p_cont = NULL;
				if((p_cont = fw_find_root_by_id(ROOT_ID_CA_PAIR)) != NULL)
				{
					close_ca_pair_dlg();
				}
			}
			break;  
			
		case CAS_C_EMM_CHILD_UNLOCK:
			DEBUG(CAS,INFO,"CAS_C_EMM_CHILD_UNLOCK\n");
			update_ca_message(IDS_CA_CLEAR_PARENT_LOCK);
			break;

		/*ECM message*/
		
		case CAS_E_CADR_NOT_ACT://�뼤���ӿ�
			DEBUG(CAS,INFO,"CAS_E_CADR_NOT_ACT\n");
			update_ca_message(IDS_CA_E_SCARD_NOT_ACT);
			break;
		case CAS_C_AUTH_LOGO_DISP://��ĿԤ��Ȩ
			DEBUG(CAS,INFO,"��ĿԤ��Ȩ!\n");
			manage_open_menu(ROOT_ID_CA_PRE_AUTH, 0, 0);
			break;
		case CAS_C_UPDATE_BALANCE://IPPV��Ŀ
			DEBUG(CAS,INFO,"IPPV��Ŀ\n");
			manage_open_menu(ROOT_ID_CA_PPV_ICON, 0, 0);
			break;
		case CAS_E_ZONE_CODE_ERR:
			DEBUG(CAS,INFO,"CAS_E_ZONE_CODE_ERR\n");
			update_ca_message(IDS_CA_E_SCARD_DISABLED);
			break;
		case CAS_E_CADR_NO_MONEY: //���ܿ�����
			DEBUG(CAS,INFO,"CAS_E_CADR_NO_MONEY\n");
			update_ca_message(IDS_CA_E_NO_MONEY);
			break;
		case CAS_E_PROG_UNAUTH://Ƶ��δ��Ȩ
			DEBUG(CAS,INFO,"CAS_E_PROG_UNAUTH\n");
			update_ca_message(IDS_NO_AUTHORIZED);
			break;  
		case CAS_E_CARD_AUTH_FAILED:
			DEBUG(CAS,INFO,"���ܿ���Ȩ����\n");
			update_ca_message(IDS_CA_ILLEGAL_AUTH);
			break;
		case CAS_E_CARD_LOCKED://���ܿ�����
			DEBUG(CAS,INFO,"CAS_E_CARD_LOCKED\n");
			update_ca_message(IDS_CAS_E_CARD_LOCKED);
			break;
		case CAS_E_SERVICE_LOCKED://�����ѱ���
			DEBUG(CAS,INFO,"CAS_E_SERVICE_LOCKED\n");
			update_ca_message(IDS_CA_E_APP_LOCKED);
			break;
		case CAS_E_EXTER_AUTHEN://���ܿ���֤ʧ��
			DEBUG(CAS,INFO,"CAS_E_EXTER_AUTHEN\n");
			update_ca_message(IDS_CA_E_EXTER_AUTHEN);
			break;
		case CAS_C_IPPT_BUY_OR_NOT://IPP��Ϣ
			DEBUG(CAS,INFO,"CAS_C_IPPT_BUY_OR_NOT\n");
			update_ca_message(IDS_IPPV_PROG);
			break;
		case CAS_E_CARD_NOT_FOUND://������ƥ��
			DEBUG(CAS,INFO,"CAS_E_CARD_NOT_FOUND\n");
			update_ca_message(IDS_CA_E_CARD_NOTMATCH);
			break;
		case CAS_E_CARD_FAILED://���ܿ�����ʧ��
			DEBUG(CAS,INFO,"CAS_E_CARD_FAILED\n");
			update_ca_message(IDS_CA_E_UPDATE_GR);
			break;
		case CAS_E_NOT_REG_CARD://��������ܿ�
			DEBUG(CAS,INFO,"CAS_E_NOT_REG_CARD\n");
			update_ca_message(IDS_CA_CARD_REMOVE);
			break;
		case CAS_C_ENTITLE_CLOSED://��Ȩ�ر�
			update_ca_message(IDS_CA_AUTH_CLOSED);
			break;
		case CAS_S_CARD_AUTHEN_UPDATING://��Ȩˢ��
			update_ca_message(IDS_CA_AUTH_UPDATE);
			break;
		case CAS_E_CARD_RENEW_RESET_STB://���ܿ�״̬���£�������
			update_ca_message(IDS_CA_STATUS_RENEW);
			break;
		case CAS_E_CARD_AUTHEN_OVERDUE:
			DEBUG(CAS,INFO,"CAS_E_CARD_AUTHEN_OVERDUE\n");
			update_ca_message(IDS_CA_AUTH_EXPIRED);
			break;
		case CAS_E_CARD_DATA_GET:// ���ܿ����ݴ���
			DEBUG(CAS,INFO,"���ܿ����ݴ���!\n");      
			update_ca_message(IDS_CA_CARD_DATA_ERROR);
			break;
		case CAS_E_ILLEGAL_CARD://���ܿ�ͨ�Ŵ���
			DEBUG(CAS,INFO,"���ܿ�ͨ�Ŵ���!\n");
			g_is_smart_card_insert = FALSE;
			update_ca_message(IDS_CA_IC_CMD_FAIL);
			break;
		case CAS_C_USERCDN_LIMIT_CTRL_ENABLE://���û��������޲�
			DEBUG(CAS,INFO,"���û��������޲�!\n");
			update_ca_message(IDS_CA_CARD_CONDITIONAL_LIMIT);
			break;
		case CAS_E_CARD_OPERATE_FAILE://���ܿ�����ʧ��
			DEBUG(CAS,INFO,"���ܿ�����ʧ��!\n");
			update_ca_message(IDS_CA_OPERATE_CARD_FAIL);
			break;
		case CAS_E_AUTHEN_EXPIRED://�н�Ŀ��Ȩ�Ѿ����ڣ��뼰ʱ����
			DEBUG(CAS,INFO,"�н�Ŀ��Ȩ�Ѿ����ڣ��뼰ʱ����\n");
			if(ui_is_fullscreen_menu(fw_get_focus_id()))
			{
				manage_open_menu(ROOT_ID_CA_ENTITLE_EXP_DLG, CAS_E_AUTHEN_EXPIRED, 0);
			}
			break;
		default:
			DEBUG(CAS,INFO,"on_ca_message_update : ui_get_curpg_scramble_flag == 1  default : event_id = [%d]\n",event_id);
			break;
	}

	DEBUG(CAS,INFO,"print for pc ilnt = 0x%x\n",p_cont);
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

	if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
	{
		fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
	}
	finger_print_flag = 1;

	open_finger_print(0, (u32)(u32)asc_num);

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

static RET_CODE on_ca_force_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = ERR_FAILURE;
	u16 view_count = 0, pg_id = 0;
	u16 cur_pg_id = 0;
	static u16 pre_pg_id = INVALID_PG_ID;
	u8 view_id;
	u8 i = 0;
	u8 focus_id = 0;
	rect_t notify ={(SCREEN_WIDTH-350)/2, (SCREEN_HEIGHT-140)/2 -20,(SCREEN_WIDTH-350)/2 + 350, (SCREEN_HEIGHT-140)/2 +120};

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
				//mgca not notice ts_id and netwrok_id
				#if 0
				if((pg.s_id == p_force_channel_info->serv_id)
							&& (pg.ts_id == p_force_channel_info->ts_id)
							&& (pg.orig_net_id == p_force_channel_info->netwrok_id)
							&&(pg_id != cur_pg_id))
				#endif
				if((pg.s_id == p_force_channel_info->serv_id)&&(pg_id != cur_pg_id))							
				{
					focus_id = fw_get_focus_id();
					if(!ui_is_fullscreen_menu(focus_id))
					{
						ui_close_all_mennus();
					}
					//set_uio_status(FALSE);
					ui_play_prog(pg_id, FALSE);
					pre_pg_id = cur_pg_id;
					ret = SUCCESS;
					break;
				}
				else
				{
					pre_pg_id = INVALID_PG_ID;
				}
			}
		}
		ui_set_notify(&notify, NOTIFY_TYPE_STRID, IDS_CA_SERVICE_EMERGENCE);
	}
	else
	{
		//set_uio_status(TRUE);
		if(pre_pg_id != INVALID_PG_ID)
		{
			ui_play_prog(pre_pg_id, FALSE);
			pre_pg_id = INVALID_PG_ID;
		}
		ret = SUCCESS;
	}
	
	return ret;
}

static RET_CODE on_new_mail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	cas_mail_headers_t *p_mail_header = (cas_mail_headers_t*)para2;
	u32 new_mail = 0;
	s8 i =0;
	u8 index;
	index = fw_get_focus_id();
  
	if(index != ROOT_ID_DO_SEARCH&& do_search_is_finish())
	{
		DEBUG(MAIN,INFO,"max_num = %d\n",p_mail_header->max_num);
		if((0 != p_mail_header->max_num) && (0xFFFF != p_mail_header->max_num))
		{
			for(i = 0; i < p_mail_header->max_num; i++)
			{
				if(p_mail_header->p_mail_head[i].new_email)
					new_mail++;
			}

			DEBUG(MAIN,INFO,"new_mail = %d\n",new_mail);
			if(new_mail > 0)
			{
				ui_set_new_mail(TRUE);
				update_email_picture(EMAIL_PIC_SHOW);
			}
			else if(p_mail_header->max_num == 50)
			{
				ui_set_new_mail(TRUE);
				update_email_picture(EMAIL_PIC_FLASH);
			}
			else
			{
				ui_set_new_mail(FALSE);
				update_email_picture(EMAIL_PIC_HIDE);
			}
		}
		else
		{
			ui_set_new_mail(FALSE);
			update_email_picture(EMAIL_PIC_HIDE);
		}
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

void on_desktop_start_roll(u16 *p_uni_str, u32 roll_mode, u32 value,u32 dis_pos)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl = NULL;
	roll_param_t p_param = {0};

	value = value?value:2;
	DEBUG(CAS,INFO,"***mg osd_display.location:%d*** value = %d\n",dis_pos,value);

	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	if(OSD_SHOW_TOP == dis_pos)
	{
		p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
	}
	else
	{
		p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
	}

	gui_stop_roll(p_ctrl);
	ctrl_set_attr(p_ctrl, OBJ_STS_SHOW);
	text_set_content_by_unistr(p_ctrl, p_uni_str);
	p_param.pace 	 = ROLL_SINGLE;
	p_param.style 	 = ROLL_LR;
	p_param.repeats	 = value;
	p_param.is_force = 1;
	gui_start_roll(p_ctrl, &p_param);
	ctrl_paint_ctrl(p_ctrl,TRUE);
	
	g_ca_msg_rolling_over = FALSE;

}

static RET_CODE on_ca_rolling_msg_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	msg_info_t *p_ca_msg_info = NULL;
	u16 uni_str[512] = {0};//0x673a;
	u8 i = 0;
	p_ca_msg_info = (msg_info_t *)para2;
	if(NULL == p_ca_msg_info)
		return ERR_FAILURE;

	DEBUG(CAS,INFO,"OSD STR =%s\n", p_ca_msg_info->data);
	DEBUG(CAS,INFO,"OSD TYPE =%d\n", p_ca_msg_info->type);
	DEBUG(CAS,INFO,"roll_mode =%d\n", p_ca_msg_info->osd_display.roll_mode);
	DEBUG(CAS,INFO,"roll_value =%d\n", p_ca_msg_info->osd_display.roll_value);
	DEBUG(CAS,INFO,"OSD location =%d\n", p_ca_msg_info->osd_display.location);
	if(p_ca_msg_info->type == 0)
	{
		gb2312_to_unicode(p_ca_msg_info->data, 511, uni_str, 511);
		if(g_ca_msg_rolling_over)
		{
			on_desktop_start_roll(uni_str,p_ca_msg_info->osd_display.roll_mode,
	 							          p_ca_msg_info->osd_display.roll_value,
	 							          p_ca_msg_info->osd_display.location);
			if(ui_is_fullscreen_menu(fw_get_focus_id()))
				ui_time_enable_heart_beat(TRUE);
		}
		else
		{
			for(i=0;i<MAX_MEMORY_ROLL_MSG;i++)
			{
				if(g_ca_roll_msg[i].ca_rolled_flag == FALSE)
					break;
			}
			
			if(i == MAX_MEMORY_ROLL_MSG)
				i = i - 1;
			
			DEBUG(CAS,INFO,"i =%d\n", i);
			memcpy(g_ca_roll_msg[i].ca_roll_uni_str,uni_str,511);
			g_ca_roll_msg[i].ca_roll_mode 	= (u8)p_ca_msg_info->osd_display.roll_mode;
			g_ca_roll_msg[i].ca_roll_value 	= p_ca_msg_info->osd_display.roll_value;
			g_ca_roll_msg[i].ca_roll_pos 	= p_ca_msg_info->osd_display.location;
			g_ca_roll_msg[i].ca_rolled_flag = TRUE;
		}
	}
	else
	{
		DEBUG(CAS_OSD, INFO,"content = %s \n",p_ca_msg_info->data);
		comm_dlg_data_t2 overdue_data =
		{
			ROOT_ID_INVALID,
			DLG_FOR_CONFIRM| DLG_STR_MODE_EXTSTR,
			DLG_FOR_CONFIRM| DLG_STR_MODE_EXTSTR,
			((SCREEN_WIDTH - 450) / 2), ((SCREEN_HEIGHT - 280) / 2),
			450, 280,
			0,(u32)p_ca_msg_info->title,
			0,(u32)p_ca_msg_info->data,
			0,
		};

		ui_comm_dlg_open3(&overdue_data);
	}
	return SUCCESS;
}

void ui_ca_rolling_post_stop(void)
{
	u8 i = 0;

	for(i=0;i<MAX_MEMORY_ROLL_MSG;i++)
	{
		if(g_ca_roll_msg[i].ca_rolled_flag)
		{
			on_desktop_start_roll(g_ca_roll_msg[i].ca_roll_uni_str,
			                            g_ca_roll_msg[i].ca_roll_mode,
			                            g_ca_roll_msg[i].ca_roll_value,
			                            g_ca_roll_msg[i].ca_roll_pos);
			g_ca_roll_msg[i].ca_rolled_flag = FALSE;
			DEBUG(CAS,INFO,"start roll memory osd i =%d\n", i);
			break;
		}
	}

	DEBUG(CAS,INFO,"roll end line : %d\n", i, __LINE__);
	if(i == MAX_MEMORY_ROLL_MSG)
	{
		g_ca_msg_rolling_over = TRUE;
		ui_time_enable_heart_beat(FALSE);
	}

	if(do_search_is_finish())	
	{
		ui_ca_do_cmd(CAS_CMD_OSD_ROLL_OVER, 0, 0);
	}
}

static RET_CODE on_ca_rolling_stop(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	rect_t frame = {0};
	control_t *p_parent = NULL;

	MT_ASSERT(p_ctrl != NULL);
	ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
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

	DEBUG(CAS_OSD,INFO," ctrl_is_rolling = %d \n",ctrl_is_rolling(p_ctrl_top));
	if((NULL != p_ctrl_top) && (ctrl_is_rolling(p_ctrl_top))&&(!g_ca_msg_rolling_over))
	{
		DEBUG(CAS_OSD,INFO," on_ca_rolling_show \n");
		ctrl_set_sts(p_ctrl_top,OBJ_STS_SHOW);
		gui_resume_roll(p_ctrl_top);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(TRUE);
	}
	if((NULL != p_ctrl_bottom) && (ctrl_is_rolling(p_ctrl_bottom))&&(!g_ca_msg_rolling_over))
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

RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
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

  return SUCCESS;
}

RET_CODE hotkey_green_process(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"\n");
	ui_ca_do_cmd(CAS_CMD_IPPV_INFO_SYNC_GET, 0,0);
	return SUCCESS;
}


BEGIN_MSGPROC(ui_desktop_proc_cas, cont_class_proc)
	ON_COMMAND(MSG_CA_INIT_OK, on_ca_init_ok)
	ON_COMMAND(MSG_CA_CLEAR_DISPLAY, on_ca_clear_OSD)
	ON_COMMAND(MSG_CA_EVT_NOTIFY, on_ca_message_update)
	ON_COMMAND(MSG_CA_FINGER_INFO, on_finger_print)
	ON_COMMAND(MSG_CA_HIDE_FINGER, on_ca_hide_finger)
	ON_COMMAND(MSG_CA_FORCE_CHANNEL_INFO, on_ca_force_channel)
	ON_COMMAND(MSG_CA_UNFORCE_CHANNEL_INFO, on_ca_force_channel) 

	ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, on_new_mail)
	ON_COMMAND(MSG_CA_SHOW_OSD, on_ca_rolling_msg_update)

	ON_COMMAND(MSG_CA_SHOW_BURSE_CHARGE, 	on_show_mgcas_notify)
	ON_COMMAND(MSG_CA_SHOW_ERROR_PIN_CODE,	on_show_mgcas_notify)
	ON_COMMAND(MSG_CA_AUTHEN_EXPIRE_DAY, 	on_show_mgcas_notify)
	ON_COMMAND(MSG_CA_OPEN_IPPV_PPT, 		on_show_mgcas_notify)
	ON_COMMAND(MSG_CA_HIDE_IPPV_PPT, 		on_show_mgcas_notify)

	ON_COMMAND(MSG_HOTKEY_GREEN, hotkey_green_process)

END_MSGPROC(ui_desktop_proc_cas, cont_class_proc)

BEGIN_KEYMAP(ui_desktop_keymap_cas, NULL)
	ON_EVENT(V_KEY_MAIL,MSG_OPEN_MENU_IN_TAB | ROOT_ID_EMAIL_MESS)
	ON_EVENT(V_KEY_HOT_XREFRESH,MSG_HOTKEY_GREEN)
END_KEYMAP(ui_desktop_keymap_cas, NULL)

BEGIN_MSGPROC(ui_ca_rolling_proc, text_class_proc)
	ON_COMMAND(MSG_ROLL_STOPPED, on_ca_rolling_stop)
END_MSGPROC(ui_ca_rolling_proc, text_class_proc)
