#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_dbg.h"
#include "sys_types.h"
#include "sys_define.h"
#include "cas_ware.h"
#include "dvbca_cas_interface.h"

#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_misc.h"
#include "mtos_event.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "dmx.h"
#include "drv_dev.h"
#include "charsto.h"
#include "../../drv/drvbase/drv_svc.h"
#include "cas_adapter.h"
#include "dvbca_cas_include.h"
#include "sys_cfg.h"

#define DVBSTBCA_SHOWPPCDAYS	(15)

//#define FILTER_DEBUG

extern cas_adapter_priv_t g_cas_priv;
u8 new_card_id[CAS_CARD_ID_MAX_LEN];
u8 old_card_id[CAS_CARD_ID_MAX_LEN];
#define DVBCA_FILTER_BUFFER_SIZE (16 * (KBYTES) + 188)
#define DVBCA_CAS_SECTION_TASK_PRIO        MDL_CAS_TASK_PRIO_BEGIN
#define DVBCA_SECTION_TASK_STACK_SIZE   (16 * 1024)
#define EMM  (1)
#define ECM  (0)
static u8 dvbca_filter_buf[2][DVBCA_FILTER_MAX_NUM][DVBCA_FILTER_BUFFER_SIZE];
#if 0
static void data_dump(u8 *p_addr, u32 size)
{
   u32 i = 0;

   for(i = 0; i < size; i++)
   {
      OS_PRINTF("%02x ", p_addr[i]);
      if ((0 == ((i + 1) % 20)) && (i != 0))
     {
         OS_PRINTF("\n");
     }
   }
   OS_PRINTF("\n");
}
#endif

void dvbcas_stb_drv_careqlock(void)
{
	BOOL ret = FALSE;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	MT_ASSERT(priv->dvbcas_lock!=0);
	ret = mtos_sem_take((os_sem_t *)(&(priv->dvbcas_lock)), 0);
	if(ret == FALSE)
	  DEBUG(DBG,INFO,"[DVBCAS]SEM TAKE ERROR\n");
}
void dvbcas_stb_drv_carequnlock(void)
{
	BOOL ret = FALSE;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	MT_ASSERT(priv->dvbcas_lock!=0);
	ret = mtos_sem_give((os_sem_t *)(&(priv->dvbcas_lock)));
	if(ret == FALSE)
		DEBUG(DBG,INFO,"[DVBCAS]SEM GIVE ERROR\n");
}

/*机顶盒实现函数*/
int DVBSTBCA_SCReset(void)
{
	scard_atr_desc_t atr_desc = {0};
    s16 ret = 0;
    u8 buf[36];
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	MT_ASSERT(priv!=NULL);
	DEBUG(DBG, INFO, "call in\n");
	
	ret = scard_reset(priv->p_smc_dev);
	if(SUCCESS != ret)
	{
		DEBUG(DBG, INFO, "cas: reset card failed\n");
		return DVBCA_FAILED;
	}

	
	atr_desc.p_buf = buf;
	ret = scard_active(priv->p_smc_dev, &atr_desc);
	if(SUCCESS != ret)
	{
		DEBUG(DBG, INFO, "cas: reset card failed\n");
		return DVBCA_FAILED;
	}
	priv->reset_flag= 1;
	return DVBCA_SUCESS;
}

static void dvbstb_free_ecm_filter(dmx_device_t *dev)
{
	int i;
	RET_CODE ret;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;

	for (i=0;i<DVBCA_FILTER_MAX_NUM;i++)
	{
		if (priv->ecm_filter[i].valid)
		{
			//free filter
			priv->ecm_filter[i].valid = FALSE;
			dmx_chan_stop(dev, priv->ecm_filter[i].req_handle);
			MT_ASSERT(ret == SUCCESS);
			ret = dmx_chan_close(dev, priv->ecm_filter[i].req_handle);
			MT_ASSERT(ret == SUCCESS);
		}
	}
}

static void dvbstb_free_emm_filter(dmx_device_t *dev)
{
	int i;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	RET_CODE ret;

	for (i=0;i<DVBCA_FILTER_MAX_NUM;i++)
	{
		if (priv->emm_filter[i].valid)
		{
			//free filter
			priv->emm_filter[i].valid = FALSE;
			dmx_chan_stop(dev, priv->emm_filter[i].req_handle);
			MT_ASSERT(ret == SUCCESS);
			ret = dmx_chan_close(dev, priv->emm_filter[i].req_handle);
			MT_ASSERT(ret == SUCCESS);
		}
	}
}

//return index  (-1 have no empty filter)
static int  dvbstb_request_ecm_filter(void)
{
	int i;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;

	for (i=0;i<DVBCA_FILTER_MAX_NUM;i++)
	{
		if (!priv->ecm_filter[i].valid)
		{
			return i;
		}
	}
	return ERR_FAILURE;
}

static int  dvbstb_request_emm_filter(void)
{
	int i;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;

	for (i=0;i<DVBCA_FILTER_MAX_NUM;i++)
	{
		if (!priv->emm_filter[i].valid)
		{
			return i;
		}
	}
	return ERR_FAILURE;
}



