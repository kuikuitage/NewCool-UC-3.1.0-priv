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

#include "BYCAca_stb.h"
#include "BYCApubtype.h"
#include "BYCAstb_ca.h"
#include "nim.h"
#include "smc_op.h"
#include "boyuan_cas_include.h"
#include "charsto.h"
#include "sys_dbg.h"


//#define CAS_BY_ADT_DEBUG

#ifdef CAS_BY_ADT_DEBUG
#define CAS_BY_ADT_PRINTF mtos_printk
#else
#define CAS_BY_ADT_PRINTF(...) do{} while(0)
#endif

static cas_adapter_priv_t *by_cas_priv;

extern void BY_CAS_Drv_FreeAllEmmReq(void);
extern void BY_CAS_Drv_FreeAllEcmReq(void);


extern u8 BY_CAS_DRV_GetSCInsertStatus(void);
extern void BY_CAS_DRV_SetSCInsertStatus(u8 uScStatus);
extern u32 BY_CAS_Drv_ClientInit(void);
extern void BY_CAS_Drv_FreeNitReq(void);
extern void BY_CAS_Drv_FreeBatReq(void);

static msg_info_t OsdMsg = {0, {0,},};
static finger_msg_t msgFinger = {{0,},};
static cas_force_channel_t force_channel= {0, 0, 0, 0, {0,},};
static msg_info_t CaFrameMsg = {0, {0,},};


u8 by_ca_nit_flag = 0;
u8 by_ca_nit_state = 0;

u8 by_ca_bat_flag = 0;
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
	/*!
	ver_num  2: by2.0  3:by3.0
	*/
	CAS_LIB_TYPE version;

}cas_by_priv_t;

/*
*Network License Manager API 
*/
BOOL get_vol_entitlement_from_ca(void)
{
	OS_PRINTF("\[%s]%d byca get entitlement\n", __FUNCTION__, __LINE__);
	return TRUE;
}

void send_event_to_ui_from_authorization(u32 event)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	
	if(event == 0)
	{
		OS_PRINTF("[%s]%d send enent fail \n",__FUNCTION__,__LINE__);
		return;
	}
	cas_send_event(p_priv->slot, CAS_ID_BY, event, 0);
	OS_PRINTF("[%s]%d send enent from authorization \n",__FUNCTION__,__LINE__);
}


/*!
  abc
  */
static void set_event(cas_by_priv_t *p_priv, u32 event)
{
	//CAS_BY_ADT_PRINTF("bycas: set_event  event  =%d  \n", event);
	if(event == 0)
	{
	return;
	}

	cas_send_event(p_priv->slot, CAS_ID_BY, event, 0);

}

  /*++
  功能：显示新邮件的标志。
  参数：
  byShow:					标志类型。含义如下：
  					BYCAS_NEW_EAMIL						新邮件通知。
  					BYCAS_NEW_EMAIL_NO_ROOM		有新邮件，但Email空间不够。
  					BYCAS_EMAIL_NONE					没有新邮件。
  --*/
BVOID BYSTBCA_EmailNotify(BU8 byShow)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	switch(byShow)
	{
		case BYCAS_NEW_EAMIL:
			set_event(p_priv, CAS_C_SHOW_NEW_EMAIL);
			CAS_BY_ADT_PRINTF("\n有新邮件\n");
			break;
		case BYCAS_NEW_EMAIL_NO_ROOM:
			set_event(p_priv, CAS_C_SHOW_SPACE_EXHAUST);
			CAS_BY_ADT_PRINTF("\n邮件满\n");
			break;
		case BYCAS_EMAIL_NONE:
			set_event(p_priv, CAS_C_HIDE_NEW_EMAIL);
			CAS_BY_ADT_PRINTF("\n无新邮件\n");
			break;
		default:
			CAS_BY_ADT_PRINTF("\n邮件错误\n");
			break;
	}
	return;
}

  /*++
  功能：通知上层区域检测结束
  参数：
  --*/
static void BYSTBCA_ZoneCheckEnd(void)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;

	OS_PRINTF("\r\n*** BYSTBCA_ZoneCheckEnd*** \r\n");
	set_event(p_priv, CAS_S_ZONE_CHECK_END);
	return;
}

/*------------ 类型1的界面：不允许用户通过遥控器取消显示的界面 -----------------------*/
/*
功能：显示用户不芄劭唇谀康忍崾信息，下面两个函数对应使用。
参数：
byMesageNo:				要显示的消息编号。
--*/
static u8 by_ecm_count = 0;

