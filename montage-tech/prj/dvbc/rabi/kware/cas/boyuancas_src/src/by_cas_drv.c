/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"


#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "charsto.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "dmx.h"

#include "BYCAca_stb.h"
#include "BYCApubtype.h"
#include "BYCAstb_ca.h"
#include "boyuan_cas_include.h"
#include "sys_cfg.h"
#include "sys_dbg.h"

//#define CAS_BY_DEBUG

#ifdef CAS_BY_DEBUG
#define CAS_BY_PRINTF mtos_printk
#else
#define CAS_BY_PRINTF(...) do{} while(0)
#endif


#define BY_CAS_SECTION_TASK_STACK_SIZE    (16 * 1024)


/*!
  abc
  */
typedef enum {
	/*!
	abc
	*/
	FILTER_FREE,

	/*!
	abc
	*/
	FILTER_BUSY
} by_filter_status_t;

#define EMM_TABLE_ID  0x81
#define ECM_TABLE_ID  0x82
#define PUBEMAIL_TABLE_ID 0x91
#define PRIEMAIL_TABLE_ID 0x92
#define PUBADRESS_TABLE_ID  0x93
#define CA_MENU_TABLE_ID  0x95
#define NIT_TABLE_ID  0x40
#define BAT_TABLE_ID  0x4a


extern u8 by_ca_bat_flag;

extern u8 by_ca_nit_flag;
extern u8 by_ca_nit_state;

u8 by_ca_zone_id[50] = {0};
 
typedef struct
{
	  /*!
	  abc
	    */
	by_filter_status_t  m_FilterStatus;
	/*!
	abc
	*/
	u8  m_byReqId;
	/*!
	abc
	*/
	u16 m_PID;
	/*!
	abc
	*/
	u8  m_uFilterData[BY_FILTER_DEPTH_SIZE];
	/*!
	abc
	*/
	u8  m_uFilterMask[BY_FILTER_DEPTH_SIZE];
	/*!
	abc
	*/
	u16 m_dwReqHandle;
	/*!
	abc
	*/
	u32 m_dwStartMS;
	/*!
	abc
	*/
	u32 m_timeout;
	/*!
	abc
	*/
	u8  *p_buf;
}by_filter_struct_t;

/*!
abc
*/
typedef struct
{
	/*!
	abc
	*/
	u8  m_byReqId;
	/*!
	abc
	*/
	u8  m_uOK;
	/*!
	abc
	*/
	u16 m_PID;
	/*!
	abc
	*/
	u8  *p_Data;
	/*!
	abc
	*/
	u16 m_uLength;
}by_data_got_t;


static by_filter_struct_t s_tFilter[BY_FILTER_MAX_NUM] = {{0,},};
static by_data_got_t  s_tData[BY_FILTER_MAX_NUM] = {{0,},};
static u8 s_tFilterBuf[BY_FILTER_MAX_NUM][BY_FILTER_BUFFER_SIZE];
static u32 u32_CaReqLock = 0;
static u8 s_uScStatus = SC_STATUS_END;


  /*!
  abc
  */
static void _By_CaReqLock(void)
{
	mtos_sem_take((os_sem_t *)&u32_CaReqLock, 0);
}
  /*!
  abc
  */
static void _By_CaReqUnLock(void)
{
	mtos_sem_give((os_sem_t *)&u32_CaReqLock);
}
  /*!
  abc
  */
static void BY_Stb_FreeFilter(by_filter_struct_t *s_tFilter)
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
void BY_CAS_Drv_FreeAllEcmReq(void)
{
	u8 uIndex = 0;
	//CAS_BY_PRINTF("[BY]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
	_By_CaReqLock();

	for(uIndex = 0; uIndex < BY_FILTER_MAX_NUM; uIndex ++)
	{
		if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
		ECM_TABLE_ID == s_tFilter[uIndex].m_byReqId)
		{
		BY_Stb_FreeFilter(&s_tFilter[uIndex]);
		BYCASTB_StreamGuardDataGot(s_tFilter[uIndex].m_byReqId,
		false, s_tFilter[uIndex].m_PID, NULL, 0);
		memset((void *)&s_tFilter[uIndex], 0, sizeof(by_filter_struct_t));
	}
	}

	_By_CaReqUnLock();

	return;
}
  /*!
  abc
  */
void BY_CAS_Drv_FreeAllEmmReq(void)
{
	u8 uIndex = 0;
	_By_CaReqLock();

	for(uIndex = 0; uIndex < BY_FILTER_MAX_NUM; uIndex ++)
	{
		if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
		EMM_TABLE_ID == s_tFilter[uIndex].m_byReqId)
		{
			BY_Stb_FreeFilter(&s_tFilter[uIndex]);
			BYCASTB_StreamGuardDataGot(s_tFilter[uIndex].m_byReqId,
			false, s_tFilter[uIndex].m_PID, NULL, 0);
			memset((void *)&s_tFilter[uIndex], 0, sizeof(by_filter_struct_t));
		}
	}

	_By_CaReqUnLock();
	return;
}