int DVBSTBCA_SetEcmFilter(IN ST_DVBCAFilter *pucFilter,					   
						   DVBCA_UINT8 ucFilterNum,					   
						   DVBCA_UINT16 usPid)
{

	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
    int index = 0,i;
    s16 ret = ERR_FAILURE;
	dmx_filter_setting_t p_param = {0};
    dmx_slot_setting_t p_slot = {0} ;
    dmx_device_t *p_dev = NULL;

	MT_ASSERT(pucFilter != NULL);
	
	p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	MT_ASSERT(NULL != p_dev->p_base);
	dvbstb_free_ecm_filter(p_dev);
	
	for (i= 0; i < DVBCA_FILTER_MAX_NUM && i < ucFilterNum; i++)
    {
		index = dvbstb_request_ecm_filter();
		if (index < 0)
		{
			DEBUG(DBG,INFO,"request ecm filter failed!\n");
			return DVBCA_FAILED;
		}

	    p_slot.in = DMX_INPUT_EXTERN0;
	    p_slot.pid = usPid;
	    p_slot.type = DMX_CH_TYPE_SECTION;
		//dvbcas_stb_drv_careqlock();
	    ret = dmx_si_chan_open(p_dev, &p_slot, &(priv->ecm_filter[index].req_handle));
	    if (SUCCESS != ret)
	    {
	        DEBUG(DBG, ERR, "open chan failed, ret=0x%08x!!!!\n", ret);
			//dvbcas_stb_drv_carequnlock();
	        return DVBCA_FAILED;
	    }

	    priv->ecm_filter[index].p_buf = dvbca_filter_buf[ECM][index];

	    ret = dmx_si_chan_set_buffer(p_dev, priv->ecm_filter[index].req_handle, priv->ecm_filter[index].p_buf, DVBCA_FILTER_BUFFER_SIZE);
	    if (SUCCESS != ret)
	    {
	        DEBUG(DBG, ERR, "chan set buffer failed! ret=0x%08x!!!!\n", ret);
	        //dvbcas_stb_drv_carequnlock();
	        return DVBCA_FAILED;
	    }
		
		memset(p_param.mask, 0, DMX_SECTION_FILTER_SIZE);
		memset(p_param.value, 0, DMX_SECTION_FILTER_SIZE);
		p_param.mask[0] = pucFilter->aucMask[0];
	    p_param.value[0] = pucFilter->aucFilter[0];
	    p_param.continuous = TRUE;
	    p_param.en_crc = 0;

	    if(pucFilter->ucLen > 1)
	    {
	      memcpy(&p_param.mask[1], &(pucFilter->aucMask[3]), pucFilter->ucLen-1);
	      memcpy(&p_param.value[1], &(pucFilter->aucFilter[3]), pucFilter->ucLen-1);
	    }
#ifdef FILTER_DEBUG
		DEBUG(DBG,INFO,"value:\n");
		DUMP(DBG,INFO,DMX_SECTION_FILTER_SIZE,p_param.value,"%x");
		
		DEBUG(DBG,INFO,"mask:\n");
		DUMP(DBG,INFO,DMX_SECTION_FILTER_SIZE,p_param.mask,"%x");
#endif
		
	    ret = dmx_si_chan_set_filter(p_dev, priv->ecm_filter[index].req_handle, &p_param);
		CHECK_RET(DBG,ret);

	    ret = dmx_chan_start(p_dev, priv->ecm_filter[index].req_handle);
	    if (SUCCESS != ret)
	    {
	        DEBUG(DBG, ERR, "chan start failed! ret=0x%08x!!!!\n", ret);
	        //dvbcas_stb_drv_carequnlock();
	        return DVBCA_FAILED;
	    }

	    priv->ecm_filter[index].pid = usPid;
	    memcpy(priv->ecm_filter[index].filter_data, &(pucFilter->aucFilter[index]), pucFilter->ucLen);
	    memcpy(priv->ecm_filter[index].filter_mask, &(pucFilter->aucMask[index]), pucFilter->ucLen);
	    priv->ecm_filter[index].start_ms = mtos_ticks_get();
		pucFilter++;
		priv->ecm_filter[index].valid = TRUE;
	    //dvbcas_stb_drv_carequnlock();
	}
    return DVBCA_SUCESS;
}
int DVBSTBCA_SetEmmFilter(IN ST_DVBCAFilter *pucFilter,							   
						   DVBCA_UINT8 ucFilterNum,					   
						   DVBCA_UINT16 usPid)
{
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
    int index = 0,i;
    s16 ret = ERR_FAILURE;	
	dmx_filter_setting_t p_param = {0};
    dmx_slot_setting_t p_slot = {0} ;
    dmx_device_t *p_dev = NULL;
	
	MT_ASSERT(pucFilter != NULL);
    DEBUG(DBG, INFO, "call in, usPid=0x%x, ucFilterNum = %d\n", 
		usPid,ucFilterNum);
	
	p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	MT_ASSERT(NULL != p_dev->p_base);
	dvbstb_free_emm_filter(p_dev);

	for (i= 0;i<DVBCA_FILTER_MAX_NUM && i<ucFilterNum;i++)
    {
		index = dvbstb_request_emm_filter();
		if (index < 0)
		{
			DEBUG(DBG,INFO,"request emm filter failed!\n");
			return DVBCA_FAILED;
		}

	    p_slot.in = DMX_INPUT_EXTERN0;
	    p_slot.pid = usPid;
	    p_slot.type = DMX_CH_TYPE_SECTION;
	    //dvbcas_stb_drv_careqlock();
	    ret = dmx_si_chan_open(p_dev, &p_slot, &(priv->emm_filter[index].req_handle));
	    if (SUCCESS != ret)
	    {
	        DEBUG(DBG, ERR, "open chan failed, ret=0x%08x!!!!\n", ret);
	        //dvbcas_stb_drv_carequnlock();
	        return DVBCA_FAILED;
	    }

	    priv->emm_filter[index].p_buf = dvbca_filter_buf[EMM][index];

	    ret = dmx_si_chan_set_buffer(p_dev, priv->emm_filter[index].req_handle, priv->emm_filter[index].p_buf, DVBCA_FILTER_BUFFER_SIZE);
	    if (SUCCESS != ret)
	    {
	        DEBUG(DBG, ERR, "chan set buffer failed! ret=0x%08x!!!!\n", ret);
	       // dvbcas_stb_drv_carequnlock();
	        return DVBCA_FAILED;
	    }

	    memset(p_param.mask, 0, DMX_SECTION_FILTER_SIZE);
	    memset(p_param.value, 0, DMX_SECTION_FILTER_SIZE);
	    p_param.mask[0] = pucFilter->aucMask[0];
	    p_param.value[0] = pucFilter->aucFilter[0];
	    p_param.continuous = TRUE;
	    p_param.en_crc = TRUE;
	    
	    if(pucFilter->ucLen > 1)
		{
			memcpy(&p_param.mask[1], &(pucFilter->aucMask[3]), pucFilter->ucLen-1);
			memcpy(&p_param.value[1], &(pucFilter->aucFilter[3]), pucFilter->ucLen-1);
		}
		
#ifdef FILTER_DEBUG		
		DEBUG(DBG,INFO,"value:\n");
		DUMP(DBG,INFO,DMX_SECTION_FILTER_SIZE,p_param.value,"%x");
		
		DEBUG(DBG,INFO,"mask:\n");
		DUMP(DBG,INFO,DMX_SECTION_FILTER_SIZE,p_param.mask,"%x");
#endif

	    ret = dmx_si_chan_set_filter(p_dev, priv->emm_filter[index].req_handle, &p_param);
	    CHECK_RET(DBG,ret);

	    ret = dmx_chan_start(p_dev, priv->emm_filter[index].req_handle);
	    if (SUCCESS != ret)
	    {
	        DEBUG(DBG, ERR, "chan start failed! ret=0x%08x!!!!\n", ret);
	        //dvbcas_stb_drv_carequnlock();
	        return DVBCA_FAILED;
	    }

	    priv->emm_filter[index].pid = usPid;
	    memcpy(priv->emm_filter[index].filter_data, &(pucFilter->aucFilter[index]), pucFilter->ucLen);
	    memcpy(priv->emm_filter[index].filter_mask, &(pucFilter->aucMask[index]), pucFilter->ucLen);
	    priv->emm_filter[index].start_ms = mtos_ticks_get();
	    DEBUG(DBG, INFO, "[DVBCA] ===emm filter data:index=%d,dvbca_filter[%d].pid=0x%x!\n",
	            index,index,priv->emm_filter[index].pid);
		pucFilter++;
		priv->emm_filter[index].valid = TRUE;
		//dvbcas_stb_drv_carequnlock();
	}
	DEBUG(DBG, INFO, "DVBSTBCA_SetEmmFilter OK\n");
	return DVBCA_SUCESS;
}