BVOID   BYSTBCA_ShowPromptMessage(BU8 byMesageNo)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	u32 event = 0;
	DEBUG(CAS, INFO, "byMesageNo:%d\n");
	if(by_ecm_count == 0)
		byMesageNo = BYCA_NO_ERROR;
	switch(byMesageNo)
	{
		case BYCA_NO_ERROR://clean
			//CAS_BY_ADT_PRINTF("\n没有错误，取消上次显示\n");
			event = CAS_S_CLEAR_DISPLAY;
			break;
		case BYCA_ERROR_InitCardCommand://智能卡初始化错误
			CAS_BY_ADT_PRINTF("\n智能卡初始化错误\n");
			event = CAS_E_CARD_INIT_FAIL;
			break;
		case BYCA_ERROR_ReadCardCommand://智能卡读卡号命令错误
			CAS_BY_ADT_PRINTF("\n智能卡读卡号命令错误\n");
			event = CAS_E_CARD_DATA_GET;
			break;
		case BYCA_ERROR_NO_CARD://请放入智能卡
			CAS_BY_ADT_PRINTF("\n请放入智能卡\n");
			event = CAS_E_CARD_REGU_INSERT;
			break;
		case BYCA_ERROR_EMM_RET://EMM错误
			CAS_BY_ADT_PRINTF("\nEMM错误 \n");
			event = CAS_E_EMM_ERROR;
			break;
		case BYCA_ERROR_ECM_RET://ECM错误
			CAS_BY_ADT_PRINTF("\nECM错误\n");
			event = CAS_E_ECM_ERROR;
			break;
		case BYCA_ECM_RET_READCOMM_ERROR://智能卡返回ECM数据错误
			CAS_BY_ADT_PRINTF("\n智能卡返回ECM数据错误 \n");
			event = CAS_E_ECM_RETURN_ERROR;
			break;
		case BYCA_EMMECM_RET_P1P2_ERROR://智能卡返回ECM EMM P1P2数据错误
			CAS_BY_ADT_PRINTF("\n智能卡返回ECM EMM P1P2数据错误 \n");
			event = CAS_E_ECMEMM_RETURN_P1P2_ERROR;
			break;
		case BYCA_EMMECM_RET_P3_ERROR://智能卡返回ECM EMM P3数据错误
			CAS_BY_ADT_PRINTF("\n智能卡返回ECM EMM P3数据错误\n");
			event = CAS_E_ECMEMM_RETURN_P3_ERROR;
			break;
		case BYCA_EMMECM_RET_DES_ERROR://智能卡用户无授权
			CAS_BY_ADT_PRINTF("\n智能卡用户无授权id9\n");
			event = CAS_E_PROG_UNAUTH;
			break;
		case BYCA_ECM_RET_NOENTITLE_ERROR://智能卡用户无授权
			CAS_BY_ADT_PRINTF("\n智能卡用户无授权id10\n");
			event = CAS_E_PROG_UNAUTH;
			break;
		case BYCA_EMMECM_RET_STORE_ERROR://智能卡返回错误
			CAS_BY_ADT_PRINTF("\n智能卡返回错误 0x0a\n");
			event = CAS_E_CARD_REG_FAILED;
			break;
		case BYCA_EMMECM_RET_CHECK_ERROR://智能卡用户无授权
			CAS_BY_ADT_PRINTF("\n智能卡用户无授权id12\n");
			event = CAS_E_PROG_UNAUTH;
			break;
		case BYCA_EMM_NOKEY_OR_NOFILE_ERROR://智芸ㄓ没奘谌?
			CAS_BY_ADT_PRINTF("\n智能卡用户奘谌╥d13\n");
			event = CAS_E_PROG_UNAUTH;
			break;
		case BYCA_ECM_OUTDATE_ERROR://智能卡用户无授权
			CAS_BY_ADT_PRINTF("\n智能卡用户无授权id14\n");
			event = CAS_E_PROG_UNAUTH;
			break;
		case BYCA_EMMECM_RET_UNKNOW_ERROR://智能卡ECM EMM返回未知错误
			CAS_BY_ADT_PRINTF("\n智能卡ECM EMM返回未知错误 0x0f\n");
			event = CAS_E_ECMEMM_RETURN_ERROR;
			break;
		case BYCA_CARD_NEED_PAIRING://节目要求机卡配对
			CAS_BY_ADT_PRINTF("\n节目要求机卡配对\n");
			event = CAS_E_CARD_DIS_PARTNER;
			break;
		case BYCA_CARD_PAIRING_OK://节目机卡配对成功
			CAS_BY_ADT_PRINTF("\n节目机卡配对成n");
			event = CAS_S_CARD_STB_PARTNER;
			break;
		case BYCA_NO_ECM_INFO://没有ECM流信息
			CAS_BY_ADT_PRINTF("\n没有ECM流信息\n");
			event = CAS_E_NO_ECM;
			break;
		case BYCA_ZONE_FORBID_ERROR://该节目被区域限播
			CAS_BY_ADT_PRINTF("\n该节目被区域限播\n");
			event = CAS_E_ZONE_CODE_ERR;
			break;
		case BYCA_UserCdn_Limit_Ctrl_Enable://该用户被条件限播
			CAS_BY_ADT_PRINTF("\n该用户被条件限播\n");
			event = CAS_C_USERCDN_LIMIT_CTRL_ENABLE;
			break;
		case BYCA_UserCdn_Limit_Ctrl_Disable://该用户被取消条件薏?
			CAS_BY_ADT_PRINTF("\n该用户被取消条件限播\n");
			event = CAS_C_USERCDN_LIMIT_CTRL_DISABLE;
			break;
		case BYCA_ZoneCdn_Limit_Ctrl_Enable://该区域被条件限播
			CAS_BY_ADT_PRINTF("\n该区域被条件限播\n");
			event = CAS_C_ZONECDN_LIMIT_CTRL_ENABLE;
			break;
		case BYCA_ZoneCdn_Limit_Ctrl_Disable://该区域被取消条件限播
			CAS_BY_ADT_PRINTF("\n该区域被取消条件限播\n");
			event = CAS_C_ZONECDN_LIMIT_CTRL_DISABLE;
			break;
		case BYCA_PAIRED_OtherCARD_ERROR://该机器与其它卡配对
			CAS_BY_ADT_PRINTF("\n该机器与其它卡配对\n");
			event = CAS_E_PAIRED_OTHERCARD_ERROR;
			break;
		case BYCA_PAIRED_OtherMachine_ERROR://该卡氪嘶鞑慌涠?
			CAS_BY_ADT_PRINTF("\n该卡与此机器不配对\n");
			event = CAS_E_PAIRED_OTHERSTB_ERROR;
			break;
		case BYCA_MASTERSLAVE_PAIRED_OK://子母卡配对成功
			CAS_BY_ADT_PRINTF("\n子母卡配对成功\n");
			event = CAS_C_MASTERSLAVE_PAIRED_OK;
			break;
		case BYCA_MASTERSLAVE_PAIRED_TIMEUPDATE://子母卡配对时间更新
			CAS_BY_ADT_PRINTF("\n子母卡配对时间更新\n");
			event = CAS_C_MASTERSLAVE_PAIRED_TIMEUPDATE;
			break;
		case BYCA_MASTERSLAVE_NEEDPAIRED://子母卡需要配对
			CAS_BY_ADT_PRINTF("\n子母卡需要配对\n");
			event = CAS_C_MASTERSLAVE_NEEDPAIRED;
			break;
		case BYCA_CARD_CANCEL_PAIRED_OK://机卡配对取消成功
			CAS_BY_ADT_PRINTF("\n机卡配对取消成功\n");
			event = CAS_C_CANCEL_PAIRED_OK;
			break;
		case STATUE_CA_CARD_ZONE_INVALID://您的智能卡不再服务区
			CAS_BY_ADT_PRINTF("\n您的智能卡不再服务区\n");
			event = CAS_C_CARD_ZONE_INVALID;
			break;
		case STATUE_CA_CARD_ZONE_CHECK_ENTER://正在寻找网络，请等待
			CAS_BY_ADT_PRINTF("\n正在寻找网络，请等待\n");
			event = CAS_C_CARD_ZONE_CHECK_ENTER;
			break;
		case STATUE_CA_CARD_ZONE_CHEKC_OK://漫游功能卡检测正常
			CAS_BY_ADT_PRINTF("\n漫游功能卡检测正常\n");
			event = CAS_C_CARD_ZONE_CHECK_OK;
			break;
		case BYCA_ERROR_CardSystemID_ERROR://智能卡系统id错误
			CAS_BY_ADT_PRINTF("\n智能卡系统id错误\n");
			event = CAS_E_CARD_AUTH_FAILED;
			break;
		case BYCA_ERROR_STBFACTORY_ID_ERROR://该机顶盒未经过认证
			CAS_BY_ADT_PRINTF("\n该机顶盒未经过认证\n");
			event = CAS_E_NO_AUTH_STB;
			break;
		default://未知错误
			CAS_BY_ADT_PRINTF("\n！！！！！未知错误\n");
			event = CAS_E_UNKNOWN_ERROR;
			break;
	}
	set_event(p_priv, event);
	return;
}