/*!
  abc
  */
void BY_CAS_Drv_FreeNitReq(void)
{
	u8 uIndex = 0;
	_By_CaReqLock();

	for(uIndex = 0; uIndex < BY_FILTER_MAX_NUM; uIndex ++)
	{
		if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
		NIT_TABLE_ID == s_tFilter[uIndex].m_byReqId)
		{
			BY_Stb_FreeFilter(&s_tFilter[uIndex]);
			memset((void *)&s_tFilter[uIndex], 0, sizeof(by_filter_struct_t));
		}
	}

	_By_CaReqUnLock();
	return;
}

/*!
  abc
  */
void BY_CAS_Drv_FreeBatReq(void)
{
	u8 uIndex = 0;
	_By_CaReqLock();

	for(uIndex = 0; uIndex < BY_FILTER_MAX_NUM; uIndex ++)
	{
		if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
			BAT_TABLE_ID == s_tFilter[uIndex].m_byReqId)
		{
			BY_Stb_FreeFilter(&s_tFilter[uIndex]);
			memset((void *)&s_tFilter[uIndex], 0, sizeof(by_filter_struct_t));
		}
	}

	_By_CaReqUnLock();
	return;
}

  /*!
  abc
  */
static void _By_AddData(u8 m_byReqId, u8 m_uOK, u16 m_PID, u8 *m_pData, u16 m_uLength, u8 uIndex)
{
	if(uIndex >= BY_FILTER_MAX_NUM)
	{
		CAS_BY_PRINTF("\n[ERROR]_By_AddData  uIndex >= BY_FILTER_MAX_NUM \n");
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
static void ca_nit_parse_descriptor(u8 *buf,s16 length)
{
	u8 *p_data = buf;
	u8  descriptor_tag = 0;
	u8  descriptor_length = 0;
	u8 *p_buf = 0;
	u8 i = 0;
	while(length > 0)
	{
		descriptor_tag = p_data[0];
		p_buf = p_data;
		descriptor_length = p_data[1];
		p_data += 2;
		length -= 2;
		if(descriptor_length == 0)
		{
			continue;
		}
		switch(descriptor_tag)
		{
			case 0x9C://
				CAS_BY_PRINTF("\n0x9c len=%d 0x%x\n",descriptor_length,*p_buf);
				for(i = 0;i<descriptor_length;i++)
				{
					CAS_BY_PRINTF("0x%02x ",p_buf[i]);
				}
				by_ca_nit_flag = 1;
				CAS_BY_PRINTF("\ngs_ca_nit_flag =%d\n",by_ca_nit_flag);
				memcpy(by_ca_zone_id,p_buf,descriptor_length);

				CAS_BY_PRINTF("\nset ok\n");
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
static void by_cas_parse_nit(u8 *p_buf)
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

	CAS_BY_PRINTF("[nit] length=%d,ver_num=%d s_num=%d last_num=%d\n",
	length,version_number,nit_section_num,nit_last_section_num);
	p_data += 10;

	ca_nit_parse_descriptor(p_data, length);

	if(nit_section_num == nit_last_section_num)
	{
		CAS_BY_PRINTF("\nparsing nit over.  by_ca_nit_flag = %d\n",by_ca_nit_flag);
		if(by_ca_nit_flag == 0)
		{
			by_ca_nit_flag = 2;
		}
		else if(by_ca_nit_flag == 1)
		{
			DEBUG(CAS, INFO, "BYCAS_NitZONEDes, \n");
			BYCAS_NitZONEDes(by_ca_zone_id);
		}
		by_ca_nit_state = 1;

	}
	else if(nit_section_num < nit_last_section_num)
	{
		match[4] = nit_section_num + 1;
		BYSTBCA_SetStreamGuardFilter(0x40,0x10,
		match,mask,5,0);
		CAS_BY_PRINTF("\nstart section_num=%d \n",match[4]);
	}
	return;
}

  /*!
  abc
  */
static void _By_SendAllData(u8 uDataNum)
{
	u8 uIndex = 0;
	if(uDataNum > BY_FILTER_MAX_NUM)
	{
		CAS_BY_PRINTF("\n[ERROR]_By_SendAllData  uDataNum > BY_FILTER_MAX_NUM \n");
		uDataNum = BY_FILTER_MAX_NUM;
	}

	for(uIndex = 0; uIndex < uDataNum; uIndex ++)
	{
		CAS_BY_PRINTF("\n~~GET DATA m_byReqId=0x%x\n", s_tData[uIndex].m_byReqId);
		if(BAT_TABLE_ID == s_tData[uIndex].m_byReqId)
		{
			CAS_BY_PRINTF("\nbat parse\n");
			BYCA_ParseBAT(s_tData[uIndex].p_Data);
			by_ca_bat_flag = 1;
		}
		else if(NIT_TABLE_ID == s_tData[uIndex].m_byReqId)
		{
			CAS_BY_PRINTF("\nnit parse\n");
			by_cas_parse_nit(s_tData[uIndex].p_Data);
		}
		else
		{
			BYCASTB_StreamGuardDataGot(s_tData[uIndex].m_byReqId, s_tData[uIndex].m_uOK,
			s_tData[uIndex].m_PID, s_tData[uIndex].p_Data, s_tData[uIndex].m_uLength);
		}

		if(NULL != s_tData[uIndex].p_Data)
		{
			CAS_BY_PRINTF("\n~~FREE m_byReqId=0x%x\n", s_tData[uIndex].m_byReqId);
			mtos_free(s_tData[uIndex].p_Data);
			s_tData[uIndex].p_Data = NULL;
		}
		memset(&s_tData[uIndex], 0, sizeof(by_data_got_t));
	}
}

static void _By_CaDataMonitor(void *pdata)
{
	u8 aFilterIndex[BY_FILTER_MAX_NUM] = { 0, };
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
		uValidFilterNum = 0;
		memset(aFilterIndex, 0, BY_FILTER_MAX_NUM);
		_By_CaReqLock();

		//Find valid Filter
		for(uIndex = 0; uIndex < BY_FILTER_MAX_NUM; uIndex ++)
		{
			if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus)
			{
				aFilterIndex[uValidFilterNum] = uIndex;
				uValidFilterNum ++;
			}
		}

		_By_CaReqUnLock();

		//Valid filter doesnot exist
		if(0 == uValidFilterNum)
		{
			mtos_task_sleep(20);
			continue;
		}
		_By_CaReqLock();
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
						CAS_BY_PRINTF("\n[ERROR] ca date timeout \n");
						_By_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, FALSE,
						s_tFilter[aFilterIndex[uIndex]].m_PID, NULL, 0, uDataNum);
						uDataNum++;
						if(ECM_TABLE_ID == s_tFilter[aFilterIndex[uIndex]].m_byReqId)
						{
							BY_Stb_FreeFilter(&s_tFilter[aFilterIndex[uIndex]]);
							CAS_BY_PRINTF("\n[ERROR] ECM filter timeout , free filter~~~~ \n");
							memset((void *)&s_tFilter[aFilterIndex[uIndex]], 0, sizeof(by_filter_struct_t));
						}
					}
				}
				else
				{
					if(s_tFilter[aFilterIndex[uIndex]].m_byReqId == NIT_TABLE_ID)
					{
						CAS_BY_PRINTF("\n````````````nit\n");
						_By_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, TRUE,
						s_tFilter[aFilterIndex[uIndex]].m_PID, p_GotData, uDataLen, uDataNum);
						uDataNum++;
					}
					else if(s_tFilter[aFilterIndex[uIndex]].m_byReqId == BAT_TABLE_ID)
					{
						CAS_BY_PRINTF("\n````````````bat\n");
						_By_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, TRUE,
						s_tFilter[aFilterIndex[uIndex]].m_PID, p_GotData, uDataLen, uDataNum);
						uDataNum++;
					}
					else
					{
						if(SC_INSERT_OVER == s_uScStatus)
						{
						#if 0
							if(s_tFilter[aFilterIndex[uIndex]].m_byReqId == ECM_TABLE_ID)
								CAS_BY_PRINTF("\n++++++++get ECM ticks=%d\n",mtos_ticks_get());
							else if (s_tFilter[aFilterIndex[uIndex]].m_byReqId == EMM_TABLE_ID)
								CAS_BY_PRINTF("\n++++++++get EMM ticks=%d\n",mtos_ticks_get());
							else if (s_tFilter[aFilterIndex[uIndex]].m_byReqId == PUBEMAIL_TABLE_ID)
								CAS_BY_PRINTF("\n++++++++get PUBEMAIL TABLE ticks=%d\n",mtos_ticks_get());
							else if (s_tFilter[aFilterIndex[uIndex]].m_byReqId == PRIEMAIL_TABLE_ID)
								CAS_BY_PRINTF("\n++++++++get PRIEMAIL TABLE ticks=%d\n",mtos_ticks_get());
							else if (s_tFilter[aFilterIndex[uIndex]].m_byReqId == CA_MENU_TABLE_ID)
								CAS_BY_PRINTF("\n++++++++get CA MENU TABLE ticks=%d\n",mtos_ticks_get());
							else if (s_tFilter[aFilterIndex[uIndex]].m_byReqId == NIT_TABLE_ID)
								CAS_BY_PRINTF("\n++++++++get NIT ticks=%d\n",mtos_ticks_get());
							else if (s_tFilter[aFilterIndex[uIndex]].m_byReqId == BAT_TABLE_ID)
								CAS_BY_PRINTF("\n++++++++get BAT ticks=%d\n",mtos_ticks_get());
						#endif		
							_By_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, TRUE,
							s_tFilter[aFilterIndex[uIndex]].m_PID, p_GotData, uDataLen, uDataNum);

							uDataNum++;
						}
						else
						{
							_By_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, FALSE,
							s_tFilter[aFilterIndex[uIndex]].m_PID, NULL, 0, uDataNum);
							uDataNum++;
						}
					}
					BY_Stb_FreeFilter(&s_tFilter[aFilterIndex[uIndex]]);
					memset((void *)&s_tFilter[aFilterIndex[uIndex]], 0, sizeof(by_filter_struct_t));
				}
			}
		}
		_By_CaReqUnLock();
		_By_SendAllData(uDataNum);
		mtos_task_sleep(10);
	}
}

  /*!
  abc
  */
