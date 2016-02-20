/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_IPP_H__
#define __UI_CA_IPP_H__

#define CA_IPP_X             		((SCREEN_WIDTH - CA_IPP_W)/2)
#define CA_IPP_Y             		((SCREEN_HEIGHT - CA_IPP_H)/2)
#define CA_IPP_W             		(830)
#define CA_IPP_H             		(450)

#define CA_IPP_HEAD_X              	(0)
#define CA_IPP_HEAD_Y              	(0)
#define CA_IPP_HEAD_W              	(CA_IPP_W)
#define CA_IPP_HEAD_H              	(36)

#define CA_IPP_FRM_X              	(0)
#define CA_IPP_FRM_Y              	(CA_IPP_HEAD_Y + CA_IPP_HEAD_H + 4)
#define CA_IPP_FRM_W              	(CA_IPP_W)
#define CA_IPP_FRM_H              	(CA_IPP_H - CA_IPP_FRM_Y)


#define CA_IPP_INFO_ITEM_X			(20)
#define CA_IPP_INFO_ITEM_Y			(20)
#define CA_IPP_INFO_ITEM_LW			(200)
#define CA_IPP_INFO_ITEM_RW			(550)
#define CA_IPP_INFO_ITEM_CNT		(5)

#define CA_IPP_INFO_RESULT_X		((CA_IPP_FRM_W - CA_IPP_INFO_RESULT_W)/2)
#define CA_IPP_INFO_RESULT_Y		(CA_IPP_INFO_ITEM_Y + COMM_ITEM_H*CA_IPP_INFO_ITEM_CNT)
#define CA_IPP_INFO_RESULT_W		(600)
#define CA_IPP_INFO_RESULT_H		(COMM_ITEM_H)

#define CA_IPP_INFO_HELP_X			((CA_IPP_FRM_W - CA_IPP_INFO_HELP_W)/2)
#define CA_IPP_INFO_HELP_Y			(CA_IPP_FRM_H - CA_IPP_INFO_HELP_H)
#define CA_IPP_INFO_HELP_W			(CA_IPP_INFO_RESULT_W)
#define CA_IPP_INFO_HELP_H			(CA_IPP_INFO_RESULT_H)

RET_CODE open_ca_ipp(u32 para1, u32 para2);

#endif

