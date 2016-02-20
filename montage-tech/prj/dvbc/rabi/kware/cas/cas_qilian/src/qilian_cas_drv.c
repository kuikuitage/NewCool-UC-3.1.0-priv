/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"

#include "string.h"

#include "lib_bitops.h"
#include "mtos_event.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "charsto.h"
#include "drv_svc.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "cas_adapter.h"
#include "dmx.h"
#include "stb_public.h"
#include "stb_app2ca.h"
#include "stb_ca2app.h"
#include "ql_cas_include.h"
#include "nim.h"

#include "sys_cfg.h"

#define NIT_TABLE_ID  0x40
#define BAT_TABLE_ID  0x4a

static CA_MSG g_QL_Msg[QL_MAX_MSG_QUEUE][MSG_CNT];
static u8 g_msg_pos_add[QL_MAX_MSG_QUEUE];
static u8 g_msg_pos_get[QL_MAX_MSG_QUEUE];
static CA_MSG msg;
msg_info_t OsdMsg = {0, {0,},};
finger_msg_t msgFinger = {{0,},};
static u8 s_tFilterBuf[QL_FILTER_MAX_NUM][QL_FILTER_BUFFER_SIZE];
static ql_filter_struct_t s_tFilter[QL_FILTER_MAX_NUM] = {{0,},};
static ql_data_got_t  s_tData[QL_FILTER_MAX_NUM] = {{0,},};
static u32 u32_CaReqLock = 0;
static u8 s_uScStatus = SC_STATUS_END;
u8 ql_ecm_count = 0;
static u16 v_channel = 0xffff;
static u16 a_channel = 0xffff;
static u16 old_v_channel = 0xffff;
static u16 old_a_channel = 0xffff;
extern u16 cas_ql_get_v_pid(void);
extern u16 cas_ql_get_a_pid(void);
extern u8 ql_ca_nit_flag;
extern u8 ql_ca_bat_flag;
extern u8 ql_ca_nit_state;
extern cas_adapter_priv_t g_cas_priv;
/*!
  abc
  */
void _Ql_CaReqLock(void)
{
  mtos_sem_take((os_sem_t *)&u32_CaReqLock, 0);
}

/*!
  abc
  */
void _Ql_CaReqUnLock(void)
{
  mtos_sem_give((os_sem_t *)&u32_CaReqLock);
}

/*!
  abc
  */
 static void Ql_Stb_FreeFilter(ql_filter_struct_t *s_tFilter)
  {
    dmx_device_t *p_dev = NULL;
    u32 ret = 0;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
            , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_chan_stop(p_dev, s_tFilter->m_dwReqHandle);
    MT_ASSERT(ret == SUCCESS);

    ret = dmx_chan_close(p_dev, s_tFilter->m_dwReqHandle);
    MT_ASSERT(ret == SUCCESS);

    return;
  }

 /*!
  abc
  */
  void Ql_CAS_Drv_ShowAllCaReq(void)
  {
  u8 uIndex = 0;

  _Ql_CaReqLock();

  for(uIndex = 0; uIndex < QL_FILTER_MAX_NUM; uIndex ++)
  {
    if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus)
    {
       OS_PRINTF("\nReqID = %d\tPid = %d\n",
        s_tFilter[uIndex].m_byReqId, s_tFilter[uIndex].m_PID);
       OS_PRINTF("Filter: %02x %02x %02x %02x %02x %02x %02x %02x \n "
        , s_tFilter[uIndex].m_uFilterData[0], s_tFilter[uIndex].m_uFilterData[1]
        , s_tFilter[uIndex].m_uFilterData[2], s_tFilter[uIndex].m_uFilterData[3]
        , s_tFilter[uIndex].m_uFilterData[4], s_tFilter[uIndex].m_uFilterData[5]
        , s_tFilter[uIndex].m_uFilterData[6], s_tFilter[uIndex].m_uFilterData[7]);
        OS_PRINTF("Filter: %02x %02x %02x %02x %02x %02x %02x %02x \n"
        , s_tFilter[uIndex].m_uFilterMask[0], s_tFilter[uIndex].m_uFilterMask[1]
        , s_tFilter[uIndex].m_uFilterMask[2], s_tFilter[uIndex].m_uFilterMask[3]
        , s_tFilter[uIndex].m_uFilterMask[4], s_tFilter[uIndex].m_uFilterMask[5]
        , s_tFilter[uIndex].m_uFilterMask[6], s_tFilter[uIndex].m_uFilterMask[7]);
    }
  }

  _Ql_CaReqUnLock();

  return;
  }

 /*!
  abc
  */
  void Ql_CAS_Drv_FreeAllEcmReq(void)
  {
    u8 uIndex = 0;

    _Ql_CaReqLock();

    for(uIndex = 0; uIndex < QL_FILTER_MAX_NUM; uIndex ++)
    {
      if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
      (ECM_TABLE_ID == s_tFilter[uIndex].m_byReqId || ECM_TABLE_ID1 == s_tFilter[uIndex].m_byReqId))
      {
        Ql_Stb_FreeFilter(&s_tFilter[uIndex]);
        STBCA_StreamGuardDataGot(s_tFilter[uIndex].m_byReqId,
          false, s_tFilter[uIndex].m_PID, NULL, 0);
        memset((void *)&s_tFilter[uIndex], 0, sizeof(ql_filter_struct_t));
      }
    }

    _Ql_CaReqUnLock();

    return;
  }
  /*!
  abc
  */
  void Ql_CAS_Drv_FreeAllEmmReq(void)
  {
    u8 uIndex = 0;

    _Ql_CaReqLock();

    for(uIndex = 0; uIndex < QL_FILTER_MAX_NUM; uIndex ++)
    {
      if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
      EMM_TABLE_ID == s_tFilter[uIndex].m_byReqId)
      {
        Ql_Stb_FreeFilter(&s_tFilter[uIndex]);
        STBCA_StreamGuardDataGot(s_tFilter[uIndex].m_byReqId,
          false, s_tFilter[uIndex].m_PID, NULL, 0);
        memset((void *)&s_tFilter[uIndex], 0, sizeof(ql_filter_struct_t));
      }
    }

    _Ql_CaReqUnLock();

    return;
  }

/*!
  abc
  */
  void Ql_CAS_Drv_FreeNitReq(void)
  {
    u8 uIndex = 0;

    _Ql_CaReqLock();

    for(uIndex = 0; uIndex < QL_FILTER_MAX_NUM; uIndex ++)
    {
      if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
      NIT_TABLE_ID == s_tFilter[uIndex].m_byReqId)
      {
        Ql_Stb_FreeFilter(&s_tFilter[uIndex]);
        memset((void *)&s_tFilter[uIndex], 0, sizeof(ql_filter_struct_t));
      }
    }

    _Ql_CaReqUnLock();

    return;
  }

/*!
  abc
  */
  void Ql_CAS_Drv_FreeBatReq(void)
  {
    u8 uIndex = 0;

    _Ql_CaReqLock();

    for(uIndex = 0; uIndex < QL_FILTER_MAX_NUM; uIndex ++)
    {
      if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
      BAT_TABLE_ID == s_tFilter[uIndex].m_byReqId)
      {
        Ql_Stb_FreeFilter(&s_tFilter[uIndex]);
        memset((void *)&s_tFilter[uIndex], 0, sizeof(ql_filter_struct_t));
      }
    }

    _Ql_CaReqUnLock();

    return;
  }


