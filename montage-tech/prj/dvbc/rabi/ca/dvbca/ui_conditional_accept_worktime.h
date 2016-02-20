/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_WORKTIME_H__
#define __UI_CONDITIONAL_ACCEPT_WORKTIME_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_WORKTIME_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_WORKTIME_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_WORKTIME_CONT_Y          ((SCREEN_HEIGHT - CONDITIONAL_ACCEPT_WORKTIME_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_WORKTIME_CONT_W          SCREEN_WIDTH
#define CONDITIONAL_ACCEPT_WORKTIME_CONT_H          SCREEN_HEIGHT

//CA frame
#define CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_X  SECOND_TITLE_X
#define CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 8)
#define CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_W  SECOND_TITLE_W
#define CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_H  360

//work duration items
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT      4
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_X        10
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_Y        20
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_LW       140
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_RW       140
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_H        35
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_V_GAP    4

#define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_X        120
#define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_Y        160
#define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_W        60
#define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_H        35

//change work time result
#define CONDITIONAL_ACCEPT_WORKTIME_RESULT_X        (COMM_ITEM_OX_IN_ROOT/2)
#define CONDITIONAL_ACCEPT_WORKTIME_RESULT_Y        (360)
#define CONDITIONAL_ACCEPT_WORKTIME_RESULT_W        (300)
#define CONDITIONAL_ACCEPT_WORKTIME_RESULT_H        (COMM_CTRL_H)

/* rect style */
#define RSI_CONDITIONAL_ACCEPT_WORKTIME_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_cas_accept_worktime(u32 para1, u32 para2);

#endif


