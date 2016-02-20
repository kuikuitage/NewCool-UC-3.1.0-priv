/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __CONFIG_CAS_H__
#define __CONFIG_CAS_H__

#define AP_CAS_ID   CAS_ID_TF
#define CONFIG_CAS_ID   CONFIG_CAS_ID_TF
#define CDCAS_CA_STDID_SIZE 8   

#ifndef CAS_CONFIG_CDCAS
#define CAS_CONFIG_CDCAS
#endif

RET_CODE machine_serial_get(u8 *p_buf, u32 size);
void on_cas_init(void);
#endif


