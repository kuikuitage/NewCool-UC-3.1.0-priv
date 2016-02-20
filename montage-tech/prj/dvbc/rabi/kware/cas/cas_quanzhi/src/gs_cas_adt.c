/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "string.h"
#include "trunk/sys_def.h"
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "stdlib.h"

#include "lib_bitops.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "drv_svc.h"
#include "pti.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "cas_adapter.h"

#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_int.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_task.h"

#include "nim.h"
#include "smc_op.h"

#include "gs_ca_process.h"

#include "lib_util.h"
#include "sys_dbg.h"
#include "ui_common.h"


#define CAS_GS_ADT_DEBUG

#ifdef CAS_GS_ADT_DEBUG
#define CAS_GS_ADT_PRINTF OS_PRINTF
#else
#define CAS_GS_ADT_PRINTF DUMMY_PRINTF
#endif

extern cas_adapter_priv_t g_cas_priv;

extern u32 GS_CAS_Drv_ClientInit(void);
extern void GS_CAS_Drv_FreeAllEmmReq(void);
extern void GS_CAS_Drv_FreeAllEcmReq(void);
extern void GS_CAS_DRV_SetSCInsertStatus(u8 uScStatus);
extern u8 GS_CAS_DRV_GetSCInsertStatus(void);
extern char GS_check_card_apdu(const unsigned char *stb_sn, unsigned char stb_sn_len,
                               unsigned char *super_cas_id, unsigned char *smartcard_id, unsigned char *cw_mask);
extern   void _Gs_CaReqUnLock(void);
extern   void _Gs_CaReqLock(void);
extern   void _Gs_refresh_osd(void);
extern char GS_get_area_code(unsigned char *area_code);
extern char GS_get_expire_date(unsigned char *expire_date);
extern u8 GSSTBCA_SetStreamGuardFilter(u8 byReqID,u16 wPID,
                                    u8 *szFilter,u8 *szMask,u8 byLen,u8 nWaitSecs);
extern void GS_CAS_Drv_FreeNITReq(void);
extern u8 GS_SetDescrCW(const u8 *szOddKey,const u8 *szEvenKey);
extern char GS_get_child_auth_info(unsigned char *auth_info, char* len);
extern char GS_set_child_auth_info(unsigned char *auth_info, char len);
extern char read_card_type(unsigned char *type);
unsigned char  gs_stb_sn[20] = {0};
unsigned char  gs_cw_mask[8] = {0};
unsigned char  gs_smartcard_id[3] = {0};
unsigned char  gs_super_cas_id[4] = {0};

static msg_info_t OsdMsg = {0, {0,},};
cas_mail_header_t gs_cas_mail_header = {0,};
cas_mail_body_t gs_cas_mail_body = {{0,},0,};

u8 gs_ca_nit_flag = 0;

u8 gs_ca_nit_state = 0;

u16 gs_ca_area_code = 0xffff;

u16 gs_card_area_code = 0xffff;

static BOOL area_limit = FALSE;


/*!
  abc
  */
typedef struct
{
  /*!
    abc
    */
  u8 slot;
  /*!
    abc
    */
  u16 v_pid;
  /*!
    abc
    */
  u16 a_pid;
  /*!
    The callback of nvram reading function
    */
  RET_CODE (*nvram_read)(u32 offset,
              u8 *p_buf, u32 *size);
  /*!
    The callback of nvram writing function
    */
  RET_CODE (*nvram_write)(u32 offset,
              u8 *p_buf, u32 size);
     /*!
   flash_start_adr
    */
  u32 flash_start_adr;
    /*!
   flash_size
    */
  u32 flash_size;
}cas_gs_priv_t;

/*
*Network License Manager API 
*/
extern unsigned long _mktime (unsigned int year, unsigned int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec);

BOOL get_vol_entitlement_from_ca(void)
{
	unsigned char expire_date[4] = {0};

	if(GS_ERR_OK == GS_get_expire_date(expire_date))
	{
		u16 expire_year;
		u32 curTime, expTime;
		utc_time_t utc_time = {0};
		time_set_t p_set={{0}};

		expire_year = (expire_date[0]<<8) |(expire_date[1]);
		if(expire_year < 1972)
		{
			DEBUG(DBG,INFO, "expire_year[%d]\n",expire_year);
			return FALSE;
		}

		sys_status_get_time(&p_set);

		time_get(&utc_time, p_set.gmt_usage);
		curTime = _mktime(utc_time.year,utc_time.month,utc_time.day,0,0,0);
		expTime = _mktime(expire_year,expire_date[2],expire_date[3],0,0,0);

		DEBUG(DBG,INFO, "curTime = %x expTime = %x\n",curTime,expTime);
		DEBUG(DBG,INFO, "%d-%d-%d\n",utc_time.year,utc_time.month,utc_time.day);
		DEBUG(DBG,INFO, "%d-%d-%d\n",expire_year,expire_date[2],expire_date[3]);

		if(expTime < curTime)
			return FALSE;

		DEBUG(DBG,INFO, "GZ have net entitlement!\n");
		return TRUE;
	}
	else
	{
		DEBUG(DBG,INFO, "GZ GS_get_expire_date fail!\n");
		return FALSE;
	}
	
	return FALSE;
}

void send_event_to_ui_from_authorization(u32 event)
{
	cas_gs_priv_t *p_priv = (cas_gs_priv_t *)g_cas_priv.cam_op[CAS_ID_GS].p_priv;
	
	if(event == 0)
	{
		CAS_GS_ADT_PRINTF("[%s]%d send fail enent \n",__FUNCTION__,__LINE__);
		return;
	}
	cas_send_event(p_priv->slot, CAS_ID_GS, event, 0);
	CAS_GS_ADT_PRINTF("[%s]%d send enent from authorization \n",__FUNCTION__,__LINE__);
}


BOOL  get_gs_area_limit_free(void)
{
  return area_limit;
}

