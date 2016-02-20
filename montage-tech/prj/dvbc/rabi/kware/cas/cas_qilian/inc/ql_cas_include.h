/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#ifndef __QI_CAS_INCLUDE_H__
#define __QI_CAS_INCLUDE_H__


#define CAS_QILIAN_DEBUG

#ifdef CAS_QILIAN_DEBUG
#define CAS_QILIAN_PRINTF OS_PRINTF
#else
#define CAS_QILIAN_PRINTF DUMMY_PRINTF
#endif


typedef signed char INT8;
typedef signed short INT16;
typedef signed long  INT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long  UINT32;
typedef unsigned char  CAS_BOOL;


#define QL_FILTER_MAX_NUM              8
//#define QL_CAS_SECTION_TASK_PRIO        45
#define QL_FILTER_DEPTH_SIZE          8
#define QL_FILTER_BUFFER_SIZE (16 * (KBYTES) + 188)
#define QL_MAX_MSG_QUEUE 2
#define MSG_CNT 20
#define QL_CAS_SECTION_TASK_STACK_SIZE    (16 * 1024)

/*!
  abc
  */
typedef enum
{
/*!
    ABC
  */
  SC_REMOVE_START,
  /*!
    ABC
  */
  SC_REMOVE_OVER,
  /*!
    ABC
  */
  SC_INSERT_START,
    /*!
    ABC
  */
  SC_INSERT_OVER,
  /*!
    ABC
  */
  SC_STATUS_END
}e_smartcard_insert_t;

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
  } ql_filter_status_t;





  typedef struct
  {
  /*!
  abc
    */
  ql_filter_status_t  m_FilterStatus;
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
  u8  m_uFilterData[QL_FILTER_DEPTH_SIZE];
  /*!
  abc
    */
  u8  m_uFilterMask[QL_FILTER_DEPTH_SIZE];
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
  }ql_filter_struct_t;


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
  }ql_data_got_t;



/*!
  abc
  */
typedef struct
{
  /*!
    abc
    */
  UINT8 slot;
  /*!
    1:current program scramble, 0:current program not scaramble
    */
  UINT8 scramble_flag;
  /*!
    abc
    */
  UINT16 network_id;
  /*!
    abc
    */
  UINT16 server_id;
  /*!
    abc
    */
   UINT16 ecm_pid;
  /*!
    abc
    */
   UINT16 emm_pid;
  /*!
    abc
    */
  UINT16 v_pid;
  /*!
    abc
    */
  UINT16 a_pid;
  /*!
    The callback of nvram reading function
    */
  RET_CODE (*nvram_read)(UINT32 offset,
              UINT8 *p_buf, UINT32 *size);
  /*!
    The callback of nvram writing function
    */
  RET_CODE (*nvram_write)(UINT32 offset,
              UINT8 *p_buf, UINT32 size);
     /*!
   flash_start_adr
    */
  UINT32 flash_start_adr;
    /*!
   flash_size
    */
  UINT32 flash_size;
  /*!
   stb and card pair flash_start_adr
    */
  u32 stb_card_pair_start_adr;
  /*!
   flash_size
    */
  u32 stb_card_pair_size;  
  /*!
   ads flash_start_adr
    */
  u32 ads_flash_start_adr;
  /*!
   ads_size
    */
  u32 ads_flash_size;
}cas_ql_priv_t;


#endif