BVOID   BYSTBCA_ShowOSDFrameMsg(const BS8* szTitle,const BS8* szOSD)//显示消息框
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;

	memset(&CaFrameMsg,0,sizeof(msg_info_t));
	strncpy(CaFrameMsg.title,szTitle,CAS_MAX_MSG_TITLE_LEN);
	strcpy(CaFrameMsg.data,szOSD);
	CaFrameMsg.type = 1;

	set_event(p_priv, CAS_C_SHOW_MSG);
	CAS_BY_ADT_PRINTF("\r\n[OSDMSG]显示OSD BYSTBCA_ShowOSDFrameMsg   szTitle =%s szOSD=%s\r\n",szTitle,szOSD);
	return;
}

  /*!
  abc
  */
BVOID   BYSTBCA_HidePromptMessage(BVOID)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	u32 event = 0;

	CAS_BY_ADT_PRINTF("\n取消显示\n");
	event = CAS_S_CLEAR_DISPLAY;
	set_event(p_priv, event);
	return;
}

  /*
  功能：以左滚字幕的方式显示OSD消息，可以是同步显示，也可以是异步显示。
  参数:
  szOSD:					用谙允镜腛SD信息。
  wDuration:				0:显示一次后立即返回;其他值为显示的具体持续秒数。
  说明：
  用户可以通过遥控器取消当前OSD显示。
  --*/
BVOID BYSTBCA_ShowOSDMsg(const BS8 *szTitle,const BS8 *szOSD,BU16 wDuration)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	if(1)
	//  if(wDuration == 0)
	{
		OsdMsg.osd_display.roll_mode = 1;
		OsdMsg.osd_display.roll_value = 1;
	}
	else
	{
		OsdMsg.osd_display.roll_mode = 0;
		OsdMsg.osd_display.roll_value = wDuration;
	}
	OsdMsg.type = 0;
	memcpy(OsdMsg.data,szOSD,256);
	set_event(p_priv, CAS_C_SHOW_OSD);
	CAS_BY_ADT_PRINTF("\r\n显示OSD BYSTBCA_ShowOSDMsg   szOSD =%s time=%d\r\n",
	                  OsdMsg.data,wDuration);
	return;
}


  /*--------------------------- 应急广播 -----------------------*/
  /*++
  功能：应急广播, 切换到指定频道。
  参数：
  wOriNetID:				原始网络ID。
  wTSID:					传送流ID。
  wServiceID:				业务ID。
  pUTCTime--5 bytes:				UTC时间。
  --*/
BVOID BYCASTBCA_UrgencyBroad(BU16 wOriNetID,BU16 wTSID,BU16 wServiceID,BU8 *pUTCTime)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;

	force_channel.lock_flag = TRUE;
	force_channel.netwrok_id = wOriNetID;
	force_channel.ts_id= wTSID;
	force_channel.serv_id= wServiceID;
	DEBUG(CAS, INFO, "\n强制切台   wOriNetID =%x wTSID=%x wServiceID=%x \n",wOriNetID,wTSID,wServiceID);
	set_event(p_priv, CAS_C_FORCE_CHANNEL);
	return;
}

  /*++
  功能：取消应急广播，切换回应急广播前用户观看的频道。
  参数：无。
  --*/
BVOID BYCASTBCA_CancelUrgencyBroad(BVOID)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	DEBUG(CAS, INFO, "\n取消强制切台\n");
	set_event(p_priv, CAS_C_UNFORCE_CHANNEL);
	return;
}

  /*
  功能：显示指纹信息。
  参数：
  wEcmPid					ECM PID
  dwCardID:				卡号。
  wDuration:				持续秒数。
  --*/
BVOID BYSTBCA_ShowFingerPrinting(BU16 wEcmPid,BU32 dwCardID,BU32 wDuration)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	if((wEcmPid == 0x1fff) && (dwCardID == 0xffffffff))
	{
	  	set_event(p_priv, CAS_C_HIDE_NEW_FINGER);
	}
	else
	{
		if(wEcmPid != 0x1fff)
		{
		sprintf((char *)(msgFinger.data),"PID:0x%x CARDID:%ld",wEcmPid,dwCardID);
		}
		else
		{
		sprintf((char *)(msgFinger.data),"CARDID:%ld",dwCardID);
		}
		msgFinger.data_len = 100;
		msgFinger.during = 0;
		set_event(p_priv, CAS_C_SHOW_NEW_FINGER);
	}

	CAS_BY_ADT_PRINTF("\n\n\n\n[BY]指纹len =%d BYSTBCA_ShowFingerPrinting ecmpid = 0x%x cardid=%x time =%d\n",
						msgFinger.data_len,wEcmPid,dwCardID,wDuration);
	return;
}

/*++
功能：获得机顶盒分配给CAS的存储空间的起点地址和大小（以字节为单位）。
参数：
	ppStartAddr:			机顶盒分配给CAS的存储空间的开始地址。
	lSize:					输出机顶盒分配给CAS的储存空间的大小。
--*/
BU8 BYSTBCA_GetDataBufferAddr(BU32 * lSize,BU32 * ppStartAddr)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;

	*lSize = p_priv->flash_size;
	*ppStartAddr = p_priv->flash_start_adr;

	CAS_BY_ADT_PRINTF(" BYSTBCA_GetDataBufferAddr %x   %x\n", *ppStartAddr, *lSize);
	return BYCA_OK;
}

/*++
功能：读取保存在ズ兄械男畔ⅰ
参数：
	pStartAddr:				要读取的存储空间的开始地址。
	plDataLen:				输入为要读取的最长数据值；输出为实际读取的大小。
	pData:					存放输出数据。
--*/
BU8 BYSTBCA_ReadDataBuffer(BU32 pStartAddr,BU32 * plDataLen,BU8 * pData)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	
	CAS_BY_ADT_PRINTF(" BYSTBCA_ReadDataBuffer  %x   %x\n",pData, *plDataLen);
	p_priv->nvram_read(pStartAddr, (u8 *)pData, plDataLen);
	return 1;
}

