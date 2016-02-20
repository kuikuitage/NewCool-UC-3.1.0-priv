/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_OPERATE_H__
#define __UI_CA_OPERATE_H__

//TILTLE
#define PROVIDER_BTN_X           (0)
#define PROVIDER_BTN_Y           (0)
#define PROVIDER_BTN_W           (PROVIDER_MBOX_BTN_W/4 - PROVIDER_BTN_V_GAP)
#define PROVIDER_BTN_H           (COMM_ITEM_H)


//mbox
#define PROVIDER_MBOX_BTN_X       (PROVIDER_BTN_X)
#define PROVIDER_MBOX_BTN_Y       (PROVIDER_BTN_Y + PROVIDER_BTN_H + PROVIDER_BTN_H_GAP)
#define PROVIDER_MBOX_BTN_W       (RIGHT_ROOT_W - 40)
#define PROVIDER_MBOX_BTN_H       (PROVIDER_BTN_H)

//plist
#define PROVIDER_PLIST_BG_X  	(PROVIDER_BTN_X)
#define PROVIDER_PLIST_BG_Y  	(PROVIDER_MBOX_BTN_Y + PROVIDER_MBOX_BTN_H + PROVIDER_BTN_H_GAP)
#define PROVIDER_PLIST_BG_W  	(PROVIDER_MBOX_BTN_W)
#define PROVIDER_PLIST_BG_H  	(RIGHT_ROOT_H - PROVIDER_PLIST_BG_Y - 200)


//other
#define PROVIDER_LIST_PAGE         8
#define PROVIDER_LIST_FIELD3       3
#define PROVIDER_LIST_FIELD4       4
#define PROVIDER_LIST_FIELD5       5

#define PROVIDER_BTN_V_GAP       4
#define PROVIDER_BTN_H_GAP       0

#define CAS_MAX_PRODUCT_ENTITLE_NUM_320   320
#define PROVIDER_ITEMMENU_CNT      4

/* rect style */
RET_CODE open_ca_operate(u32 para1, u32 para2);

#endif
