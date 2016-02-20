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

#include "gs_ca_process.h"

#include "smc_op.h"
#include "smc_pro.h"

#include "lib_util.h"
#include "sys_dbg.h"

#define CAS_GS_DEBUG

#ifdef CAS_GS_DEBUG
#define CAS_GS_PRINTF OS_PRINTF
#else
#define CAS_GS_PRINTF DUMMY_PRINTF
#endif

#define GS_FILTER_DEPTH_SIZE          8
#define GS_FILTER_MAX_NUM              2
#define GS_CAS_SECTION_TASK_STACK_SIZE    (16 * 1024)
#define GS_CAS_SECTION_TASK_PRIO        48//32
#define GS_FILTER_BUFFER_SIZE (16 * (KBYTES) + 188)


//#define MEMTEST
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
  } gs_filter_status_t;


  typedef struct
  {
  /*!
  abc
    */
  gs_filter_status_t  m_FilterStatus;
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
  u8  m_uFilterData[GS_FILTER_DEPTH_SIZE];
  /*!
  abc
    */
  u8  m_uFilterMask[GS_FILTER_DEPTH_SIZE];
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
  }gs_filter_struct_t;

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
#ifdef MEMTEST
  u8  *p_Data;
#else
  u8  p_Data[1024];
#endif
  /*!
  abc
    */
  u32 m_uLength;
  }gs_data_got_t;



static gs_filter_struct_t s_tFilter[GS_FILTER_MAX_NUM] = {{0,},};
static gs_data_got_t  s_gs_tData[GS_FILTER_MAX_NUM] = {{0,},};
static u8 s_tFilterBuf[GS_FILTER_MAX_NUM][GS_FILTER_BUFFER_SIZE];
static u32 u32_CaReqLock = 0;
static u8 s_uScStatus = SC_STATUS_END;
static utc_time_t g_GS_CA_TIME = {0};
extern unsigned char  gs_cw_mask[8];
char  gs_cw[16];
extern void _Gs_EcmDecrypt(unsigned short sw1sw2);
static u16 crc_old_ecm = 0;
static u16 crc_old_osd = 0;
static u16 crc_old_mail = 0;
static u16 crc_old_emm = 0;
static u16 v_channel = 0xffff;
static u16 a_channel = 0xffff;
static u16 old_v_channel = 0xffff;
static u16 old_a_channel = 0xffff;
extern u16 cas_gs_get_v_pid(void);
extern u16 cas_gs_get_a_pid(void);

extern u8 gs_ca_nit_state;
extern u8 gs_ca_nit_flag;

extern u16 gs_ca_area_code;
extern u16 gs_card_area_code;

extern u8 GSSTBCA_SetStreamGuardFilter(u8 byReqID,u16 wPID,
                                    u8 *szFilter,u8 *szMask,u8 byLen,u8 nWaitSecs);
/*!
  abc
  */
void _Gs_CaReqLock(void)
{
  mtos_sem_take((os_sem_t *)&u32_CaReqLock, 0);
}

/*!
  abc
  */
void _Gs_CaReqUnLock(void)
{
  mtos_sem_give((os_sem_t *)&u32_CaReqLock);
}

/*!
  abc
  */
unsigned char GS_apdu_package_v1(
  unsigned char *cache,
  unsigned char cla,
  unsigned char ins,
  unsigned char p1,
  unsigned char p2)
{
  mtos_task_delay_ms(5);
  cache[0] = cla;
  cache[1] = ins;
  cache[2] = p1;
  cache[3] = p2;
  return 4;
}

/*!
  abc
  */
unsigned char GS_apdu_package_v2(
  unsigned char *cache,
  unsigned char cla,
  unsigned char ins,
  unsigned char p1,
  unsigned char p2,
  unsigned char le)
{
  mtos_task_delay_ms(5);
  cache[0] = cla;
  cache[1] = ins;
  cache[2] = p1;
  cache[3] = p2;
  cache[4] = le;
  return 5;
}

/*!
  abc
  */
unsigned char GS_apdu_package_v3(
  unsigned char *cache,
  unsigned char cla,
  unsigned char ins,
  unsigned char p1,
  unsigned char p2,
  unsigned char lc,
  unsigned char *input_data)
{
  unsigned char i = 0;
  mtos_task_delay_ms(5);  
  cache[0] = cla;
  cache[1] = ins;
  cache[2] = p1;
  cache[3] = p2;
  cache[4] = lc;
  for(i = 0; i < lc; ++ i)
  {
    cache[5 + i] = input_data[i];
  }
  return 5 + lc;
}

/*!
  abc
  */
unsigned char GS_apdu_package_v4(
  unsigned char *cache,
  unsigned char cla,
  unsigned char ins,
  unsigned char p1,
  unsigned char p2,
  unsigned char lc,
  unsigned char *input_data)
{
  unsigned char i = 0;
  mtos_task_delay_ms(5);  
  cache[0] = cla;
  cache[1] = ins;
  cache[2] = p1;
  cache[3] = p2;
  cache[4] = lc;
  for(i = 0; i < lc; ++ i)
  {
    cache[5 + i] = input_data[i];
  }
  return 5 + lc;
}

/*!
  abc
  */
unsigned short GS_crc16ccitt(unsigned char *pBuffer, unsigned long uBufSize)
{
  unsigned short uCcitt16 = 0xFFFF;
  unsigned long i = 0;

  for(i = 0; i < uBufSize; i ++)
  {
    uCcitt16 = (uCcitt16 >> 8) | (uCcitt16 << 8);
    uCcitt16 ^= pBuffer[i];
    uCcitt16 ^= (uCcitt16 & 0xFF) >> 4;
    uCcitt16 ^= (uCcitt16 << 8) << 4;
    uCcitt16 ^= ((uCcitt16 & 0xFF) << 4) << 1;
  }
  return ~uCcitt16;
}

/*!
  abc
  */
int GS_serial_read(unsigned char *cache, unsigned int length,
                        unsigned int *byte_readed, unsigned int timeout)
{
  int err = 0;
  scard_device_t *p_smc_dev = NULL;
  p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
  MT_ASSERT(NULL != p_smc_dev);

  err = scard_read(p_smc_dev , (u8 *)cache, (u32)length, (u32 *)byte_readed);
#ifdef CAS_GS_DEBUG
  {
	int i;
	OS_PRINTF("[GS]APDU_READ: ");
	for(i=0; i<*byte_readed; i++)
	{
		OS_PRINTF("%02x\t",cache[i]);
		if((i%8 == 0) && (i!=0))
			OS_PRINTF("\n");
	}
	OS_PRINTF(" \n");
  }
#endif
  return err;
}

/*!
  abc
  */
unsigned char GS_serial_write(unsigned char *cache, unsigned int length,
                    unsigned int *byte_written, unsigned int timeout)
{
  unsigned char err = 0;
  scard_device_t *p_smc_dev = NULL;
  p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
  MT_ASSERT(NULL != p_smc_dev);
  //err = UART_Write(handle_pc, cache, length, byte_written, timeout);
  err = scard_write(p_smc_dev, (u8 *)cache, (u32)length, (u32 *)byte_written);
#ifdef CAS_GS_DEBUG
  {
    int i;
    OS_PRINTF("[GS]APDU_WRITE: ");
    for(i=0; i<*byte_written; i++)
    {
    	OS_PRINTF("%02x\t",cache[i]);
    	if((i%8 == 0) && (i!=0))
    		OS_PRINTF("\n");
    }
    OS_PRINTF(" \n");
  }
#endif
  return err;
}

/*!
  abc
  */
void GS_serial_flush(void)
{
  return;
}

/*!
  abc
  */