void CAS_Drv_ShowAllCaReq(void)
{
	u8 uIndex = 0;
	_By_CaReqLock();

	for(uIndex = 0; uIndex < BY_FILTER_MAX_NUM; uIndex ++)
	{
		if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus)
		{
			CAS_BY_PRINTF("\nReqID = %d\tPid = %d\n",
			s_tFilter[uIndex].m_byReqId, s_tFilter[uIndex].m_PID);
			CAS_BY_PRINTF("Filter: %02x %02x %02x %02x %02x %02x %02x %02x \n "
				, s_tFilter[uIndex].m_uFilterData[0], s_tFilter[uIndex].m_uFilterData[1]
				, s_tFilter[uIndex].m_uFilterData[2], s_tFilter[uIndex].m_uFilterData[3]
				, s_tFilter[uIndex].m_uFilterData[4], s_tFilter[uIndex].m_uFilterData[5]
				, s_tFilter[uIndex].m_uFilterData[6], s_tFilter[uIndex].m_uFilterData[7]);
			CAS_BY_PRINTF("Filter: %02x %02x %02x %02x %02x %02x %02x %02x \n"
				, s_tFilter[uIndex].m_uFilterMask[0], s_tFilter[uIndex].m_uFilterMask[1]
				, s_tFilter[uIndex].m_uFilterMask[2], s_tFilter[uIndex].m_uFilterMask[3]
				, s_tFilter[uIndex].m_uFilterMask[4], s_tFilter[uIndex].m_uFilterMask[5]
				, s_tFilter[uIndex].m_uFilterMask[6], s_tFilter[uIndex].m_uFilterMask[7]);
		}
	}

	_By_CaReqUnLock();
	return;
}
  /*!
  abc
  */