void DVBSTBCA_ScrSetCW(DVBCA_UINT16 usEcmPid,
					  IN DVBCA_UINT8 *pucOddKey,
					  IN DVBCA_UINT8 *pucEvenKey,
					  DVBCA_UINT8 ucKeyLen)
{
	
	u16 v_pid = 0, v_channel = 0;
	u16 a_pid = 0, a_channel = 0;
	RET_CODE ret ;
	dmx_device_t *p_dev = NULL;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;

	MT_ASSERT(priv != NULL);
	MT_ASSERT(pucOddKey != NULL);
	MT_ASSERT(pucEvenKey != NULL);
#ifdef FILTER_DEBUG
	DEBUG(DBG, INFO,"usEcmPid:%#x,pucKeyLen:%d\n",
		usEcmPid,ucKeyLen);
	DEBUG(DBG, INFO,
				"OddKey:0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\tEvenKey:0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\n"
	, pucOddKey[0], pucOddKey[1], pucOddKey[2], pucOddKey[3]
	, pucOddKey[4], pucOddKey[5], pucOddKey[6], pucOddKey[7]
	, pucEvenKey[0], pucEvenKey[1], pucEvenKey[2], pucEvenKey[3]
	, pucEvenKey[4], pucEvenKey[5], pucEvenKey[6], pucEvenKey[7]);
#endif

	p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	MT_ASSERT(NULL != p_dev->p_base);

	v_pid = priv->cur_v_pid;
	a_pid = priv->cur_a_pid;

	if (priv->used_chV != 0xffff)
	{
		ret = dmx_descrambler_onoff(p_dev, priv->used_chV, FALSE);
		priv->used_chV = 0xffff;
	}
	if (priv->used_chA != 0xffff)
	{
		ret = dmx_descrambler_onoff(p_dev, priv->used_chA, FALSE);
		priv->used_chA = 0xffff;
	}

	ret = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
	if (ret != SUCCESS)
	{
		DEBUG(DBG,ERR," get chan id by v_pid failed!!!!\n");
		return ;
	}

	ret = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
	if (ret != SUCCESS)
	{
		DEBUG(DBG,ERR," get chan id by a_pid failed!!!!\n");
		return ;
	}

	if ((a_channel != 0xffff) && (a_pid != 0x1fff))
	{
		ret = dmx_descrambler_onoff(p_dev,a_channel,TRUE);
		if (ret != SUCCESS)
		{
			DEBUG(DBG,ERR,"enable audio descrambler failed!!!!\n");
			//MT_ASSERT(0);
		}
		ret = dmx_descrambler_set_odd_keys(p_dev, a_channel, (u8 *)pucEvenKey, 8);
		CHECK_RET(DBG,ret);
		ret = dmx_descrambler_set_even_keys(p_dev, a_channel, (u8 *)pucOddKey, 8);
		CHECK_RET(DBG,ret);
	}
	else
	{
		DEBUG(DBG,INFO,"No audio prpgram playing now!!!!\n");
	}

	if (v_channel != 0xffff)
	{
		ret = dmx_descrambler_onoff(p_dev, v_channel, TRUE);
		if (ret != SUCCESS)
		{
			DEBUG(DBG,ERR,"enable video descrambler failed!!!!\n");
			//MT_ASSERT(0);
		}

		ret = dmx_descrambler_set_odd_keys(p_dev, v_channel, (u8 *)pucEvenKey, 8);
		CHECK_RET(DBG,ret);
		ret = dmx_descrambler_set_even_keys(p_dev, v_channel, (u8 *)pucOddKey, 8);
		CHECK_RET(DBG,ret);
	}
	else
	{
		DEBUG(DBG,INFO,"No video prpgram playing now!!!!\n");
	}

	priv->used_chV = v_channel;
	priv->used_chA = a_channel;

}



int DVBSTBCA_SCPBRun(IN DVBCA_UINT8 *pucCommand,
					DVBCA_UINT16 usCommandLen,
					OUT DVBCA_UINT8 *pucReply,
					OUT DVBCA_UINT16 *pusReplyLen,
					OUT DVBCA_UINT8 *pucStatusWord)
{
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	MT_ASSERT(priv!=NULL);
	scard_opt_desc_t opt_desc = {0};
	RET_CODE ret = ERR_FAILURE;
	s8 retry_times = 3;
	s8 index = 0;
	scard_atr_desc_t atr_desc = {0};
	static u8 card_update_flag = 1;

	MT_ASSERT(pucCommand != NULL);
	MT_ASSERT(usCommandLen != 0);
	MT_ASSERT(pucReply != NULL);
	MT_ASSERT(pusReplyLen != NULL);
	MT_ASSERT(pucStatusWord != NULL);

	//DEBUG(DBG, INFO, "call in!\n");

	//data_dump(pucCommand, (u32)usCommandLen);

	opt_desc.p_buf_out = pucCommand;
	opt_desc.bytes_out = usCommandLen;
	opt_desc.p_buf_in = pucReply;
	opt_desc.bytes_in_actual = 0;

	if(priv->reset_flag == 1)
	{
		mtos_task_lock();
		do
		{
			ret = scard_pro_rw_transaction(&opt_desc);
			if(ret == SUCCESS)
			{
//				DEBUG(DBG, INFO, "T0 transaction success!\n");
				break;
			}
			else
			{
				mtos_task_sleep(200);
				retry_times--;
			}
		}while (retry_times > 0);
		mtos_task_unlock();
	}

	if(ret == SUCCESS)
	{
		if(opt_desc.bytes_in_actual > 2)
		{
		  pucStatusWord[0] = pucReply[opt_desc.bytes_in_actual - 2];
		  pucStatusWord[1] = pucReply[opt_desc.bytes_in_actual - 1];
		  *pusReplyLen = opt_desc.bytes_in_actual - 2;
		}
		else if(opt_desc.bytes_in_actual == 2)
		{
		  pucStatusWord[0] = pucReply[0];
		  pucStatusWord[1] = pucReply[1];
		  *pusReplyLen = 0;
		}
		else
		{
		  DEBUG(DBG, INFO, "SendDataToSmartCard error! \n");
		  MT_ASSERT(0);
		  return DVBCA_FAILED;
		}

		//data_dump(opt_desc.p_buf_in, opt_desc.bytes_in_actual);

	}
	else
	{
		DEBUG(DBG, ERR, "T0 transaction failed!\n");
		//data_dump(pucCommand, usCommandLen);
		usCommandLen = 0;
		if(card_update_flag == 1)
		{
			for (index = 0; index < 3; index ++)
			{
				ret = scard_active(priv->p_smc_dev, &atr_desc);
				if (SUCCESS == ret)
				{
					DEBUG(DBG, INFO, "card reset ok: %d!\n", index);
		 			break;
				}
				else
				{
					DEBUG(DBG, INFO, "card reset failed!\n");
					mtos_task_sleep(200);
				}
			}
		}
		mtos_task_sleep(200);
		return DVBCA_FAILED;
	}
	return DVBCA_SUCESS;
}