/*!
  abc
  */
  static u32 smc_run_cmd(u8 *wr_buf, u16 bytes_out, u8 *rd_buf, u16 bytes_in, u16 *re_len)
  {
  scard_opt_desc_t rwopt = {0};
  RET_CODE ret = SUCCESS;

  rwopt.p_buf_out = wr_buf;
  rwopt.bytes_out = bytes_out;
  rwopt.p_buf_in = rd_buf;
  rwopt.bytes_in_target = (u32)bytes_in;
  rwopt.bytes_in_actual = 0;

  ret = scard_pro_rw_transaction(&rwopt);

  *re_len = (u16)(rwopt.bytes_in_actual);

  if (SUCCESS != ret)
  {
    OS_PRINTF("\r\nrwopt.bytes_in_actual=%d", rwopt.bytes_in_actual);
    OS_PRINTF("\r\n Transmission fail [%d]!",ret);
    return 1;
  }
  else
  {
    OS_PRINTF("\n Transmission successful!");
  }
  return 0;
  }

/*!
  abc
  */
  static u8 drv_smc_reset(u8 *pbyATR, u8 *pbyLen)
  {
  u32 err = 0;
  u8 uResetTimes = 0;
  scard_atr_desc_t s_atr = {0};
  u8 buf_atr[256] = {0,};
  scard_device_t *p_smc_dev = NULL;
  s_atr.p_buf = buf_atr;

  if(NULL == pbyATR || NULL == pbyLen)
  {
    OS_PRINTF("\nSCReset error parameter!\n");
    return false;
  }

  p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
  MT_ASSERT(NULL != p_smc_dev);

  do
  {
    err = scard_active(p_smc_dev, &s_atr);
    uResetTimes++;
  }while(err != 0 && uResetTimes < 3);
  if(0 != err)
  {
    OS_PRINTF("%s : %s : %d : error_code = 0x%08x \n", __FILE__, __FUNCTION__, __LINE__, err);
    return false;
  }
  else
  {
    memcpy(pbyATR, s_atr.p_buf, s_atr.atr_len);
    *pbyLen = (u8)s_atr.atr_len;
    return true;
  }

  return false;
  }



/*!
  abc
  */
static void set_event(cas_ql_priv_t *p_priv, u32 event)
{
  CAS_QILIAN_PRINTF("qlcas: set_event  event  =%d  \n", event);
  if(event == 0)
  {
    return;
  }

 cas_send_event(p_priv->slot, CAS_ID_QL, event, 0);

}

  /*!
  abc
  */
  static void _Ql_AddData(u8 m_byReqId, u8 m_uOK, u16 m_PID, u8 *m_pData, u16 m_uLength, u8 uIndex)
  {
  if(uIndex >= QL_FILTER_MAX_NUM)
  {
    CAS_QILIAN_PRINTF("\n[ERROR]_Ql_AddData  uIndex >= QL_FILTER_MAX_NUM \n");
    return;
  }

  s_tData[uIndex].m_byReqId = m_byReqId;
  s_tData[uIndex].m_uOK = m_uOK;
  s_tData[uIndex].m_PID = m_PID;
  if(NULL == m_pData || 0 == m_uLength)
  {
    s_tData[uIndex].p_Data = NULL;
  }
  else
  {
    s_tData[uIndex].p_Data = mtos_malloc(m_uLength);
    memcpy(s_tData[uIndex].p_Data, m_pData, m_uLength);
  }
    s_tData[uIndex].m_uLength = m_uLength;
  }

  /*!
  abc
  */

static void ca_nit_parse_descriptor(u8 *buf,
                           s16 length)
{
  u8 *p_data = buf;
  u8  descriptor_tag = 0;
  u8  descriptor_length = 0;

  while(length > 0)
  {
    descriptor_tag = p_data[0];
    descriptor_length = p_data[1];

    switch(p_data[0])
    {
      case 0x9C://
        STBCA_CurrentNitDes(p_data);
        ql_ca_nit_flag = 1;
        OS_PRINTF("\nql_ca_nit_flag =%d\n",ql_ca_nit_flag);

        break;
      default:
        break;
    }
    length -= (descriptor_length + 2) ;
    p_data = (p_data + descriptor_length + 2);
  }
}

/*!
  abc
  */
