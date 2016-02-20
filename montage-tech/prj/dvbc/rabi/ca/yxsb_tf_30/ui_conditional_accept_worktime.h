/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_WORKTIME_H__
#define __UI_CONDITIONAL_ACCEPT_WORKTIME_H__


//work duration items
#define TF_WORKTIME_WORK_ITEM_CNT    					(4)

#define TF_WORKTIME_WORK_ITEM_X        	(COMM_ITEM_OY_IN_ROOT/2)
#define TF_WORKTIME_WORK_ITEM_Y        	(COMM_ITEM_OY_IN_ROOT/2)
#define TF_WORKTIME_WORK_ITEM_LW       	(400)
#define TF_WORKTIME_WORK_ITEM_RW       	(400)
#define TF_WORKTIME_WORK_ITEM_H        	(40)
#define TF_WORKTIME_WORK_ITEM_V_GAP    	(20)

#define TF_WORKTIME_OK_ITEM_X        	(120)
#define TF_WORKTIME_OK_ITEM_Y        	(180)
#define TF_WORKTIME_OK_ITEM_W        	(60)
#define TF_WORKTIME_OK_ITEM_H        	(35)

//change work time result
#define TF_WORKTIME_CHANGE_RESULT_X     (10)
#define TF_WORKTIME_CHANGE_RESULT_Y     (300)
#define TF_WORKTIME_CHANGE_RESULT_W     (260)
#define TF_WORKTIME_CHANGE_RESULT_H     (35)

/* rect style */
#define RSI_TF_WORKTIME_FRM       RSI_WINDOW_1

RET_CODE open_conditional_accept_worktime(u32 para1, u32 para2);

#endif