void gs_set_cw_stop_play(void)
{
	char  p_gs_cw[16];
	memset(p_gs_cw,0,sizeof(p_gs_cw));
	GS_SetDescrCW((const u8 *)p_gs_cw,(const u8 *)(p_gs_cw + 8));
}

/*!
  abc
  */
static void set_event(cas_gs_priv_t *p_priv, u32 event)
{
  CAS_GS_ADT_PRINTF("gscas: set_event  event  =%d  \n", event);
  if(event == 0)
  {
    return;
  }

 cas_send_event(p_priv->slot, CAS_ID_GS, event, 0);

}

/*!
  abc
  */
void _Gs_ShowOSDMsg(const u8 *szOSD)
{
  cas_gs_priv_t *p_priv = (cas_gs_priv_t *)g_cas_priv.cam_op[CAS_ID_GS].p_priv;

  OsdMsg.osd_display.roll_mode = 1;
  OsdMsg.osd_display.roll_value = 3;
  OsdMsg.type = 0;
  memcpy(OsdMsg.data,szOSD,256);
  set_event(p_priv, CAS_C_SHOW_OSD);
  CAS_GS_ADT_PRINTF("\r\n显示OSD _Gs_ShowOSDMsg   szOSD =%s\r\n",
                      OsdMsg.data);
  return;
}

/*!
  abc
  */