static void ql_cas_parse_nit(u8 *p_buf)
{
  u8 *p_data = p_buf;
  u8 version_number = 0;
  u16 length = 0;
  unsigned char  match[16] = {0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned char  mask[16] =  {0xff,0,0,0,0xff,0,0,0,0,0,0,0,0,0,0,0};
  u8 nit_section_num = 0;
  u8 nit_last_section_num = 0;

  version_number = (u8)((p_data[5] & 0x3E) >> 1);
  length = (u16)(((p_data[8] & 0x0f) << 8) | p_data[9]);
  nit_section_num = p_buf[6];
  nit_last_section_num = p_buf[7];

  OS_PRINTF("[nit] length=%d,ver_num=%d s_num=%d last_num=%d\n",
                    length,version_number,nit_section_num,nit_last_section_num);
  p_data += 10;

  ca_nit_parse_descriptor(p_data, length);

  if(nit_section_num == nit_last_section_num)
  {
    OS_PRINTF("\nparsing nit over.  ql_ca_nit_flag = %d\n",ql_ca_nit_flag);
    if(ql_ca_nit_flag == 0)
    {
        ql_ca_nit_flag = 2;
    }

    ql_ca_nit_state = 1;

  }
  else if(nit_section_num < nit_last_section_num)
  {
      match[4] = nit_section_num + 1;
      STBCA_SetStreamGuardFilter(0x40,0x10,
      match,mask,5,0);
      OS_PRINTF("\nstart section_num=%d \n",match[4]);
  }


  return;
}

/*!
  abc
  */
  static void _Ql_SendAllData(u8 uDataNum)
  {
    u8 uIndex = 0;

    if(uDataNum > QL_FILTER_MAX_NUM)
    {
      CAS_QILIAN_PRINTF("\n[ERROR]_Ql_SendAllData  uDataNum > QL_FILTER_MAX_NUM \n");
      uDataNum = QL_FILTER_MAX_NUM;
    }

    for(uIndex = 0; uIndex < uDataNum; uIndex ++)
    {
      CAS_QILIAN_PRINTF("\n~~GET DATA m_byReqId=0x%x\n", s_tData[uIndex].m_byReqId);
      if(BAT_TABLE_ID == s_tData[uIndex].m_byReqId)
      {
        OS_PRINTF("\nbat parse\n");
        STBCA_StreamGuardDataGot(s_tData[uIndex].m_byReqId, s_tData[uIndex].m_uOK,
          s_tData[uIndex].m_PID, s_tData[uIndex].p_Data, s_tData[uIndex].m_uLength);
        ql_ca_bat_flag = 1;
      }
      else if(NIT_TABLE_ID == s_tData[uIndex].m_byReqId)
      {
        OS_PRINTF("\nnit parse\n");
        ql_cas_parse_nit(s_tData[uIndex].p_Data);
      }
      else
      {
        STBCA_StreamGuardDataGot(s_tData[uIndex].m_byReqId, s_tData[uIndex].m_uOK,
          s_tData[uIndex].m_PID, s_tData[uIndex].p_Data, s_tData[uIndex].m_uLength);
      }

      if(NULL != s_tData[uIndex].p_Data)
      {
        CAS_QILIAN_PRINTF("\n~~FREE m_byReqId=0x%x\n", s_tData[uIndex].m_byReqId);
        mtos_free(s_tData[uIndex].p_Data);
        s_tData[uIndex].p_Data = NULL;
      }
      memset(&s_tData[uIndex], 0, sizeof(ql_data_got_t));
     }
  }


/*!
  abc
  */
static void _Ql_CaDataMonitor(void *pdata)
{
    u8 aFilterIndex[QL_FILTER_MAX_NUM] = { 0, };
    u8 uValidFilterNum = 0;
    u32 uDataLen = 0;
    u8 *p_GotData = NULL;
    u32 err = 0;
    u8 uDataNum = 0;
    u8 uIndex = 0;
    dmx_device_t *p_dev = NULL;
    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    while(1)
    {
      //ECM && EMM
      _Ql_CaReqLock();
      uValidFilterNum = 0;
      memset(aFilterIndex, 0, QL_FILTER_MAX_NUM);


      //Find valid Filter
      for(uIndex = 0; uIndex < QL_FILTER_MAX_NUM; uIndex ++)
      {
      if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus)
      {
          aFilterIndex[uValidFilterNum] = uIndex;
        uValidFilterNum ++;
      }
    }



    //Valid filter doesnot exist
    if(0 == uValidFilterNum)
    {
      _Ql_CaReqUnLock();
      mtos_task_sleep(20);
      continue;
    }

    //Read Data
    uDataNum = 0;
    for(uIndex = 0; uIndex < uValidFilterNum; uIndex ++)
    {
      if (FILTER_BUSY == s_tFilter[aFilterIndex[uIndex]].m_FilterStatus)
      {
        err = dmx_si_chan_get_data(p_dev, s_tFilter[aFilterIndex[uIndex]].m_dwReqHandle,
                                     &p_GotData, &uDataLen);
        if (err != 0)
        {
          if(mtos_ticks_get() - s_tFilter[aFilterIndex[uIndex]].m_dwStartMS
                 > s_tFilter[aFilterIndex[uIndex]].m_timeout)
          {
              OS_PRINTF("\n[ERROR] ca date timeout \n");
              _Ql_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, FALSE,
                             s_tFilter[aFilterIndex[uIndex]].m_PID, NULL, 0, uDataNum);
              uDataNum++;
              if(ECM_TABLE_ID == s_tFilter[aFilterIndex[uIndex]].m_byReqId ||
                ECM_TABLE_ID1 == s_tFilter[aFilterIndex[uIndex]].m_byReqId)
              {
                Ql_Stb_FreeFilter(&s_tFilter[aFilterIndex[uIndex]]);
                OS_PRINTF("\n[ERROR] ECM filter timeout , free filter~~~~ \n");
                memset((void *)&s_tFilter[aFilterIndex[uIndex]], 0, sizeof(ql_filter_struct_t));
              }
          }
        }
        else
        {
          if(uDataLen > 4096)
            continue;
      #if 0  
          if(s_tFilter[aFilterIndex[uIndex]].m_byReqId == NIT_TABLE_ID)
          {
             OS_PRINTF("\n````````````nit\n");
            _Ql_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, TRUE,
                s_tFilter[aFilterIndex[uIndex]].m_PID, p_GotData, uDataLen, uDataNum);
             uDataNum++;
          }
          else
     #endif	
          {
            if(SC_INSERT_OVER == s_uScStatus)
            {
                if((s_tFilter[aFilterIndex[uIndex]].m_byReqId == ECM_TABLE_ID) ||
                   (s_tFilter[aFilterIndex[uIndex]].m_byReqId == ECM_TABLE_ID1))
                {
					OS_PRINTF("\n++++++++get ecm ticks=%d\n",mtos_ticks_get());
					OS_PRINTF("\n[收到ECM数据][%d]Table_id=0x%x, pid=0x%x.\n",
								uIndex,
								s_tFilter[aFilterIndex[uIndex]].m_byReqId,
								s_tFilter[aFilterIndex[uIndex]].m_PID);
				}

				if ((s_tFilter[aFilterIndex[uIndex]].m_byReqId == 0x40) || (s_tFilter[aFilterIndex[uIndex]].m_byReqId == 0x4a))
				{
					OS_PRINTF("[%s:%d]get data, req_id=%d\n", __FUNCTION__, __LINE__,s_tFilter[aFilterIndex[uIndex]].m_byReqId);
				}
				
                _Ql_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, TRUE,
                  			s_tFilter[aFilterIndex[uIndex]].m_PID, p_GotData, uDataLen, uDataNum);
                uDataNum++;
            }
            else
            {
                _Ql_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, FALSE,
                s_tFilter[aFilterIndex[uIndex]].m_PID, NULL, 0, uDataNum);
                uDataNum++;
            }
          }

          Ql_Stb_FreeFilter(&s_tFilter[aFilterIndex[uIndex]]);
          memset((void *)&s_tFilter[aFilterIndex[uIndex]], 0, sizeof(ql_filter_struct_t));          
          
        }
      }
    }
    _Ql_CaReqUnLock();
    _Ql_SendAllData(uDataNum);
    mtos_task_sleep(10);
    }
}

  /*!
  abc
  */
  u32 QL_CAS_Drv_ClientInit(void)
  {
    BOOL err = 0;
    u32 *p_stack = NULL;
    //create ca req lock semphore

    CAS_QILIAN_PRINTF(" QL_CAS_Drv_ClientInit!mtos_sem_create  u32_CaReqLock\n");

    err = mtos_sem_create((os_sem_t *)&u32_CaReqLock,1);
    if (!err)
    {
      OS_PRINTF("%s : %s : %d : error_code = 0x%08x \n",
        __FILE__, __FUNCTION__, __LINE__, err);
    }

    p_stack = (u32 *)mtos_malloc(QL_CAS_SECTION_TASK_STACK_SIZE);


    err = mtos_task_create((u8 *)"CA_Monitor",
            _Ql_CaDataMonitor,
            (void *)0,
            QL_CAS_SECTION_TASK_PRIO,
            p_stack,
            QL_CAS_SECTION_TASK_STACK_SIZE);

    if (!err)
    {
      OS_PRINTF("Create data monitor task error = 0x%08x!\n", err);
      return 1;
    }

    return 0;
  }

 /*++
  功能：返回智能卡当前状态。
  参数：pbyStatus：	返回智能卡状态，为BYCAS_SC_OUT、BYCAS_SC_IN两种之一。
  --*/
  u8 QL_CAS_DRV_GetSCInsertStatus(void)
  {
    return s_uScStatus;
  }
  /*!
  abc
  */
  void QL_CAS_DRV_SetSCInsertStatus(u8 uScStatus)
  {
    s_uScStatus = uScStatus;
  }
  /*!
  abc
  */
  BVOID QLSTBCA_GetSCStatus(BU8 * pbyStatus)
  {
	  if(QL_CAS_DRV_GetSCInsertStatus() == SC_INSERT_OVER)
	  {
	    *pbyStatus = 1;
	  }
	  else
	  {
	    *pbyStatus = 0;
	  }

	  return;
  }

  /*++
  功能：通知上层区域检测结束
  参数：
  --*/
void STBCA_ZoneCheckEnd(void)
{
    cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;

    OS_PRINTF("\r\n*** STBCA_ZoneCheckEnd*** \r\n");
    set_event(p_priv, CAS_S_ZONE_CHECK_END);
    return;
}


  
/*----------------------------------以下接口是STB提供给CAS调用--------------------------------------------*/
/*++
功能：申请机顶盒创建num个消息栈，每个消息栈最多有20条消息排队。一般后进先出原则。
参数：
	BU8  MSGQUE1;
	STBCA_MSGQUENE_Init(2);
返回:
    1:  执行成功；
    0:  失败
--*/
  
