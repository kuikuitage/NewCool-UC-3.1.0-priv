/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_TR_ACCEPT_PIN_H__
#define __UI_TR_ACCEPT_PIN_H__

//pin modify items
#define TR_ACCEPT_PIN_ITEM_CNT      4

#define TR_ACCEPT_PIN_ITEM_X        (COMM_ITEM_OX_IN_ROOT/2)
#define TR_ACCEPT_PIN_ITEM_Y        (COMM_ITEM_OX_IN_ROOT/2)
#define TR_ACCEPT_PIN_ITEM_LW       (SYS_RIGHT_CONT_W - 2*TR_ACCEPT_PIN_ITEM_X)/3
#define TR_ACCEPT_PIN_ITEM_RW       (SYS_RIGHT_CONT_W - 2*TR_ACCEPT_PIN_ITEM_X)/3*2
#define TR_ACCEPT_PIN_ITEM_H        (COMM_ITEM_H)

#define TR_ACCEPT_PIN_ITEM_V_GAP    (20)

//change pin result
#define TR_ACCEPT_CHANGE_RESULT_X        (SYS_RIGHT_CONT_W - TR_ACCEPT_CHANGE_RESULT_W - 10)/2
#define TR_ACCEPT_CHANGE_RESULT_W        (200)
#define TR_ACCEPT_CHANGE_RESULT_H        (COMM_ITEM_H)

RET_CODE open_tr_accept_pin(u32 para1, u32 para2);

#endif