u32 BY_CAS_Drv_ClientInit(void)
{
	BOOL err = 0;
	u32 *p_stack = NULL;
	//create ca req lock semphore

	CAS_BY_PRINTF(" BY_CAS_Drv_ClientInit!mtos_sem_create  u32_CaReqLock\n");
	err = mtos_sem_create((os_sem_t *)&u32_CaReqLock,1);
	if (!err)
	{
		CAS_BY_PRINTF("%s : %s : %d : error_code = 0x%08x \n",
				__FILE__, __FUNCTION__, __LINE__, err);
	}

	p_stack = (u32 *)mtos_malloc(BY_CAS_SECTION_TASK_STACK_SIZE);

	err = mtos_task_create((u8 *)"CA_Monitor",
	_By_CaDataMonitor,
	(void *)0,
	MDL_CAS_TASK_PRIO_BEGIN,
	p_stack,
	BY_CAS_SECTION_TASK_STACK_SIZE);

	if (!err)
	{
		CAS_BY_PRINTF("Create data monitor task error = 0x%08x!\n", err);
		return 1;
	}

	return 0;
}

  /*++
  功能：返回智能卡当前状态。
  参数：pbyStatus：	返回智能卡状态，为BYCAS_SC_OUT、BYCAS_SC_IN两种之一。
  --*/
u8 BY_CAS_DRV_GetSCInsertStatus(void)
{
	return s_uScStatus;
}
  /*!
  abc
  */
void BY_CAS_DRV_SetSCInsertStatus(u8 uScStatus)
{
	s_uScStatus = uScStatus;
}
  /*!
  abc
  */
BVOID BYSTBCA_GetSCStatus(BU8 * pbyStatus)
{
	if(BY_CAS_DRV_GetSCInsertStatus() == SC_INSERT_OVER)
	{
		*pbyStatus = BYCAS_SC_IN;
	}
	else
	{
		*pbyStatus = BYCAS_SC_OUT;
	}
	return;
}

  /*++
  功能：挂起当前线程。
  参数：
  dwmsec:			毫秒。
  --*/
