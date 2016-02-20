/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_MOTHER_H__
#define __UI_CONDITIONAL_ACCEPT_MOTHER_H__


//items
#define CAS_MOTHER_TITLE_MBOX_X        	(5)
#define CAS_MOTHER_TITLE_MBOX_Y       	(5)
#define CAS_MOTHER_TITLE_MBOX_W        	(RIGHT_ROOT_W - CAS_MOTHER_TITLE_MBOX_X*2 - 40)
#define CAS_MOTHER_TITLE_MBOX_H        	(COMM_ITEM_H)

//title
#define CAS_MOTHER_TITLE_CNT      5
#define CAS_MOTHER_TITLE_OFFSET  10

//list
#define CAS_MOTHER_LIST_X             	(CAS_MOTHER_TITLE_MBOX_X)
#define CAS_MOTHER_LIST_Y             	(CAS_MOTHER_TITLE_MBOX_Y + CAS_MOTHER_TITLE_MBOX_H + 5)
#define CAS_MOTHER_LIST_W            	(CAS_MOTHER_TITLE_MBOX_W)
#define CAS_MOTHER_LIST_H             	(RIGHT_ROOT_H - CAS_MOTHER_LIST_Y - 72)


/*others*/
#define CAS_MOTHER_LIST_PAGE          	(12)
#define CAS_MOTHER_LIST_FIELD_NUM      	(5)

RET_CODE open_conditional_accept_mother(u32 para1, u32 para2);

#endif


