/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_TR_ACCEPT_LEVEL_H__
#define __UI_TR_ACCEPT_LEVEL_H__

/* coordinate */

//level control items
#define TR_LEVEL_CONTROL_ITEM_CNT      (3)
#define TR_LEVEL_CONTROL_ITEM_X        (COMM_ITEM_OY_IN_ROOT/2)
#define TR_LEVEL_CONTROL_ITEM_Y        (COMM_ITEM_OY_IN_ROOT/2)
#define TR_LEVEL_CONTROL_ITEM_LW       ((SYS_RIGHT_CONT_W - 2*TR_LEVEL_CONTROL_ITEM_X)/3)
#define TR_LEVEL_CONTROL_ITEM_RW       ((SYS_RIGHT_CONT_W - 2*TR_LEVEL_CONTROL_ITEM_X)/3*2)
#define TR_LEVEL_CONTROL_ITEM_H        (COMM_ITEM_H)
#define TR_LEVEL_CONTROL_ITEM_V_GAP    (20)

//change level result
#define TR_ACCEPT_LEVEL_CHANGE_RESULT_X        	((SYS_RIGHT_CONT_W - TR_ACCEPT_LEVEL_CHANGE_RESULT_W - 10)/2)
#define TR_ACCEPT_LEVEL_CHANGE_RESULT_W       	(200)
#define TR_ACCEPT_LEVEL_CHANGE_RESULT_H        	(COMM_ITEM_H)

/* others */
#define TR_ACCEPT_WATCH_LEVEL_TOTAL 9

RET_CODE open_tr_accept_level(u32 para1, u32 para2);

#endif