BU8 BYSTBCA_Sleep(BU32 dwmsec)
{
    mtos_task_sleep(dwmsec);
    return BYCA_OK;
}

  /*++
  功能：初始化信号量,初始化后信号量没有信号。
  参数：pSemaphore:				指向信号量的指针。
    flag:						  初始化信号量值
  --*/
BU8 BYSTBCA_SemaphoreInit(BY_Semaphore *pSemaphore,BU8 flag)
{
    BOOL err = 0;
    CAS_BY_PRINTF("\nBYSTBCA_SemaphoreInit = %x flag = %d \n", pSemaphore, flag);
    err = mtos_sem_create((os_sem_t *)pSemaphore, flag);
    if (!err)
    {
      CAS_BY_PRINTF("\nBYSTBCA_SemaphoreInit error = 0x%08x!\n", err);
      return BYCA_ERROR;
    }
    return BYCA_OK;
}

  /*++
  功能：释放一个信号量。
  参数：pSemaphore: 			指向信号量的指针。
  --*/
BU8 BYSTBCA_SemaphoreDestory(BY_Semaphore *pSemaphore)
{
    u8 err = 0;
    if(NULL == pSemaphore)
    {
      CAS_BY_PRINTF("\nBYSTBCA_SemaphoreSignal param error\n");
      return BYCA_ERROR;
    }
    err = mtos_sem_destroy((os_sem_t *)pSemaphore,0);
    if (!err)
    {
      CAS_BY_PRINTF("\nBYSTBCA_SemaphoreDestory error = 0x%08x!\n", err);
      return BYCA_ERROR;
    }
    return BYCA_OK;
}

  /*++
  功能：给信号量加信号。
  参数：pSemaphore: 			指向信号量的指针。
  --*/
BU8 BYSTBCA_SemaphoreSignal(BY_Semaphore *pSemaphore)
{
    u32 err = 0;
    if(NULL == pSemaphore)
    {
      CAS_BY_PRINTF("\nBYSTBCA_SemaphoreSignal param error\n");
      return BYCA_ERROR;
    }

    err = mtos_sem_give((os_sem_t *)pSemaphore);

    if(!err)
    {
      CAS_BY_PRINTF("\nBYSTBCA_SemaphoreSignal error = 0x%08x!\n", err);
      return BYCA_ERROR;
    }
    return BYCA_OK;
}

  /*++
  功能：等待信号量,等待成功之后,信号量为无信号。
  参数：pSemaphore: 			指向信号量的指针。
  --*/
BU8 BYSTBCA_SemaphoreWait(BY_Semaphore *pSemaphore)
{
	if(NULL == pSemaphore)
	{
		CAS_BY_PRINTF("\nBYSTBCA_SemaphoreSignal param error!\n");
		return BYCA_ERROR;
	}

	mtos_sem_take((os_sem_t *)pSemaphore, 0);
	return BYCA_OK;
}

  /*++
  功能：CAS模块向机顶盒注册线程。
  参数：
  szName:					注册的线程名称。
  pTaskFun:				要注册的线程函数。
  --*/
BU8 BYSTBCA_RegisterTask(const BS8 *szName,pThreadFunc pTaskFun)
{
    u32 err = 0;
    u32 *p_stack = NULL;
    static u8 byPriority = BY_CAS_SECTION_TASK_PRIO+1;
    if(NULL == pTaskFun || NULL == szName)
    {
		CAS_BY_PRINTF("BYSTBCA_RegisterTask parameter error!\n");
		return BYCA_ERROR;
    }
    if(strcmp(szName,"BycaECMPIDTask") == 0)
		byPriority = BY_CAS_SECTION_TASK_PRIO + 1;
    else if(strcmp(szName,"BycaEMMPIDTask") == 0)
		byPriority = BY_CAS_SECTION_TASK_PRIO + 2;
    else if(strcmp(szName,"BYCAFlashtask") == 0)
		byPriority = BY_CAS_SECTION_TASK_PRIO + 3;
    else
    {
		CAS_BY_PRINTF("BYSTBCA_RegisterTask  error!\n");
		return BYCA_ERROR;
	}


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
		CAS_BY_PRINTF("BYSTBCA_RegisterTask error = 0x%08x!\n", err);
		return BYCA_ERROR;
	}
    CAS_BY_PRINTF("\nBYSTBCA_RegisterTask success Name[%s], Priority[%d]\n", szName, byPriority - 1);
    return BYCA_OK;
}

  /*++
  功能：设置过滤器接收CAS私有数据。
  参数：
  byReqID:				请求收表的请求号。机顶盒应该保存该数据，接收到CAS私有数据后调用BYCASTB_StreamGuardDataGot()时应该将该数据返回。
  wEcmPID:				需要过滤的流的PID。
  szFilter:				过滤器的值，为一个8个字节的数组，相当于8*8的过滤器。
  szMask:					过滤器的掩码，为一个8个字节的数组，与8*8的过滤器对应，当szMask的某一bit位为0时表示要过滤的表对应的bit位不需要与szFilter里对应的bit位相同，当szMask某一位为1时表示要过滤的表对应的bit位一定要与szFilter里对应的bit位相同。
  byLen:					过滤器的长度，为8。
  nWaitSecs:			收此表最长的等待时间，如果为0则无限等待，单位秒。
  --*/