/*++
功能：向机顶盒的存储空间写信息。
参数：
	lStartAddr:				要写的存储空间的开始地址。
	plDataLen:				淙胛吹氖莸某ざ龋皇涑鑫慈氲氖导食ざ取?
	pData:					要写的数据。
--*/
BU8 BYSTBCA_WriteDataBuffer(BU32 pStartAddr,BU32 DataLen,BU8 * pData)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	
	CAS_BY_ADT_PRINTF(" BYSTBCA_WriteDataBuffer %x	 %x\n", pStartAddr, DataLen);
	p_priv->nvram_write(pStartAddr, (u8 *)pData, DataLen);
	return 1;
}

  /*
  功能：通知APP锁定某个频道信号，在该特定的频点寻找漫游信息。
  参数:	KHZ;
  返回:  1，表示频点已锁到，否则未锁定该频点。
  --*/
BU8 BYCA_SetZoneFreq(BU32 Freq)
{
	nim_device_t *p_nim_dev;
	RET_CODE ret = 0;
	unsigned char  match[16] = {0x4a,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char  mask[16] =  {0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	nim_channel_info_t chanl_info = { 0 };
	static u8 cnt = 0;

	if(cnt ==0)
	{
		DEBUG(CAS, INFO, "\nBYCA_SetZoneFreq %d\n",Freq);
		p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
		DEV_IDT_TYPE,
		SYS_DEV_TYPE_NIM);

		chanl_info.frequency = Freq;
		chanl_info.param.dvbc.symbol_rate = 6875;
		chanl_info.param.dvbc.modulation = NIM_MODULA_QAM64;

		DEBUG(CAS, INFO, "\nchanl_info.frequency %d\n",chanl_info.frequency);
		ret = nim_channel_connect(p_nim_dev, &chanl_info, 0);

		if(chanl_info.lock  == 1)
		{
			DEBUG(CAS, INFO, "\nLOCK\n");
			BYSTBCA_SetStreamGuardFilter(0x4a,0x11,
			match,mask,1,1);
			DEBUG(CAS, INFO, "\nset bat ok\n");
		}
		else
		{
			by_ca_bat_flag = 2;
			DEBUG(CAS, INFO, "\nUNLOCK\n");
		}
		cnt = 1;
		return chanl_info.lock;
	}
	return 0;
}

/*!
  abc
  */

u8 bycas_smartcard_status(void)
{
	u32 param;
	scard_device_t *p_smc_dev = NULL;
		p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
		MT_ASSERT(NULL != p_smc_dev);
	CAS_BY_ADT_PRINTF("[BY]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
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
static RET_CODE cas_by_init()
{
	u32 ret = 0;
	static u32 init_flag = 0;

	CAS_BY_ADT_PRINTF("[BY]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
	if(!init_flag)
	{
		CAS_BY_ADT_PRINTF("[BY] cas_by_init\n");

		BY_CAS_Drv_ClientInit();

		BYCASTB_AddDebugMsg(1);
		ret = BYCASTB_Init(1);

		if(ret != BYCA_OK)
		{
			CAS_BY_ADT_PRINTF("[BY] BYCASTB_Init return error! ret=%d",ret);
			return ERR_FAILURE;
		}
		CAS_BY_ADT_PRINTF("[BY] BYCASTB_Init return SUCCESS!\n");
		init_flag = 1;
		mtos_task_sleep(300);
	}
	return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_by_deinit()
{
	CAS_BY_ADT_PRINTF("[BY]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
	return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_by_card_remove()
{
	BU8 pbyStatus;
	CAS_BY_ADT_PRINTF("boyuan cas remove \n");
	BYSTBCA_GetSCStatus(&pbyStatus);

	BY_CAS_DRV_SetSCInsertStatus(SC_REMOVE_START);
	BY_CAS_Drv_FreeAllEmmReq();
	BY_CAS_Drv_FreeAllEcmReq();
	BYCASTB_SCRemove();
	if(BYCAS_SC_IN == pbyStatus)
		BYCASTB_Init(0xff);
	BY_CAS_DRV_SetSCInsertStatus(SC_REMOVE_OVER);

	return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_by_card_reset(u32 slot, card_reset_info_t *p_info)
{
	cas_by_priv_t *p_priv =  (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	BU32 cardid = 0;
	BU8 ret = 0;
	CAS_BY_ADT_PRINTF("\nboyuan cas  card insert \n");

	//p_priv->p_smc_dev = (scard_device_t *)p_info->p_smc;
	p_priv->slot = slot;
	scard_pro_register_op((scard_device_t *)p_info->p_smc);


	BY_CAS_DRV_SetSCInsertStatus(SC_INSERT_START);
	BYCASTB_SCInsert();
	BY_CAS_DRV_SetSCInsertStatus(SC_INSERT_OVER);
	ret = BYCASTB_GetCardID(&cardid);
	CAS_BY_ADT_PRINTF("\nret = %d cardid = %d\n",ret,cardid);
	return SUCCESS;
}

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

/*!
  abc
  */
static RET_CODE cas_by_io_ctrl(u32 cmd, void *param)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;
	cas_table_info_t *p_Cas_table_info = NULL;
	cas_card_info_t *p_card_info = NULL;
	RET_CODE ret = 0;
	u32 i = 0;
	BU32 dwCardID = 0;
	BU32 dwVer = 0;
	BU8 CaSysSp[40] = {0};
	BU8 CaSysCt[40] = {0};
	u8 ver1 = 0;
	u8 ver2 = 0;
	u8 ver3 = 0;
	u8 ver4 = 0;
	u8 date[4] = {0};
	//  BU16 ProductCount = 0;
	static SBYCAServiceInfo tBYServiceInfo;
	static CAMenu_Product CaMenu_product ;
	static MAIL_DATA by20email;
	static MAIL_DATA by30email;
	
	product_entitles_info_t *p_entitles_info = NULL;
	cas_mail_headers_t *p_mail_headers = NULL;
	cas_mail_body_t *p_mailcontent = NULL;
	BU16 email_count = 0;
	BU16 new_email_count = 0;

	u16 year = 0;
	u8 month = 0;
	u8 day = 0;
	msg_info_t *p_msg_info = NULL;
	finger_msg_t *p_finger_info = NULL;
	cas_force_channel_t *p_force_channel_info = NULL;
	unsigned char  match[16] = {0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char  mask[16] =  {0xff,0,0,0,0xff,0,0,0,0,0,0,0,0,0,0,0};
	nim_device_t *p_nim_dev;
	nim_channel_info_t *p_nim_info;

	switch(cmd)
	{
		case CAS_IOCMD_STOP:
			CAS_BY_ADT_PRINTF("\n CAS_IOCMD_STOP \n");
			break;
		case CAS_IOCMD_SET_ECMEMM:
			p_Cas_table_info = (cas_table_info_t *)param;
			memset(&tBYServiceInfo,0,sizeof(SBYCAServiceInfo));
			//if(BY_CAS_DRV_GetSCInsertStatus()==SC_INSERT_OVER)
			{
				CAS_BY_ADT_PRINTF("\nBYCASTB_SetCurEcmInfos(BYCAS_LIST_START,BNULL) ticks=%d~~~~~~~~~~~~~~~~~~~~~\n",mtos_ticks_get());
				BY_CAS_Drv_FreeAllEcmReq();
				BYCASTB_SetCurEcmInfos(BYCAS_LIST_START,BNULL);
				by_ecm_count = 0;
				for(i = 0; i < CAS_MAX_ECMEMM_NUM; i++)
				{
					if(1 == BYCASTB_CASIDVerify(p_Cas_table_info->ecm_info[i].ca_system_id))
					{
						tBYServiceInfo.m_wEcmPid = p_Cas_table_info->ecm_info[i].ecm_id;
						tBYServiceInfo.m_byServiceCount = 1;
						tBYServiceInfo.m_wServiceID[0] = p_Cas_table_info->service_id;
						BYCASTB_SetCurEcmInfos(BYCAS_LIST_ADD, &tBYServiceInfo);
						by_ecm_count ++;
						CAS_BY_ADT_PRINTF("\necm succ BYCAS_LIST_ADD 0x%x serviceid=0x%x\n",
						p_Cas_table_info->ecm_info[i].ecm_id,p_Cas_table_info->service_id);
					}
					else
					{
						BYCASTB_SetCurEcmInfos(BYCAS_LIST_END,BNULL);
						CAS_BY_ADT_PRINTF("\nBYCASTB_SetCurEcmInfos(BYCAS_LIST_END,BNULL)\n");
					}
				}
			}
			break;
		case CAS_IOCMD_VIDEO_PID_SET:
			p_priv->v_pid = *((u16 *)param);
			CAS_BY_ADT_PRINTF("\nSET v_pid [0x%x]\n",  p_priv->v_pid);
			break;
		case CAS_IOCMD_AUDIO_PID_SET:
			p_priv->a_pid = *((u16 *)param);
			CAS_BY_ADT_PRINTF("\nSET a_pid [0x%x]\n",  p_priv->a_pid);
			break;
	    case CAS_IOCMD_CARD_INFO_GET:
			p_card_info = (cas_card_info_t *)param;
			if(BYCASTB_GetCardID(&dwCardID) != true)
			{
				dwCardID = 0;
				CAS_BY_ADT_PRINTF("\n[BY] get id error\n");
			}
			else
			{
				CAS_BY_ADT_PRINTF("\n[BY] dwCardID = %d\n",  dwCardID);
			}

			sprintf((char *)(p_card_info->sn),"%ld",(u32)dwCardID);
			CAS_BY_ADT_PRINTF("\n[BY] CardID = %s\n",  p_card_info->sn);
			if(BYCASTB_GetVer(&dwVer,CaSysSp,CaSysCt) != true)
			{
				dwVer = 0;
				CaSysSp[0] = 0;
				CaSysCt[0] = 0;
				CAS_BY_ADT_PRINTF("\n[BY] get ca_ver error\n");
				break;
			}
			else
			{
				CAS_BY_ADT_PRINTF("\n[BY] 0x%08x   sp=%s  ct=%s\n",dwVer,CaSysSp,CaSysCt);
			}
			ver1 = (u8)((dwVer&0xff000000) >> 24);
			ver2 = (u8)((dwVer&0x00ff0000) >> 16);
			ver3 = (u8)((dwVer&0x0000ff00) >> 8);
			ver4 = (u8)(dwVer&0x000000ff);
			sprintf((char *)(p_card_info->cas_ver),"%d.%d.%d.%d", ver1, ver2, ver3, ver4);
			CAS_BY_ADT_PRINTF("\n[BY] Cas Ver = %s\n",  p_card_info->cas_ver);

			snprintf((char *)(p_card_info->cas_manu_name),CAS_MAXLEN_MANUFACTURERNAME,"%s",CaSysSp);
			snprintf((char *)(p_card_info->desc),CAS_CARD_DESC_MAX_LEN,"%s",CaSysCt);

			break;
		case CAS_IOCMD_ENTITLE_INFO_GET:
			if(!bycas_smartcard_status())
			{
				CAS_BY_ADT_PRINTF("[UI] smartcard out \n");
				break;
			}	
			
			p_entitles_info = (product_entitles_info_t *)param;
			memset(&CaMenu_product,0,sizeof(CAMenu_Product));
			if(BYCASTB_GetCaMenuProduct(&CaMenu_product) == BYCA_OK)
			{
				p_entitles_info->max_num = CaMenu_product.CaMenu_number;

				for(i = 0;i < CaMenu_product.CaMenu_number; i++)
				{
					p_entitles_info->p_entitle_info[i].product_id
						= (u32)CaMenu_product.Camenu_Unit[i].Product_ID;

					CAS_BY_ADT_PRINTF("\n[BY] name:%s\n", CaMenu_product.CaMenu_Name[i].ProductName);				
					snprintf(p_entitles_info->p_entitle_info[i].product_name,CAS_PRODUCT_NAME_MAX_LEN,"%s",CaMenu_product.CaMenu_Name[i].ProductName);
	  	
					__by_MjdToDate(CaMenu_product.Camenu_Unit[i].Purchase_start_time,date);
					year = 2000 + date[0];
					month = date[1];
					day = date[2];
					CAS_BY_ADT_PRINTF("\n[BY]  year=%d,month=%d,day=%d", year, month, day);
					p_entitles_info->p_entitle_info[i].start_time[0] = (u8)(year / 100);
					p_entitles_info->p_entitle_info[i].start_time[1] = (u8)(year % 100);
					p_entitles_info->p_entitle_info[i].start_time[2] = month;
					p_entitles_info->p_entitle_info[i].start_time[3] = day;

					__by_MjdToDate(CaMenu_product.Camenu_Unit[i].Purchase_end_time,date);
					year = 2000 + date[0];
					month = date[1];
					day = date[2];
					CAS_BY_ADT_PRINTF("\n[BY]  year=%d,month=%d,day=%d", year, month, day);
					p_entitles_info->p_entitle_info[i].expired_time[0] = (u8)(year / 100);
					p_entitles_info->p_entitle_info[i].expired_time[1] = (u8)(year % 100);
					p_entitles_info->p_entitle_info[i].expired_time[2] = month;
					p_entitles_info->p_entitle_info[i].expired_time[3] = day;
				}
			}
			else
			{
				CAS_BY_ADT_PRINTF("\n[BY]  BYCASTB_GetCaMenuProduct error\n");
				return ERR_FAILURE;
			}
			break;
		case CAS_IOCMD_MAIL_HEADER_GET:
			BYCASTB_GetEmailCount(&email_count,&new_email_count);

			p_mail_headers = (cas_mail_headers_t *)param;
			p_mail_headers->total_email_num =  email_count;
			p_mail_headers->new_mail_num =  new_email_count;
			CAS_BY_ADT_PRINTF("\n [BY]  -------total--%d----new---%d---------\n",
				email_count,new_email_count);
			if(p_priv->version == CAS_LIB_BY20)//by2.0ver
			{
				memset(&by20email,0,sizeof(MAIL_DATA));
				p_mail_headers->max_num = email_count;
				CAS_BY_ADT_PRINTF("\n [BY] ------ver2.0---------\n");
				for(i = 0; i < email_count; i++)
				{
					CAS_BY_ADT_PRINTF("\n[BY]------mail-%d------\n",i + 1);
					BYCASTB_GetEmailContent(i,&by20email);

					p_mail_headers->p_mail_head[i].m_id = by20email.MessageId;
					if(by20email.MessageFlag == 3)
					{
						CAS_BY_ADT_PRINTF("\n[BY]------old------\n");
						p_mail_headers->p_mail_head[i].new_email = 0;
					}
					else
					{
						CAS_BY_ADT_PRINTF("\n[BY]------new------\n");
						p_mail_headers->p_mail_head[i].new_email = 1;
					}

					memcpy(p_mail_headers->p_mail_head[i].subject,by20email.Title,80);
					CAS_BY_ADT_PRINTF("\n[BY]------titls= %s -----0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
						p_mail_headers->p_mail_head[i].subject,
						by20email.SendTime[0],
						by20email.SendTime[1],
						by20email.SendTime[2],
						by20email.SendTime[3],
						by20email.SendTime[4]);
					__by_MjdToDate(by20email.SendTime,date);
					year = 2000 + date[0];
					month = date[1];
					day = date[2];
					p_mail_headers->p_mail_head[i].creat_date[0] = (u8)(year / 100);
					p_mail_headers->p_mail_head[i].creat_date[1] = (u8)(year % 100);
					p_mail_headers->p_mail_head[i].creat_date[2] = month;
					p_mail_headers->p_mail_head[i].creat_date[3] = day;
					p_mail_headers->p_mail_head[i].creat_date[4] = by20email.SendTime[2];
					p_mail_headers->p_mail_head[i].creat_date[5] = by20email.SendTime[3];
					p_mail_headers->p_mail_head[i].creat_date[6] = by20email.SendTime[4];
				}
			}
			else if(p_priv->version == CAS_LIB_BY30)//by3.0ver
			{
			    memset(&by30email,0,sizeof(MAIL_DATA));
				p_mail_headers->max_num = email_count;
				CAS_BY_ADT_PRINTF("\n[BY]------ver3.0---------\n");
				for(i = 0; i < email_count; i++)
				{
					CAS_BY_ADT_PRINTF("\n[BY]------mail-%d------\n",i + 1);
					BYCASTB_GetEmailContent(i,&by30email);

					p_mail_headers->p_mail_head[i].m_id = by30email.MessageId;
					CAS_BY_ADT_PRINTF("\n[BY] by30email.MessageId %d\n",by30email.MessageId);
					if(by30email.MessageFlag == 3)
					{
						CAS_BY_ADT_PRINTF("\n[BY]------old------\n");
						p_mail_headers->p_mail_head[i].new_email = 0;
					}
					else
					{
						CAS_BY_ADT_PRINTF("\n[BY]------new------\n");
						p_mail_headers->p_mail_head[i].new_email = 1;
					}


					memcpy(p_mail_headers->p_mail_head[i].subject,by30email.Title,80);
					CAS_BY_ADT_PRINTF("\n[BY]------titls= %s -----0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
						p_mail_headers->p_mail_head[i].subject,
						by30email.SendTime[0],
						by30email.SendTime[1],
						by30email.SendTime[2],
						by30email.SendTime[3],
						by30email.SendTime[4]);
					__by_MjdToDate(by30email.SendTime,date);
					year = 2000 + date[0];
					month = date[1];
					day = date[2];
					p_mail_headers->p_mail_head[i].creat_date[0] = (u8)(year / 100);
					p_mail_headers->p_mail_head[i].creat_date[1] = (u8)(year % 100);
					p_mail_headers->p_mail_head[i].creat_date[2] = month;
					p_mail_headers->p_mail_head[i].creat_date[3] = day;
					p_mail_headers->p_mail_head[i].creat_date[4] = by30email.SendTime[2];
					p_mail_headers->p_mail_head[i].creat_date[5] = by30email.SendTime[3];
					p_mail_headers->p_mail_head[i].creat_date[6] = by30email.SendTime[4];
				}
			}
			else
			{
				CAS_BY_ADT_PRINTF("\n[BY]------bycas ver error------\n");
				return ERR_FAILURE;
			}
			break;
		case CAS_IOCMD_MAIL_BODY_GET:
			p_mailcontent = (cas_mail_body_t *)param;
			BYCASTB_EmailRead(p_mailcontent->mail_id);
			if(p_priv->version == CAS_LIB_BY20)//by2.0ver
			{
				u32 mail_index = *(u32 *)param;
				memset(&by20email,0,sizeof(MAIL_DATA));
				if(BYCASTB_GetEmailContent(mail_index,&by20email) == BYCA_OK)
				{
					CAS_BY_ADT_PRINTF("\n~~~~~~~by20~~~~~~mail_id = %d~~~~~~~~~~~~\n",p_mailcontent->mail_id);
					CAS_BY_ADT_PRINTF("\nemail.ContentLen=%d\n",by20email.ContentLen);
					CAS_BY_ADT_PRINTF("\nemail.Content=%s\n",by20email.Content);
					memcpy(p_mailcontent->data,by20email.Content,by20email.ContentLen);
					p_mailcontent->data_len = by20email.ContentLen;
				}
				else
				{
					CAS_BY_ADT_PRINTF("[BY]get email content error\n");
				}
			}
			else if(p_priv->version == CAS_LIB_BY30)//by3.0ver
			{
				u32 mail_index = *(u32 *)param;
				memset(&by30email,0,sizeof(MAIL_DATA));
				if(BYCASTB_GetEmailContent(mail_index,&by30email) == BYCA_OK)
				{
					CAS_BY_ADT_PRINTF("\n~~~~~by30~~~~~mail_id = %d~~~~~~~~~~~~\n",p_mailcontent->mail_id);
					CAS_BY_ADT_PRINTF("\nemail.ContentLen=%d\n",by30email.ContentLen);
					CAS_BY_ADT_PRINTF("\nemail.Content=%s\n",by30email.Content);
					memcpy(p_mailcontent->data,by30email.Content,by30email.ContentLen);
					p_mailcontent->data_len = by30email.ContentLen;
					ret = SUCCESS;
				}
				else
				{
					CAS_BY_ADT_PRINTF("[BY]get email content error mail_index=%d \n",mail_index);
					return ERR_FAILURE;
				}
			}
			else
			{
				CAS_BY_ADT_PRINTF("\n------bycas ver error------\n");
				return ERR_FAILURE;
			}
			break;
		case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
		{
			u32 mail_index = *(u32 *)param;
			CAS_BY_ADT_PRINTF("\r\n[BY] DEL=%d\n",(u16)mail_index);
			if(BYCASTB_DelEmail(mail_index + 1) != BYCA_OK)
			{
				CAS_BY_ADT_PRINTF("[BY] Del email fail mail_index=%d\n",mail_index);
			}
		}
		break;
		case CAS_IOCMD_FACTORY_SET:
			CAS_BY_ADT_PRINTF("\r\nCAS_IOCMD_FACTORY_SET\n");
			BY_CAS_Drv_FreeAllEmmReq();
			CAS_BY_ADT_PRINTF("\r\nCAS_IOCMD_FACTORY_SET emm\n");
			BY_CAS_Drv_FreeAllEcmReq();
			CAS_BY_ADT_PRINTF("\r\nCAS_IOCMD_FACTORY_SET ecm\n");
			if(BY_CAS_DRV_GetSCInsertStatus()==SC_INSERT_OVER)
			BYCASTB_Init(0xff);
			CAS_BY_ADT_PRINTF("\r\nCAS_IOCMD_FACTORY_SET over\n");
			break;
		case CAS_IOCMD_MAIL_DELETE_ALL:
			CAS_BY_ADT_PRINTF("\r\nDEL ALL\n");
			BYCASTB_DelEmail(0);
			break;
		case CAS_IOCMD_OSD_GET:
			p_msg_info = (msg_info_t *)param;
			memcpy(p_msg_info, &OsdMsg, sizeof(OsdMsg));
			CAS_BY_ADT_PRINTF("\nGET=%s\n", OsdMsg.data);
			CAS_BY_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_OSD_GET\n");
			break;
		case CAS_IOCMD_MSG_GET:
			p_msg_info = (msg_info_t *)param;
			memcpy(p_msg_info, &CaFrameMsg, sizeof(CaFrameMsg));
			CAS_BY_ADT_PRINTF("\n[OSDMSG]GET=%s\n", CaFrameMsg.title);
			CAS_BY_ADT_PRINTF("\n[OSDMSG]GET=%s\n", CaFrameMsg.data);
			CAS_BY_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_IPP_NOTIFY_INFO_GET\n");
			break;
		case CAS_IOCMD_FINGER_MSG_GET:
			p_finger_info = (finger_msg_t *)param;
			memcpy(p_finger_info, &msgFinger, sizeof(msgFinger));
			DEBUG(CAS, INFO, "\n[BY]finger GET=%s\n", msgFinger.data);
			DEBUG(CAS, INFO, "[BY] CAS_RCV_IOCMD---> CAS_IOCMD_FINGER_GET\n");
			break;
		case CAS_IOCMD_FORCE_CHANNEL_INFO_GET:
			p_force_channel_info = (cas_force_channel_t *)param;
			p_force_channel_info->lock_flag = force_channel.lock_flag;
			p_force_channel_info->netwrok_id = force_channel.netwrok_id;
			p_force_channel_info->serv_id= force_channel.serv_id;
			p_force_channel_info->ts_id= force_channel.ts_id;
			break;
		case CAS_IOCMD_ZONE_CHECK:
			CAS_BY_ADT_PRINTF("\n BY_CAS_DRV_GetSCInsertStatus %d\n",
			BY_CAS_DRV_GetSCInsertStatus());
			if(bycas_smartcard_status())
			{
				CAS_BY_ADT_PRINTF("\n card in\n");
				i = 0;
				while(1)
				{
					i ++;
					if(i > 200)
					{
						BYSTBCA_ZoneCheckEnd();
						CAS_BY_ADT_PRINTF("\n card error\n");
						return SUCCESS;
					}
					if(BY_CAS_DRV_GetSCInsertStatus() == SC_INSERT_OVER)
					{
						if(BYCASTB_GetCardID(&dwCardID) == true)//makesure the card is bycard
						break;
					}
					mtos_task_sleep(10);
				}
				p_nim_info = (nim_channel_info_t *)param;
				CAS_BY_ADT_PRINTF("\nset tuner_lock_delivery %d %d %d\n",p_nim_info->frequency,
				p_nim_info->param.dvbc.symbol_rate,p_nim_info->param.dvbc.modulation);
				p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
				                                                DEV_IDT_TYPE,
				                                                SYS_DEV_TYPE_NIM);

				nim_channel_connect(p_nim_dev, p_nim_info, 0);
				if(p_nim_info->lock == 1)
				{
					CAS_BY_ADT_PRINTF("\n lock \n");
				}
				else
				{
					CAS_BY_ADT_PRINTF("\n unlock \n");
					BYSTBCA_ZoneCheckEnd();
					return SUCCESS;
				}
				by_ca_nit_flag = 0;
				by_ca_nit_state = 0;
				by_ca_bat_flag = 0;

          //开nit接收
				BYSTBCA_SetStreamGuardFilter(0x40,0x10,
				match,mask,5,0);
				i = 0;
				while((by_ca_nit_state == 0) && (i < 200))
				{
					i++;
					mtos_task_sleep(10);
				}

				if(by_ca_nit_flag == 1)//get nit
				{
					CAS_BY_ADT_PRINTF("\nGET nit\n");
					BY_CAS_Drv_FreeNitReq();
				}
				else if(by_ca_nit_flag == 2)//nit no data
				{
					CAS_BY_ADT_PRINTF("\nGET nit no data\n");
					BY_CAS_Drv_FreeNitReq();
					BYSTBCA_ZoneCheckEnd();
					return SUCCESS;
				}
				else
				{
					CAS_BY_ADT_PRINTF("\nno nit~~~~~~~~~\n");
					BY_CAS_Drv_FreeNitReq();
					BYSTBCA_ZoneCheckEnd();
					return SUCCESS;
				}

				i = 0;
				while(by_ca_bat_flag == 0 && i < 400)
				{
					i++;
					mtos_task_sleep(10);
				}
				if(by_ca_bat_flag == 1)//get nit
				{
					CAS_BY_ADT_PRINTF("\nGET bat\n");
					BY_CAS_Drv_FreeBatReq();
				}
				else if(by_ca_bat_flag == 2)
				{
					CAS_BY_ADT_PRINTF("\nbat fre unlock\n");
				}
				else
				{
					CAS_BY_ADT_PRINTF("\nno bat\n");
					BY_CAS_Drv_FreeBatReq();
				}

			}
			BYSTBCA_ZoneCheckEnd();

			break;
		default:
			break;
	}
	return ret;
}

/*!
  abc
  */
u16 cas_by_get_v_pid(void)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;

	return p_priv->v_pid;
}
/*!
  abc
  */
u16 cas_by_get_a_pid(void)
{
	cas_by_priv_t *p_priv = (cas_by_priv_t *)by_cas_priv->cam_op[CAS_ID_BY].p_priv;

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
				if(BYCA_OK == BYCASTB_CASIDVerify(ca_system_id))
				{
					OS_PRINTF("\nBYCAS EMM emm pid =0x%x,Desc.CA_system_id =0x%x\n",
					emm_pid,ca_system_id);
					BY_CAS_Drv_FreeAllEmmReq();
					BYCASTB_SetEmmPids(1, &emm_pid);
				}
			break;
				default:
			break;
		}
		length -= descriptor_length;
		p_data = p_data + descriptor_length;
	}
}

/*!
  abc
  */
static void by_cas_parse_cat(u8 *p_buf)
{
	u8 *p_data = p_buf;
	u8 version_number = 0;
	u16 length = 0;

	version_number = (u8)((p_data[5] & 0x3E) >> 1);
	length = (u16)(((p_data[1] & 0x0f) << 8) | p_data[2]);


	CAS_BY_ADT_PRINTF("[CAT] length=%d,version_number=%d\n",
	        length,version_number);
	p_data += 8;

	cat_parse_descriptor(p_data, length - 9);

	CAS_BY_ADT_PRINTF("\r\nparsing cat over.");
	return;
}
/*!
  abc
  */
s32 by_cas_table_prase(u32 t_id, u8 *p_buf, u32 *p_result)
{
	u16 section_len = MAKE_WORD(p_buf[2], p_buf[1] & 0x0f) + 3;

	CAS_BY_ADT_PRINTF("\n  cas_by  t_id = %d  table_id = %d section_len = %d ",
	t_id, p_buf[0], section_len);

	if(t_id == CAS_TID_PMT)
	{
		CAS_BY_ADT_PRINTF("\nNNNNNNNNNget pmt#######\n");
		/*
		memcpy(by_cas_priv.pmt_data, p_buf, section_len);
		by_cas_priv.pmt_length = section_len;
		tf_cas_parse_pmt(by_cas_priv.pmt_data);
		*/
	}
	else if(t_id == CAS_TID_CAT)
	{
		CAS_BY_ADT_PRINTF("\nNNNNNNNNNget cat#######\n");
		memcpy(by_cas_priv->cat_data, p_buf, section_len);
		by_cas_priv->cat_length = section_len;
		by_cas_parse_cat(by_cas_priv->cat_data);
	}
	else if(t_id == CAS_TID_NIT)
	{
	}

	return 0;
}

/*!
  abc
  */
RET_CODE by_cas_table_resent(void)
{
	if(by_cas_priv->pmt_length != 0)
	{
		//tf_cas_parse_pmt(by_cas_priv.pmt_data);
	}
	if(by_cas_priv->cat_length != 0)
	{
		//tf_cas_parse_cat(by_cas_priv.cat_data);
	}
	return SUCCESS;
}
/*!
  abc
  */
RET_CODE cas_by_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
	cas_by_priv_t *p_priv = NULL;
	by_cas_priv = cas_get_private_data();  

	OS_PRINTF("cas_by_attach by_cas_priv :%#x\n",by_cas_priv);
	by_cas_priv->cam_op[CAS_ID_BY].attached = 1;
	by_cas_priv->cam_op[CAS_ID_BY].inited  = 0;

	by_cas_priv->cam_op[CAS_ID_BY].func.init
		= cas_by_init;
	by_cas_priv->cam_op[CAS_ID_BY].func.deinit
		= cas_by_deinit;
	by_cas_priv->cam_op[CAS_ID_BY].func.identify
		= NULL;
	by_cas_priv->cam_op[CAS_ID_BY].func.io_ctrl
		= cas_by_io_ctrl;
	by_cas_priv->cam_op[CAS_ID_BY].func.table_process
		= by_cas_table_prase;
	by_cas_priv->cam_op[CAS_ID_BY].func.table_resent
		= by_cas_table_resent;

	by_cas_priv->cam_op[CAS_ID_BY].func.card_remove = cas_by_card_remove;
	by_cas_priv->cam_op[CAS_ID_BY].func.card_reset = cas_by_card_reset;

	p_priv = mtos_malloc(sizeof(cas_by_priv_t));
	by_cas_priv->cam_op[CAS_ID_BY].p_priv = (cas_by_priv_t *)p_priv;
	memset(p_priv, 0x00, sizeof(cas_by_priv_t));

	p_priv->version = CAS_LIB_BY30;
	//p_priv->version = p_cfg->cas_lib_type;
	p_priv->flash_size = p_cfg->flash_size;
	p_priv->flash_start_adr = p_cfg->flash_start_adr;
	CAS_BY_ADT_PRINTF("\nbycas:flash  %x %x\n", p_priv->flash_size, p_priv->flash_start_adr);
	p_priv->nvram_read = p_cfg->nvram_read;
	p_priv->nvram_write = p_cfg->nvram_write;

	*p_cam_id = (u32)(&by_cas_priv->cam_op[CAS_ID_BY]);

	return SUCCESS;
}