void DVBSTBCA_ReadBuffer(DVBCA_UINT32 uiAddr, 
						OUT DVBCA_UINT8 *pucData,
						DVBCA_UINT32 uiLen)
{
#if 0
	u32 length = uiLen;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	u32 start_address = uiAddr + priv->task_priv.flash_start_add;
	void *p_dev = NULL;
	RET_CODE ret;
	
	MT_ASSERT(NULL != pucData);
	DEBUG(DBG, INFO, "call in!\n addr:%#x,read len:%d\n",uiAddr,uiLen);
	DEBUG(DBG, INFO, "start address :%#x",priv->task_priv.flash_start_add);

	p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
	MT_ASSERT(NULL != p_dev);

	//-------- param	identfy------------------
	if ((start_address + length) > 
		(priv->task_priv.flash_start_add + priv->task_priv.flash_size))
	{
		DEBUG(DBG, ERR, "Read flash out of range!\n");
		return;
	}
	
	//-------- param	identfy------------------
	if (0 != length)
	{
		ret = charsto_read(p_dev, (u32)start_address, (u8 *)pucData, length);
		if(SUCCESS != ret)
		{
			DEBUG(DBG, ERR, "Read data from nvram failed!\n");
		}
	}
	{
		int i;
		for (i=0;i<length&&i<10;i++)
			OS_PRINTF("%x,",pucData[i]);
		OS_PRINTF("\n");
	}
#else
	cas_adt_dvbca_priv_t *p_priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;

	OS_PRINTF(" DVBSTBCA_ReadBuffer uiAddr=%d   pucData[%x]   uiLen[%d]\n",uiAddr, pucData, uiLen);
	p_priv->nvram_read(uiAddr, (u8 *)pucData, (u32 *)&uiLen);
#endif

}

void DVBSTBCA_WriteBuffer(DVBCA_UINT32 uiAddr, 
						IN DVBCA_UINT8 *pucData,
						DVBCA_UINT32 uiLen)
{	

#if 1
	cas_adt_dvbca_priv_t *p_priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	
	OS_PRINTF(" DVBSTBCA_WriteBuffer uiAddr=%d   pucData[%x]   uiLen[%d]\n",uiAddr, pucData, uiLen);
	p_priv->nvram_write(uiAddr, (u8 *)pucData, uiLen);

#else
       u32 length = uiLen;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	u32 start_address = uiAddr + priv->task_priv.flash_start_add;
	void *p_dev = NULL;
	RET_CODE ret;
	
	MT_ASSERT(NULL != pucData);
	DEBUG(DBG, INFO, "call in!\n addr:%#x,read len:%d\n",uiAddr,uiLen);

	p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
	MT_ASSERT(NULL != p_dev);

	////-------- param	identfy------------------
	if ((start_address + length) > 
		(priv->task_priv.flash_start_add + priv->task_priv.flash_size))
	{
		DEBUG(DBG, ERR, "Write flash out of range!\n");
		return;
	}
	
	//-------- param	identfy------------------
	if (0 != length)
	{
		ret = charsto_writeonly(p_dev, (u32)start_address, (u8 *)pucData, length);
		if(SUCCESS != ret)
		{
			DEBUG(DBG, ERR, "Write data to nvram failed!\n");
		}
	}
	{
		int i;
		for (i=0;i<length&&i<10;i++)
			OS_PRINTF("%x,",pucData[i]);
		OS_PRINTF("\n");
	}
#endif
}

void DVBSTBCA_FormatBuffer(DVBCA_UINT32 uiAddr, DVBCA_UINT32 uiLen)
{

#if 1
	cas_adt_dvbca_priv_t *p_priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	
	p_priv->nvram_erase(uiAddr, uiLen);

        OS_PRINTF(" DVBSTBCA_FormatBuffer uiAddr=%d  uiLen[%d]\n",uiAddr,uiLen);
#else
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	void *p_dev = NULL;
	RET_CODE ret;
	DEBUG(DBG, INFO, "call in!\n addr:%#x,read len:%d\n",uiAddr,uiLen);

	p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
	MT_ASSERT(NULL != p_dev);

	ret = charsto_erase(p_dev, priv->task_priv.flash_start_add, 1);
	if(ret != SUCCESS)
	{
		DEBUG(DBG, ERR, "format buffer error!\n");
	}
#endif
}

void DVBSTBCA_ShowPPCMessage(DVBCA_UINT16 usEcmPid, DVBCA_UINT8 ucDay)
{
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	DEBUG(DBG,INFO,"call in usEcmPid = %x,ucDay = %d\n",usEcmPid,ucDay);
	if(priv->cur_ecm_pid == usEcmPid)
	{
		if(ucDay < DVBSTBCA_SHOWPPCDAYS)
		{
			priv->expiring_days = ucDay;
			cas_send_event(priv->slot, CAS_ID_DVBCA, CAS_E_AUTHEN_RECENT_EXPIRE, (u32)ucDay);
		}
	}
}

