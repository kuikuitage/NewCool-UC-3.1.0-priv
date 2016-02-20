/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_INFO_H__
#define __UI_CA_INFO_H__

//TILTLE
#define ITEMMENU_BTN_X          	(0)
#define ITEMMENU_BTN_Y           	(0)
#define ITEMMENU_BTN_W           	((RIGHT_ROOT_W - COMM_ITEM_OY_IN_ROOT/2)/4 - ITEMMENU_BTN_V_GAP)
#define ITEMMENU_BTN_H           	(COMM_ITEM_H)

//mbox
#define CA_MBOX_BTN_X           	(30)
#define CA_MBOX_BTN_Y           	(ITEMMENU_BTN_Y + ITEMMENU_BTN_H + ITEMMENU_BTN_H_GAP)
#define CA_MBOX_BTN_W           	(RIGHT_ROOT_W - COMM_ITEM_OY_IN_ROOT/2 - CA_PLIST_BG_X)
#define CA_MBOX_BTN_H           	(ITEMMENU_BTN_H)


#define ITEMMENU_BTN_V_GAP       	(4)
#define ITEMMENU_BTN_H_GAP       	(0)

//plist
#define CA_PLIST_BG_X  				(30)
#define CA_PLIST_BG_Y  				(CA_MBOX_BTN_Y + CA_MBOX_BTN_H + ITEMMENU_BTN_H_GAP)
#define CA_PLIST_BG_W  				(CA_MBOX_BTN_W)
#define CA_PLIST_BG_H  				(RIGHT_ROOT_H - CA_PLIST_BG_Y - 100)


//other
#define CA_LIST_BG_PAGE         	(8)
#define CA_LIST_BG_FIELD        	(3)
#define CA_LIST_BG1_FIELD      		(6)

#define CA_ITME_COUNT           	(3)
#define CA_ITME_COUNT1         		(6)

//title_item
#define ITEMMENU_CNT    			(4)
#define ITEMMENU_CNT1   			(3)
#define ITEMMENU_CNT2   			(6)

//total ipp
#define CA_TOTAL_IPP_LW  			(150)
#define CA_TOTAL_IPP_RW  			(150)
#define CA_TOTAL_IPP_H  			(30)
#define CA_TOTAL_IPP_GAP  			(10)
#define CA_TOTAL_IPP_X  			((CA_MBOX_BTN_W - CA_TOTAL_IPP_LW - CA_TOTAL_IPP_RW - CA_TOTAL_IPP_GAP)/2)
#define CA_TOTAL_IPP_Y  			(RIGHT_ROOT_H - COMM_ITEM_H)

#define CA_INFO_DETITLE_MAX_NUM     (100)

/* rect style */
RET_CODE open_ca_info(u32 para1, u32 para2);

#endif
