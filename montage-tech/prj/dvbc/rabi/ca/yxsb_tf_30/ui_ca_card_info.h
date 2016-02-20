/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_CARD_INFO_H__
#define __UI_CA_CARD_INFO_H__

//CA info items
#define TF_CARD_INFO_ITEM_CNT      			(13)

#define ACCEPT_INFO_CA_INFO_ITEM_X          (COMM_ITEM_OX_IN_ROOT/2)
#define ACCEPT_INFO_CA_INFO_ITEM_Y          (COMM_ITEM_OY_IN_ROOT/2)
#define ACCEPT_INFO_CA_INFO_ITEM_LW       	(COMM_ITEM_LW)
#define ACCEPT_INFO_CA_INFO_ITEM_RW       	(RIGHT_ROOT_W - COMM_ITEM_LW - COMM_ITEM_OX_IN_ROOT)
#define ACCEPT_INFO_CA_INFO_ITEM_H         	(40)

#define ACCEPT_CA_INFO_ITEM_V_GAP    0
#define TF_CA_INFO_PAIR_LIST_NUM	        (5)


RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif


