/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_PAIR_H__
#define __UI_CONDITIONAL_ACCEPT_PAIR_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_PAIR_CONT_X          ((COMM_BG_W - CONDITIONAL_ACCEPT_PAIR_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_PAIR_CONT_Y          ((COMM_BG_H - CONDITIONAL_ACCEPT_PAIR_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_PAIR_CONT_W          COMM_BG_W
#define CONDITIONAL_ACCEPT_PAIR_CONT_H          COMM_BG_H


//CA info items
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_CNT      2
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_X        ((CONDITIONAL_ACCEPT_PAIR_CONT_W - (CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_LW + CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_RW))/2)
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_Y        200
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_LW      400
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_RW      500
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_H        35
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_V_GAP    20
#define CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_H_GAP    10

//pair list
#define PAIR_LIST_H        200
#define PAIR_LIST_PAGE         5
#define PAIR_LIST_FIELD      1

//pair list rect
#define PAIR_LIST_MIDL         0
#define PAIR_LIST_MIDT         5
#define PAIR_LIST_MIDW        (CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_RW - 2 * PAIR_LIST_MIDL)
#define PAIR_LIST_MIDH         (PAIR_LIST_H - 2 * PAIR_LIST_MIDT)
#define PAIR_LIST_VGAP         2



/* font style */

/* others */

RET_CODE open_conditional_accept_pair(u32 para1, u32 para2);

#endif