BU8 STBCA_MSGQUENE_Init(BU8 num)
{
MT_ASSERT(num <= QL_MAX_MSG_QUEUE);
memset((u8 *)g_QL_Msg, 0, sizeof(CA_MSG)*MSG_CNT*QL_MAX_MSG_QUEUE);
g_msg_pos_add[0] = 0;
g_msg_pos_get[0] = 0;
g_msg_pos_add[1] = 0;
g_msg_pos_get[1] = 0;

return TRUE;
}

/*++
功能：从第NO个消息队列中获取头条消息，非阻塞型。
参数：如果队列为空，则  参数msg 为空
返回:    成功获取到消息，队列当前消息为下条消息内容。
	CA_MSG *msg1= NULL;
	STBCA_GETCAMSG(0,msg1);
	if(msg1 != NULL )
	{
		//从第一个消息队列中获取并  分析消息
	}
	CA_MSG *msg2= NULL;
	STBCA_GETCAMSG(1,msg2);
	if(msg2 != NULL )
	{
		//从第二个消息队列中获取并  分析消息
	}
--*/
CA_MSG* STBCA_GETCAMSG(BU8 NO)
{


    if ((g_msg_pos_add[NO] == 0)) //当前消息栈为空
      return NULL;

    if ((g_msg_pos_get[NO] > g_msg_pos_add[NO])) //当前消息栈消息被取完，清空数据
    {
      g_msg_pos_add[NO] = 0;
      g_msg_pos_get[NO] = 0;
      memset((u8 *)g_QL_Msg[NO], 0, sizeof(CA_MSG)*MSG_CNT);

      return NULL;
    }

    msg.msgparm1 = g_QL_Msg[NO][g_msg_pos_get[NO]].msgparm1;
    msg.msgparm2 = g_QL_Msg[NO][g_msg_pos_get[NO]].msgparm2;
    msg.msgtype =  g_QL_Msg[NO][g_msg_pos_get[NO]].msgtype;
	

    if ((g_msg_pos_add[NO] >= g_msg_pos_get[NO]) && (g_msg_pos_get[NO] + 1) < MSG_CNT)
   { 
      g_msg_pos_get[NO]++;
    }

    return (&msg);
}

/*++
功能：在第NO个消息队列中增加一条消息，如果队列满则删除所有的消息后增加消息。
参数：
	static CA_MSG msg1;
	msg1.msgtype = 1;
	msg1.msgparm1 = 0x33;
	msg1.msgparm2 = 0x44;
	
	STBCA_SENDCAMSG(0,&msg1);
	STBCA_SENDCAMSG(1,&msg1);
--*/
void STBCA_SENDCAMSG(BU8 NO,CA_MSG* msg)
{

    //if the queue is full, clean the data 
    if (g_msg_pos_add[NO] >= MSG_CNT)
    {
      g_msg_pos_add[NO] = 0;
      g_msg_pos_get[NO] = 0;
      memset((u8 *)g_QL_Msg[NO], 0, sizeof(CA_MSG)*MSG_CNT);
    }

    g_QL_Msg[NO][g_msg_pos_add[NO]].msgparm1 = msg->msgparm1;
    g_QL_Msg[NO][g_msg_pos_add[NO]].msgparm2 = msg->msgparm2;
    g_QL_Msg[NO][g_msg_pos_add[NO]].msgtype  = msg->msgtype;

    g_msg_pos_add[NO]++;

}
//这个接口暂时不用。
void STBCA_ReleaseCAMSG(BU8 NO,CA_MSG* msg)
{

}
 

/*++
功能：挂起当前线程。
参数：
	dwMicroseconds:			毫秒。
--*/
void STBCA_Sleep( int dwMicroseconds )
{
    mtos_task_sleep(dwMicroseconds);
}

/*++
功能：得到一个随机8bit数据，0--255之间。
参数：
	dwMicroseconds:			毫秒。
--*/
BU8 STBCA_RAND_GET( void )
{
   return (rand() % 256);
}

/*++
功能：初始化信号量。
参数：
	pSemaphore:				指向信号量的指针。
	flag 为1表示初始后的信号有个信号等待。0则表示没。
返回:
 1: 成功执行
 0: 表示执行出错
CA_SemaphoreInit
--CA_SemaphoreInit(&gBycaParseDatasem,1);*/
BU8 STBCA_SemaphoreInit( CA_Semaphore *pSemaphore ,BU8 flag)
{
    BOOL err = 0;
    CAS_QILIAN_PRINTF("\n%s = %x flag = %d \n", __FUNCTION__,pSemaphore, flag);
    err = mtos_sem_create((os_sem_t *)pSemaphore, flag);
    if (!err)
    {
      CAS_QILIAN_PRINTF("\n%s error = 0x%08x!\n", err);
      return 0;
    }
    return 1;
}
/*
demo:
	static CA_Semaphore ParseDatasem;
	STBCA_SemaphoreInit(&ParseDatasem,1)
	
	STBCA_SemaphoreWait(&ParseDatasem);
	......
	STBCA_SemaphoreSignal(&ParseDatasem);

*/

/*++
功能：给信号量加信号。
参数：
	pSemaphore: 			指向信号量的指针。
返回:
   	
--*/
void STBCA_SemaphoreSignal( CA_Semaphore *pSemaphore )
{
    u32 err = 0;

    if(NULL == pSemaphore)
    {
      CAS_QILIAN_PRINTF("\nSTBCA_SemaphoreSignal param error\n");
      return ;
    }


    err = mtos_sem_give((os_sem_t *)pSemaphore);

    if(!err)
    {
      CAS_QILIAN_PRINTF("\nSTBCA_SemaphoreSignal error = 0x%08x!\n", err);
      return;
    }
    return;
}

/*++
功能：等待信号量,等待成功之后,信号量为无信号。
参数：
	pSemaphore: 			指向信号量的指针。
返回:
   同上		
--*/
void STBCA_SemaphoreWait( CA_Semaphore *pSemaphore )
{
  if(NULL == pSemaphore)
  {
    CAS_QILIAN_PRINTF("\nSTBCA_SemaphoreWait param error!\n");
    return;
  }

  mtos_sem_take((os_sem_t *)pSemaphore, 0);
  return;
}

/*++
功能：CAS模块向机顶盒注册线程。
参数：
	szName:					注册的线程名称。
	pTaskFun:				要注册的线程函数。
返回:
   1: 成功执行
 0: 表示执行出错
--*/
BU8	STBCA_RegisterTask(const char* szName, pThreadFunc pTaskFun)
{
    u32 err = 0;
    u32 *p_stack = NULL;
    static u8 byPriority = QL_CAS_SECTION_TASK_PRIO+1;

    if(NULL == pTaskFun || NULL == szName)
    {
      CAS_QILIAN_PRINTF("STBCA_RegisterTask parameter error!\n");
      return 0;
    }
    
    /*if(strcmp(szName,"caDemuxReq") == 0)
      byPriority = QL_CAS_SECTION_TASK_PRIO + 1;
    else if(strcmp(szName,"CAFlashtask") == 0)
      byPriority = QL_CAS_SECTION_TASK_PRIO + 2;
    else
    {
      CAS_QILIAN_PRINTF("STBCA_RegisterTask  error!\n");
      return 0;
    }*/

    p_stack = (u32 *)mtos_malloc(8 * 1024);
    err = mtos_task_create((u8 *)szName,
    (void (*)(void *))pTaskFun,
    (void *)0,
      byPriority,
      p_stack,
      8 * 1024);
    byPriority++;

    if(!err)
    {
      CAS_QILIAN_PRINTF("STBCA_RegisterTask error = 0x%08x!\n", err);
      return 0;
    }
    CAS_QILIAN_PRINTF("\nSTBCA_RegisterTask success Name[%s], Priority[%d]\n", szName, byPriority - 1);
    return 1;
  }

