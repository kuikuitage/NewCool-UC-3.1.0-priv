/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_OPERATE_H__
#define __UI_CA_OPERATE_H__

#define CA_OPE_COUNT           				(2)

//mbox
#define CA_OPE_MBOX_X           			(30)
#define CA_OPE_MBOX_Y           			(5)
#define CA_OPE_MBOX_W          				(SYS_RIGHT_CONT_W-2*CA_OPE_MBOX_X)
#define CA_OPE_MBOX_H           			(COMM_ITEM_H)

//plist
#define CA_OPE_LIST_BG_X  					(30)
#define CA_OPE_LIST_BG_Y  					(CA_OPE_MBOX_Y + CA_OPE_MBOX_H + 5)
#define CA_OPE_LIST_BG_W  					(SYS_RIGHT_CONT_W - 90)
#define CA_OPE_LIST_BG_H  					(SYS_RIGHT_CONT_H - CA_OPE_LIST_BG_Y - 30)

//other
#define CA_OPE_LIST_PAGE         12
#define CA_OPE_LIST_FIELD        2

//product id & entitle time
#define CA_INFO_PRODUCT_ID_W            	(80)
#define CA_INFO_PRODUCT_ID_OFFSET         	(15)
#define CA_OPE_ID_W         				(300)
#define CA_OPE_ID_OFFSET					(400)
#define CA_OPE_MBOX_OFFSET					(30)




/* rect style */
RET_CODE open_ca_operate(u32 para1, u32 para2);

#endif