const char * _dvbcas_e2s_msg(DVBCA_MESSAGE msg)
{
	switch(msg)
	{
		case DVBCA_MESSAGE_INSERTCARD:/*加扰节目，请插入智能卡*/
			return "CAS_E_CARD_REGU_INSERT";
		case DVBCA_MESSAGE_BADCARD:/*无法识别卡，不能使用*/
			return "CAS_E_ILLEGAL_CARD";
		case DVBCA_MESSAGE_NOENTITLE:/*没有授权*/
			return "CAS_E_PROG_UNAUTH";
		case DVBCA_MESSAGE_NOOPER_TYPE:/*卡中不存在节目运营商*/
			return "CAS_E_NO_OPTRATER_INFO";
		case DVBCA_MESSAGE_BLOCK_AREA:/*区域禁播*/
			return "CAS_E_ZONE_DISPLAY";
		case DVBCA_MESSAGE_EXPIRED:/*授权过期*/
			return "CAS_E_AUTHEN_EXPIRED";
		case DVBCA_MESSAGE_PASSWORD_ERROR:/*运营商密码错误*/
			return "CAS_E_PASSWORD_ERROR";
		case DVBCA_MESSAGE_ERRREGION:/*区域不正确*/
			return "CAS_E_ZONE_CODE_ERR";
		case DVBCA_MESSAGE_PAIRING_ERROR:/*机卡不匹配*/
			return "CAS_E_CARD_NOT_FOUND";
		case DVBCA_MESSAGE_WATCHLEVEL:/*节目级别高于设定观看级别*/
			return "CAS_E_CARD_CHILD_UNLOCK";
		case DVBCA_MESSAGE_EXPIRED_CARD:/*智能卡过期*/
			return "CAS_E_CARD_OVERDUE";
		case DVBCA_MESSAGE_DECRYPTFAIL:/*节目解密失败*/	
			return "CAS_E_CW_INVALID";
		case DVBCA_MESSAGE_WORKTIME:/*不在工作时段内*/
			return "CAS_E_CARD_OUT_WORK_TIME";
			break;
		default:
			return "__ERROR";
	     	break;
	}
}
void DVBSTBCA_ShowMessage(DVBCA_UINT16 usEcmPid, 
						 DVBCA_UINT8 ucMessageType)
{
	u32 event = 0;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	MT_ASSERT(priv != NULL);
	DEBUG(DBG,INFO,"-------------------usECMPID = %d--------ucMessageType = %d---------------------\n",usEcmPid,ucMessageType);
	DEBUG(DBG,INFO,"priv->cur_ecm_pid = %d>>>>>>\n",priv->cur_ecm_pid);
	DEBUG(DBG,INFO,"EcmPid:%#x msg[%s]\n",usEcmPid,_dvbcas_e2s_msg(ucMessageType/*usEcmPid*/));
	if(priv->cur_ecm_pid == usEcmPid)
	{
			switch(ucMessageType)
			{
			case DVBCA_MESSAGE_INSERTCARD:/*加扰节目，请插入智能卡*/
				event = CAS_E_CARD_REGU_INSERT;
				break;
			case DVBCA_MESSAGE_BADCARD:/*无法识别卡，不能使用*/
				event = CAS_E_ILLEGAL_CARD;
				break;
			case DVBCA_MESSAGE_NOENTITLE:/*没有授权*/
				event = CAS_E_PROG_UNAUTH;
				break;
			case DVBCA_MESSAGE_NOOPER_TYPE:/*卡中不存在节目运营商*/
				event = CAS_E_NO_OPTRATER_INFO;
				break;
			case DVBCA_MESSAGE_BLOCK_AREA:/*区域禁播*/
				event = CAS_E_ZONE_DISPLAY;
				break;
			case DVBCA_MESSAGE_EXPIRED:/*授权过期*/
				event = CAS_E_AUTHEN_EXPIRED;
				break;
			case DVBCA_MESSAGE_PASSWORD_ERROR:/*运营商密码错误*/
				event = CAS_E_PASSWORD_ERROR;
				break;
			case DVBCA_MESSAGE_ERRREGION:/*区域不正确*/
				event = CAS_E_ZONE_CODE_ERR;
				break;
			case DVBCA_MESSAGE_PAIRING_ERROR:/*机卡不匹配*/
				event = CAS_E_CARD_PAIROTHER;
				break;
			case DVBCA_MESSAGE_WATCHLEVEL:/*节目级别高于设定观看级别*/
				event = CAS_E_CARD_CHILD_UNLOCK;
				break;
			case DVBCA_MESSAGE_EXPIRED_CARD:/*智能卡过期*/
				event = CAS_E_CARD_OVERDUE;
				break;
			case DVBCA_MESSAGE_DECRYPTFAIL:/*节目解密失败*/	
				event = CAS_E_CW_INVALID;
				break;
			case DVBCA_MESSAGE_WORKTIME:/*不在工作时段内*/
				event = CAS_E_CARD_OUT_WORK_TIME;
				break;
			default:
				DEBUG(DBG,INFO," ucMessageType :%s !\n", _dvbcas_e2s_msg(ucMessageType));
				MT_ASSERT(0);
	          	break;
			}
	}
	cas_send_event(priv->slot, CAS_ID_DVBCA, event, 0);
}

void DVBSTBCA_UpgradeFreq(ST_DVBCA_FREQ *pstFreq, unsigned char ucFreqSum)
{
       u8 i;
	u32 event = 0;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	MT_ASSERT(priv != NULL);
	DEBUG(DBG, INFO, "ucFreqSum:%d\n", ucFreqSum);
	DEBUG(DBG, INFO, "research prog\n");

      priv->upgrade_freq.upgrade_freq_num = ucFreqSum;
      for(i = 0; i< ucFreqSum; i++)
      {
            DEBUG(DBG, INFO, "ucModunation:%d\n", pstFreq[i].ucModunation);
            DEBUG(DBG, INFO, "uiFrequency:%d\n", pstFreq[i].uiFrequency);
            DEBUG(DBG, INFO, "uiSymbolrate:%d\n", pstFreq[i].uiSymbolrate);
      
            priv->upgrade_freq.upgrade_freq_info[i].ucModunation = pstFreq[i].ucModunation;
            priv->upgrade_freq.upgrade_freq_info[i].uiFrequency   = pstFreq[i].uiFrequency;
            priv->upgrade_freq.upgrade_freq_info[i].uiSymbolrate  = pstFreq[i].uiSymbolrate;
      }
        
	event = CAS_UPGRADEFREQ;
	cas_send_event(priv->slot, CAS_ID_DVBCA, event, (u32)&priv->upgrade_freq);
}