/*++
功能：获得机顶盒分配给CAS的存储空间的起点地址和大小（以字节为单位）。
参数：
	ppStartAddr:			机顶盒分配给CAS的存储空间的开始地址。
	lSize:					输出机顶盒分配给CAS的储存空间的大小。
--*/
void STBCA_GetDataBufferAddr(BU32* lSize, BU32* ppStartAddr)
{
  cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;

  *lSize = p_priv->flash_size;
  *ppStartAddr = p_priv->flash_start_adr;

  CAS_QILIAN_PRINTF(" STBCA_GetDataBufferAddr %x   %x\n", *ppStartAddr, *lSize);
  return ;
}

/*++
功能：读取保存在机顶盒中的信息。
参数：
	pStartAddr:				要读取的存储空间的开始地址。
	plDataLen:				输入为要读取的最长数据值；输出为实际读取的大小。
	pData:					存放输出数据。
返回:
   1: 成功执行
   0: 表示执行出错
--*/
BU8 STBCA_ReadDataBuffer(BU32* pStartAddr, BU32* plDataLen, BYTE* pData)
{
  cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;

  CAS_QILIAN_PRINTF(" STBCA_ReadDataBuffer	%x	 %x\n",pData, *plDataLen);
  p_priv->nvram_read((BU32)*pStartAddr, (UINT8 *)pData, plDataLen);
  return 1;
}


