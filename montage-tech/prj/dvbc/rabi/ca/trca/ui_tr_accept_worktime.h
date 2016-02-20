/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_TR_ACCEPT_WORKTIME_H__
#define __UI_TR_ACCEPT_WORKTIME_H__


//work duration items
#define TR_ACCEPT_WORKTIME_WORK_ITEM_CNT    	(4)
#define TR_ACCEPT_WORKTIME_WORK_ITEM_X        	(COMM_ITEM_OY_IN_ROOT/2)
#define TR_ACCEPT_WORKTIME_WORK_ITEM_Y        	(COMM_ITEM_OY_IN_ROOT/2)
#define TR_ACCEPT_WORKTIME_WORK_ITEM_LW       	((SYS_RIGHT_CONT_W - 2*TR_ACCEPT_WORKTIME_WORK_ITEM_X)/3)
#define TR_ACCEPT_WORKTIME_WORK_ITEM_RW       	((SYS_RIGHT_CONT_W - 2*TR_ACCEPT_WORKTIME_WORK_ITEM_X)/3*2)
#define TR_ACCEPT_WORKTIME_WORK_ITEM_H        	(COMM_ITEM_H)
#define TR_ACCEPT_WORKTIME_WORK_ITEM_V_GAP    	(20)

//change work time result
#define TR_ACCEPT_WORKTIME_CHANGE_RESULT_X        ((SYS_RIGHT_CONT_W - TR_ACCEPT_WORKTIME_CHANGE_RESULT_W - 10)/2)
#define TR_ACCEPT_WORKTIME_CHANGE_RESULT_W        (200)
#define TR_ACCEPT_WORKTIME_CHANGE_RESULT_H        (COMM_ITEM_H)

/* rect style */
#define RSI_TR_ACCEPT_WORKTIME_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_tr_accept_worktime(u32 para1, u32 para2);

#endif