void DVBSTBCA_HideMessage(DVBCA_UINT16 usEcmPid, 
						 DVBCA_UINT8 ucMessageType)
{
	u32 event = 0;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	MT_ASSERT(priv != NULL);	DEBUG(DBG,INFO,"-------------------usEcmPid = %d-------ucMessageType = %d---------------------------------!\n",usEcmPid,ucMessageType);
	if(priv->cur_ecm_pid == usEcmPid)
	{
		switch(ucMessageType)
		{
			case DVBCA_MESSAGE_INSERTCARD:/*加扰节目，请插入智能卡*/
			case DVBCA_MESSAGE_BADCARD:/*无法识别卡，不能使用*/
			case DVBCA_MESSAGE_NOENTITLE:/*没有授权*/
			case DVBCA_MESSAGE_NOOPER_TYPE:/*卡中不存在节目运营商*/
			case DVBCA_MESSAGE_BLOCK_AREA:/*区域禁播*/
			case DVBCA_MESSAGE_EXPIRED:/*授权过期*/
			case DVBCA_MESSAGE_PASSWORD_ERROR:/*运营商密码错误*/
			case DVBCA_MESSAGE_ERRREGION:/*区域不正确*/
			case DVBCA_MESSAGE_PAIRING_ERROR:/*机卡不匹配*/
			case DVBCA_MESSAGE_WATCHLEVEL:/*节目级别高于设定观看级别*/
			case DVBCA_MESSAGE_EXPIRED_CARD:/*智能卡过期*/
			case DVBCA_MESSAGE_DECRYPTFAIL:/*节目解密失败*/	
			case DVBCA_MESSAGE_WORKTIME:/*不在工作时段内*/
				event = CAS_S_CLEAR_DISPLAY;
				break;
			default:
			  	break;
		}
	}
	cas_send_event(priv->slot, CAS_ID_DVBCA, event, 0);
	cas_send_event(priv->slot, CAS_ID_DVBCA, CAS_S_CLEAR_DISPLAY, 0);
}
void DVBSTBCA_ShowFingerMessage(DVBCA_UINT16 usEcmPid, 
						 DVBCA_UINT32 uiCardID)
{   
	u8 WHITE[4]= {0xff,0xff,0xff,0xff};
	u8 BLACK[4]= {0x00,0x00,0x00,0x00};
	
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	DEBUG(DBG,INFO," SHOW FINGER MESSAGE :call in usEcmPid = %x,ucDay = %x\n",
				usEcmPid,uiCardID);
	if(priv->cur_ecm_pid == usEcmPid)
	{
		dvbcas_stb_drv_careqlock();
		memcpy(priv->dvbca_msgFinger.show_color,WHITE, 4);
		memcpy(priv->dvbca_msgFinger.show_color_back, BLACK, 4);
		priv->dvbca_msgFinger.show_postion = 0;
		priv->dvbca_msgFinger.show_postion_x= 450;
		priv->dvbca_msgFinger.show_postion_y = 500;
		sprintf((char *)priv->dvbca_msgFinger.data,"%x",uiCardID);
		//memcpy(priv->dvbca_msgFinger.data,&uiCardID,sizeof(uiCardID));
		cas_send_event(priv->slot, CAS_ID_DVBCA, CAS_C_SHOW_NEW_FINGER, 0);
		dvbcas_stb_drv_carequnlock();
		DEBUG(DBG,INFO,"FINGER set success>>>>\n");
	}
}
void DVBSTBCA_HideFingerMessage(void)
{
	u32 event = 0;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	DEBUG(DBG,INFO,"HIDE FINGER MASSAGE\n");
	event = CAS_C_HIDE_NEW_FINGER;
	cas_send_event(priv->slot, CAS_ID_DVBCA, event, 0);
}
void DVBSTBCA_NewEmailNotify(DVBCA_UINT16 usEmailID)
{
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	DEBUG(DBG,INFO,"NEW email notify");
	if(usEmailID != 0)
	{
		cas_send_event(priv->slot,CAS_ID_DVBCA,CAS_C_SHOW_NEW_EMAIL,0);
	}
	else
	{
		cas_send_event(priv->slot,CAS_ID_DVBCA,CAS_C_HIDE_NEW_EMAIL,0);
	}
}

void DVBSTBCA_HideUrgent(void)
{
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	DEBUG(DBG,INFO,"DVBSTBCA_HideUrgent");
	cas_send_event(priv->slot,CAS_ID_DVBCA,CAS_C_HIDE_URGENT,0);	
}

//紧急通告暂不处理
void DVBSTBCA_ShowUrgent(IN DVBCA_UINT8 *pucTitle,IN DVBCA_UINT8 *pucContent)
{
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	DEBUG(DBG,INFO,"DVBSTBCA_ShowUrgent");
	dvbcas_stb_drv_careqlock();
	memcpy(priv->urgent.Title,pucTitle, strlen((char*)pucTitle));
	memcpy(priv->urgent.Content,pucContent, strlen((char*)pucContent));
	dvbcas_stb_drv_carequnlock();
	cas_send_event(priv->slot,CAS_ID_DVBCA,CAS_C_SHOW_URGENT,0);
}