/*++
功能：向机顶盒的存储空间写信息。
参数：
	lStartAddr:				要写的存储空间的开始地址。
	plDataLen:				输入为要写的数据的长度；输出为写入的实际长度。
	pData:					要写的数据。
返回:
   1: 成功执行
   0: 表示执行出错
--*/
BU8 STBCA_WriteDataBuffer(BU32 pStartAddr, BU32 plDataLen, const BYTE* pData)
{
  cas_ql_priv_t *p_priv =  (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;

  CAS_QILIAN_PRINTF(" STBCA_WriteDataBuffer %x	 %x\n", pStartAddr, plDataLen);

  p_priv->nvram_write(pStartAddr, (UINT8 *)pData, plDataLen);
  return 1;
}


/*++
功能：设置过滤器接收CAS私有数据。
参数：
	ReqID:				请求收表的table id。机顶盒应该保存该数据，接收到CAS私有数据后调用STBCA_StreamGuardDataGot()处理。
	wPID:				需要过滤的流的PID。
	szFilter:				过滤器的值，为一个8个字节的数组。
	szMask:					过滤器的掩码，为一个8个字节的数组。
	u8Len:					过滤器的长度，为8。
	nWaitSeconds:			收此表最长的等待时间，如果为0则无限等待，单位秒。
返回:
   同上		
	
--*/
BU8 STBCA_SetStreamGuardFilter(BYTE ReqID, WORD wPID, const BYTE* szFilter, const BYTE* szMask,BYTE u8Len, int nWaitSeconds)
{
    u32 err = 0;
    u8 uIndex = 0;
    dmx_filter_setting_t p_param = {0};
    dmx_slot_setting_t p_slot = {0} ;
    dmx_device_t *p_dev = NULL;
	if ((ReqID == 0x40) || (ReqID == 0x40))
	{
		OS_PRINTF("\n[申请私有数据]ReqID=0x%x,pid=0x%x waitsecs=%d\n",
                                  ReqID,wPID,nWaitSeconds);
	}

    _Ql_CaReqLock();
    for(uIndex = 0; uIndex < QL_FILTER_MAX_NUM; uIndex ++)
    {
	    if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus
	      && ReqID == s_tFilter[uIndex].m_byReqId
	      && ECM_TABLE_ID != ReqID
	      && ECM_TABLE_ID1 != ReqID) // byReqID = 1ê?ecm
	    {
	      if((memcmp(szFilter, s_tFilter[uIndex].m_uFilterData, u8Len) == 0)
	        && (memcmp(szMask, s_tFilter[uIndex].m_uFilterMask, u8Len) == 0)
	        && (s_tFilter[uIndex].m_PID == wPID))
	      {
	        OS_PRINTF("Emm filter is the same, don't request new filter!\n");
	        _Ql_CaReqUnLock();
	        return TRUE;
	      }
	      else
	      {
	        OS_PRINTF("Emm filter is different, cancel old filter and request new filter!\n");
	        Ql_Stb_FreeFilter(&s_tFilter[uIndex]);
	        memset((void *)&s_tFilter[uIndex], 0, sizeof(ql_filter_struct_t));
	      }
	    }
    }

    for(uIndex = 0; uIndex < QL_FILTER_MAX_NUM; uIndex ++)
    {
      if(FILTER_FREE == s_tFilter[uIndex].m_FilterStatus)
      {
        break;
      }
    }

    if(uIndex >= QL_FILTER_MAX_NUM)
    {
      OS_PRINTF("[ERROR]Filter array is full!\n");
      _Ql_CaReqUnLock();
      Ql_CAS_Drv_ShowAllCaReq();
      Ql_CAS_Drv_FreeAllEcmReq();
      _Ql_CaReqLock();
      for(uIndex = 0; uIndex < QL_FILTER_MAX_NUM; uIndex ++)
      {
        if(FILTER_FREE == s_tFilter[uIndex].m_FilterStatus)
        {
          break;
        }
      }
      if(uIndex >= QL_FILTER_MAX_NUM)
      {
        _Ql_CaReqUnLock();
        return FALSE;
      }

    }
      p_dev = (dmx_device_t *)dev_find_identifier(NULL
            , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
      MT_ASSERT(NULL != p_dev->p_base);

      p_slot.in = DMX_INPUT_EXTERN0;
      p_slot.pid = wPID;
      p_slot.type = DMX_CH_TYPE_SECTION;

      err = dmx_si_chan_open(p_dev,&p_slot,&s_tFilter[uIndex].m_dwReqHandle);
      if (0 != err)
      {
      OS_PRINTF("\r\n%s : %s : %d : error_code = 0x%08x \n", __FILE__, __FUNCTION__, __LINE__, err);
      _Ql_CaReqUnLock();
      return FALSE;
    }

    s_tFilter[uIndex].p_buf = s_tFilterBuf[uIndex];
    err = dmx_si_chan_set_buffer(p_dev,s_tFilter[uIndex].m_dwReqHandle,s_tFilter[uIndex].p_buf,
      QL_FILTER_BUFFER_SIZE);
    if(err != SUCCESS)
    {
      OS_PRINTF("\r\n[ERROR]set filter buffer failed!\n");
      _Ql_CaReqUnLock();
      return FALSE;
    }
    p_param.continuous = 1;
    p_param.en_crc = FALSE;//DISABLE FOR CA DATA
    if(u8Len > DMX_SECTION_FILTER_SIZE)
    {
      OS_PRINTF("make len err!\n");
      _Ql_CaReqUnLock();
      return FALSE;
    }
    memcpy(p_param.value, szFilter, u8Len);
    memcpy(p_param.mask, szMask, u8Len);
    err = dmx_si_chan_set_filter(p_dev,s_tFilter[uIndex].m_dwReqHandle,&p_param);
    if(err != SUCCESS)
    {
      OS_PRINTF("\n[ERROR]set filter failed!\n");
      _Ql_CaReqUnLock();
      return FALSE;
    }
    err = dmx_chan_start(p_dev,s_tFilter[uIndex].m_dwReqHandle);
    if(err != SUCCESS)
    {
      OS_PRINTF("\n[ERROR]start channel  failed!\n");
      _Ql_CaReqUnLock();
      return FALSE;
    }

    if(0 == nWaitSeconds || (ECM_TABLE_ID != ReqID && ECM_TABLE_ID1 != ReqID))
    {
      s_tFilter[uIndex].m_timeout = 0xffffffff;
    }
    else
    {
      s_tFilter[uIndex].m_timeout = nWaitSeconds * 1000;
    }
    s_tFilter[uIndex].m_FilterStatus = FILTER_BUSY;
    s_tFilter[uIndex].m_byReqId = ReqID;
    s_tFilter[uIndex].m_PID = wPID;
    memcpy(s_tFilter[uIndex].m_uFilterData, szFilter, u8Len);
    memcpy(s_tFilter[uIndex].m_uFilterMask, szMask, u8Len);
    s_tFilter[uIndex].m_dwStartMS = mtos_ticks_get();
    OS_PRINTF("\nReqID =0x%x \n ", ReqID);
    if(ReqID == ECM_TABLE_ID)
      OS_PRINTF("\nECM filter set ticks=%d\n",mtos_ticks_get());
    _Ql_CaReqUnLock();
    return TRUE;
}

/*++
功能：打印调试信息。
参数：
	pszMsg:					调试信息内容。
--*/
void STBCA_AddDebugMsg(const char *pszMsg)
{
    OS_PRINTF("%s\n", pszMsg);
    return;
}

/*++
	功能: 通知菜单，显示邮件图标
	 参数
	 0,表示显示新邮件图标
	 1，表示显示邮箱已满
	 2, 表示没有新邮件图标
*/
void STBCA_EmailNotify(BU8 type)
{
  cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;
  switch(type)
  {
    case 0:
        set_event(p_priv, CAS_C_SHOW_NEW_EMAIL);
        CAS_QILIAN_PRINTF("\n有新邮件\n");
        break;
    case 1:
        set_event(p_priv, CAS_C_SHOW_SPACE_EXHAUST);
        CAS_QILIAN_PRINTF("\n邮件满\n");
        break;
    case 2:
        set_event(p_priv, CAS_C_HIDE_NEW_EMAIL);
        CAS_QILIAN_PRINTF("\n无新邮件\n");
        break;
    default:
        CAS_QILIAN_PRINTF("\n邮件错误\n");
        break;
    }
    return;
  }
/*++
功能：返回智能卡当前状态。
返回值：0表示没卡插入；1表示有卡	
--*/
BU8 STBCA_GetSCStatus(void )
{
    return (s_uScStatus==SC_INSERT_OVER ? 1: 0);
}

/*++
功能：ca库申请复位智能卡。
--*/
void STBCA_SMARTCARD_ReSet(void)
{
	scard_device_t *p_smc_dev = NULL;
	int ret;
	CAS_QILIAN_PRINTF(" %s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

	p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
	MT_ASSERT(NULL != p_smc_dev);

	ret = scard_reset(p_smc_dev);
	return ;
}
/*++
功能：实现与智能卡之间的通讯。采用7816标准。
参数：
	num:			通讯编号，无实际意义。
	bSendData:			待发送的一串命令字节。
	bReplyLen:			发送时，是指定数据长度；读数据时读取的数据长度。
	bReplyData:         		返回的数据串.
返回值   :
	0   : 物理通讯失败
	1   : 成功
--*/
BU8 STBCA_SCAPDU(BU8 num, BYTE* bSendData,BU32* bReplyLen, BYTE* bReplyData)
{
  u32 err = 0;
  u16 num_read_ok = 0;
  u8 Answer[32] = {0};
  u32 AnswerLength = 0;
  u8 uTransTimes = 0;
  u16 uIndex = 0;
  u8 uWholeChar[1000] = {0};
  u8 uChar[4] = {0};
  BU16 wCommandLen = 0;

  if(NULL == bSendData || NULL == bReplyData || NULL == bReplyLen)
  {
    CAS_QILIAN_PRINTF("STBCA_SCPBRun error parameter!\n");
    return FALSE;
  }
  wCommandLen = *bReplyLen;

  for(uIndex = 0; (uIndex < wCommandLen) && (uIndex < (sizeof(uWholeChar) / 3)); uIndex ++)
  {
   // CAS_QILIAN_PRINTF((char *)uChar, "%02x ", bSendData[uIndex]);
    strcat((char *)uWholeChar, (char *)uChar);
  }
  CAS_QILIAN_PRINTF("%s\n", uWholeChar);

  do
  {
    err = smc_run_cmd((u8 *)bSendData, wCommandLen, bReplyData, 255, &num_read_ok);
    uTransTimes++;
  }while(0 != err && uTransTimes < 3);

  if(0 == err)
  {
    memset(uWholeChar, 0, sizeof(uWholeChar));
    //OS_PRINTF("Reply: ReplyLen = %d\n", num_read_ok);
    for(uIndex = 0; (uIndex < num_read_ok) && (uIndex < (sizeof(uWholeChar) / 3)); uIndex ++)
    {
      //CAS_BY_PRINTF((char *)uChar, "%02x ", byszReply[uIndex]);
      strcat((char *)uWholeChar, (char *)uChar);
    }
    //OS_PRINTF("%s\n", uWholeChar);

    *bReplyLen = num_read_ok;
    return TRUE;
  }

  drv_smc_reset((u8 *)Answer, (u8 *)&AnswerLength);

  return FALSE;
}


/*
功能：显示用户不能观看节目等提示信息，下面两个函数对应使用。
参数：	
	b8MesageNo:				OSD 要显示的消息类型
--*/
void   STBCA_ShowOSDPromptMsg(BYTE b8MesageNo)
{
cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;
u32 event = 0;
if(ql_ecm_count == 0)
  b8MesageNo = STBCA_NO_ERROR;
switch(b8MesageNo)
{
case STBCA_NO_ERROR://clean
  CAS_QILIAN_PRINTF("\n没有错误，取消上次显示\n");
  event = CAS_S_CLEAR_DISPLAY;
  break;
case STBCA_ERROR_InitCardCommand://智能卡初始化错误
  OS_PRINTF("\n智能卡初始化错误\n");
  event = CAS_E_CARD_INIT_FAIL;
  break;
case STBCA_ERROR_ReadCardCommand://智能卡读卡号命令错误
  CAS_QILIAN_PRINTF("\n智能卡读卡号命令错误\n");
  event = CAS_E_CARD_DATA_GET;
  break;
case STBCA_ERROR_NO_CARD://请放入智能卡
  CAS_QILIAN_PRINTF("\n请放入智能卡\n");
  event = CAS_E_CARD_REGU_INSERT;
  break;
case STBCA_ERROR_EMM_RET://EMM错误
  CAS_QILIAN_PRINTF("\nEMM错误 \n");
  event = CAS_E_EMM_ERROR;
  break;
case STBCA_ERROR_ECM_RET://ECM错误
  CAS_QILIAN_PRINTF("\nECM错误\n");
  event = CAS_E_ECM_ERROR;
  break;
case STBCA_ECM_RET_READCOMM_ERROR://智能卡返回ECM数据错误
  CAS_QILIAN_PRINTF("\n智能卡返回ECM数据错误 \n");
  event = CAS_E_ECM_RETURN_ERROR;
  break;
case STBCA_EMMECM_RET_P1P2_ERROR://智能卡返回ECM EMM P1P2数据错误
  CAS_QILIAN_PRINTF("\n智能卡返回ECM EMM P1P2数据错误 \n");
  event = CAS_E_ECMEMM_RETURN_P1P2_ERROR;
  break;
case STBCA_EMMECM_RET_P3_ERROR://智能卡返回ECM EMM P3数据错误
  CAS_QILIAN_PRINTF("\n智能卡返回ECM EMM P3数据错误\n");
  event = CAS_E_ECMEMM_RETURN_P3_ERROR;
  break;
case STBCA_EMMECM_RET_DES_ERROR://智能卡用户无授权
  CAS_QILIAN_PRINTF("\n智能卡用户无授权id9\n");
  event = CAS_E_PROG_UNAUTH;
  break;
case STBCA_ECM_RET_NOENTITLE_ERROR://智能卡用户无授权
  CAS_QILIAN_PRINTF("\n智能卡用户无授权id10\n");
  event = CAS_E_PROG_UNAUTH;
  break;
case STBCA_EMMECM_RET_STORE_ERROR://智能卡返回错误
  CAS_QILIAN_PRINTF("\n智能卡返回错误 0x0a\n");
  event = CAS_E_CARD_REG_FAILED;
  break;
case STBCA_EMMECM_RET_CHECK_ERROR://智能卡用户无授权
  CAS_QILIAN_PRINTF("\n智能卡用户无授权id12\n");
  event = CAS_E_PROG_UNAUTH;
  break;
case STBCA_EMM_NOKEY_OR_NOFILE_ERROR://智芸ㄓ没奘谌?
  CAS_QILIAN_PRINTF("\n智能卡用户奘谌╥d13\n");
  event = CAS_E_PROG_UNAUTH;
  break;
case STBCA_ECM_OUTDATE_ERROR://智能卡用户无授权
  CAS_QILIAN_PRINTF("\n智能卡用户无授权id14\n");
  event = CAS_E_PROG_UNAUTH;
  break;
case STBCA_EMMECM_RET_UNKNOW_ERROR://智能卡ECM EMM返回未知错误
  CAS_QILIAN_PRINTF("\n智能卡ECM EMM返回未知错误 0x0f\n");
  event = CAS_E_ECMEMM_RETURN_ERROR;
  break;
case STBCA_CARD_NEED_PAIRING://节目要求机卡配对
  CAS_QILIAN_PRINTF("\n节目要求机卡配对\n");
  event = CAS_E_CARD_DIS_PARTNER;
  break;
case STBCA_CARD_PAIRING_OK://节目机卡配对成功
  CAS_QILIAN_PRINTF("\n节目机卡配对成n");
  event = CAS_S_CARD_STB_PARTNER;
  break;
case STBCA_NO_ECM_INFO://没有ECM流信息
  CAS_QILIAN_PRINTF("\n没有ECM流信息\n");
  event = CAS_E_NO_ECM;
  break;
case STBCA_ZONE_FORBID_ERROR://该节目被区域限播
  CAS_QILIAN_PRINTF("\n该节目被区域限播\n");
  event = CAS_E_ZONE_CODE_ERR;
  break;
case STBCA_UserCdn_Limit_Ctrl_Enable://该用户被条件限播
  CAS_QILIAN_PRINTF("\n该用户被条件限播\n");
  event = CAS_C_USERCDN_LIMIT_CTRL_ENABLE;
  break;
case STBCA_UserCdn_Limit_Ctrl_Disable://该用户被取消条件薏?
  CAS_QILIAN_PRINTF("\n该用户被取消条件限播\n");
  event = CAS_C_USERCDN_LIMIT_CTRL_DISABLE;
  break;
case STBCA_ZoneCdn_Limit_Ctrl_Enable://该区域被条件限播
  CAS_QILIAN_PRINTF("\n该区域被条件限播\n");
  event = CAS_C_ZONECDN_LIMIT_CTRL_ENABLE;
  break;
case STBCA_ZoneCdn_Limit_Ctrl_Disable://该区域被取消条件限播
  CAS_QILIAN_PRINTF("\n该区域被取消条件限播\n");
  event = CAS_C_ZONECDN_LIMIT_CTRL_DISABLE;
  break;
case STBCA_PAIRED_OtherCARD_ERROR://该机器与其它卡配对
  CAS_QILIAN_PRINTF("\n该机器与其它卡配对\n");
  event = CAS_E_PAIRED_OTHERCARD_ERROR;
  break;
case STBCA_PAIRED_OtherMachine_ERROR://该卡氪嘶鞑慌涠?
  CAS_QILIAN_PRINTF("\n该卡与此机器不配对\n");
  event = CAS_E_PAIRED_OTHERSTB_ERROR;
  break;
case STATUE_CA_TIME_VAILD://子母卡配对时间更新
  CAS_QILIAN_PRINTF("\n子母卡配对时间更新\n");
  event = CAS_C_MASTERSLAVE_PAIRED_TIMEUPDATE;
  break;
case STATUE_CA_NEED_PAIR://子母卡需要配对
  CAS_QILIAN_PRINTF("\n子母卡需要配对\n");
  event = CAS_C_MASTERSLAVE_NEEDPAIRED;
  break;
case STATUE_CA_CARD_ZONE_INVALID://您的智能卡不再服务区
  CAS_QILIAN_PRINTF("\n您的智能卡不再服务区\n");
  event = CAS_C_CARD_ZONE_INVALID;
  break;
case STATUE_CA_CARD_ZONE_CHECK_ENTER://正在寻找网络，请等待
  CAS_QILIAN_PRINTF("\n正在寻找网络，请等待\n");
  event = CAS_C_CARD_ZONE_CHECK_ENTER;
  break;
case STATUE_CA_CARD_ZONE_CHEKC_OK://漫游功能卡检测正常
  CAS_QILIAN_PRINTF("\n漫游功能卡检测正常\n");
  event = CAS_C_CARD_ZONE_CHECK_OK;
  break;
default://未知错误
  CAS_QILIAN_PRINTF("\n！！！！！未知错误\n");
  event = CAS_E_UNKNOWN_ERROR;
  break;
}
set_event(p_priv, event);
return;
}


/*--------------------------- 类型2的界面：用户可以取消显示的界面 -----------------------*/
/*
功能：显示OSD消息，可以是同步显示，也可以是异步显示。注意到期提示要优先显示
参数:	
	szOSD:					用于显示的OSD字符。
	wDuration:			为0时为到期提示。
typedef enum {
	MSG_DATA_REMINDED=0,
		// 兼容老的版本，0为到期提示类型
	MSG_DISPLAY_ICON=1,	
		//有新邮件的图标显示
	MSG_POP_TEXT=2,	
		//弹出显示框，需要手动清除
	MSG_DISPLAY_MAILBOX =4,
		//弹出邮箱菜单
	MSG_SCROLL_TOP = 7,
		//在屏幕顶部滚动字符
	MSG_SCROLL_BOTTOM=8,
		//在屏幕底部滚动字符
	MSG_POP_TOP=14,
		//在屏幕顶部显示字符
	MSG_POP_BOTTOM=15
	   //在屏幕底部显示字符
}CAMSG_Display_Type;
说明：
	用户可以通过遥控器取消当前OSD显示。
	gx方案的消息:
		如果到期提示在显示，其它osd消息不可以覆盖。
		如果用户退出到期消息，方可显示其它OSD消息。
		如果其它消息在显示，收到到期提示消息后，要及时显示到期提示内容。
--*/
void   STBCA_ShowOSDTXTMsg(const char* szOSD, CAMSG_Display_Type wDuration)
{
  u8 uOSDLen = 0;
  u32 event = 0;
    cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;

  if(NULL == szOSD)
  {
    return;
  }
  switch(wDuration)
  {
    case MSG_DATA_REMINDED:
        //event = ;
        break;
    case MSG_DISPLAY_ICON:
        event = CAS_C_SHOW_NEW_EMAIL;
        break;
    case MSG_POP_TEXT:
        //event = ;
        break;
    case MSG_DISPLAY_MAILBOX:
        //event = ;
        break;
    case MSG_SCROLL_TOP:
        event = CAS_C_SHOW_OSD;
		OsdMsg.osd_display.roll_mode = 0;
		OsdMsg.osd_display.roll_value = 0;
		OsdMsg.osd_display.location = 0;
		OsdMsg.type = 0;
        break;       
    case MSG_SCROLL_BOTTOM:
        event = CAS_C_SHOW_OSD;
		OsdMsg.osd_display.roll_mode = 0;
		OsdMsg.osd_display.roll_value = 0;
		OsdMsg.osd_display.location = 1;
		OsdMsg.type = 0;
        break;  
    case MSG_POP_TOP:
        //event = ;
        break;  
    case MSG_POP_BOTTOM:
        //event = ;
        break;  
    default:
        break;
  }
  uOSDLen = strlen(szOSD);
  OS_PRINTF("szMsg = %s Show Style = %d \n",
          szOSD,
          wDuration);

  strncpy((char*)OsdMsg.data, szOSD, uOSDLen);

  set_event(p_priv, event);
}

/*++
功能：显示指纹信息，以图标方式显示指纹,显示位置一般和邮件其它图标不重叠
参数：szOSD == NULL 时清除屏幕已显示的消息。
--*/
void STBCA_FingerSHOW(const char* szOSD)
{
    cas_ql_priv_t *p_priv = (cas_ql_priv_t *)g_cas_priv.cam_op[CAS_ID_QL].p_priv;
    if(szOSD == NULL)
    {
      set_event(p_priv, CAS_C_HIDE_NEW_FINGER);
    }
    else
    {
      msgFinger.data_len = strlen(szOSD);
      strncpy((char*)msgFinger.data,szOSD,msgFinger.data_len);
      //msgFinger.data_len = 100;
      msgFinger.during = 0;
      set_event(p_priv, CAS_C_SHOW_NEW_FINGER);
    }
    return;
}
/*++
功能:     设置当前节目的解扰密钥
参数：节目serverID
--*/

void STBCA_ScrSetCW( BU16  serverID, const BU8* pb8OddKey, const BU8* pb8EvenKey,BU8 KeyLen)
{
  u16 v_pid = 0;
  u16 a_pid = 0;
  dmx_device_t *p_dev = NULL;
  RET_CODE ret = 0;

  if((NULL == pb8OddKey) || (NULL == pb8EvenKey))
  {
    return;
  }

  OS_PRINTF("\nOddKey %02x %02x %02x %02x %02x %02x %02x %02x   ticks=%d~~~~~~~~~~~~~~~~~~~~~\n"
    , pb8OddKey[0], pb8OddKey[1], pb8OddKey[2], pb8OddKey[3]
    , pb8OddKey[4], pb8OddKey[5], pb8OddKey[6], pb8OddKey[7],mtos_ticks_get());
   OS_PRINTF("EvenKey %02x %02x %02x %02x %02x %02x %02x %02x\n"
    , pb8EvenKey[0], pb8EvenKey[1], pb8EvenKey[2], pb8EvenKey[3]
    , pb8EvenKey[4], pb8EvenKey[5], pb8EvenKey[6], pb8EvenKey[7]);

  p_dev = (dmx_device_t *)dev_find_identifier(NULL
  , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(NULL != p_dev->p_base);

  if(old_v_channel != 0xffff)
  {
    ret = dmx_descrambler_onoff(p_dev,old_v_channel,FALSE);
    old_v_channel = 0xffff;
  }
  if(old_a_channel != 0xffff)
  {
    ret = dmx_descrambler_onoff(p_dev,old_a_channel,FALSE);
    old_a_channel = 0xffff;
  }

  v_pid = cas_ql_get_v_pid();
  a_pid = cas_ql_get_a_pid();

  OS_PRINTF("set key v_pid= %x  a_pid = %x\n", v_pid, a_pid);
  ret = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
  if(ret != SUCCESS)
  return;
  
  ret = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
  if(ret != SUCCESS)
  return;
  OS_PRINTF("set key v_channel= %x  a_channel = %x\n", v_channel, a_channel);

  if(v_channel != 0xffff)
  {
    ret = dmx_descrambler_onoff(p_dev,v_channel,TRUE);
    if(ret != SUCCESS)
      return;
    ret = dmx_descrambler_set_odd_keys(p_dev,v_channel,(u8 *)pb8OddKey,8);
    ret = dmx_descrambler_set_even_keys(p_dev,v_channel,(u8 *)pb8EvenKey,8);
  }

  if (a_channel != 0xffff)
  {
    ret = dmx_descrambler_onoff(p_dev,a_channel,TRUE);
    if(ret != SUCCESS)
      return;
    ret = dmx_descrambler_set_odd_keys(p_dev,a_channel,(u8 *)pb8OddKey,8);
    ret = dmx_descrambler_set_even_keys(p_dev,a_channel,(u8 *)pb8EvenKey,8);
  }

  old_v_channel = v_channel;
  old_a_channel = a_channel;
  return;
}
/*
功能：在特定的频点寻找区域信息。
参数:	区域调制器的频率KHZ;
返回:  1，表示频点已锁到，否则未锁定该频点。
注意:  信号锁定后，此时ca会申请过滤BAT数据。
--*/
BU8 STBCA_SetZoneFreq( BU32 Freq )
{
  nim_device_t *p_nim_dev;
  RET_CODE ret = 0;
  unsigned char  match[16] = {0x4a,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned char  mask[16] =  {0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  nim_channel_info_t chanl_info = { 0 };
  static u8 cnt = 0;
  OS_PRINTF("[%s:%d]cnt=%d\n", __FUNCTION__, __LINE__, cnt);
  if(cnt ==0)
  {
    OS_PRINTF("\nSTBCA_SetZoneFreq %d\n",Freq);
    p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
                        DEV_IDT_TYPE,
                        SYS_DEV_TYPE_NIM);

    chanl_info.frequency = Freq;
    chanl_info.param.dvbc.symbol_rate = 6875;
    chanl_info.param.dvbc.modulation = NIM_MODULA_QAM64;

    ret = nim_channel_connect(p_nim_dev, &chanl_info, 0);

    if(chanl_info.lock  == 1)
    {
      OS_PRINTF("\nLOCK\n");
      STBCA_SetStreamGuardFilter(0x4a,0x11,
                        match,mask,1,1);
      OS_PRINTF("\nset bat ok\n");
    }
    else
    {
      ql_ca_bat_flag = 2;
      OS_PRINTF("\nUNLOCK\n");
    }
    cnt = 1;
    return chanl_info.lock;
  }
  return 0;
}



