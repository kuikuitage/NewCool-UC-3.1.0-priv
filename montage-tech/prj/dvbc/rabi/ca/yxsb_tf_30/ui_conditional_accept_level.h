/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_LEVEL_H__
#define __UI_CONDITIONAL_ACCEPT_LEVEL_H__

/* coordinate */

//level control items
#define TF_LEVEL_CONTROL_ITEM_CNT      3
#define TF_LEVEL_CONTROL_ITEM_X        (COMM_ITEM_OY_IN_ROOT/2)
#define TF_LEVEL_CONTROL_ITEM_Y        (COMM_ITEM_OY_IN_ROOT/2)
#define TF_LEVEL_CONTROL_ITEM_LW       400
#define TF_LEVEL_CONTROL_ITEM_RW       400
#define TF_LEVEL_CONTROL_ITEM_H        40
#define TF_LEVEL_CONTROL_ITEM_V_GAP    20

#define TF_CHANGE_OK_X        140
#define TF_CHANGE_OK_Y        130
#define TF_CHANGE_OK_W        60
#define TF_CHANGE_OK_H        35

//change level result
#define TF_CHANGE_RESULT_X        50
#define TF_CHANGE_RESULT_Y        250
#define TF_CHANGE_RESULT_W       260
#define TF_CHANGE_RESULT_H        90

/* rect style */
#define RSI_TF_FRM       				RSI_WINDOW_1
/* font style */

/* others */
#define TF_WATCH_LEVEL_TOTAL 			(15)

RET_CODE open_conditional_accept_level(u32 para1, u32 para2);

#endif