void DVBSTBCA_ShowOSDMessage(IN ST_DVBCA_MSG *pstAdvertize)
{
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	MT_ASSERT(pstAdvertize != NULL);
	DEBUG(DBG,INFO,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>show osd message\n");
	DEBUG(DBG,INFO,"msg1: %s\n",pstAdvertize->aucText);
	int ret;
	ret = DVBCASTB_GetCardID((DVBCA_UINT8 *)old_card_id);
	if(ret != 0)
	{
		DEBUG(DBG,INFO,"get old card id failed!!!\n");
	}
	else
	{
		DEBUG(DBG,INFO,"new card id = %s\n",old_card_id);
	}
	dvbcas_stb_drv_careqlock();
	priv->dvbca_OsdMsg.display_pos = pstAdvertize->ucDispPos;
/*	if(pstAdvertize->ucDispPos == 1)
		priv->dvbca_OsdMsg.display_pos = 0;
	else if(pstAdvertize->ucDispPos == 0)
		priv->dvbca_OsdMsg.display_pos = 1;*/
	priv->dvbca_OsdMsg.display_pos = 0;//not smooth when two osd roll.
	priv->dvbca_OsdMsg.osd_display.roll_value = pstAdvertize->ucDispCount;
	priv->dvbca_OsdMsg.display_time = pstAdvertize->ucIntervalTime;
	DEBUG(DBG,INFO,"pstAdvertize->ucDispPos = %d\n",pstAdvertize->ucDispPos);
	DEBUG(DBG,INFO,"pstAdvertize->ucDispCount = %d\n",pstAdvertize->ucDispCount);
	DEBUG(DBG,INFO,"pstAdvertize->ucDispWay = %d\n",pstAdvertize->ucDispWay);
	DEBUG(DBG,INFO,"pstAdvertize->ucDispDirection = %d\n",pstAdvertize->ucDispDirection);
	DEBUG(DBG,INFO,"pstAdvertize->ucIntervalTime = %d\n",pstAdvertize->ucIntervalTime);
    DEBUG(DBG,INFO,"display_back_color r[%d] g[%d] b[%d]  a[%d] \n",pstAdvertize->stBgColor.ucRed,
        pstAdvertize->stBgColor.ucGreen,pstAdvertize->stBgColor.ucBlue,pstAdvertize->stBgColor.ucAlpha);
	priv->dvbca_OsdMsg.display_back_color[R_INDEX] = pstAdvertize->stBgColor.ucRed;
	priv->dvbca_OsdMsg.display_back_color[G_INDEX] = pstAdvertize->stBgColor.ucGreen;
	priv->dvbca_OsdMsg.display_back_color[B_INDEX] = pstAdvertize->stBgColor.ucBlue;
	priv->dvbca_OsdMsg.display_back_color[A_INDEX] = pstAdvertize->stBgColor.ucAlpha;


	priv->dvbca_OsdMsg.display_color[R_INDEX] = pstAdvertize->stFontColor.ucRed;
	priv->dvbca_OsdMsg.display_color[G_INDEX] = pstAdvertize->stFontColor.ucGreen;
	priv->dvbca_OsdMsg.display_color[B_INDEX] = pstAdvertize->stFontColor.ucBlue;
	priv->dvbca_OsdMsg.display_color[A_INDEX] = pstAdvertize->stFontColor.ucAlpha;

	priv->dvbca_OsdMsg.content_len = strlen((char*)pstAdvertize->aucText) + 1;
	memcpy((char *)priv->dvbca_OsdMsg.data, (char*)pstAdvertize->aucText, priv->dvbca_OsdMsg.content_len);
	dvbcas_stb_drv_carequnlock();
	cas_send_event(priv->slot, CAS_ID_DVBCA, CAS_C_SHOW_OSD, 0);
    DEBUG(DBG,INFO,"show osd message>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

void DVBSTBCA_HideOSDMessage(void)
{
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	DEBUG(DBG,INFO,"HIDE OSD MESSAGE\n");
	cas_send_event(priv->slot, CAS_ID_DVBCA, CAS_C_HIDE_OSD_UP, 0);
}

//回复OSD 显示暂不处理
void DVBSTBCA_ResumeOSDMessage(void)
{
	int ret;
	ret = DVBCASTB_GetCardID((DVBCA_UINT8 *)new_card_id);
	if(ret != 0)
	{
		DEBUG(DBG,INFO,"get new card id failed!!!\n");
	}
	else
	{
		DEBUG(DBG,INFO,"new card id = %s\n",new_card_id);
	}
	if(0 == strcmp((char *)old_card_id, (char *)new_card_id))
	{
		cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	    DEBUG(DBG,INFO,"DVBSTBCA_ResumeOSDMessage\n");
		cas_send_event(priv->slot, CAS_ID_DVBCA, CAS_C_RESUME_OSD, 0);
	}	
}

int DVBSTBCA_RegisterTask(IN const char *szName,
						 DVBCA_UINT8 ucPriority,
						 IN void *pTaskFun,
						 IN void *pParam,
						 DVBCA_UINT16 usStackSize)
{
	u8 ret = 0;
	u8 uIndex = 0;
	u32 byPriority = 0;
	u32 *p_stack = NULL;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	MT_ASSERT(priv != NULL);
	MT_ASSERT(NULL != szName);
	MT_ASSERT(NULL != pTaskFun);

	DEBUG(DBG, INFO, 
		":\nName[%s], Priority[%d],size[%d]\n", szName, ucPriority,usStackSize);

	for(uIndex = 0; uIndex < DVBCA_CAS_TASK_MAX; uIndex++)
	{
		if(0 == priv->task_record[uIndex].is_busy)
		{
			priv->task_record[uIndex].is_busy= 1;
			break;
		}
	}

	if(uIndex >= DVBCA_CAS_TASK_MAX)
	{
		DEBUG(DBG, INFO, "too many task or invalid task priority!\n");
		return DVBCA_FAILED;
	}

	if (0 != priv->task_priv.task_prio)
	{
		byPriority = priv->task_priv.task_prio;
	}
	else
	{
		byPriority = DVBCA_TASK_PRIORITY_0;
	}

	priv->task_record[uIndex].task_prio = byPriority + uIndex;

	p_stack = (u32 *)mtos_malloc(usStackSize);
	MT_ASSERT(NULL != p_stack);

	ret = mtos_task_create((u8 *)szName,
		(void *)pTaskFun,
		pParam,
		priv->task_record[uIndex].task_prio,
		p_stack,
		usStackSize);

	if (!ret)
	{
		DEBUG(DBG, INFO, "[DVBCA]create task error = 0x%08x!\n", ret);
		priv->task_record[uIndex].is_busy = 0;
		return DVBCA_FAILED;
	}

	DEBUG(DBG, INFO,"[DVBCA]create task success!Priority[%d]\n", priv->task_record[uIndex].task_prio);
	return DVBCA_SUCESS;
}/*DVBSTBCA_RegisterTask*/

void DVBSTBCA_Sleep(DVBCA_UINT16 usMilliSeconds)
{
	//DEBUG(DBG,INFO, "[DVBCAS]DVBSTBCA_Sleep:%dms\n", usMilliSeconds);
	mtos_task_sleep(usMilliSeconds);
}/*DVBSTBCA_Sleep*/

void DVBSTBCA_SemaphoreInit(OUT DVBCA_SEMAPHORE *pSemaphore,
						   DVBCA_UINT32 uiInitValue)
{
	DVBCA_UINT32 i;
	os_sem_t p_sem;
	DEBUG(DBG,INFO, "call in! uiInitValue = %d\n", uiInitValue);

	if(TRUE != mtos_sem_create(&p_sem, 0))
	{
		DEBUG(DBG,INFO, "init semphore failed!\n");
		MT_ASSERT(0);
		return;
	}
	for (i=0;i<uiInitValue;i++)
	{
		if(TRUE != mtos_sem_give(&p_sem))
		{
			DEBUG(DBG,INFO, "Signal semphore failed!\n");
			MT_ASSERT(0);
			return;
		}
	}
	*pSemaphore = (DVBCA_SEMAPHORE)p_sem;
}/*DVBSTBCA_SemaphoreInit*/

void DVBSTBCA_SemaphoreSignal(INOUT DVBCA_SEMAPHORE *pSemaphore)
{
	MT_ASSERT(pSemaphore != NULL);
	if(TRUE != mtos_sem_give((os_sem_t *)pSemaphore))
	{
		DEBUG(DBG,INFO, "Signal semphore failed!\n");
	}
}/*DVBSTBCA_SemaphoreSignal*/

void DVBSTBCA_SemaphoreWait(INOUT DVBCA_SEMAPHORE *pSemaphore)
{
	MT_ASSERT(pSemaphore != NULL);
	if(TRUE != mtos_sem_take((os_sem_t *)pSemaphore, 0))
	{
		DEBUG(DBG,INFO, "waite semphore failed!\n");
	}
}/*DVBSTBCA_SemaphoreWait*/

void DVBSTBCA_SemaphoreDestory(INOUT DVBCA_SEMAPHORE *pSemaphore)
{
	MT_ASSERT(pSemaphore != NULL);
	if(TRUE != mtos_sem_destroy((os_sem_t *)pSemaphore, MTOS_DEL_ALWAYS))
	{
		DEBUG(DBG,INFO, "[DVBCAS]CAS destory semphore failed!\n");
	}
}/*DVBSTBCA_SemaphoreDestory*/

void *DVBSTBCA_Malloc(DVBCA_UINT32 uiBufSize)
{
	void *p_buf = NULL;
	DEBUG(DBG,INFO,"call in size = %d\n",uiBufSize);
	p_buf = mtos_malloc(uiBufSize);
	MT_ASSERT(NULL != p_buf);
	DEBUG(DBG,INFO,"finish!\n");	
	return p_buf;
}/*DVBSTBCA_Malloc*/

void DVBSTBCA_Free(void *pBuf)
{
	DEBUG(DBG,INFO,"call in\n");
	mtos_free(pBuf);
	DEBUG(DBG,INFO,"finish!\n");	
	
}/*DVBSTBCA_Free*/

void DVBSTBCA_Memset(void *pDestBuf, DVBCA_UINT8 ucValue, DVBCA_UINT32 uiSize)
{
	memset((char*)pDestBuf,ucValue,uiSize);
}/*DVBSTBCA_Memset*/

void DVBSTBCA_Memcpy(void *pDestBuf, void *pSrcBuf, DVBCA_UINT32 uiSize)
{
    memcpy(pDestBuf, pSrcBuf, uiSize);
}/*DVBSTBCA_Memcpy*/

DVBCA_UINT32 DVBSTBCA_Strlen(char *pString)
{
	return strlen((const char*)pString);
}/*DVBSTBCA_Strlen*/

typedef char    *ck_va_list;
typedef unsigned int  CK_NATIVE_INT;
typedef unsigned int  ck_size_t;
#define CK_AUPBND         	  (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);

void DVBSTBCA_Printf(IN const char *fmt,...)
{	
	ck_va_list p_args = NULL;
	unsigned int  printed_len = 0;
	char    printk_buf[200];

	CK_VA_START(p_args, fmt);
	printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)fmt, p_args);

	CK_VA_END(p_args);
	OS_PRINTF(printk_buf);
	OS_PRINTF("\n");

}/*DVBSTBCA_Printf*/

void dvbca_stb_drv_ca_data_monitor(void *pData)
{
    int i = 0;
    s32 nRet = SUCCESS;
    u8 *p_GotData = NULL;	
    u32 uDataLen = 0;
    dmx_device_t *p_dev = NULL;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	static u16 pre_ecm_pid = 0xffff;
	DEBUG(DBG,INFO,"----------------call in!\n");
    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

	while(1)
	{
		//DEBUG(DBG,INFO,"--------------------delay_diver = %d----------------\n",delay_diver);
		if (SMC_CARD_REMOVE == priv->card_status )
		{
		//	OS_PRINTF("priv->card_status = %d\n",priv->card_status);
		//	OS_PRINTF("priv->cur_ecm_pid = %d\n",priv->cur_ecm_pid);
		//	OS_PRINTF("pre_ecm_pid = %d\n",pre_ecm_pid);
			
			if (priv->cur_ecm_pid != pre_ecm_pid
				&& priv->cur_ecm_pid != 0)
			{
			
				OS_PRINTF("CAS_ID_DVBCA = %d\n");
				cas_send_event(priv->slot, 
					CAS_ID_DVBCA, CAS_E_CARD_REGU_INSERT, 0);
			}
			pre_ecm_pid = priv->cur_ecm_pid;	
		}
		for(i=0;i<DVBCA_FILTER_MAX_NUM;i++)
		{
	        //dvbcas_stb_drv_careqlock();
	        if (priv->ecm_filter[i].valid)
	        {
		        nRet = dmx_si_chan_get_data(p_dev, priv->ecm_filter[i].req_handle,
		                    &p_GotData, &uDataLen);
				if (nRet == SUCCESS)
				{
				#ifdef FILTER_DEBUG
					DEBUG(DBG,INFO,"priv->ecm_filter[%d].pid=%x,uDataLen:%d\n", 
						i, priv->ecm_filter[i].pid,uDataLen);
					DUMP(DBG,INFO,16,p_GotData,"%x");
				#endif
					DVBCASTB_PrivateDataGot(priv->ecm_filter[i].pid,
						p_GotData,uDataLen);
				}
				else
				{
					//DEBUG(DBG, ERR, "get ecm_filter[%d] data failed!\n", i);
				}
	        }
			//dvbcas_stb_drv_carequnlock();
		}
		for(i=0;i<DVBCA_FILTER_MAX_NUM;i++)
		{
	        //dvbcas_stb_drv_careqlock();
	        if (priv->emm_filter[i].valid)
	        {
		        nRet = dmx_si_chan_get_data(p_dev, priv->emm_filter[i].req_handle,
		                    &p_GotData, &uDataLen);
				if (nRet == SUCCESS)
				{
				#ifdef FILTER_DEBUG
					DEBUG(DBG,INFO,"priv->emm_filter[%d].pid=%x,uDataLen:%d\n", 
						i, priv->emm_filter[i].pid,uDataLen);
					DUMP(DBG,INFO,16,p_GotData,"%x");
				#endif
					DVBCASTB_PrivateDataGot(priv->emm_filter[i].pid,
						p_GotData,uDataLen);
				}
				else
				{
					//DEBUG(DBG, ERR, "get emm_filter[%d] data failed!\n", i);
				}
	        }
			//dvbcas_stb_drv_carequnlock();
		}
		mtos_task_sleep(20);
	}

	DEBUG(DBG,INFO,"dvbca_stb_drv_ca_data_monitor>>>>>>>>>>>>>>>>>>>>>>>>>>>>!\n");
}

RET_CODE dvbca_stb_drv_adt_init(void)
{
	u32 *p_stack = NULL;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	BOOL nRet = 0;
	u8 task_name[] = "dvbca_stb_drv_ca_data_monitor";

	DEBUG(DBG,INFO,"call in!\n");
	
	p_stack = (u32 *)mtos_malloc(DVBCA_SECTION_TASK_STACK_SIZE);
	MT_ASSERT(NULL != p_stack);

	if(priv->monitor_priv.task_prio_monitor == 0)
		nRet = mtos_task_create(task_name,
				dvbca_stb_drv_ca_data_monitor,
				(void *)0,
				DVBCA_CAS_SECTION_TASK_PRIO,
				p_stack,
				DVBCA_SECTION_TASK_STACK_SIZE);
	else
		nRet = mtos_task_create(task_name,
				dvbca_stb_drv_ca_data_monitor,
				(void *)0,
				priv->monitor_priv.task_prio_monitor,
				p_stack,
				DVBCA_SECTION_TASK_STACK_SIZE);

	if (nRet == FALSE)
	{
		DEBUG(DBG,INFO,"Create data monitor task error = 0x%08x!\n", nRet);
		return ERR_FAILURE;
	}

	DEBUG(DBG,INFO,"****dvbca_stb_drv_adt_Init ok!\n");
	return SUCCESS;
}