BU8 BYSTBCA_SetStreamGuardFilter(BU8 byReqID,BU16 wPID,
                                    BU8 *szFilter,BU8 *szMask,BU8 byLen,BU16 nWaitSecs)
{
	u32 err = 0;
	u8 uIndex = 0;
	dmx_filter_setting_t p_param = {0};
	dmx_slot_setting_t p_slot = {0} ;
	dmx_device_t *p_dev = NULL;
	#if 0
    CAS_BY_PRINTF("\n[BY]设置过滤表 byReqID=0x%x,pid=0x%x waitsecs=%d\n",
                                  byReqID,wPID,nWaitSecs);
	#endif
    _By_CaReqLock();
    for(uIndex = 0; uIndex < BY_FILTER_MAX_NUM; uIndex ++)
    {
	    if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus
	      && byReqID == s_tFilter[uIndex].m_byReqId
	      && ECM_TABLE_ID != byReqID)
	    {
			if((memcmp(szFilter, s_tFilter[uIndex].m_uFilterData, byLen) == 0)
					&& (memcmp(szMask, s_tFilter[uIndex].m_uFilterMask, byLen) == 0)
					&& (s_tFilter[uIndex].m_PID == wPID))
			{
				CAS_BY_PRINTF("Emm filter is the same, don't request new filter!\n");
				_By_CaReqUnLock();
				return TRUE;
			}
			else
			{
				CAS_BY_PRINTF("Emm filter is different, cancel old filter and request new filter!\n");
				BY_Stb_FreeFilter(&s_tFilter[uIndex]);
				memset((void *)&s_tFilter[uIndex], 0, sizeof(by_filter_struct_t));
			}
	    }
    }

    for(uIndex = 0; uIndex < BY_FILTER_MAX_NUM; uIndex ++)
    {
		if(FILTER_FREE == s_tFilter[uIndex].m_FilterStatus)
		{
			break;
		}
    }

    if(uIndex >= BY_FILTER_MAX_NUM)
    {
		CAS_BY_PRINTF("[ERROR]Filter array is full!\n");
		_By_CaReqUnLock();
		CAS_Drv_ShowAllCaReq();
		BY_CAS_Drv_FreeAllEcmReq();
		_By_CaReqLock();
		for(uIndex = 0; uIndex < BY_FILTER_MAX_NUM; uIndex ++)
		{
			if(FILTER_FREE == s_tFilter[uIndex].m_FilterStatus)
			{
				break;
			}
		}
		if(uIndex >= BY_FILTER_MAX_NUM)
		{
			_By_CaReqUnLock();
			return FALSE;
		}

    }
	p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	MT_ASSERT(NULL != p_dev->p_base);

	p_slot.in = DMX_INPUT_EXTERN0;
	p_slot.pid = wPID;
	p_slot.type = DMX_CH_TYPE_SECTION;

	err = dmx_si_chan_open(p_dev,&p_slot,&s_tFilter[uIndex].m_dwReqHandle);
	if (0 != err)
	{
		CAS_BY_PRINTF("\r\n[BY]%s : %s : %d : error_code = 0x%08x \n", __FILE__, __FUNCTION__, __LINE__, err);
		_By_CaReqUnLock();
		return FALSE;
	}

    s_tFilter[uIndex].p_buf = s_tFilterBuf[uIndex];
    err = dmx_si_chan_set_buffer(p_dev,s_tFilter[uIndex].m_dwReqHandle,s_tFilter[uIndex].p_buf,
      BY_FILTER_BUFFER_SIZE);
    if(err != SUCCESS)
    {
	      CAS_BY_PRINTF("\r\n[ERROR]set filter buffer failed!\n");
	      _By_CaReqUnLock();
	      return FALSE;
    }
    p_param.continuous = 1;
    p_param.en_crc = FALSE;//DISABLE FOR CA DATA
    if(byLen > DMX_SECTION_FILTER_SIZE)
    {
	      CAS_BY_PRINTF("[BY]make len err!\n");
	      _By_CaReqUnLock();
	      return FALSE;
    }
    memcpy(p_param.value, szFilter, byLen);
    memcpy(p_param.mask, szMask, byLen);
    err = dmx_si_chan_set_filter(p_dev,s_tFilter[uIndex].m_dwReqHandle,&p_param);
    if(err != SUCCESS)
    {
	      CAS_BY_PRINTF("\n[ERROR]set filter failed!\n");
	      _By_CaReqUnLock();
	      return FALSE;
    }
    err = dmx_chan_start(p_dev,s_tFilter[uIndex].m_dwReqHandle);
    if(err != SUCCESS)
    {
		CAS_BY_PRINTF("\n[ERROR]start channel  failed!\n");
		_By_CaReqUnLock();
		return FALSE;
    }

    if(0 == nWaitSecs || ECM_TABLE_ID != byReqID)
    {
		s_tFilter[uIndex].m_timeout = 0xffffffff;
    }
    else
    {
    	s_tFilter[uIndex].m_timeout = nWaitSecs * 1000;
    }
    s_tFilter[uIndex].m_FilterStatus = FILTER_BUSY;
    s_tFilter[uIndex].m_byReqId = byReqID;
    s_tFilter[uIndex].m_PID = wPID;
    memcpy(s_tFilter[uIndex].m_uFilterData, szFilter, byLen);
    memcpy(s_tFilter[uIndex].m_uFilterMask, szMask, byLen);
    s_tFilter[uIndex].m_dwStartMS = mtos_ticks_get();
  //  CAS_BY_PRINTF("\n[BY]设置成功!  byReqID =0x%x pid = 0x%x\n ", byReqID,wPID);
    //if(byReqID == ECM_TABLE_ID)
      //CAS_BY_PRINTF("\n[BY]ECM filter set ticks=%d\n",mtos_ticks_get());
    _By_CaReqUnLock();
    return BYCA_OK;
 }

  /*++
  功能：CA程序用此函数设置解扰器。将当前周期及下一周期的CW送给解扰器。
  参数：
  wEcmPID:				CW所属的ECMPID。
  szOddKey:				奇CW的数据。
  szEvenKey:				偶CW的数据。
  byKeyLen:				CW的长度。
  bTaingControl:			true：允许录像,false：不允许录像。
  --*/
