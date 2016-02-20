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
#include "mtos_int.h"
#include "mtos_misc.h"
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


#include "nim.h"
#include "smc_op.h"
#include "stb_public.h"
#include "stb_app2ca.h"
#include "stb_ca2app.h"
#include "ql_cas_include.h"


extern void Ql_CAS_Drv_ShowAllCaReq(void);
extern void Ql_CAS_Drv_FreeAllEcmReq(void);
extern void Ql_CAS_Drv_FreeAllEmmReq(void);
extern void Ql_CAS_Drv_FreeNitReq(void);
extern void Ql_CAS_Drv_FreeBatReq(void);  
extern u8 QL_CAS_DRV_GetSCInsertStatus(void);
extern void QL_CAS_DRV_SetSCInsertStatus(u8 uScStatus);
extern BVOID QLSTBCA_GetSCStatus(BU8 * pbyStatus);

extern void STBCA_ZoneCheckEnd(void);
extern u32 QL_CAS_Drv_ClientInit(void);
extern cas_adapter_priv_t g_cas_priv;
extern msg_info_t OsdMsg;
extern finger_msg_t msgFinger;
extern u8 ql_ecm_count ;
u8 ql_ca_nit_flag = 0;
u8 ql_ca_bat_flag = 0;
u8 ql_ca_nit_state = 0;


/*
*Network License Manager API 
*/
BOOL get_vol_entitlement_from_ca(void)
{
	CAMenu_Product CaMenu_product = {0};

	if(STBCA_GetCaMenuProduct(&CaMenu_product) == true)
	{			  
		if(CaMenu_product.CaMenu_number > 0)
		{
			OS_PRINTF("[%s]%d get entitlement\n", __FUNCTION__, __LINE__);
			return TRUE;
		}
		OS_PRINTF("[%s]%d empty entitlement\n", __FUNCTION__, __LINE__);
	}
	else
	{
		OS_PRINTF("\[%s]%d get entitlement fail\n", __FUNCTION__, __LINE__);
	}
	return FALSE;
}

void send_event_to_ui_from_authorization(u32 event)
{
	cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;
	
	if(event == 0)
	{
		OS_PRINTF("[%s]%d send fail enent \n",__FUNCTION__,__LINE__);
		return;
	}
	cas_send_event(p_priv->slot, CAS_ID_QL, event, 0);
	OS_PRINTF("[%s]%d send enent from authorization \n",__FUNCTION__,__LINE__);
}



/*!
  abc
  */

static void __by_MjdToDate(u8 *pu8Mjd, u8 *pu8Date)
{
    u32 u32Mjd = 0;
    u32 u32Y1 = 0;
    u32 u32M1 = 0;
    u32 u32D1 = 0;
    u32 u32Wd = 0;

    u32Mjd = (pu8Mjd[0] << 8) | pu8Mjd[1];
    u32Y1 = (u32Mjd * 100 - 1507820) / 36525;
    u32M1 = (u32Mjd * 10000 - 149561000 - u32Y1 * 36525 / 100 * 10000) / 306001;
    u32D1 = u32Mjd - 14956 - (u32Y1 * 36525 / 100) - (u32M1 * 306001 / 10000);

    if((u32M1 == 14) || (u32M1 == 15))
    {
      u32Y1++;
      u32M1 -= 12;
    }
    u32M1--;
    u32Wd = ((u32Mjd + 2) % 7) + 1;

    u32Y1 -= 100;
    pu8Date[0] = (U8)(u32Y1&0xFF);
    pu8Date[1] = (U8)(u32M1&0xFF);
    pu8Date[2] = (U8)(u32D1&0xFF);
    pu8Date[3] = (U8)(u32Wd&0xFF);
}