u8 gscas_smartcard_status(void)
{
  u32 param = 0;
  scard_device_t *p_smc_dev = NULL;
  p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
  MT_ASSERT(NULL != p_smc_dev);

  dev_io_ctrl(p_smc_dev,0x01,(u32)&param);

  if(param == 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

static RET_CODE gs_cas_card_reset(void)
{
#ifdef DTMB_PROJECT
	u8 i = 0;
	u8 atr[32] = {0};
	scard_atr_desc_t atr_desc = {0};
	scard_device_t *p_smc_dev = NULL;

	CAS_GS_ADT_PRINTF("[%s] %d \n",__FUNCTION__,__LINE__);
	p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
	if(NULL == p_smc_dev)
	{
		CAS_GS_ADT_PRINTF("[%s] %d  fail\n",__FUNCTION__,__LINE__);
		return SUCCESS;
	}

	if(SUCCESS != scard_reset(p_smc_dev))
	{
		CAS_GS_ADT_PRINTF("cas: reset card failed\n");
		return ERR_FAILURE;
	}

	atr_desc.p_buf = atr;
	if(scard_active(p_smc_dev, &atr_desc) != SUCCESS)
	{
		CAS_GS_ADT_PRINTF("cas: scard_active failed\n");
		return ERR_FAILURE;
	}
	#if 1	
	OS_PRINTF("ATR[%d]:",atr_desc.atr_len);
	for(i=0; i<atr_desc.atr_len; i++)
	{
	OS_PRINTF("%02x ",atr_desc.p_buf[i]);
	}
	OS_PRINTF("\n");
	#endif
#endif
	return SUCCESS;
}


/*!
  abc
  */
static RET_CODE cas_gs_init(void)
{
  static u32 init_flag = 0;

  if(!init_flag)
  {
    CAS_GS_ADT_PRINTF("cas_gs_init\n");

    GS_CAS_Drv_ClientInit();

    init_flag = 1;
    mtos_task_sleep(300);
    cas_init_nvram_data(CAS_ID_GS);
  }
  return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_gs_deinit(void)
{
  return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_gs_card_remove(void)
{
  CAS_GS_ADT_PRINTF("\ngs cas remove \n");

  GS_CAS_DRV_SetSCInsertStatus(SC_REMOVE_START);
  GS_CAS_Drv_FreeAllEmmReq();
  GS_CAS_Drv_FreeAllEcmReq();
  gs_set_cw_stop_play();
  GS_CAS_DRV_SetSCInsertStatus(SC_REMOVE_OVER);
  CAS_GS_ADT_PRINTF("\ngs cas remove end\n");
  return SUCCESS;
}

void gs_get_new_mail(gs_mail_t *p_mail)
{
  u16 year = 0;
  cas_gs_priv_t *p_priv =  (cas_gs_priv_t *)g_cas_priv.cam_op[CAS_ID_GS].p_priv;

  year = (p_mail->year_high_byte << 8) | p_mail->year_low_byte;

  memset(&gs_cas_mail_header, 0, sizeof(cas_mail_header_t));
  memset(&gs_cas_mail_body, 0, sizeof(cas_mail_body_t));

  gs_cas_mail_header.special_id = (u32)p_mail->mail_id;

  gs_cas_mail_header.creat_date[0] = year / 100;
  gs_cas_mail_header.creat_date[1] = year % 100;
  gs_cas_mail_header.creat_date[2] = p_mail->month;
  gs_cas_mail_header.creat_date[3] = p_mail->day;
  gs_cas_mail_header.creat_date[4] = p_mail->hour;
  gs_cas_mail_header.creat_date[5] = p_mail->minute;

  memcpy(gs_cas_mail_header.sender, p_mail->sender, p_mail->sender_length);
  memcpy(gs_cas_mail_header.subject, p_mail->caption, p_mail->caption_length);
  memcpy(gs_cas_mail_body.data, p_mail->content, p_mail->content_length);
  gs_cas_mail_body.data_len = p_mail->content_length;
  CAS_GS_ADT_PRINTF("\n*****************************************\n");
  CAS_GS_ADT_PRINTF("\nspecial_id = 0x%x\n", gs_cas_mail_header.special_id);
  CAS_GS_ADT_PRINTF("\ncreat_date0 = 0x%x\n", gs_cas_mail_header.creat_date[0]);
  CAS_GS_ADT_PRINTF("\ncreat_date1 = 0x%x\n", gs_cas_mail_header.creat_date[1]);
  CAS_GS_ADT_PRINTF("\ncreat_date2 = 0x%x\n", gs_cas_mail_header.creat_date[2]);
  CAS_GS_ADT_PRINTF("\ncreat_date3 = 0x%x\n", gs_cas_mail_header.creat_date[3]);
  CAS_GS_ADT_PRINTF("\ncreat_date4 = 0x%x\n", gs_cas_mail_header.creat_date[4]);
  CAS_GS_ADT_PRINTF("\ncreat_date5 = 0x%x\n", gs_cas_mail_header.creat_date[5]);
  CAS_GS_ADT_PRINTF("\nsender = %s\n", gs_cas_mail_header.sender);
  CAS_GS_ADT_PRINTF("\nsubject = %s\n", gs_cas_mail_header.subject);
  CAS_GS_ADT_PRINTF("\ndata = %s\n", gs_cas_mail_body.data);

  if(cas_compare_mail((u8 *)&(gs_cas_mail_header.special_id),MAIL_COMPARE_ID) == 1)
    cas_send_evt_to_adapter(p_priv->slot, CAS_ID_GS, CAS_EVT_MAIL_MASK);
  else
    CAS_GS_ADT_PRINTF("\ngs cas  oldmail\n");

}

/*!
  abc
  */
static RET_CODE cas_gs_card_reset(u32 slot, card_reset_info_t *p_info)
{
  cas_gs_priv_t *p_priv =  (cas_gs_priv_t *)g_cas_priv.cam_op[CAS_ID_GS].p_priv;
  char err = 0;
  U8 i = 0;
  CAS_GS_ADT_PRINTF("\ngs cas  card insert \n");
  CAS_GS_ADT_PRINTF("\nstb id = %s    len =%d\n\n\n\n", gs_stb_sn, strlen(gs_stb_sn));
  p_priv->slot = slot;
  scard_pro_register_op((scard_device_t *)p_info->p_smc);

  GS_CAS_DRV_SetSCInsertStatus(SC_INSERT_START);
  CAS_GS_ADT_PRINTF("\nGS_check_card_apdu\n");
  mtos_task_sleep(10);

  gs_cas_card_reset();
	
  _Gs_CaReqLock();
  err = GS_check_card_apdu(gs_stb_sn, strlen(gs_stb_sn), gs_super_cas_id,
    gs_smartcard_id, gs_cw_mask);
  _Gs_CaReqUnLock();
  if(err != GS_ERR_OK)
  {
    OS_PRINTF("\n[ERR] cas_gs_card_reset error!!!\n");
    return ERR_FAILURE;
  }

  CAS_GS_ADT_PRINTF("\nerr = %d\n",err);

  CAS_GS_ADT_PRINTF("super_cas_id = \n");
  for(i = 0;i < 4; i++)
  {
    CAS_GS_ADT_PRINTF("%d ",gs_super_cas_id[i]);
  }
  CAS_GS_ADT_PRINTF("\n");

  CAS_GS_ADT_PRINTF("smartcard_id = \n");
  for(i = 0;i < 3; i++)
  {
    CAS_GS_ADT_PRINTF("%d ",gs_smartcard_id[i]);
  }
  CAS_GS_ADT_PRINTF("\n");

  CAS_GS_ADT_PRINTF("cw_mask = \n");
  for(i = 0;i < 8; i++)
  {
    CAS_GS_ADT_PRINTF("%d ",gs_cw_mask[i]);
  }
  CAS_GS_ADT_PRINTF("\n");

  _Gs_CaReqLock();
  err = GS_get_area_code((unsigned char *)(&gs_card_area_code));
  _Gs_CaReqUnLock();
  mtos_task_sleep(10);
  CAS_GS_ADT_PRINTF("\nSMC gs_card_area_code = 0x%04x\n",
    gs_card_area_code);

  GS_CAS_DRV_SetSCInsertStatus(SC_INSERT_OVER);
  _Gs_refresh_osd();
  return SUCCESS;
}

/*!
  abc
  */
void _Gs_EcmDecrypt(unsigned short sw1sw2)
{
    cas_gs_priv_t *p_priv = (cas_gs_priv_t *)g_cas_priv.cam_op[CAS_ID_GS].p_priv;
    u32 event = 0;

      switch (sw1sw2)
      {
        case GS_SW1SW2_MEMORY_ERROR:
            CAS_GS_ADT_PRINTF("\n智能卡内存出错\n");
            event = CAS_E_SW1SW2_MEMORY_ERROR;
            break;
        case GS_SW1SW2_WRONG_LC_LENGTH:
            CAS_GS_ADT_PRINTF("\nAPDU命令长度(lc)错误\n");
            event = CAS_E_SW1SW2_WRONG_LC_LENGTH;
            break;
        case GS_SW1SW2_INCORRECT_DATA:
            CAS_GS_ADT_PRINTF("\n数据错误\n");
            event = CAS_E_SW1SW2_INCORRECT_DATA;
            break;
        case GS_SW1SW2_INCORRECT_PARA:
            CAS_GS_ADT_PRINTF("\nAPDU参数错误\n");
            event = CAS_E_SW1SW2_INCORRECT_PARA;
            break;
        case GS_SW1SW2_WRONG_LE_LENGTH:
            CAS_GS_ADT_PRINTF("\nAPDU 返回字节长度（le）错误\n");
            event = CAS_E_SW1SW2_WRONG_LE_LENGTH;
            break;
        case GS_SW1SW2_INS_NOT_SUPPORTED:
            CAS_GS_ADT_PRINTF("\nAPDU命令不支持\n");
            event = CAS_E_SW1SW2_INS_NOT_SUPPORTED;
            break;
        case GS_SW1SW2_CLA_NOT_SUPPORTED:
            CAS_GS_ADT_PRINTF("\nAPDU 类型码不支持\n");
            event = CAS_E_SW1SW2_CLA_NOT_SUPPORTED;
            break;
        case GS_SW1SW2_OK:
            CAS_GS_ADT_PRINTF("\n一切正常\n");
            event = CAS_S_CLEAR_DISPLAY;
            break;
        case GS_SW1SW2_GENERAL_ERROR:
            CAS_GS_ADT_PRINTF("\n未知类型错误\n");
            event = CAS_E_UNKNOW_ERR;
            break;
        case GS_SW1SW2_EXPIRED:
            CAS_GS_ADT_PRINTF("\n频道未授权\n");
            event = CAS_E_PROG_UNAUTH;
            break;
        case GS_SW1SW2_STB_SN_NOT_MATCH:
            CAS_GS_ADT_PRINTF("\n机卡不配对\n");
            event = CAS_E_CARD_DIS_PARTNER;
            break;
        case GS_SW1SW2_CARD_NOT_INIT:
            CAS_GS_ADT_PRINTF("\n智能卡未初始化\n");
            event = CAS_E_CARD_INIT_FAIL;
            break;
        case GS_SW1SW2_INCORRECT_SUPER_CAS_ID:
            CAS_GS_ADT_PRINTF("\n错误的SUPER_CAS_ID\n");
            event = CAS_E_SW1SW2_INCORRECT_SUPER_CAS_ID;
            break;
        case GS_SW1SW2_INCORRECT_SMARTCARD_ID:
            CAS_GS_ADT_PRINTF("\n错误的智能卡号\n");
            event = CAS_E_SW1SW2_INCORRECT_SMARTCARD_ID;
            break;
        case GS_SW1SW2_PPV_EXCEED:
            CAS_GS_ADT_PRINTF("\nPPV过期\n");
            event = CAS_E_IPPV_NO_BOOK;
            break;
        case GS_SW1SW2_NOT_PAIRED:
            CAS_GS_ADT_PRINTF("\n机卡不配对\n");
            event = CAS_E_CARD_DIS_PARTNER;
            break;
        case GS_SW1SW2_ECM_NOT_AUTHORIZED:
            CAS_GS_ADT_PRINTF("\n频道未授权\n");
            event = CAS_E_PROG_UNAUTH;
            break;
        case GS_SW1SW2_LIMIT_AREA:
            CAS_GS_ADT_PRINTF("\n区域限播\n");
            event = CAS_E_ZONE_CODE_ERR;
            break;
        case GS_SW1SW2_LIMIT_CARD:
            CAS_GS_ADT_PRINTF("\n卡限播\n");
            event = CAS_C_CARD_ZONE_INVALID;
            break;
        case GS_SW1SW2_NOT_CHILD_MODE:
            CAS_GS_ADT_PRINTF("\n智能卡没有设置为子卡模式\n");
            event = CAS_E_SW1SW2_NOT_CHILD_MODE;
            break;
        case GS_SW1SW2_NO_CHILD_AUTH:
            CAS_GS_ADT_PRINTF("\n子卡未授权\n");
            event = CAS_C_MASTERSLAVE_NEEDPAIRED;
            break;
        case GS_SW1SW2_CHILD_NOT_AUTHORIZED:
            CAS_GS_ADT_PRINTF("\n子卡未授权\n");
            event = CAS_C_MASTERSLAVE_NEEDPAIRED;
            break;
        case GS_SW1SW2_CHILD_AUTH_TIMEOUT:
            CAS_GS_ADT_PRINTF("\n设置子卡授权超时\n");
            event = CAS_E_SW1SW2_CHILD_AUTH_TIMEOUT;
            break;
        case GS_SW1SW2_CHILD_AUTH_EXPIRED:
            CAS_GS_ADT_PRINTF("\n子卡未授权\n");
            event = CAS_C_MASTERSLAVE_NEEDPAIRED;
            break;
        default:
            CAS_GS_ADT_PRINTF("\n!!!!!!!!!!!_Gs_EcmDecrypt UNKNOWN 0x%x\n",sw1sw2);
            break;
      }
      set_event(p_priv, event);
	  if (event != CAS_S_CLEAR_DISPLAY)
	  {
		gs_set_cw_stop_play();
	  }
}


/*++
功能：通知上层区域检测结束
参数：
--*/
static void cas_gs_ZoneCheckEnd(void)
{
    cas_gs_priv_t *p_priv = (cas_gs_priv_t *)g_cas_priv.cam_op[CAS_ID_GS].p_priv;

    CAS_GS_ADT_PRINTF("\r\n*** cas_gs_ZoneCheckEnd*** \r\n");
    set_event(p_priv, CAS_S_ZONE_CHECK_END);
    return;
}

/*!
  abc
  */
extern void gs_get_sys_time(utc_time_t *);  
static RET_CODE cas_gs_io_ctrl(u32 cmd, void *param)
{
  cas_gs_priv_t *p_priv = (cas_gs_priv_t *)g_cas_priv.cam_op[CAS_ID_GS].p_priv;
  cas_table_info_t *p_Cas_table_info = NULL;
  cas_card_info_t *p_card_info = NULL;
  chlid_card_status_info *p_feed_info = NULL;
  u16 i = 0;
  RET_CODE ret = 0;
  static u8 szFilter[5] = {0};
  static u8 szMask[5] = {0};
  char err = GS_ERR_OK;
  unsigned char expire_date[4] = {0};
  unsigned char area_code[2] = {0};
  unsigned char card_type;
  msg_info_t *p_msg_info = NULL;
  cas_mail_headers_t *p_cas_mail_headers = NULL;
  cas_mail_header_t *p_cas_mail_header = NULL;
  cas_mail_body_t *p_cas_mail_body = NULL;
  u8 *p_data_buffer = NULL;
  //u8 p_data_buffer_body[500] = {0};

  unsigned char  match[16] = {0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned char  mask[16] =  {0xff,0,0,0,0xff,0,0,0,0,0,0,0,0,0,0,0};
  nim_device_t *p_nim_dev = NULL;
  nim_channel_info_t *p_nim_info = NULL;

  switch(cmd)
  {
    case CAS_IOCMD_STOP:
      CAS_GS_ADT_PRINTF("\nSTOP\n");
      break;
    case CAS_IOCMD_SET_ECMEMM:
      GS_CAS_Drv_FreeAllEcmReq();
      set_event(p_priv, CAS_S_CLEAR_DISPLAY);
      p_Cas_table_info = (cas_table_info_t *)param;
      if(GS_CAS_DRV_GetSCInsertStatus() == SC_INSERT_OVER)
      {
        CAS_GS_ADT_PRINTF("\nGSCASTB_SetEcm ticks=%d~~~\n",mtos_ticks_get());
        for(i = 0; i < CAS_MAX_ECMEMM_NUM; i ++)
        {
          if(p_Cas_table_info->ecm_info[i].ca_system_id
             == ((u8)gs_super_cas_id[0] << 8 | (u8)gs_super_cas_id[1]))
          {
              memset(szFilter,0,5);
              memset(szMask,0,5);
              szFilter[0] = 0x81;
              szFilter[1] = (u8)gs_super_cas_id[0];
              szFilter[2] = (u8)gs_super_cas_id[1];
              szFilter[3] = (u8)gs_super_cas_id[2];
              szFilter[4] = (u8)gs_super_cas_id[3];

              szMask[0] = 0xff;
              szMask[1] = 0xff;
              szMask[2] = 0xff;
              szMask[3] = 0xff;
              szMask[4] = 0xff;

              GSSTBCA_SetStreamGuardFilter(1, p_Cas_table_info->ecm_info[i].ecm_id,
                                           szFilter, szMask, 5, 0);
          }
        }
      }
      else
      {
          CAS_GS_ADT_PRINTF("\nSET CARD　OUT~~~~~\n");
          if(p_Cas_table_info->ecm_info[0].ca_system_id != 0)
          {
              CAS_GS_ADT_PRINTF("\nSET CARD　OUT~~do~~~\n");
              cas_send_event(p_priv->slot, CAS_ID_GS, CAS_S_ADPT_CARD_REMOVE, CAS_ID_GS);
          }
      }

      break;
    case CAS_IOCMD_VIDEO_PID_SET:
      p_priv->v_pid = *((u16 *)param);
      CAS_GS_ADT_PRINTF("\nSET v_pid [0x%x]\n",  p_priv->v_pid);
      break;
    case CAS_IOCMD_AUDIO_PID_SET:
      p_priv->a_pid = *((u16 *)param);
      CAS_GS_ADT_PRINTF("\nSET a_pid [0x%x]\n",  p_priv->a_pid);
      break;
    case CAS_IOCMD_CARD_INFO_GET:
      if(GS_CAS_DRV_GetSCInsertStatus() == SC_INSERT_OVER)
      {
        p_card_info = (cas_card_info_t *)param;
		p_card_info->card_state = 0;
        sprintf((char *)(p_card_info->sn),"%d",
                (gs_smartcard_id[0]<<16) | (gs_smartcard_id[1]<<8) |gs_smartcard_id[2]);
        CAS_GS_ADT_PRINTF("\n CardID = %s\n",  p_card_info->sn);

        _Gs_CaReqLock();
        err = GS_get_expire_date(expire_date);
        _Gs_CaReqUnLock();
        if(GS_ERR_OK != err)
        {
          CAS_GS_ADT_PRINTF("\nexpire_date err!!!!!!!!!!!\n");
          memset(expire_date,0,4);
        }
        else
            CAS_GS_ADT_PRINTF("\nexpire_date 0x%02x 0x%02x 0x%02x 0x%02x\n",
                              expire_date[0], expire_date[1], expire_date[2], expire_date[3]);

        _Gs_CaReqLock();
        err = GS_get_area_code(area_code);
        _Gs_CaReqUnLock();
        if(GS_ERR_OK != err)
        {
          CAS_GS_ADT_PRINTF("\ngs_area_code_test err!!!!!!!!!!!\n");
          memset(area_code,0,2);
        }
        else
            CAS_GS_ADT_PRINTF("\ngs_area_code_test 0x%02x 0x%02x\n",
                              area_code[0], area_code[1]);

        memcpy(p_card_info->area_code,area_code,2);
        memcpy(p_card_info->expire_date,expire_date,4);

		read_card_type(&(card_type));
		CAS_GS_ADT_PRINTF("[GS] card_type = %x \n",card_type);
		if(card_type == 0xA5)
			p_card_info->card_type = 1;
		else
			p_card_info->card_type = 0;

      }
	  else
	  {
		 p_card_info = (cas_card_info_t *)param;
		 p_card_info->card_state = 1;
		 CAS_GS_ADT_PRINTF("[debug] card remove \n");
	  }
      break;
	case CAS_IOCMD_MON_CHILD_STATUS_GET:
      p_feed_info = (chlid_card_status_info *)param;
      ret = read_card_type((unsigned char *)&(p_feed_info->is_child));
      if(p_feed_info->is_child != 0xA5)
      {
        ret = GS_get_child_auth_info((unsigned char *)p_feed_info->feed_data, (char *)&(p_feed_info->length));
      }
  	   //DEBUG(QZCA,INFO,"p_feed_info->is_child=%d,ret=%d\n",p_feed_info->is_child,ret);
      if(GS_ERR_OK != ret)
      {
          OS_PRINTF("Read feed data from master failed. Error code:%d\n", ret);
          break;
      }
      break;
	case CAS_IOCMD_MON_CHILD_STATUS_SET:
      p_feed_info = (chlid_card_status_info *)param;
      ret = GS_set_child_auth_info((unsigned char *)p_feed_info->feed_data, (char)p_feed_info->length);
      	//DEBUG(QZCA,INFO,"p_feed_info->is_child=%d,ret=%d\n",p_feed_info->is_child,ret);
      if(GS_ERR_OK != ret)
      {
        OS_PRINTF("Write feed data to slaver failed. Error code:%d\n", ret);
        break;
      }
      break;
    case CAS_IOCMD_OSD_GET:
      p_msg_info = (msg_info_t *)param;
      memcpy(p_msg_info, &OsdMsg, sizeof(OsdMsg));
      CAS_GS_ADT_PRINTF("\nGET=%s\n", OsdMsg.data);
      CAS_GS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_OSD_GET\n");
      break;
    case CAS_IOCMD_GET_CURRENT_MAIL:
       p_cas_mail_header = (cas_mail_header_t *)param;
       p_data_buffer = (u8 *)p_cas_mail_header + sizeof(cas_mail_header_t);
       {
          memcpy(p_cas_mail_header, &gs_cas_mail_header, sizeof(cas_mail_header_t));
          p_cas_mail_header->body_len = gs_cas_mail_body.data_len;
          memcpy(p_data_buffer,gs_cas_mail_body.data,gs_cas_mail_body.data_len);
       }

      CAS_GS_ADT_PRINTF(" mail subject: %s body LEN: %d email_body: %s\n",
        p_cas_mail_header->subject, p_cas_mail_header->body_len, p_data_buffer);
      break;
    case CAS_IOCMD_MAIL_HEADER_GET:
      p_cas_mail_headers = (cas_mail_headers_t *)param;
      cas_get_mail_headers(CAS_ID_GS, (u8 *)p_cas_mail_headers);
      CAS_GS_ADT_PRINTF("[mail] total_email_num= %d\n",
        p_cas_mail_headers->max_num);
      for(i = 0;i < p_cas_mail_headers->max_num; i ++)
      {
        CAS_GS_ADT_PRINTF("[mail] subject = %s m_id=0x%x read=%d\n",
          p_cas_mail_headers->p_mail_head[i].subject,
          p_cas_mail_headers->p_mail_head[i].m_id,
          p_cas_mail_headers->p_mail_head[i].new_email);
      }

      break;

    case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
      CAS_GS_ADT_PRINTF("[mail] MAIL_DELETE_BY_INDEX  =%d\n",*(u32 *)param);
      cas_del_mail_data(CAS_ID_GS, *(u32 *)param);
      break;

    case CAS_IOCMD_MAIL_DELETE_ALL:
      CAS_GS_ADT_PRINTF("[mail] MAIL_DELETE_ALL\n");
      cas_del_all_mail_data(CAS_ID_GS);
      break;

    case CAS_IOCMD_MAIL_BODY_GET:
      p_cas_mail_body = (cas_mail_body_t *)param;
	  CAS_GS_ADT_PRINTF("[mail] MAIL_ID  =%d\n",p_cas_mail_body->mail_id);
      cas_get_mail_body(CAS_ID_GS, p_cas_mail_body->data, p_cas_mail_body->mail_id);
      CAS_GS_ADT_PRINTF("[mail] MAIL_BODY_GET  =%s\n",p_cas_mail_body->data);
      break;

    case CAS_IOCMD_ZONE_CHECK:
      CAS_GS_ADT_PRINTF("\n CAS_IOCMD_ZONE_CHECK start\n");

      p_nim_info = (nim_channel_info_t *)param;
      OS_PRINTF("\nset tuner_lock_delivery %d %d\n",p_nim_info->frequency,
        p_nim_info->param.dvbc.symbol_rate);
      p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,DEV_IDT_TYPE,SYS_DEV_TYPE_NIM);

      nim_channel_connect(p_nim_dev, p_nim_info, 0);
      if(p_nim_info->lock == 1)
      {
          OS_PRINTF("\n mainfre lock \n");
      }
      else
      {
          OS_PRINTF("\n mainfre unlock \n");
          cas_gs_ZoneCheckEnd();
          return SUCCESS;
      }
      gs_ca_nit_state = 0;
      gs_ca_nit_flag = 0;
      GSSTBCA_SetStreamGuardFilter(0x40, 0x10,match, mask, 5, 0);

      i = 0;
      while((gs_ca_nit_state == 0) && (i < 500))
      {
          i++;
          mtos_task_sleep(10);
      }

      if(gs_ca_nit_flag == 1)//get nit
      {
          OS_PRINTF("\nGET nit have areacode\n");
      }
      else if(gs_ca_nit_flag == 2)//get nit
      {
          OS_PRINTF("\nGET nit no areacode\n");
      }
      else//nit timeout
      {
          OS_PRINTF("\nno nit~~~~~~~~~\n");
      }
      GS_CAS_Drv_FreeNITReq();
      cas_gs_ZoneCheckEnd();

      break;
    case CAS_IOCMD_SYSTEM_TIME_GET:
      {
        //(utc_time_t *)p_systime = (utc_time_t *)param;
        gs_get_sys_time((utc_time_t *)(param));
        ret = 0;
        //OS_PRINTK("GS_IO_CMD:%d-%d-%d,%d:%d:%d\n", 
          //            param->year, param->month, param->day, 
          //            param->hour, param->minute, param->second);
        #if 0
        p_systime->year = CurrentTime[0]+2000;
        p_systime->month = CurrentTime[1];
        p_systime->day = CurrentTime[2];
        p_systime->hour = CurrentTime[3];
        p_systime->minute = CurrentTime[4];
        #endif
      }
      
      break;
    default:
      break;
  }
  return ret;
}

/*!
  abc
  */
u16 cas_gs_get_v_pid(void)
{
  cas_gs_priv_t *p_priv = (cas_gs_priv_t *)g_cas_priv.cam_op[CAS_ID_GS].p_priv;

  return p_priv->v_pid;
}
/*!
  abc
  */
u16 cas_gs_get_a_pid(void)
{
  cas_gs_priv_t *p_priv = (cas_gs_priv_t *)g_cas_priv.cam_op[CAS_ID_GS].p_priv;

  return p_priv->a_pid;
}

/*!
  abc
  */
static void cat_parse_descriptor(u8 *buf,
                           s16 length)
{
  u8 *p_data = buf;
  u8  descriptor_tag = 0;
  u8  descriptor_length = 0;
  u16 ca_system_id = 0;
  u16 emm_pid = 0;
  static u8 szFilter[8] = {0};
  static u8 szMask[8] = {0};

  while(length > 0)
  {
    descriptor_tag = p_data[0];
    descriptor_length = p_data[1];
    p_data += 2;
    length -= 2;
    if(descriptor_length == 0)
    {
      continue;
    }
    switch(descriptor_tag)
    {
      case 0x09://DVB_DESC_CA:
        ca_system_id = MAKE_WORD(p_data[1], p_data[0]);
        emm_pid = MAKE_WORD(p_data[3],p_data[2] & 0x1F);
        if(ca_system_id == ((u8)gs_super_cas_id[0] << 8 | (u8)gs_super_cas_id[1]))
        {
          OS_PRINTF("\nGSCAS EMM emm pid =0x%x,Desc.CA_system_id =0x%x\n",
          emm_pid,ca_system_id);
          GS_CAS_Drv_FreeAllEmmReq();
          if(GS_CAS_DRV_GetSCInsertStatus() == SC_INSERT_OVER)
          {
            memset(szFilter,0,8);
            memset(szMask,0,8);
            szFilter[0] = 0x80;
            szFilter[1] = (u8)gs_super_cas_id[0];
            szFilter[2] = (u8)gs_super_cas_id[1];
            szFilter[3] = (u8)gs_super_cas_id[2];
            szFilter[4] = (u8)gs_super_cas_id[3];
            szFilter[5] = (u8)gs_smartcard_id[0];
            szFilter[6] = (u8)gs_smartcard_id[1];
            szFilter[7] = (u8)gs_smartcard_id[2];

            szMask[0] = 0xff;
            szMask[1] = 0xff;
            szMask[2] = 0xff;
            szMask[3] = 0xff;
            szMask[4] = 0xff;
            szMask[5] = 0xff;
            szMask[6] = 0xff;
            szMask[7] = 0xff;
            GSSTBCA_SetStreamGuardFilter(2,emm_pid,szFilter,szMask,8,0);
          }
        }
        break;
      default:
        break;
    }
    length -= descriptor_length;
    p_data = p_data + descriptor_length;
  }
}

static void gs_pmt_parse_descriptor(u8 *buf, s16 length,u16 program_number)
{
    u8 *p_data = buf;
	u16 ca_system_id = 0;
	u16 ecm_pid = 0;
    u8 descriptor_tag = 0;
    u8 descriptor_length = 0;
	static u8 szFilter[5] = {0};
	static u8 szMask[5] = {0};

    while (length > 0)
    {
        descriptor_tag = p_data[0];
        descriptor_length = p_data[1];
        CAS_GS_ADT_PRINTF("[%s]:LINE:%d,descriptor_tag=0x%x\n", __FUNCTION__, __LINE__, descriptor_tag);

        if(0 == descriptor_length)
        {
            break;
        }

        length -= 2;
        p_data += 2;
        switch (descriptor_tag)
        {
            case 0x09:  /* DVB_DESC_CA */
            {
				if(p_data != NULL)
				{
					ca_system_id = (p_data[0] << 8) | p_data[1];
        			ecm_pid = ((p_data[2]&0x1F) << 8) | p_data[3];
				}

                CAS_GS_ADT_PRINTF("[%s]:LINE:%d,[PMT] ecm_pid = 0x%x, system_id=0x%x\n", __FUNCTION__, __LINE__,ecm_pid, ca_system_id);
                if(ca_system_id == ((u8)gs_super_cas_id[0] << 8 | (u8)gs_super_cas_id[1]))
                {
					if(GS_CAS_DRV_GetSCInsertStatus() == SC_INSERT_OVER)
					{
					  CAS_GS_ADT_PRINTF("\nGSCASTB_SetEcm ticks=%d~~~\n",mtos_ticks_get());

        				memset(szFilter,0,5);
        				memset(szMask,0,5);
        				szFilter[0] = 0x81;
        				szFilter[1] = (u8)gs_super_cas_id[0];
        				szFilter[2] = (u8)gs_super_cas_id[1];
        				szFilter[3] = (u8)gs_super_cas_id[2];
        				szFilter[4] = (u8)gs_super_cas_id[3];
        		
        				szMask[0] = 0xff;
        				szMask[1] = 0xff;
        				szMask[2] = 0xff;
        				szMask[3] = 0xff;
        				szMask[4] = 0xff;
        		
        				GSSTBCA_SetStreamGuardFilter(1, ecm_pid,szFilter, szMask, 5, 0);
					}
                }
                else
                {
                    CAS_GS_ADT_PRINTF("[%s]:LINE:%d, not DESC CA!\n", __FUNCTION__, __LINE__);
                }
            }
                break;
            default:
                break;
        }
        length -= descriptor_length;
        p_data = p_data + descriptor_length;
    }
}

static void gs_cas_parse_pmt(u8 *p_buf)
{
    u8 table_id = 0;
    s16 section_length = 0;
    u16 program_number = 0;
    u8 version_number = 0;
    u16 pcr_pid = 0;
    u16 program_info_length = 0;
    u8 stream_type = 0;
    u16 elementary_pid = 0;
    u8 es_info_length = 0;
    u8 *p_data = NULL;

    CAS_GS_ADT_PRINTF("[GS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    MT_ASSERT(p_buf != NULL);

    p_data = p_buf;

    if(NULL != p_data)
    {
        table_id = p_data[0];
        section_length = (((U16)p_data[1] << 8) | p_data[2]) & 0x0FFF;
        if(section_length > 0)
        {
            program_number = (U16)p_data[3] << 8 | p_data[4];
            version_number = (p_data[5] & 0x3E) >> 1;
            pcr_pid = ((U16)p_data[8] << 8 | p_data[9]) & 0x1FFF;

            program_info_length = ((U16)p_data[10] << 8 | p_data[11]) & 0x0FFF;
            p_data += 12;
            //  GUOTONG_SetEcmPID(CA_INFO_CLEAR, NULL);
            gs_pmt_parse_descriptor(p_data, program_info_length,program_number);
            p_data += program_info_length;
            section_length -= (13 + program_info_length);//head and crc
            CAS_GS_ADT_PRINTF("[DS]stream_type=0x%x \n",p_data[0]);

            while(section_length > 0)
            {
                stream_type = p_data[0];
                elementary_pid = ((U16)p_data[1] << 8 | p_data[2]) & 0x1FFF;
                CAS_GS_ADT_PRINTF(" stream_type=0x%x, elementary_PID=0x%x\n", stream_type, elementary_pid);
                es_info_length = ((U16)p_data[3] << 8 | p_data[4]) & 0x0FFF;
                p_data += 5;
                gs_pmt_parse_descriptor(p_data, es_info_length,program_number);
                p_data += es_info_length;
                section_length -= (es_info_length + 5);
            }
        }
    }
}

/*!
  abc
  */
static void gs_cas_parse_cat(u8 *p_buf)
{
  u8 *p_data = p_buf;
  u8 version_number = 0;
  u16 length = 0;

  version_number = (u8)((p_data[5] & 0x3E) >> 1);
  length = (u16)(((p_data[1] & 0x0f) << 8) | p_data[2]);


  CAS_GS_ADT_PRINTF("[CAT] length=%d,version_number=%d\n",
                    length,version_number);
  p_data += 8;

  cat_parse_descriptor(p_data, length - 9);

  CAS_GS_ADT_PRINTF("\r\nparsing cat over.");
  return;
}

/*!
  abc
  */
s32 gs_cas_table_prase(u32 t_id, u8 *p_buf, u32 *p_result)
{
  u16 section_len = MAKE_WORD(p_buf[2], p_buf[1] & 0x0f) + 3;

  CAS_GS_ADT_PRINTF("\n  cas_gs  t_id = %d  table_id = %d section_len = %d ",
    t_id, p_buf[0], section_len);

  if(t_id == CAS_TID_PMT)
  {
    CAS_GS_ADT_PRINTF("\nNNNNNNNNNget pmt#######\n");
	memcpy(g_cas_priv.pmt_data, p_buf, section_len);
    g_cas_priv.pmt_length = section_len;

  }
  else if(t_id == CAS_TID_CAT)
  {
    CAS_GS_ADT_PRINTF("\nNNNNNNNNNget cat#######\n");
    memcpy(g_cas_priv.cat_data, p_buf, section_len);
    g_cas_priv.cat_length = section_len;
    gs_cas_parse_cat(g_cas_priv.cat_data);
  }
  else if(t_id == CAS_TID_NIT)
  {
  }

  return 0;
}


/*!
  abc
  */
RET_CODE gs_cas_table_resent(void)
{
  if(g_cas_priv.pmt_length != 0)
  {
  	 CAS_GS_ADT_PRINTF("[%s]:%d \n",__FUNCTION__,__LINE__);
	 gs_cas_parse_pmt(g_cas_priv.pmt_data);
  }
  if(g_cas_priv.cat_length != 0)
  {
  	 CAS_GS_ADT_PRINTF("[%s]:%d \n",__FUNCTION__,__LINE__);
     gs_cas_parse_cat(g_cas_priv.cat_data);
  }
  return SUCCESS;
}
/*!
  abc
  */
RET_CODE cas_gs_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
  cas_gs_priv_t *p_priv = NULL;
  CAS_GS_ADT_PRINTF("\ncas cas_gs_attach \n");
  g_cas_priv.cam_op[CAS_ID_GS].inited = 0;
  g_cas_priv.cam_op[CAS_ID_GS].card_reset_by_cam = 1;
  g_cas_priv.cam_op[CAS_ID_GS].attached = 1;

  g_cas_priv.cam_op[CAS_ID_GS].func.init
    = cas_gs_init;
  g_cas_priv.cam_op[CAS_ID_GS].func.deinit
    = cas_gs_deinit;
  g_cas_priv.cam_op[CAS_ID_GS].func.identify
    = NULL;
  g_cas_priv.cam_op[CAS_ID_GS].func.io_ctrl
    = cas_gs_io_ctrl;
  g_cas_priv.cam_op[CAS_ID_GS].func.table_process
    = gs_cas_table_prase;
  g_cas_priv.cam_op[CAS_ID_GS].func.table_resent
    = gs_cas_table_resent;

  g_cas_priv.cam_op[CAS_ID_GS].func.card_remove = cas_gs_card_remove;
  g_cas_priv.cam_op[CAS_ID_GS].func.card_reset = cas_gs_card_reset;

  p_priv = mtos_malloc(sizeof(cas_gs_priv_t));
  if(p_priv ==  NULL)
  {
      CAS_GS_ADT_PRINTF("[divi],p_priv malloc error ! \n");
      return ERR_FAILURE;
  }
  g_cas_priv.cam_op[CAS_ID_GS].p_priv = (cas_gs_priv_t *)p_priv;
  memset(p_priv, 0x00, sizeof(cas_gs_priv_t));

  p_priv->flash_size = p_cfg->flash_size;
  p_priv->flash_start_adr = p_cfg->flash_start_adr;
  CAS_GS_ADT_PRINTF("\ngscas:flash  %x %x\n", p_priv->flash_size, p_priv->flash_start_adr);
  p_priv->nvram_read = p_cfg->nvram_read;
  p_priv->nvram_write = p_cfg->nvram_write;
#if 1
  if(p_cfg->machine_serial_get == NULL)
  {
    memset(gs_stb_sn, 0, sizeof(gs_stb_sn));
    strcpy(gs_stb_sn,"100001");
  }
  else if(p_cfg->machine_serial_get((u8 *)gs_stb_sn, sizeof(gs_stb_sn)) != SUCCESS)
  {
    memset(gs_stb_sn, 0, sizeof(gs_stb_sn));
    strcpy(gs_stb_sn,"100001");
  }

  CAS_GS_ADT_PRINTF("~~~~~~~~~~~~~~stb id = %s    len =%d\n",gs_stb_sn, strlen(gs_stb_sn));

  area_limit = p_cfg->area_limit_free;
#endif
  *p_cam_id = (u32)(&g_cas_priv.cam_op[CAS_ID_GS]);

  return SUCCESS;
}