static u16 v_channel = 0xffff;
static u16 a_channel = 0xffff;
static u16 old_v_channel = 0xffff;
static u16 old_a_channel = 0xffff;
extern u16 cas_by_get_v_pid(void);
extern u16 cas_by_get_a_pid(void);
  /*!
  abc
  */
BU8 BYSTBCA_SetDescrCW(BU16 wEcmPid,BU8 byKeyLen,
              const BU8 *szOddKey,const BU8 *szEvenKey,BU8 bTaingControl)
{
	u16 v_pid = 0;
	u16 a_pid = 0;
	dmx_device_t *p_dev = NULL;
	RET_CODE ret = 0;
	if((NULL == szOddKey) || (NULL == szEvenKey))
	{
		return BYCA_ERROR;
	}
#if 0
	CAS_BY_PRINTF("\nOddKey %02x %02x %02x %02x %02x %02x %02x %02x   ticks=%d~~~~~~~~~~~~~~~~~~~~~\n"
	, szOddKey[0], szOddKey[1], szOddKey[2], szOddKey[3]
	, szOddKey[4], szOddKey[5], szOddKey[6], szOddKey[7],mtos_ticks_get());
	CAS_BY_PRINTF("EvenKey %02x %02x %02x %02x %02x %02x %02x %02x\n"
	, szEvenKey[0], szEvenKey[1], szEvenKey[2], szEvenKey[3]
	, szEvenKey[4], szEvenKey[5], szEvenKey[6], szEvenKey[7]);
#endif
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

	v_pid = cas_by_get_v_pid();
	a_pid = cas_by_get_a_pid();

	CAS_BY_PRINTF("set key v_pid= %x  a_pid = %x\n", v_pid, a_pid);
	ret = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
	if(ret != SUCCESS)
	return BYCA_ERROR;

	ret = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
	if(ret != SUCCESS)
	return BYCA_ERROR;
	CAS_BY_PRINTF("set key v_channel= %x  a_channel = %x\n", v_channel, a_channel);

	if(v_channel != 0xffff)
	{
		ret = dmx_descrambler_onoff(p_dev,v_channel,TRUE);
		if(ret != SUCCESS)
			return BYCA_ERROR;
		ret = dmx_descrambler_set_odd_keys(p_dev,v_channel,(u8 *)szOddKey,8);
		ret = dmx_descrambler_set_even_keys(p_dev,v_channel,(u8 *)szEvenKey,8);
	}

	if (a_channel != 0xffff)
	{
	ret = dmx_descrambler_onoff(p_dev,a_channel,TRUE);
	if(ret != SUCCESS)
		return BYCA_ERROR;
	ret = dmx_descrambler_set_odd_keys(p_dev,a_channel,(u8 *)szOddKey,8);
	ret = dmx_descrambler_set_even_keys(p_dev,a_channel,(u8 *)szEvenKey,8);
	}

	old_v_channel = v_channel;
	old_a_channel = a_channel;
	return BYCA_OK;
}

  /*++
  功能：打印调试信息。
  参数：Param1,Param2,pszMsg:					调试信息内容。
  --*/