u8 qlcas_smartcard_status(void)
{
	u32 param;
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
/*!
  abc
  */
static RET_CODE cas_ql_init(void)
{
  u8 ret = 0;
  static u32 init_flag = 0;

  if(!init_flag)
  {
    CAS_QILIAN_PRINTF("cas_qi_init\n");

    QL_CAS_Drv_ClientInit();

    ret = STBCA_Init();

    if(ret != 1)
    {
      OS_PRINTF("STBCA_Init return error! ret=%u xxxx",ret);
      return ERR_FAILURE;
    }
    OS_PRINTF("STBCA_Init return SUCCESS!\n");
    init_flag = 1;
    mtos_task_sleep(300);
  }
  return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_ql_deinit(void)
{
  return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_ql_card_remove(void)
{
  CAS_QILIAN_PRINTF("Ql cas remove \n");

  QL_CAS_DRV_SetSCInsertStatus(SC_REMOVE_START);
  Ql_CAS_Drv_FreeAllEmmReq();
  Ql_CAS_Drv_FreeAllEcmReq();
  STBCA_SCRemove();
  QL_CAS_DRV_SetSCInsertStatus(SC_REMOVE_OVER);

  return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_ql_card_reset(u32 slot, card_reset_info_t *p_info)
{
  cas_ql_priv_t *p_priv =  (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;
  BU32 cardid = 0;
  BU16 zoneid = 0;
  BU8 ret = 0;
  OS_PRINTF("\nql cas  card insert \n");

  //p_priv->p_smc_dev = (scard_device_t *)p_info->p_smc;
  p_priv->slot = slot;
  scard_pro_register_op((scard_device_t *)p_info->p_smc);
  QL_CAS_DRV_SetSCInsertStatus(SC_INSERT_START);
	mtos_task_sleep(10);	//防止七联CA卡无法成功复位
  ret = STBCA_SCInsert();
	mtos_task_sleep(10);
  QL_CAS_DRV_SetSCInsertStatus(SC_INSERT_OVER);
  ret = STBCA_GetSmartCardID(&cardid,&zoneid);
  OS_PRINTF("\nret = %d cardid = %d\n",ret,cardid);
  return SUCCESS;
}

static RET_CODE cas_ql_identify(u16 ca_sys_id)
{
  return (STBCA_IsLYCA(ca_sys_id));  
}


/*!
  abc
  */
static RET_CODE cas_ql_io_ctrl(u32 cmd, void *param)
{
  cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;
  cas_table_info_t *p_Cas_table_info = NULL;
  cas_card_info_t *p_card_info = NULL;
  RET_CODE ret = 0;
  u32 i = 0;
  BU32 dwCardID = 0;
  BU16 ZoneID = 0;
  BU32 dwVer = 0;
  u8 ver1 = 0;
  u8 ver2 = 0;
  u8 ver3 = 0;
  u8 ver4 = 0;
  u8 date[4] = {0};
//  BU16 ProductCount = 0;
  CAMenu_Product CaMenu_product = {0};
  product_entitles_info_t *p_entitles_info = NULL;
  cas_mail_headers_t *p_mail_headers = NULL;
  cas_mail_body_t *p_mailcontent = NULL;
  BU16 email_count = 0;
  BU16 new_email_count = 0;
  Email_Data email = {0};
  u16 year = 0;
  u8 month = 0;
  u8 day = 0;
  msg_info_t *p_msg_info = NULL;
  finger_msg_t *p_finger_info = NULL;
  unsigned char  match[16] = {0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned char  mask[16] =  {0xff,0,0,0,0xff,0,0,0,0,0,0,0,0,0,0,0};
  nim_device_t *p_nim_dev;
  nim_channel_info_t *p_nim_info;

  u16 ca_sys_id[3]; 
  u16 ecm_pid[3];
  u16 svc_pid[3];
				
  switch(cmd)
  {
    case CAS_IOCMD_STOP:
      CAS_QILIAN_PRINTF("\nSTOP\n");
      // BYCASTB_SetCurEcmInfos(0, NULL);
      break;
    case CAS_IOCMD_SET_ECMEMM:
       p_Cas_table_info = (cas_table_info_t *)param;
      //if(BY_CAS_DRV_GetSCInsertStatus()==SC_INSERT_OVER)
      {
        OS_PRINTF("\nCAS_IOCMD_SET_ECMEMM ticks=%d~~~~~~~~~~~~~~~~~~~~~\n",mtos_ticks_get());
        Ql_CAS_Drv_FreeAllEcmReq();
        ql_ecm_count = 0;
        for(i = 0; i < CAS_MAX_ECMEMM_NUM; i++)
        {
          if(1 == STBCA_IsLYCA(p_Cas_table_info->ecm_info[i].ca_system_id))
          {			
			ca_sys_id[ql_ecm_count] = p_Cas_table_info->ecm_info[i].ca_system_id;
			ecm_pid[ql_ecm_count] = p_Cas_table_info->ecm_info[i].ecm_id;
			svc_pid[ql_ecm_count] = p_Cas_table_info->service_id;
			ql_ecm_count ++;
            OS_PRINTF("\necm succ QLCAS_LIST_ADD ecm_id=0x%x serviceid=0x%x\n",
              p_Cas_table_info->ecm_info[i].ecm_id,p_Cas_table_info->service_id);
						
          }
        }
		if (ql_ecm_count == 1) 	//单CA_SYSTEM_ID时按实际信息传输
		{
			STBCA_SetCurECMInfo(ecm_pid[0], svc_pid[0]);
			OS_PRINTF("\n[切台更新ECM信息]ca_system_id=0x%x, ecm_id=0x%x,svc_id=0x%x.\n", 
										ca_sys_id[0],
										ecm_pid[0],
										svc_pid[0]);
		}			
		else if (ql_ecm_count > 1)//多CA_SYSTEM_ID时，只传递ID为0X7818对应的ECM_PID
		{
			for (i = 0; i < ql_ecm_count; i++)
			{
				if (ca_sys_id[i] == 0x7818)
				{
					STBCA_SetCurECMInfo(ecm_pid[i], svc_pid[i]);
					OS_PRINTF("\n[切台更新ECM信息]ca_system_id=0x%x, ecm_id=0x%x,svc_id=0x%x.\n", 
										ca_sys_id[i],
										ecm_pid[i],
										svc_pid[i]);
					break;
				}
			}
		}
      }
      break;
    case CAS_IOCMD_VIDEO_PID_SET:
      p_priv->v_pid = *((u16 *)param);
      OS_PRINTF("\nSET v_pid [0x%x]\n",  p_priv->v_pid);
      break;
    case CAS_IOCMD_AUDIO_PID_SET:
      p_priv->a_pid = *((u16 *)param);
      OS_PRINTF("\nSET a_pid [0x%x]\n",  p_priv->a_pid);
      break;
    case CAS_IOCMD_CARD_INFO_GET:
	  p_card_info = (cas_card_info_t *)param;
      if(!qlcas_smartcard_status())
      {
        p_card_info->card_state = 1;
      	OS_PRINTF("[UI] smartcard out \n");
      	break;
      }
	  else
	  {
		p_card_info->card_state = 0;
	  }
      
      if(STBCA_GetSmartCardID(&dwCardID,&ZoneID) != true)
      {
        dwCardID = 0;
        ZoneID= 0;
        CAS_QILIAN_PRINTF("\n get id error\n");
      }
      else
      {
        CAS_QILIAN_PRINTF("\ndwCardID = %d\n",  dwCardID);
      }

      sprintf((char *)(p_card_info->sn),"%ld",(u32)dwCardID);
      //CAS_BY_ADT_PRINTF("\n CardID = %s\n",  p_card_info->sn);
      STBCA_GetVer(&dwVer);
      CAS_QILIAN_PRINTF("\n 0x%08x \n",dwVer);
      
      ver1 = (u8)(dwVer >> 24);
      ver2 = (u8)((dwVer&0x00ff0000) >> 16);
      ver3 = (u8)((dwVer&0x0000ff00) >> 8);
      ver4 = (u8)(dwVer&0x000000ff);
      sprintf((char *)(p_card_info->cas_ver),"%d.%d.%d.%d", ver1, ver2, ver3, ver4);
      CAS_QILIAN_PRINTF("\n Cas Ver = %s\n",  p_card_info->cas_ver);

	  if(0 == (p_card_info->card_type = (u8)STBCA_isDaughterCard(&dwCardID)))
	  {
	  	CAS_QILIAN_PRINTF("\n mother card \n");
	  }
	  else
	  {
		sprintf((char *)(p_card_info->mother_card_id),"%ld",(u32)dwCardID);
		CAS_QILIAN_PRINTF("\n child card \n");
	  }
	  
      break;
    case CAS_IOCMD_ENTITLE_INFO_GET:
      if(!qlcas_smartcard_status())
      {
      	OS_PRINTF("[UI] smartcard out \n");
      	break;
      }
      p_entitles_info = (product_entitles_info_t *)param;
	  memset(&CaMenu_product,0,sizeof(CAMenu_Product));
      if(STBCA_GetCaMenuProduct(&CaMenu_product) == true)
      {				
        p_entitles_info->max_num = CaMenu_product.CaMenu_number;
		OS_PRINTF("[%s:%d]p_entitles_info->max_num=%d\n", __FUNCTION__, __LINE__, p_entitles_info->max_num);
        for(i = 0;i < CaMenu_product.CaMenu_number; i++)
        {
          p_entitles_info->p_entitle_info[i].product_id
            = (u32)CaMenu_product.Camenu_Unit[i].Product_ID;

		  OS_PRINTF("\n[BY] name:%s\n", CaMenu_product.CaMenu_Name[i].ProductName);				
		  snprintf(p_entitles_info->p_entitle_info[i].product_name,CAS_PRODUCT_NAME_MAX_LEN,"%s",CaMenu_product.CaMenu_Name[i].ProductName);

		  __by_MjdToDate(CaMenu_product.Camenu_Unit[i].Purchase_start_time,date);
          year = 2000 + date[0];
          month = date[1];
          day = date[2];
          OS_PRINTF("\nstart year=%d,month=%d,day=%d", year, month, day);
          p_entitles_info->p_entitle_info[i].start_time[0] = (u8)(year / 256);
          p_entitles_info->p_entitle_info[i].start_time[1] = (u8)(year % 256);
          p_entitles_info->p_entitle_info[i].start_time[2] = month;
          p_entitles_info->p_entitle_info[i].start_time[3] = day;
										
          __by_MjdToDate(CaMenu_product.Camenu_Unit[i].Purchase_end_time,date);
          year = 2000 + date[0];
          month = date[1];
          day = date[2];
          OS_PRINTF("\nend year=%d,month=%d,day=%d", year, month, day);
          p_entitles_info->p_entitle_info[i].expired_time[0] = (u8)(year / 256);
          p_entitles_info->p_entitle_info[i].expired_time[1] = (u8)(year % 256);
          p_entitles_info->p_entitle_info[i].expired_time[2] = month;
          p_entitles_info->p_entitle_info[i].expired_time[3] = day;
        }
      }
      else
      {
        OS_PRINTF("\nSTBCA_GetCaMenuProduct error\n");
      }
      break;
    case CAS_IOCMD_MAIL_HEADER_GET:
      STBCA_GetEmailCount(&email_count,&new_email_count);

      p_mail_headers = (cas_mail_headers_t *)param;
      p_mail_headers->total_email_num =  email_count;
	  p_mail_headers->max_num =  email_count;
      p_mail_headers->new_mail_num =  new_email_count;
      CAS_QILIAN_PRINTF("\n-------total--%d----new---%d---------\n",
        email_count,new_email_count);

      for(i = 0; i < email_count; i++)
      {
        CAS_QILIAN_PRINTF("\n------mail-%d------\n",i + 1);
        STBCA_GetEmailContent(i,&email);
  
        p_mail_headers->p_mail_head[i].m_id = email.MessageId;
        if(email.MessageFlag == 3)
        {
          CAS_QILIAN_PRINTF("\n------old------\n");
          p_mail_headers->p_mail_head[i].new_email = 0;
        }
        else
        {
          CAS_QILIAN_PRINTF("\n------new------\n");
          p_mail_headers->p_mail_head[i].new_email = 1;
        }
  
  
        memcpy(p_mail_headers->p_mail_head[i].subject,email.Title,EMail_Title_MAXLEN);
        CAS_QILIAN_PRINTF("\n------titls= %s -----0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
          p_mail_headers->p_mail_head[i].subject,
          email.SendTime[0],
          email.SendTime[1],
          email.SendTime[2],
          email.SendTime[3],
          email.SendTime[4]);
        __by_MjdToDate(email.SendTime
            ,date);
        year = 2000 + date[0];
        month = date[1];
        day = date[2];
		CAS_QILIAN_PRINTF("[debug] 0x%02x 0x%02x 0x%02x \n",year,month,day);
        p_mail_headers->p_mail_head[i].creat_date[0] = (u8)(year / 100);
        p_mail_headers->p_mail_head[i].creat_date[1] = (u8)(year % 100);
        p_mail_headers->p_mail_head[i].creat_date[2] = month;
        p_mail_headers->p_mail_head[i].creat_date[3] = day;
        p_mail_headers->p_mail_head[i].creat_date[4] = email.SendTime[2];
        p_mail_headers->p_mail_head[i].creat_date[5] = email.SendTime[3];
        p_mail_headers->p_mail_head[i].creat_date[6] = email.SendTime[4];
      }

      break;
    case CAS_IOCMD_MAIL_BODY_GET:
      p_mailcontent = (cas_mail_body_t *)param;
      STBCA_EmailRead(p_mailcontent->mail_id);

      STBCA_GetEmailContent(p_mailcontent->mail_id,&email);
      CAS_QILIAN_PRINTF("\n~~~~mail_id = %d~~~~~~~~~~~~\n",p_mailcontent->mail_id);
      CAS_QILIAN_PRINTF("\nemail.ContentLen=%d\n",email.ContentLen);
      CAS_QILIAN_PRINTF("\nemail.Content=%s\n",email.Content);
      memcpy(p_mailcontent->data,email.Content,email.ContentLen);
      p_mailcontent->data_len = email.ContentLen;
    
      break;
    case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
      {
        u32 mail_index = *(u32 *)param;
        CAS_QILIAN_PRINTF("\r\nDEL=%d\n",(u16)mail_index);
        STBCA_DelEmail(mail_index + 1);
      }
      break;
    case CAS_IOCMD_FACTORY_SET:
      OS_PRINTF("\r\nCAS_IOCMD_FACTORY_SET\n");
      Ql_CAS_Drv_FreeAllEmmReq();
      OS_PRINTF("\r\nCAS_IOCMD_FACTORY_SET emm\n");
      Ql_CAS_Drv_FreeAllEcmReq();
      OS_PRINTF("\r\nCAS_IOCMD_FACTORY_SET ecm\n");
      //if(QL_CAS_DRV_GetSCInsertStatus()==SC_INSERT_OVER)
       // STBCA_Init();
      OS_PRINTF("\r\nCAS_IOCMD_FACTORY_SET over\n");
      break;
    case CAS_IOCMD_MAIL_DELETE_ALL:
      CAS_QILIAN_PRINTF("\r\nDEL ALL\n");
      STBCA_DelEmail(0);
      break;
    case CAS_IOCMD_OSD_GET:
      p_msg_info = (msg_info_t *)param;
      memcpy(p_msg_info, &OsdMsg, sizeof(OsdMsg));
      CAS_QILIAN_PRINTF("\nGET=%s\n", OsdMsg.data);
      CAS_QILIAN_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_OSD_GET\n");
      break;
    case CAS_IOCMD_FINGER_MSG_GET:
      p_finger_info = (finger_msg_t *)param;
      memcpy(p_finger_info, &msgFinger, sizeof(msgFinger));
      CAS_QILIAN_PRINTF("\nGET=%s\n", msgFinger.data);
      CAS_QILIAN_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_FINGER_GET\n");
      break;
    case CAS_IOCMD_ZONE_CHECK:
      OS_PRINTF("\n QL_CAS_DRV_GetSCInsertStatus %d\n",
      QL_CAS_DRV_GetSCInsertStatus());
      if(qlcas_smartcard_status())
      {
          OS_PRINTF("\n card in\n");
          i = 0;
          while(1)
          {
              i ++;
              if(i > 200)
              {
                  STBCA_ZoneCheckEnd();
                  OS_PRINTF("\n card error\n");
                  return SUCCESS;
              }
              if(QL_CAS_DRV_GetSCInsertStatus() == SC_INSERT_OVER)
              {
                  if(STBCA_GetSmartCardID(&dwCardID,&ZoneID) == true)//makesure the card is ql card
                  {
				  	OS_PRINTF("[%s:%d]Get card info!\n", __FUNCTION__, __LINE__);
					break;
				  }    
              }
              mtos_task_sleep(10);
          }
          p_nim_info = (nim_channel_info_t *)param;
          OS_PRINTF("\nset tuner_lock_delivery %d %d %d\n",p_nim_info->frequency,
		    p_nim_info->param.dvbc.symbol_rate,p_nim_info->param.dvbc.modulation);
          p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
                                                                DEV_IDT_TYPE,
                                                                SYS_DEV_TYPE_NIM);

          nim_channel_connect(p_nim_dev, p_nim_info, 0);
          if(p_nim_info->lock == 1)
          {
              OS_PRINTF("\n lock \n");
          }
          else
          {
              OS_PRINTF("\n unlock \n");
              STBCA_ZoneCheckEnd();
              return SUCCESS;
          }
          ql_ca_nit_flag = 0;
          ql_ca_nit_state = 0;
          ql_ca_bat_flag = 0;          

          //开nit接收
          STBCA_SetStreamGuardFilter(0x40,0x10,
          match,mask,5,0);
          i = 0;
          while((ql_ca_nit_state == 0) && (i < 200))
          {
              i++;
              mtos_task_sleep(10);
          }

          if(ql_ca_nit_flag == 1)//get nit
          {
              OS_PRINTF("\nGET nit\n");
              Ql_CAS_Drv_FreeNitReq();
          }
          else if(ql_ca_nit_flag == 2)//nit no data
          {
              OS_PRINTF("\nGET nit no data\n");
              Ql_CAS_Drv_FreeNitReq();
              STBCA_ZoneCheckEnd();
              return SUCCESS;
          }
          else
          {
              OS_PRINTF("\nno nit~~~~~~~~~\n");
              Ql_CAS_Drv_FreeNitReq();
              STBCA_ZoneCheckEnd();
              return SUCCESS;
          }

          i = 0;
          while(ql_ca_bat_flag == 0 && i < 400)
          {
              i++;
              mtos_task_sleep(10);
          }
          if(ql_ca_bat_flag == 1)//get bat
          {
              OS_PRINTF("\nGET bat\n");
              Ql_CAS_Drv_FreeBatReq();
          }
          else if(ql_ca_bat_flag == 2)
          {
             OS_PRINTF("\nbat fre unlock\n");
          }
          else
          {
            OS_PRINTF("\nno bat\n");
            Ql_CAS_Drv_FreeBatReq();
          }


      }
      STBCA_ZoneCheckEnd();

      break;
    default:
      break;
  }
  return ret;
}

/*!
  abc
  */
u16 cas_ql_get_v_pid(void)
{
  cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;

  return p_priv->v_pid;
}
/*!
  abc
  */
u16 cas_ql_get_a_pid(void)
{
  cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;

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
  u16 ca_system_id[3] = {0, 0, 0};
  u16 emm_pid[3] = {0, 0, 0};
  u8  emm_cnt = 0;
  u8  i = 0;
  u16 tmp_ca_sys_id = 0;
  u16 tmp_emm_pid = 0;

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
        tmp_ca_sys_id = MAKE_WORD(p_data[1], p_data[0]);
        tmp_emm_pid = MAKE_WORD(p_data[3],p_data[2] & 0x1F);
				
        if(1 == STBCA_IsLYCA(tmp_ca_sys_id))
        {
		  ca_system_id[emm_cnt] = tmp_ca_sys_id;
		  emm_pid[emm_cnt] = tmp_emm_pid;
		  emm_cnt++;
          Ql_CAS_Drv_FreeAllEmmReq();
        }
        break;
      default:
        break;
    }
    length -= descriptor_length;
    p_data = p_data + descriptor_length;
  }

  if (emm_cnt == 1)
  {
  	STBCA_SetEmmPid(emm_pid[0]);
	OS_PRINTF("\nQLCAS EMM emm pid =0x%x,Desc.CA_system_id =0x%x\n", 
				emm_pid[0],ca_system_id[0]);
  }
  else if (emm_cnt > 1)
  {
  	for (i=0; i<emm_cnt; i++)
  	{
		if (ca_system_id[i] == 0x7818)
		{
			STBCA_SetEmmPid(emm_pid[i]);
			OS_PRINTF("\nQLCAS EMM emm pid =0x%x,Desc.CA_system_id =0x%x\n", 
						emm_pid[i],ca_system_id[i]);
			break;
			
		}
	}
  }
}

/*!
  abc
  */
static void ql_cas_parse_cat(u8 *p_buf)
{
  u8 *p_data = p_buf;
  u8 version_number = 0;
  u16 length = 0;

  version_number = (u8)((p_data[5] & 0x3E) >> 1);
  length = (u16)(((p_data[1] & 0x0f) << 8) | p_data[2]);


  CAS_QILIAN_PRINTF("[CAT] length=%d,version_number=%d\n",
                    length,version_number);
  p_data += 8;

  cat_parse_descriptor(p_data, length - 9);

  CAS_QILIAN_PRINTF("\r\nparsing cat over.");
  return;
}
/*!
  abc
  */
s32 ql_cas_table_prase(u32 t_id, u8 *p_buf, u32 *p_result)
{
  u16 section_len = MAKE_WORD(p_buf[2], p_buf[1] & 0x0f) + 3;

  CAS_QILIAN_PRINTF("\n  cas_by  t_id = %d  table_id = %d section_len = %d ",
    t_id, p_buf[0], section_len);

  if(t_id == CAS_TID_PMT)
  {
    OS_PRINTF("\nNNNNNNNNNget pmt#######\n");
/*
    memcpy(g_cas_priv.pmt_data, p_buf, section_len);
    g_cas_priv.pmt_length = section_len;
    tf_cas_parse_pmt(g_cas_priv.pmt_data);
*/
  }
  else if(t_id == CAS_TID_CAT)
  {
    OS_PRINTF("\n******get cat******\n");
    memcpy(g_cas_priv.cat_data, p_buf, section_len);
    g_cas_priv.cat_length = section_len;
    ql_cas_parse_cat(g_cas_priv.cat_data);
  }
  else if(t_id == CAS_TID_NIT)
  {
  }

  return 0;
}

/*!
  abc
  */
RET_CODE ql_cas_table_resent(void)
{
  if(g_cas_priv.pmt_length != 0)
  {
    //tf_cas_parse_pmt(g_cas_priv.pmt_data);
  }
  if(g_cas_priv.cat_length != 0)
  {
    //tf_cas_parse_cat(g_cas_priv.cat_data);
  }
  return SUCCESS;
}
/*!
  abc
  */
RET_CODE cas_qi_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
  cas_ql_priv_t *p_priv = NULL;
  OS_PRINTF("\nql cas cas_ql_attach \n");
  g_cas_priv.cam_op[CAS_ID_QL].attached = 1;
  g_cas_priv.cam_op[CAS_ID_QL].inited  = 0;
  g_cas_priv.cam_op[CAS_ID_QL].card_reset_by_cam = 1;

  g_cas_priv.cam_op[CAS_ID_QL].func.init
    = cas_ql_init;
  g_cas_priv.cam_op[CAS_ID_QL].func.deinit
    = cas_ql_deinit;
  g_cas_priv.cam_op[CAS_ID_QL].func.identify
    = cas_ql_identify;
  g_cas_priv.cam_op[CAS_ID_QL].func.io_ctrl
    = cas_ql_io_ctrl;
  g_cas_priv.cam_op[CAS_ID_QL].func.table_process
    = ql_cas_table_prase;
  g_cas_priv.cam_op[CAS_ID_QL].func.table_resent
    = ql_cas_table_resent;

  g_cas_priv.cam_op[CAS_ID_QL].func.card_remove = cas_ql_card_remove;
  g_cas_priv.cam_op[CAS_ID_QL].func.card_reset = cas_ql_card_reset;

  p_priv = mtos_malloc(sizeof(cas_ql_priv_t));
  g_cas_priv.cam_op[CAS_ID_QL].p_priv = (cas_ql_priv_t *)p_priv;
  memset(p_priv, 0x00, sizeof(cas_ql_priv_t));
  p_priv->flash_size = p_cfg->flash_size;
  p_priv->flash_start_adr = p_cfg->flash_start_adr;
  CAS_QILIAN_PRINTF("\nqlcas:flash  %x %x\n", p_priv->flash_size, p_priv->flash_start_adr);
  p_priv->nvram_read = p_cfg->nvram_read;
  p_priv->nvram_write = p_cfg->nvram_write;

  *p_cam_id = (u32)(&g_cas_priv.cam_op[CAS_ID_QL]);

  return SUCCESS;
}