char GS_check_card_apdu(const unsigned char *stb_sn, unsigned char stb_sn_len,
                unsigned char *super_cas_id, unsigned char *smartcard_id, unsigned char *cw_mask)
{
  unsigned char in[128] = {0};
  unsigned char out[128] = {0};
  unsigned char cache[32] = {0};

  unsigned char check_reply_length = 19; /* 应答包长度 */
  unsigned int bytes_readed = 0;
  unsigned int bytes_written = 0;
  unsigned int crc = 0;
  char r = 0;
  char byte_write = 0;
  cache[0] = stb_sn_len;
  memcpy((u8 *)&cache[1], (u8 *)stb_sn, (u8)stb_sn_len);
  crc = GS_crc16ccitt(&cache[1], stb_sn_len);
  cache[stb_sn_len + 1] = (unsigned char)(crc >> 8);
  cache[stb_sn_len + 2] = (unsigned char)crc;

  byte_write = GS_apdu_package_v3((unsigned char *)out, (unsigned char)GS_CLA
    , (unsigned char)GS_INS_CHECK_SMARTCARD_APDU, 0, 0
    , (unsigned char)(stb_sn_len + 3), (unsigned char *)cache);
  //send package head
  GS_serial_write((unsigned char *)out, 5, (unsigned int *)(&bytes_written), 100);
  GS_serial_flush();
  //read ack
  r = GS_serial_read((unsigned char *)in, 1, (unsigned int *)(&bytes_readed), 300);
  if(r > 0)
  {
    CAS_GS_PRINTF("[GS]%s %d GS_serial_read_error \n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  if((unsigned char)in[0] != GS_INS_CHECK_SMARTCARD_APDU)
  {
    CAS_GS_PRINTF("[GS]%s %d GS_INS_CHECK_SMARTCARD_APDU error in[0] = %02x\n",__FUNCTION__,__LINE__,in[0]);
  	return GS_ERR_COMM;
  }
  //process rest data
  if(byte_write - 5)
  {
    GS_serial_write((unsigned char *)(out + 5), byte_write - 5,(unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
  }
  r = GS_serial_read((unsigned char *)in, 2, (unsigned int *)(&bytes_readed), 300);
  if(r > 0)
  {
  	CAS_GS_PRINTF("[GS]%s %d GS_serial_read_error \n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  if(((unsigned char)in[bytes_readed - 2] != 0x90) || ((unsigned char)in[bytes_readed - 1] != 0x00))
  {
    CAS_GS_PRINTF("[GS]%s %d APDU CALL error \n",__FUNCTION__,__LINE__);
    return GS_ERR_NOT_INI;
  }
  byte_write = GS_apdu_package_v2((unsigned char *)out,
      GS_CLA, GS_INS_GET_STAGE_DATA, 0, 0, (unsigned char)(check_reply_length - 2));
  //send package head
  GS_serial_write((unsigned char *)out, 5, (unsigned int *)(&bytes_written), 100);
  GS_serial_flush();
  //read ack
  r = GS_serial_read((unsigned char *)in, 1, (unsigned int *)(&bytes_readed), 300);
  if(r > 0)
  {
    CAS_GS_PRINTF("[GS]%s %d GS_serial_read_error \n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  if((unsigned char)in[0] != GS_INS_GET_STAGE_DATA)
  {
    CAS_GS_PRINTF("[GS]%s %d GS_INS_GET_STAGE_DATA error in[0] = %0x\n",__FUNCTION__,__LINE__,in[0]);
  	return GS_ERR_COMM;
  }
  //process rest data
  if(byte_write - 5)
  {
    GS_serial_write((unsigned char *)(out + 5),byte_write - 5, (unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
  }
  r = GS_serial_read((unsigned char *)in, check_reply_length, (unsigned int *)(&bytes_readed), 300);
  if(r > 0)
  {
    CAS_GS_PRINTF("[GS]%s %d GS_serial_read_error \n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }

  OS_PRINTF("\nreturn:\n");
  {
    u8 i = 0;
    for(i = 0; i < bytes_readed; i ++)
    {
      OS_PRINTF(" 0x%02x ",in[i]);
    }
  }
  OS_PRINTF("\nok\n");
  if(((unsigned char)in[bytes_readed - 2] == 0x90) && ((unsigned char)in[bytes_readed - 1] == 0x00))
  {
    memcpy(super_cas_id, in, 4);
    memcpy(smartcard_id, &in[4], 3);
    memcpy(cw_mask, &in[7], 8);
    return GS_ERR_OK;
  }
  CAS_GS_PRINTF("[GS]%s %d NOT INI \n",__FUNCTION__,__LINE__);
  return GS_ERR_NOT_INI;
}



//处理EMM， 数据从CA_SYSTEM_ID开始，直至数据包结束
//emm_data，EMM数据包
//emm_len，EMM数据包长度
/*!
  abc
  */
char GS_process_emm_apdu(const unsigned char *emm_data, unsigned char emm_len)
{
  unsigned char in[128] = {0};
  unsigned char out[128] = {0};
  static unsigned char emm_back[128] = {0};

  const unsigned char emm_reply = 2;
  unsigned int bytes_readed = 0;
  unsigned int bytes_written = 0;
  char r = 0;
  char byte_write = 0;
  if(emm_len > 128)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_ERR_DATA_LENGTH emm_len = %d \n",__FUNCTION__,__LINE__,emm_len);
    return GS_ERR_DATA_LENGTH;
  }
  if(GS_memncmp((u8 *)emm_data, (u8 *)emm_back, (u8)emm_len) == 0)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_ERR_OLD_DATA \n");
    return GS_ERR_OLD_DATA;//已经处理过的旧数据
  }
  byte_write = GS_apdu_package_v3((unsigned char *)out,GS_CLA, GS_INS_PROCESS_EMM, 0, 0,
      (unsigned char)emm_len, (unsigned char *)emm_data);
  //send package head
  GS_serial_write((unsigned char *)out, 5, (unsigned int *)(&bytes_written), 100);
  GS_serial_flush();
  //read ack
  r = GS_serial_read((unsigned char *)in, 1, (unsigned int *)(&bytes_readed), 300);
  if(r > 0)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error \n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  if((unsigned char)in[0] != GS_INS_PROCESS_EMM)
  {
  	CAS_GS_PRINTF("[GS] GS_INS_PROCESS_EMM ERROR \n");
  	return GS_ERR_COMM;
  }
  //process rest data
  if(byte_write - 5)
  {
    GS_serial_write((unsigned char *)(out + 5), byte_write - 5,
      (unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
  }
  r = GS_serial_read((unsigned char *)in, emm_reply, (unsigned int *)(&bytes_readed), 600);
  if(r > 0)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error \n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }

  if(bytes_readed != emm_reply)
  {
  	CAS_GS_PRINTF("[GS]%s:%d bytes_readed error bytes_readed=%d emm_reply=%d\n",__FUNCTION__,__LINE__,bytes_readed,emm_reply);
  	return GS_ERR_COMM;
  }
  if(((unsigned char)in[bytes_readed - 2] == 0x90) && ((unsigned char)in[bytes_readed - 1] == 0x00))
  {
    memcpy(emm_back, emm_data, emm_len);
    return GS_ERR_OK;
  }
  CAS_GS_PRINTF("\nError GS_process_emm_apdu return 0x%02x %02x\n",in[bytes_readed - 2] , in[bytes_readed - 1]);
  return GS_ERR_UNKNOWN;
}

//处理ECM， 数据从CA_SYSTEM_ID开始，直至数据包结束
//cw,奇在前，偶在后
//ecm_data，ECM数据包
//ecm_len，ECM数据包长度
//cw_mask，握手时得到的
/*!
  abc
  */


BOOL drv_smc_reset(void)
{
  u32 err = 0;
  u8 uResetTimes = 0;
  scard_atr_desc_t s_atr = {0};
  u8 buf_atr[256] = {0,};
  scard_device_t *p_smc_dev = NULL;
  s_atr.p_buf = buf_atr;

  OS_PRINTF("drv_smc_reset start\n");
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
    return FALSE;
  }
  else
  {
    OS_PRINTF("drv_smc_reset ok\n");
    {
      extern u8 gs_stb_sn[20];
      extern u8 gs_cw_mask[8];
      extern u8 gs_smartcard_id[3];
      extern u8 gs_super_cas_id[4];
      
      GS_check_card_apdu(gs_stb_sn, strlen(gs_stb_sn), gs_super_cas_id,
                         gs_smartcard_id, gs_cw_mask);
    }
    return TRUE;
  }
}

char GS_process_ecm_apdu(const unsigned char *ecm_data, unsigned char ecm_len,
          unsigned char *cw_mask, unsigned char *cw,unsigned short *err_code)
{
  unsigned char in[128] = {0};
  unsigned char out[128] = {0};
  static unsigned char ecm_back[128] = {0};

  const unsigned char ecm_reply = 20;
  unsigned int bytes_readed = 0;
  unsigned int bytes_written = 0;
  unsigned int crc = 0;
  char r = 0;
  char byte_write = 0;
  int i = 0;


  if(ecm_len > 128)
  {
    CAS_GS_PRINTF("[err] ecm_len > 128\n");
    return GS_ERR_DATA_LENGTH;
  }
    
  CAS_GS_PRINTF("\nget new ecm crc_old_ecm=%d\n",crc_old_ecm);
  if(GS_memncmp((u8 *)ecm_back, (u8 *)ecm_data, (u8)ecm_len) == 0&& crc_old_ecm != 0)
  {
  	CAS_GS_PRINTF("[GS]%s:%d old ecm data \n",__FUNCTION__,__LINE__);
  	return GS_ERR_OLD_DATA;//已经处理过的旧数据
  }
  CAS_GS_PRINTF("\nget new ecm~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  byte_write = GS_apdu_package_v3((unsigned char *)out,GS_CLA, GS_INS_PROCESS_ECM_APDU, 0, 0,
    (unsigned char)ecm_len, (unsigned char *)ecm_data);
  //send package head
  GS_serial_write((unsigned char *)out, 5, (unsigned int *)(&bytes_written), 100);
  GS_serial_flush();
  //read ack
  r = GS_serial_read((unsigned char *)in, 1, (unsigned int *)(&bytes_readed), 300);
  if(r > 0)
  {
    CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
    return GS_ERR_COMM;
  }
  if((unsigned char)in[0] != GS_INS_PROCESS_ECM_APDU)
  {
  	return GS_ERR_COMM;
  }
  //process rest data
  if(byte_write - 5)
  {
    GS_serial_write((unsigned char *)(out + 5),byte_write - 5, (unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
  }
  r = GS_serial_read((unsigned char *)in, 2,(unsigned int *)(&bytes_readed), 600);
  if(r > 0)
  {
    CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
    return GS_ERR_COMM;  
  }
  
    *err_code = (in[bytes_readed - 2]<<8) | in[bytes_readed - 1];
  if((in[bytes_readed - 2] != 0x90) || (in[bytes_readed - 1] != 0x00))
  {
    CAS_GS_PRINTF("[err] 333333 code= 0x%02x %02x\n", in[bytes_readed - 2], in[bytes_readed - 1]);
    return GS_ERR_ECM;
  }
  byte_write = GS_apdu_package_v2((unsigned char *)out, GS_CLA,
                    GS_INS_GET_STAGE_DATA, 0, 0, (unsigned char)(ecm_reply - 2));
  //send package head
  GS_serial_write((unsigned char *)out, 5, (unsigned int *)(&bytes_written), 100);
  GS_serial_flush();
  //read ack
  r = GS_serial_read((unsigned char *)in, 1, (unsigned int *)(&bytes_readed), 300);
  if(r > 0)
  {
    CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
    return GS_ERR_COMM;
  }
  if((unsigned char)in[0] != GS_INS_GET_STAGE_DATA)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_INS_GET_STAGE_DATA ERROR \n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  //process rest data
  if(byte_write - 5)
  {
    GS_serial_write((unsigned char *)(out + 5),byte_write - 5, (unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
  }
  r = GS_serial_read((unsigned char *)in, ecm_reply, (unsigned int *)(&bytes_readed), 600);
  if(r > 0)
  {
    CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
    return GS_ERR_COMM;
  }
  crc = GS_crc16ccitt(in, 16);
  if(crc != (in[bytes_readed - 4] << 8) + in[bytes_readed - 3])
  {
    CAS_GS_PRINTF("[GS]%s:%d CRC ERROR\n",__FUNCTION__,__LINE__);
    return GS_ERR_CRC; 
  }
  
  *err_code = (in[bytes_readed - 2]<<8) | in[bytes_readed - 1];
  if((in[bytes_readed - 2] == 0x90) && (in[bytes_readed - 1] == 0x00))
  {
    memcpy(cw, in, 16);
    for(i = 0; i < 8; ++ i)
    {
      cw[i] = cw[i] ^ cw_mask[i];
      cw[8 + i] = cw[8 + i] ^ cw_mask[i];
    }
    memcpy(ecm_back, ecm_data, ecm_len);

    return GS_ERR_OK;
  }
  CAS_GS_PRINTF("[GS] out 0x%02x %02x\n", in[bytes_readed - 2], in[bytes_readed - 1]);
  return GS_ERR_ECM;
}
//取授权截止时间，返回的4个字节以yy-m-d的格式
/*!
  abc
  */
char GS_get_expire_date(unsigned char *expire_date)
{
  unsigned char in[128] = {0};
  unsigned char out[128] = {0};
  const char reply = 6;
  char r = 0;
  unsigned int bytes_readed = 0;
  unsigned int bytes_written = 0;
  bytes_written = GS_apdu_package_v2((unsigned char *)out,
        GS_CLA, GS_INS_GET_EXPIRE_DATE, 0, 0, (unsigned char)reply);
  GS_serial_write((unsigned char *)out, 5,(unsigned int *)(&bytes_written), 100);
  GS_serial_flush();
  //read ack
  r = GS_serial_read((unsigned char *)in, 1,(unsigned int *)(&bytes_readed), 300);
  if(r > 0)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  if((unsigned char)in[0] != GS_INS_GET_EXPIRE_DATE)return GS_ERR_COMM;
  r = GS_serial_read((unsigned char *)in, reply,(unsigned int *)(&bytes_readed), 600);
  if(r > 0)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  if(bytes_readed != reply)return GS_ERR_COMM;
  if((unsigned char)(in[bytes_readed - 2] == 0x90) && ((unsigned char)in[bytes_readed - 1] == 0x00))
  {
    memcpy(expire_date, in, 4);
    return GS_ERR_OK;
  }
  
  CAS_GS_PRINTF("[GS]%s:%d comm_error\n",__FUNCTION__,__LINE__);
  return GS_ERR_COMM;
}


//读取区域码，区域码为2字节
/*!
  abc
  */
char GS_get_area_code(unsigned char *area_code)
{
  unsigned char in[128] = {0};
  unsigned char out[128] = {0};
  const char reply = 4;
  char r = 0;
  unsigned int bytes_readed = 0;
  unsigned int bytes_written = 0;
  bytes_written = GS_apdu_package_v2((unsigned char *)out,GS_CLA, GS_INS_GET_AREA_CODE, 0, 0, (unsigned char)reply);
  GS_serial_write((unsigned char *)out, 5,(unsigned int *)(&bytes_written), 100);
  GS_serial_flush();
  //read ack
  r = GS_serial_read((unsigned char *)in, 1,(unsigned int *)(&bytes_readed), 300);
  if(r > 0)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  if((unsigned char)in[0] != GS_INS_GET_AREA_CODE)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_INS_GET_AREA_CODE_ERROR\n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  r = GS_serial_read((unsigned char *)in, reply,(unsigned int *)(&bytes_readed), 600);
  if(r > 0)
  {
  	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  if(bytes_readed != reply)
  {
  	CAS_GS_PRINTF("[GS]%s:%d bytes_readed_error\n",__FUNCTION__,__LINE__);
  	return GS_ERR_COMM;
  }
  if(((unsigned char)in[bytes_readed - 2] == 0x90) && ((unsigned char)in[bytes_readed - 1] == 0x00))
  {
    memcpy(area_code, in, 2);
    return GS_ERR_OK;
  }

  CAS_GS_PRINTF("[GS]%s:%d comm_error\n",__FUNCTION__,__LINE__);
  return GS_ERR_COMM;
}


/*!
  abc
  */
char GS_memncmp(u8 *dst, u8 *src, u8 length)
{
  unsigned char i = 0;
  for(i = 0; i < length; ++ i)
  {
    if((*(dst + i)) != (*(src + i)))
	{
		CAS_GS_PRINTF("[GS]%s:%d error\n",__FUNCTION__,__LINE__);
		return -1;
    }
  }
  return 0;
}

//读取母卡的对子卡授权信息
char GS_get_child_auth_info(unsigned char *auth_info, char* len)
{
	unsigned char in[128];
	unsigned char out[128];
	const char reply = 20;
	int ret;
	unsigned int bytes_readed, bytes_written;

	CAS_GS_PRINTF("[GS]%s:%d \n",__FUNCTION__,__LINE__);
	bytes_written = GS_apdu_package_v2((unsigned char *)out, GS_CLA, GS_INS_GET_CHILD_AUTH_INFO, 0, 0, (unsigned char)reply);
	GS_serial_write((unsigned char *)out, 5, (unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
	//read ack
    ret = GS_serial_read((unsigned char *)in, 1, (unsigned int *)(&bytes_readed), 300);
	DEBUG(ERR,INFO,"ret=%x bytes_readed = %d\n",ret,bytes_readed);
	
	if(ret > 0)
	{
		CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
	}
	
    if((unsigned char)in[0] != GS_INS_GET_CHILD_AUTH_INFO)
	{
		CAS_GS_PRINTF("[GS]%s:%d GS_INS_GET_CHILD_AUTH_INFO ERROR\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
	}
	
	ret = GS_serial_read((unsigned char *)in, reply, (unsigned int *)(&bytes_readed), 600);
	DEBUG(ERR,INFO,"ret = %d print the in data,bytes_readed = %d (hex):\n",ret,bytes_readed);
	
    if(ret > 0)
    {
    	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
    if(bytes_readed != reply)
    {
    	CAS_GS_PRINTF("[GS]%s:%d bytes_readed_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
	if(((unsigned char)in[bytes_readed - 2] == 0x90) && ((unsigned char)in[bytes_readed - 1] == 0x00))
	{
		memcpy(auth_info, in, reply);
		*len = reply;
		return GS_ERR_OK;
	}

	CAS_GS_PRINTF("[GS]%s:%d GS_ERR_COMM\n",__FUNCTION__,__LINE__);
	return GS_ERR_COMM;
}

//设置子卡授权（把子卡插入母机中，把刚才读取的子卡授权信息写入子卡）
char GS_set_child_auth_info(unsigned char *auth_info, char len)
{
	unsigned char in[128];
	unsigned char out[128];
	char reply = 2;
	char ret;
	unsigned int bytes_readed, bytes_written;
	unsigned int byte_to_write;
	
	byte_to_write = GS_apdu_package_v3((unsigned char *)out, GS_CLA, GS_INS_SET_CHILD_AUTH_INFO, 0, 0, (unsigned char)len, (unsigned char *)auth_info);
	GS_serial_write((unsigned char *)out, 5, (unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
	//read ack
    ret = GS_serial_read((unsigned char *)in, 1, (unsigned int *)(&bytes_readed), 300);
	if(ret > 0)
	{
		CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
	}
    if((unsigned char)in[0] != GS_INS_SET_CHILD_AUTH_INFO)
	{	
		CAS_GS_PRINTF("[GS]%s:%d GS_INS_SET_CHILD_AUTH_INFO_ERROR\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
	//process rest data
    GS_serial_write((unsigned char *)out + 5, byte_to_write - 5, (unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
	ret = GS_serial_read((unsigned char *)in, reply, (unsigned int *)(&bytes_readed), 600);
    if(ret > 0)
    {
    	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
    if(bytes_readed != reply)
    {
    	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
	
	if(((unsigned char)in[bytes_readed - 2] == 0x90) && ((unsigned char)in[bytes_readed - 1] == 0x00))
	{
		CAS_GS_PRINTF("[GS]%s:%d GS_OK\n",__FUNCTION__,__LINE__);
		return GS_ERR_OK;
	}

	CAS_GS_PRINTF("[GS]%s:%d GS_ERR_COMM\n",__FUNCTION__,__LINE__);
	return GS_ERR_COMM;
}

//读取子卡的授权截止时间, 返回的4个字节以yy-m-d的格式
char read_child_expire_date(unsigned char *expire_date)
{
	unsigned char in[128];
	unsigned char out[128];
	const char reply = 6;
	char ret;
	unsigned int bytes_readed, bytes_written;
	bytes_written = GS_apdu_package_v2((unsigned char *)out, GS_CLA, GS_INS_READ_CHILD_AUTH_INFO, 0, 0, (unsigned char)reply);
	GS_serial_write((unsigned char *)out, 5, (unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
	//read ack
    ret = GS_serial_read((unsigned char *)in, 1, (unsigned int *)(&bytes_readed), 300);
	if(ret > 0)
	{
		CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
	}
    if((unsigned char)in[0] != GS_INS_READ_CHILD_AUTH_INFO)
    {
    	CAS_GS_PRINTF("[GS]%s:%d GS_INS_READ_CHILD_AUTH_INFO_ERROR\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
	ret = GS_serial_read((unsigned char *)in, reply, (unsigned int *)(&bytes_readed), 600);
    if(ret > 0)
    {
    	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
    if(bytes_readed != reply)
    {
    	CAS_GS_PRINTF("[GS]%s:%d GS_ERR_COMM\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
	if(((unsigned char)in[bytes_readed - 2] == 0x90) && ((unsigned char)in[bytes_readed - 1] == 0x00))
	{
		memcpy(expire_date, in, reply - 2);
		return GS_ERR_OK;
	}

	CAS_GS_PRINTF("[GS]%s:%d GS_ERR_COMM\n",__FUNCTION__,__LINE__);
	return GS_ERR_COMM;
}

//读取智能卡类型， 如果返回0xa5则为子卡，否则为母卡
char read_card_type(unsigned char *type)
{
	unsigned char in[128];
	unsigned char out[128];
	const char reply = 3;
	char ret;
	unsigned int bytes_readed, bytes_written;
	bytes_written = GS_apdu_package_v2((unsigned char *)out, GS_CLA, GS_INS_GET_CARD_TYPE, 0, 0, (unsigned char)reply);
	GS_serial_write((unsigned char *)out, 5, (unsigned int *)(&bytes_written), 100);
    GS_serial_flush();
	//read ack
    ret = GS_serial_read((unsigned char *)in, 1, (unsigned int *)(&bytes_readed), 300);
	if(ret > 0)
	{
		CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
	}
    if((unsigned char)in[0] != GS_INS_GET_CARD_TYPE)
    {
    	CAS_GS_PRINTF("[GS]%s:%d GS_INS_GET_CARD_TYPE_ERROR\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
	ret = GS_serial_read((unsigned char *)in, reply, (unsigned int *)(&bytes_readed), 600);
    if(ret > 0)
    {
    	CAS_GS_PRINTF("[GS]%s:%d GS_serial_read_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
    if(bytes_readed != reply)
    {
    	CAS_GS_PRINTF("[GS]%s:%d bytes_readed_error\n",__FUNCTION__,__LINE__);
		return GS_ERR_COMM;
    }
	if(((unsigned char)in[bytes_readed - 2] == 0x90) && ((unsigned char)in[bytes_readed - 1] == 0x00))
	{
		*type = in[0];
		CAS_GS_PRINTF("[GS]%s:%d type=%x\n",__FUNCTION__,__LINE__,*type);
		return GS_ERR_OK;
	}
	CAS_GS_PRINTF("[GS]%s:%d GS_ERR_COMM\n",__FUNCTION__,__LINE__);
	return GS_ERR_COMM;
}

/*!
  abc
  */
  static void GS_Stb_FreeFilter(gs_filter_struct_t *s_tFilter)
  {
    dmx_device_t *p_dev = NULL;
    u32 ret = 0;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
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
  void GS_CAS_Drv_FreeAllEcmReq(void)
  {
    u8 uIndex = 0;

    _Gs_CaReqLock();

    for(uIndex = 0; uIndex < GS_FILTER_MAX_NUM; uIndex ++)
    {
      if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
          1 == s_tFilter[uIndex].m_byReqId)
      {
        GS_Stb_FreeFilter(&s_tFilter[uIndex]);
        memset((void *)&s_tFilter[uIndex], 0, sizeof(gs_filter_struct_t));
      }
    }
    _Gs_CaReqUnLock();

    return;
  }
  /*!
  abc
  */
  void GS_CAS_Drv_FreeAllEmmReq(void)
  {
    u8 uIndex = 0;

    _Gs_CaReqLock();

    for(uIndex = 0; uIndex < GS_FILTER_MAX_NUM; uIndex ++)
    {
      if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
      2 == s_tFilter[uIndex].m_byReqId)
      {
        GS_Stb_FreeFilter(&s_tFilter[uIndex]);
        memset((void *)&s_tFilter[uIndex], 0, sizeof(gs_filter_struct_t));
      }
    }

    _Gs_CaReqUnLock();

    return;
  }

  /*!
  abc
  */
  void GS_CAS_Drv_FreeNITReq(void)
  {
    u8 uIndex = 0;

    for(uIndex = 0; uIndex < GS_FILTER_MAX_NUM; uIndex ++)
    {
      if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus &&
      0x40 == s_tFilter[uIndex].m_byReqId)
      {
        GS_Stb_FreeFilter(&s_tFilter[uIndex]);
        memset((void *)&s_tFilter[uIndex], 0, sizeof(gs_filter_struct_t));
      }
    }

    _Gs_CaReqUnLock();

    return;
  }

  /*!
  abc
  */
  static void _Gs_AddData(u8 m_byReqId, u8 m_uOK, u16 m_PID,
      u8 *m_pData, u32 m_uLength, u8 uIndex)
  {
  if(uIndex >= GS_FILTER_MAX_NUM)
  {
    CAS_GS_PRINTF("\n[ERROR]_Gs_AddData  uIndex >= BY_FILTER_MAX_NUM \n");
    return;
  }

  s_gs_tData[uIndex].m_byReqId = m_byReqId;
  s_gs_tData[uIndex].m_uOK = m_uOK;
  s_gs_tData[uIndex].m_PID = m_PID;
#ifdef MEMTEST
  if(NULL == m_pData || 0 == m_uLength)
  {
    s_gs_tData[uIndex].p_Data = NULL;
  }
  else
  {
    s_gs_tData[uIndex].p_Data = mtos_malloc(m_uLength);
    //s_gs_tData[uIndex].p_Data = malloc(m_uLength);
    memcpy(s_gs_tData[uIndex].p_Data, m_pData, m_uLength);
  }
#else
  memcpy(s_gs_tData[uIndex].p_Data, m_pData, m_uLength);
#endif
    s_gs_tData[uIndex].m_uLength = m_uLength;
  }



u8 GS_SetDescrCW(const u8 *szOddKey,const u8 *szEvenKey)
{
  u16 v_pid = 0;
  u16 a_pid = 0;
  dmx_device_t *p_dev = NULL;
  RET_CODE ret = 0;

  if((NULL == szOddKey) || (NULL == szEvenKey))
  {
    return FALSE;
  }

  CAS_GS_PRINTF("\nOddKey %02x %02x %02x %02x %02x %02x %02x %02x   ticks=%d\n"
    , szOddKey[0], szOddKey[1], szOddKey[2], szOddKey[3]
    , szOddKey[4], szOddKey[5], szOddKey[6], szOddKey[7],mtos_ticks_get());
   CAS_GS_PRINTF("EvenKey %02x %02x %02x %02x %02x %02x %02x %02x\n"
    , szEvenKey[0], szEvenKey[1], szEvenKey[2], szEvenKey[3]
    , szEvenKey[4], szEvenKey[5], szEvenKey[6], szEvenKey[7]);

  p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
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

  v_pid = cas_gs_get_v_pid();
  a_pid = cas_gs_get_a_pid();

  CAS_GS_PRINTF("set key v_pid= %x  a_pid = %x\n", v_pid, a_pid);
  ret = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
  if(ret != SUCCESS)
  return FALSE;

  ret = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
  if(ret != SUCCESS)
  return FALSE;
  CAS_GS_PRINTF("set key v_channel= %x  a_channel = %x\n", v_channel, a_channel);

  if(v_channel != 0xffff)
  {
    ret = dmx_descrambler_onoff(p_dev,v_channel,TRUE);
    if(ret != SUCCESS)
      return FALSE;
    ret = dmx_descrambler_set_odd_keys(p_dev,v_channel,(u8 *)szOddKey,8);
    ret = dmx_descrambler_set_even_keys(p_dev,v_channel,(u8 *)szEvenKey,8);
  }

  if (a_channel != 0xffff)
  {
    ret = dmx_descrambler_onoff(p_dev,a_channel,TRUE);
    if(ret != SUCCESS)
      return FALSE;
    ret = dmx_descrambler_set_odd_keys(p_dev,a_channel,(u8 *)szOddKey,8);
    ret = dmx_descrambler_set_even_keys(p_dev,a_channel,(u8 *)szEvenKey,8);
  }

  old_v_channel = v_channel;
  old_a_channel = a_channel;
  return TRUE;
}



void _Gs_refresh_osd(void)
{
  CAS_GS_PRINTF("\n_Gs_refresh_osd\n");
  crc_old_ecm = 0;
}

extern void _Gs_ShowOSDMsg(const u8 *szOSD);


extern void gs_get_new_mail(gs_mail_t *p_mail);

void gs_mail_test1(void)
{
  gs_mail_t gs_mail = {0};
  gs_mail.mail_id = 0x0f;
  gs_mail.year_high_byte = 0x07;
  gs_mail.year_low_byte = 0xda;
  gs_mail.month = 0x04;
  gs_mail.day = 0x15;
  gs_mail.hour = 0x0f;
  gs_mail.minute = 0x16;
  gs_mail.second = 0x1a;
  gs_mail.sender_length = 0x0b;
  memcpy(gs_mail.sender, "mail sender" ,
    gs_mail.sender_length);
  gs_mail.caption_length = 0x0c;
  memcpy(gs_mail.caption, "mail cap2222",
    gs_mail.caption_length);
  gs_mail.content_length = 0x3c;
  memcpy(gs_mail.content,
    "hello,world,this is a mail.the smartcard is:111",
    gs_mail.content_length);

  CAS_GS_PRINTF("mail_id=0x%02x\n",
    gs_mail.mail_id);
  CAS_GS_PRINTF("year_high_byte=0x%02x\n",
    gs_mail.year_high_byte);
  CAS_GS_PRINTF("year_low_byte=0x%02x\n",
    gs_mail.year_low_byte);
  CAS_GS_PRINTF("month=0x%02x\n",
    gs_mail.month);
  CAS_GS_PRINTF("day=0x%02x\n",
    gs_mail.day);
  CAS_GS_PRINTF("hour=0x%02x\n",
    gs_mail.hour);
  CAS_GS_PRINTF("minute=0x%02x\n",
    gs_mail.minute);
  CAS_GS_PRINTF("second=0x%02x\n",
    gs_mail.second);
  CAS_GS_PRINTF("sender_length=0x%02x\n",
    gs_mail.sender_length);
  CAS_GS_PRINTF("sender=%s\n",
    gs_mail.sender);
  CAS_GS_PRINTF("caption_length=0x%02x\n",
    gs_mail.caption_length);
  CAS_GS_PRINTF("caption=%s\n",
    gs_mail.caption);
  CAS_GS_PRINTF("content_length=0x%02x\n",
    gs_mail.content_length);
  CAS_GS_PRINTF("content=%s\n",
    gs_mail.content);
  gs_get_new_mail(&gs_mail);
}

void gs_mail_test2(void)
{
  gs_mail_t gs_mail = {0};
  gs_mail.mail_id = 0x01;
  gs_mail.year_high_byte = 0x07;
  gs_mail.year_low_byte = 0xda;
  gs_mail.month = 0x04;
  gs_mail.day = 0x15;
  gs_mail.hour = 0x0f;
  gs_mail.minute = 0x16;
  gs_mail.second = 0x1a;
  gs_mail.sender_length = 0x0b;
  memcpy(gs_mail.sender, "mail sender" ,
  gs_mail.sender_length);
  gs_mail.caption_length = 0x0c;
  memcpy(gs_mail.caption, "mail cap2222",
  gs_mail.caption_length);
  gs_mail.content_length = 0x3c;
  memcpy(gs_mail.content,
  "hello,world,this is a mail.the smartcard is:222",
  gs_mail.content_length);

  CAS_GS_PRINTF("mail_id=0x%02x\n",
    gs_mail.mail_id);
  CAS_GS_PRINTF("year_high_byte=0x%02x\n",
    gs_mail.year_high_byte);
  CAS_GS_PRINTF("year_low_byte=0x%02x\n",
    gs_mail.year_low_byte);
  CAS_GS_PRINTF("month=0x%02x\n",
    gs_mail.month);
  CAS_GS_PRINTF("day=0x%02x\n",
    gs_mail.day);
  CAS_GS_PRINTF("hour=0x%02x\n",
    gs_mail.hour);
  CAS_GS_PRINTF("minute=0x%02x\n",
    gs_mail.minute);
  CAS_GS_PRINTF("second=0x%02x\n",
    gs_mail.second);
  CAS_GS_PRINTF("sender_length=0x%02x\n",
    gs_mail.sender_length);
  CAS_GS_PRINTF("sender=%s\n",
    gs_mail.sender);
  CAS_GS_PRINTF("caption_length=0x%02x\n",
    gs_mail.caption_length);
  CAS_GS_PRINTF("caption=%s\n",
    gs_mail.caption);
  CAS_GS_PRINTF("content_length=0x%02x\n",
    gs_mail.content_length);
  CAS_GS_PRINTF("content=%s\n",
    gs_mail.content);
  gs_get_new_mail(&gs_mail);
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
  u8 i = 0;

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
      case 0x5f://
        CAS_GS_PRINTF("\n\n\n0x%02x descriptor_length=%d  length=%d\n",
          descriptor_tag,descriptor_length,length);
        for(i = 0; i < descriptor_length; i ++)
        {
          CAS_GS_PRINTF("0x%02x ",p_data[i]);
        }
        gs_ca_nit_flag = 1;
        CAS_GS_PRINTF("\ngs_ca_nit_flag =%d\n",gs_ca_nit_flag);
        CAS_GS_PRINTF("\nset ok\n");
        gs_ca_area_code = (p_data[2] << 8) | p_data[3];
        CAS_GS_PRINTF("\nnit gs_ca_area_code = 0x%04x\n",gs_ca_area_code);
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
static void gs_cas_parse_nit(u8 *p_buf)
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

  CAS_GS_PRINTF("[nit] length=%d,ver_num=%d s_num=%d last_num=%d\n",
                    length,version_number,nit_section_num,nit_last_section_num);
  p_data += 10;

  ca_nit_parse_descriptor(p_data, length);

  if(nit_section_num == nit_last_section_num)
  {
    if(gs_ca_nit_flag == 0)
    {
        gs_ca_nit_flag = 2;
    }
    gs_ca_nit_state = 1;
  }
  else if(nit_section_num < nit_last_section_num)
  {
      match[4] = nit_section_num + 1;
      GSSTBCA_SetStreamGuardFilter(0x40, 0x10,
             match, mask, 5, 0);
      CAS_GS_PRINTF("\nstart section_num=%d \n",match[4]);  
  }
  CAS_GS_PRINTF("\r\nparsing nit over.\n");
  return;
}

void gs_save_sys_time(utc_time_t time)
{
  g_GS_CA_TIME.year   = time.year;
  g_GS_CA_TIME.month  = time.month;
  g_GS_CA_TIME.day    = time.day;
  g_GS_CA_TIME.hour   = time.hour;
  g_GS_CA_TIME.minute = time.minute;
  g_GS_CA_TIME.second = time.second;
}

void gs_get_sys_time(utc_time_t *time)
{
  *time = g_GS_CA_TIME;  
}
extern BOOL  get_gs_area_limit_free(void);

static void _Gs_SendAllData(u8 uDataNum)
{
    u8 uIndex = 0;
    u16 i = 0;
    char err = 0;
    unsigned short ca_error_code = 0;
    u8 *time;

    if(uDataNum > GS_FILTER_MAX_NUM)
    {
      CAS_GS_PRINTF("\n[ERROR]_Gs_SendAllData  uDataNum > BY_FILTER_MAX_NUM \n");
      uDataNum = GS_FILTER_MAX_NUM;
    }

    for(uIndex = 0; uIndex < uDataNum; uIndex ++)
    {
      if(1 == s_gs_tData[uIndex].m_byReqId) //ecm
      {
          u16 crc_a = 0;
          u16 crc_b = 0;
          u8 crc_1 = 0;
          u8 crc_2 = 0;
          u8 area_flag = 0;

          crc_1 = s_gs_tData[uIndex].p_Data[s_gs_tData[uIndex].m_uLength - 2];
          crc_2 = s_gs_tData[uIndex].p_Data[s_gs_tData[uIndex].m_uLength - 1];
          crc_b = (crc_1 << 8) | crc_2;

          if(crc_old_ecm != crc_b)
          {
            //CAS_GS_PRINTF("ecm crc 1 \n");
            crc_a = GS_crc16ccitt((unsigned char *)(s_gs_tData[uIndex].p_Data + 3),
                                  s_gs_tData[uIndex].p_Data[2]-2);
            //CAS_GS_PRINTF("ecm crc 2 \n");
            if(crc_b == crc_a)
            {
              //CAS_GS_PRINTF("crc pass \n");
              //CAS_GS_PRINTF("sent new ecm\n");
              err = GS_process_ecm_apdu((const unsigned char *)(s_gs_tData[uIndex].p_Data + 3),
                  (s_gs_tData[uIndex].m_uLength - 3),  gs_cw_mask,gs_cw, &ca_error_code);
			  //mother stb Feeding daughter card after a period of time to stop playing
              if((err == GS_ERR_COMM || err == GS_ERR_ECM) && (0x700B != ca_error_code))
              {
                drv_smc_reset();
                err = GS_process_ecm_apdu((const unsigned char *)(s_gs_tData[uIndex].p_Data + 3),
                  (s_gs_tData[uIndex].m_uLength - 3),  gs_cw_mask,gs_cw, &ca_error_code);
           
              }
              crc_old_ecm = crc_b;

              #if 1
              {
                utc_time_t ca_curn_time;
                time = s_gs_tData[uIndex].p_Data + 10;
                ca_curn_time.year = (*time) * 256 + *(time + 1);
                ca_curn_time.month = *(time + 2);
                ca_curn_time.day = *(time + 3);
                ca_curn_time.hour = *(time + 4);
                ca_curn_time.minute = *(time + 5);
                ca_curn_time.second = *(time + 6);
                OS_PRINTF("****SYS_TIME****\n");
                OS_PRINTF("%d-%d-%d,%d:%d:%d\n", 
                      ca_curn_time.year, ca_curn_time.month, ca_curn_time.day, 
                      ca_curn_time.hour, ca_curn_time.minute, ca_curn_time.second);
                OS_PRINTF("****************\n");
                gs_save_sys_time(ca_curn_time);
              }
              
              #endif
              
              if(gs_ca_nit_flag == 0)//no nit
              {
                CAS_GS_PRINTF("\n[no nit]");
                if(gs_card_area_code == 0xffff)//测试卡
                {
                  CAS_GS_PRINTF("\n[test card]");
                  area_flag = 0;
                }
                else
                {
                  CAS_GS_PRINTF("\n[not testcard]");
                  area_flag = 1;
                }
                if (gs_ca_area_code == 0xffff)
                  area_flag = 0;  
              }
              else if(gs_ca_nit_flag == 2)//no nit desc
              {
                CAS_GS_PRINTF("\n[no nit desc]");
                area_flag = 0;
              }
              else//have nit
              {
                CAS_GS_PRINTF("\n[have nit desc]");
                if(gs_card_area_code == 0xffff)
                {
                  CAS_GS_PRINTF("\n[test card]");                
                  area_flag = 0;
                }
                else if(gs_ca_area_code == 0xffff)
                {
                  CAS_GS_PRINTF("\n[nit area code =0xffff]");                
                  area_flag = 0;
                }
                else if(gs_card_area_code == gs_ca_area_code)
                {
                  CAS_GS_PRINTF("\n[area code ok]");                
                  area_flag = 0;
                }
                else
                {
                  CAS_GS_PRINTF("\n[gs_card_area_code =0x%x gs_ca_area_code =0x%x]", 
                    gs_card_area_code, gs_ca_area_code);                
                  area_flag = 1;
                }
              }
              #if 0
              if(TRUE == get_gs_area_limit_free())
              {
                  area_flag = 0;
              }
              #endif
              if(area_flag == 1)
              {
                 _Gs_EcmDecrypt(GS_SW1SW2_LIMIT_AREA);
                 CAS_GS_PRINTF("\n区域不对0x%04x != 0x%04x\n",
                 gs_card_area_code,gs_ca_area_code);
              }
              else
              {
                  if(ca_error_code != 0)
                    _Gs_EcmDecrypt(ca_error_code);


                  if(err == GS_ERR_OK)
                    GS_SetDescrCW((const u8 *)gs_cw,(const u8 *)(gs_cw + 8));
                  else if(err == GS_ERR_UNKNOWN)
                    CAS_GS_PRINTF("\necm 未知错误\n");
              }
            }
            else
            {
                CAS_GS_PRINTF("\necm crc错误\n");
            }
          }
      }
      else if(2 == s_gs_tData[uIndex].m_byReqId)//emm
      {
        if(*(char *)(s_gs_tData[uIndex].p_Data + 10) == 0x01)
        {
          u16 crc_a = 0;
          u16 crc_b = 0;
          u8 crc_1 = 0;
          u8 crc_2 = 0;
          u8 data[256]={0};

          crc_1 = s_gs_tData[uIndex].p_Data[s_gs_tData[uIndex].m_uLength - 2];
          crc_2 = s_gs_tData[uIndex].p_Data[s_gs_tData[uIndex].m_uLength - 1];
          crc_b = (crc_1 << 8) | crc_2;

          if(crc_old_osd != crc_b)
          {
            CAS_GS_PRINTF("\nEMM 短信@@@@@@@@@\n");
            //CAS_GS_PRINTF("emm crc 1 \n");


            crc_a = GS_crc16ccitt((unsigned char *)(s_gs_tData[uIndex].p_Data + 3),
              s_gs_tData[uIndex].p_Data[2] - 2);
            //CAS_GS_PRINTF("emm crc 2 \n");
            if(crc_b == crc_a)
            {
              //CAS_GS_PRINTF("crc pass \n");

              crc_old_osd = crc_b;
              memset(data,256,0);

              memcpy(data,s_gs_tData[uIndex].p_Data + 13
                , (*(s_gs_tData[uIndex].p_Data + 11)) << 8 | (*(s_gs_tData[uIndex].p_Data + 12)));
              _Gs_ShowOSDMsg(data);
              CAS_GS_PRINTF("\n%s\n", data);
            }
          }

        }
        else if(*(char *)(s_gs_tData[uIndex].p_Data + 10) == 0x02)
        {
          u16 crc_a = 0;
          u16 crc_b = 0;
          u8 crc_1 = 0;
          u8 crc_2 = 0;
          gs_mail_t gs_mail = {0};

          memset((void *)&gs_mail,0,sizeof(gs_mail_t));
          crc_1 = s_gs_tData[uIndex].p_Data[s_gs_tData[uIndex].m_uLength - 2];
          crc_2 = s_gs_tData[uIndex].p_Data[s_gs_tData[uIndex].m_uLength - 1];
          crc_b = (crc_1 << 8) | crc_2;
          if(crc_old_mail != crc_b)
          {
            CAS_GS_PRINTF("\nEMM 邮件@@@@@@@@@@@@@@@\n");
            //CAS_GS_PRINTF("emm crc 1 \n");

            crc_a = GS_crc16ccitt((unsigned char *)(s_gs_tData[uIndex].p_Data + 3),
              s_gs_tData[uIndex].p_Data[2] - 2);
            //CAS_GS_PRINTF("emm crc 2 \n");

            if(crc_b == crc_a)
            {
              //CAS_GS_PRINTF("crc pass \n");

              crc_old_mail = crc_b;
              CAS_GS_PRINTF("\n邮件邮件邮件0x%x\n",crc_old_mail);

              gs_mail.mail_id = *(s_gs_tData[uIndex].p_Data + 13);
              gs_mail.year_high_byte = *(s_gs_tData[uIndex].p_Data + 14);
              gs_mail.year_low_byte = *(s_gs_tData[uIndex].p_Data + 15);
              gs_mail.month = *(s_gs_tData[uIndex].p_Data + 16);
              gs_mail.day = *(s_gs_tData[uIndex].p_Data + 17);
              gs_mail.hour = *(s_gs_tData[uIndex].p_Data + 18);
              gs_mail.minute = *(s_gs_tData[uIndex].p_Data + 19);
              gs_mail.second = *(s_gs_tData[uIndex].p_Data + 20);
              gs_mail.sender_length = *(s_gs_tData[uIndex].p_Data + 21);
              memcpy(gs_mail.sender,
                s_gs_tData[uIndex].p_Data + 22 ,gs_mail.sender_length);
              gs_mail.caption_length =
                *(s_gs_tData[uIndex].p_Data + 22 + gs_mail.sender_length);
              memcpy(gs_mail.caption,
                s_gs_tData[uIndex].p_Data + 23 + gs_mail.sender_length,
                gs_mail.caption_length);
			  gs_mail.content_length =
				*(s_gs_tData[uIndex].p_Data + 23 + gs_mail.sender_length + gs_mail.caption_length);
			  memcpy(gs_mail.content,
   				s_gs_tData[uIndex].p_Data + 24 + gs_mail.sender_length + gs_mail.caption_length,
  				gs_mail.content_length);

              CAS_GS_PRINTF("mail_id=0x%02x\n", gs_mail.mail_id);
              CAS_GS_PRINTF("year_high_byte=0x%02x\n", gs_mail.year_high_byte);
              CAS_GS_PRINTF("year_low_byte=0x%02x\n", gs_mail.year_low_byte);
              CAS_GS_PRINTF("month=0x%02x\n", gs_mail.month);
              CAS_GS_PRINTF("day=0x%02x\n", gs_mail.day);
              CAS_GS_PRINTF("hour=0x%02x\n", gs_mail.hour);
              CAS_GS_PRINTF("minute=0x%02x\n", gs_mail.minute);
              CAS_GS_PRINTF("second=0x%02x\n", gs_mail.second);
              CAS_GS_PRINTF("sender_length=0x%02x\n", gs_mail.sender_length);
              CAS_GS_PRINTF("sender=%s\n", gs_mail.sender);
              CAS_GS_PRINTF("caption_length=0x%02x\n", gs_mail.caption_length);
              CAS_GS_PRINTF("caption=%s\n", gs_mail.caption);
              CAS_GS_PRINTF("content_length=0x%02x\n", gs_mail.content_length);
              CAS_GS_PRINTF("content=%s\n", gs_mail.content);
              gs_get_new_mail(&gs_mail);


            }
          }
        }
        else
        {
          u16 crc_a = 0;
          u16 crc_b = 0;
          u8 crc_1 = 0;
          u8 crc_2 = 0;

          crc_1 = s_gs_tData[uIndex].p_Data[s_gs_tData[uIndex].m_uLength - 2];
          crc_2 = s_gs_tData[uIndex].p_Data[s_gs_tData[uIndex].m_uLength - 1];
          crc_b = (crc_1 << 8) | crc_2;

          if(crc_old_emm != crc_b)
          {
            //CAS_GS_PRINTF("emm crc 1 \n");

            crc_a =
              GS_crc16ccitt((unsigned char *)(s_gs_tData[uIndex].p_Data + 3),
                s_gs_tData[uIndex].p_Data[2] - 2);
             //CAS_GS_PRINTF("emm crc 2 \n");

            if(crc_b == crc_a)
            {
              //CAS_GS_PRINTF("crc pass \n");
              //CAS_GS_PRINTF("sent new emm\n");
              err = GS_process_emm_apdu((const unsigned  char *)(s_gs_tData[uIndex].p_Data + 3),
                                        (s_gs_tData[uIndex].m_uLength - 3));
              if(err == GS_ERR_OK)
              {
                //CAS_GS_PRINTF("\nget emm succ\n",err);
                crc_old_ecm = 0;
                crc_old_emm = crc_b;
              }
              else if(err == GS_ERR_UNKNOWN)
              {
                  CAS_GS_PRINTF("\n~~GET DATA m_byReqId=0x%x uDataNum = %d\n",
                                s_gs_tData[uIndex].m_byReqId,uDataNum);

                  for(i = 0; i < s_gs_tData[uIndex].m_uLength; i ++)
                  {
                    CAS_GS_PRINTF("%02x ", *(u8 *)(s_gs_tData[uIndex].p_Data + i));
                  }
                  CAS_GS_PRINTF("\n");
                  CAS_GS_PRINTF("\nemm 未知错误\n");
              }
            }
            else
            {
                  CAS_GS_PRINTF("\n~~GET DATA m_byReqId=0x%x uDataNum = %d\n",
                                s_gs_tData[uIndex].m_byReqId,uDataNum);

                  for(i = 0; i < s_gs_tData[uIndex].m_uLength; i ++)
                  {
                    CAS_GS_PRINTF("%02x ", *(u8 *)(s_gs_tData[uIndex].p_Data + i));
                  }
                  CAS_GS_PRINTF("\n");
                CAS_GS_PRINTF("\nemm crc错误\n");
            }
          }
        }
      }
      else if(0x40 == s_gs_tData[uIndex].m_byReqId)//nit
      {
        CAS_GS_PRINTF("\n~~GET DATA m_byReqId=0x%x len = %d\n",
                      s_gs_tData[uIndex].m_byReqId,s_gs_tData[uIndex].m_uLength);

        for(i = 0; i < s_gs_tData[uIndex].m_uLength; i ++)
        {
          CAS_GS_PRINTF("%02x ", *(u8 *)(s_gs_tData[uIndex].p_Data + i));
        }
        CAS_GS_PRINTF("\n");      
        CAS_GS_PRINTF("\nnit parse\n");
        gs_cas_parse_nit(s_gs_tData[uIndex].p_Data);
        CAS_GS_PRINTF("\nnit parse ok\n");
      }
#ifdef MEMTEST
      if(NULL != s_gs_tData[uIndex].p_Data)
      {
        //CAS_GS_PRINTF("\n~~FREE m_byReqId=0x%x\n", s_gs_tData[uIndex].m_byReqId);
        mtos_free(s_gs_tData[uIndex].p_Data);
        //free(s_gs_tData[uIndex].p_Data);
        s_gs_tData[uIndex].p_Data = NULL;
      }
#endif
      memset(&s_gs_tData[uIndex], 0, sizeof(gs_data_got_t));
     }
  }


/*!
  abc
  */
static void _Gs_CaDataMonitor(void *pdata)
{
    u8 aFilterIndex[GS_FILTER_MAX_NUM] = { 0, };
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
      memset(aFilterIndex, 0, GS_FILTER_MAX_NUM);
      _Gs_CaReqLock();

      //Find valid Filter
      for(uIndex = 0; uIndex < GS_FILTER_MAX_NUM; uIndex ++)
      {
        if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus)
        {
          aFilterIndex[uValidFilterNum] = uIndex;
          uValidFilterNum ++;
        }
      }

  _Gs_CaReqUnLock();

    //Valid filter doesnot exist
    if(0 == uValidFilterNum)
    {
      mtos_task_sleep(20);
      continue;
    }
    _Gs_CaReqLock();
    //Read Data
    uDataNum = 0;
    for(uIndex = 0; uIndex < uValidFilterNum; uIndex ++)
    {
      if (FILTER_BUSY == s_tFilter[aFilterIndex[uIndex]].m_FilterStatus)
      {
        
        err = dmx_si_chan_get_data(p_dev, s_tFilter[aFilterIndex[uIndex]].m_dwReqHandle,
                                     &p_GotData, &uDataLen);
        
        if(err == 0)
        {
           if(p_GotData[0] != 0x80 && p_GotData[0] != 0x81 && p_GotData[0] != 0x40)
           {
              u8 test = 0;
              CAS_GS_PRINTF("\n~~rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr\n");

              for(test = 0; test < uDataLen; test ++)
              {
                CAS_GS_PRINTF("%02x ", p_GotData[test]);
              }
              CAS_GS_PRINTF("\n");
              continue;
           }
            
           if(p_GotData[0] == 0x40)
           {
            _Gs_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, TRUE,
              s_tFilter[aFilterIndex[uIndex]].m_PID, p_GotData, (u32)uDataLen, uDataNum);
            GS_Stb_FreeFilter(&s_tFilter[uIndex]);
            memset((void *)&s_tFilter[uIndex], 0, sizeof(gs_filter_struct_t));

           }
           else
           {
            _Gs_AddData(s_tFilter[aFilterIndex[uIndex]].m_byReqId, TRUE,
              s_tFilter[aFilterIndex[uIndex]].m_PID, p_GotData, (u32)uDataLen, uDataNum);
           }

           uDataNum++;

        }
      }
    }
    _Gs_CaReqUnLock();
    _Gs_SendAllData(uDataNum);
    
    mtos_task_sleep(10);
    }
}

 /*!
  abc
  */
void GS_CAS_Drv_ShowAllCaReq(void)
{
  u8 uIndex = 0;

  _Gs_CaReqLock();

  for(uIndex = 0; uIndex < GS_FILTER_MAX_NUM; uIndex ++)
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

  _Gs_CaReqUnLock();

  return;
}

/*!
  abc
  */
u8 GSSTBCA_SetStreamGuardFilter(u8 byReqID,u16 wPID,
                                    u8 *szFilter,u8 *szMask,u8 byLen,u8 nWaitSecs)
{
    u32 err = 0;
    u8 uIndex = 0;
    dmx_filter_setting_t p_param = {0};
    dmx_slot_setting_t p_slot = {0} ;
    dmx_device_t *p_dev = NULL;

    CAS_GS_PRINTF("\n设置过滤表 byReqID=0x%x,pid=0x%x waitsecs=%d\n",
                                  byReqID,wPID,nWaitSecs);

    _Gs_CaReqLock();
    for(uIndex = 0; uIndex < GS_FILTER_MAX_NUM; uIndex ++)
    {
    if(FILTER_BUSY == s_tFilter[uIndex].m_FilterStatus
      && byReqID == s_tFilter[uIndex].m_byReqId
      && 1 != byReqID) // byReqID = 1是ecm
    {
      if((memcmp(szFilter, s_tFilter[uIndex].m_uFilterData, byLen) == 0)
        && (memcmp(szMask, s_tFilter[uIndex].m_uFilterMask, byLen) == 0)
        && (s_tFilter[uIndex].m_PID == wPID))
      {
        CAS_GS_PRINTF("Emm filter is the same, don't request new filter!\n");
        _Gs_CaReqUnLock();
        return TRUE;
      }
      else
      {
        OS_PRINTF("Emm filter is different, cancel old filter and request new filter!\n");
        GS_Stb_FreeFilter(&s_tFilter[uIndex]);
        memset((void *)&s_tFilter[uIndex], 0, sizeof(gs_filter_struct_t));
      }
    }
    }

    for(uIndex = 0; uIndex < GS_FILTER_MAX_NUM; uIndex ++)
    {
      if(FILTER_FREE == s_tFilter[uIndex].m_FilterStatus)
      {
        break;
      }
    }

    if(uIndex >= GS_FILTER_MAX_NUM)
    {
      OS_PRINTF("[ERROR]Filter array is full!\n");
      _Gs_CaReqUnLock();
      GS_CAS_Drv_ShowAllCaReq();
      GS_CAS_Drv_FreeAllEcmReq();
      _Gs_CaReqLock();
      for(uIndex = 0; uIndex < GS_FILTER_MAX_NUM; uIndex ++)
      {
        if(FILTER_FREE == s_tFilter[uIndex].m_FilterStatus)
        {
          break;
        }
      }
      if(uIndex >= GS_FILTER_MAX_NUM)
      {
        _Gs_CaReqUnLock();
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
      _Gs_CaReqUnLock();
      return FALSE;
    }

    s_tFilter[uIndex].p_buf = s_tFilterBuf[uIndex];
    err = dmx_si_chan_set_buffer(p_dev,s_tFilter[uIndex].m_dwReqHandle,s_tFilter[uIndex].p_buf,
      GS_FILTER_BUFFER_SIZE);
    if(err != SUCCESS)
    {
      OS_PRINTF("\r\n[ERROR]set filter buffer failed!\n");
      _Gs_CaReqUnLock();
      return FALSE;
    }
    p_param.continuous = 1;
    p_param.en_crc = FALSE;//DISABLE FOR CA DATA
    if(byLen > DMX_SECTION_FILTER_SIZE)
    {
      OS_PRINTF("make len err!\n");
      _Gs_CaReqUnLock();
      return FALSE;
    }
    memcpy(p_param.value, szFilter, byLen);
    memcpy(p_param.mask, szMask, byLen);
    err = dmx_si_chan_set_filter(p_dev,s_tFilter[uIndex].m_dwReqHandle,&p_param);
    if(err != SUCCESS)
    {
      OS_PRINTF("\n[ERROR]set filter failed!\n");
      _Gs_CaReqUnLock();
      return FALSE;
    }
    err = dmx_chan_start(p_dev,s_tFilter[uIndex].m_dwReqHandle);
    if(err != SUCCESS)
    {
      OS_PRINTF("\n[ERROR]start channel  failed!\n");
      _Gs_CaReqUnLock();
      return FALSE;
    }

    if(0 == nWaitSecs || 1 != byReqID)
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
    CAS_GS_PRINTF("\n设置成功!  byReqID =0x%x \n ", byReqID);
    if(byReqID == 1)
      OS_PRINTF("\nECM filter set ticks=%d\n",mtos_ticks_get());
    _Gs_CaReqUnLock();
    return TRUE;
}

  /*!
  abc
  */
u32 GS_CAS_Drv_ClientInit(void)
{
	BOOL err = 0;
	u32 *p_stack = NULL;
	//create ca req lock semphore

	CAS_GS_PRINTF(" GS_CAS_Drv_ClientInit!mtos_sem_create  u32_CaReqLock\n");

	err = mtos_sem_create((os_sem_t *)&u32_CaReqLock,1);
	if (!err)
	{
		OS_PRINTF("%s : %s : %d : error_code = 0x%08x \n",__FILE__, __FUNCTION__, __LINE__, err);
	}

	p_stack = (u32 *)mtos_malloc(GS_CAS_SECTION_TASK_STACK_SIZE);


	err = mtos_task_create((u8 *)"CA_Monitor",
	    _Gs_CaDataMonitor,
	    (void *)0,
	    GS_CAS_SECTION_TASK_PRIO,
	    p_stack,
	    GS_CAS_SECTION_TASK_STACK_SIZE);

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
  /*!
  abc
  */
u8 GS_CAS_DRV_GetSCInsertStatus(void)
{
	return s_uScStatus;
}
  /*!
  abc
  */
void GS_CAS_DRV_SetSCInsertStatus(u8 uScStatus)
{
	s_uScStatus = uScStatus;
}