void BYSTBCA_AddDebugMsg(BU32 Param1,BU32 Param2,const BS8 *pszMsg)
{
    //CAS_BY_PRINTF("[BYDEBUG %d][0x%x][0x%x] %s \n",mtos_ticks_get(),Param1,Param2,pszMsg);
}

static u8 drv_smc_reset(u8 *pbyATR, u8 *pbyLen)
{
	u32 err = 0;
	u8 uResetTimes = 0;
	scard_atr_desc_t s_atr = {0};
	u8 buf_atr[256] = {0,};
	scard_device_t *p_smc_dev = NULL;
	s_atr.p_buf = buf_atr;
	CAS_BY_PRINTF("[BY]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

	if(NULL == pbyATR || NULL == pbyLen)
	{
		CAS_BY_PRINTF("\nSCReset error parameter!\n");
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
		CAS_BY_PRINTF("%s : %s : %d : error_code = 0x%08x \n", __FILE__, __FUNCTION__, __LINE__, err);
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

BU8 BYCASTB_SCReset(BU8 *pATR)
{
	BU8 ret = false;
	BU8 len = 0;
	ret = drv_smc_reset(pATR, &len);
	return ret;
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
		CAS_BY_PRINTF("\r\nrwopt.bytes_in_actual=%d", rwopt.bytes_in_actual);
		CAS_BY_PRINTF("\r\n Transmission fail [%d]!",ret);
		return 1;
	}
	else
	{
		CAS_BY_PRINTF("\n Transmission successful!");
	}
	return 0;
}

  /*++
  功能：实现与智能卡之间的通讯。采用7816标准。
  参数：
  byReaderNo:				对应的读卡器编号。
  pbyLen:					输入为待发送命令字节的长度;输出为返回字节长度。
  byszCommand:			待发送的一串命令字节。
  szReply:				返回的数据，空间长度恒为256字节。
  --*/
BU8 BYSTBCA_SCAPDU(BU8 byReaderNo,const BU8 *byszCommand,BU16 *pbyLen,BU8 *byszReply)
{
	u32 err = 0;
	u16 num_read_ok = 0;
	u8 Answer[32] = {0};
	u32 AnswerLength = 0;
	u8 uTransTimes = 0;
	//u16 uIndex = 0;
	//u8 uChar[4] = {0};
	BU16 wCommandLen = 0;
	if(NULL == byszCommand || NULL == byszReply || NULL == pbyLen)
	{
		CAS_BY_PRINTF("TFSTBCA_SCPBRun error parameter!\n");
		return FALSE;
	}
	wCommandLen = *pbyLen;
#if 0
	CAS_BY_PRINTF("[BY] Reply: sendLen = %d\n", wCommandLen);
	for(uIndex = 0; uIndex < wCommandLen; uIndex ++)
	{
		CAS_BY_PRINTF((char *)uChar, "%02x ", byszCommand[uIndex]);
	}
	CAS_BY_PRINTF("\n");
#endif
	do
	{
		err = smc_run_cmd((u8 *)byszCommand, wCommandLen, byszReply, 255, &num_read_ok);
		uTransTimes++;
	}while(0 != err && uTransTimes < 3);

	if(0 == err)
	{
#if 0
		CAS_BY_PRINTF("[BY] Reply: ReplyLen = %d\n", num_read_ok);
		for(uIndex = 0; uIndex < num_read_ok ; uIndex ++)
		{
			CAS_BY_PRINTF((char *)uChar, "%02x ", byszReply[uIndex]);
		}
		CAS_BY_PRINTF("\n");
#endif
		*pbyLen = num_read_ok;
		return TRUE;
	}

	drv_smc_reset((u8 *)Answer, (u8 *)&AnswerLength);
	CAS_BY_PRINTF("[BY] smartcard apdu fail \n");
	return FALSE;
}

  /*
  功能: 提供随机数据
  参数: blen 长度pbBuf 返回blen个随机数
  */
BU8 BYSTBCA_Random(BU8 blen,BU8 *pbBuf)
{
    return 1;
}

  /*
  功能：以滚字幕的方式显示OSD消息，可以是同步显示，也可以是异步显示。
  参数:
  ADScroll_Info:
  说明：
  用户可以通过遥控器。
  --*/
BVOID BYSTBCA_ShowPromptOSDMsg(const ADScroll_Info *padInfo)/* 这个暂时可以不用实现*/
{
    return;
}




