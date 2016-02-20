/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_WORKTIME_H__
#define __UI_CONDITIONAL_ACCEPT_WORKTIME_H__

  #define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT	(5)
  #define CONDITIONAL_ACCEPT_WORKTIME_HELP_RSC_CNT	       (15)
  #define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_V_GAP    20
  //CA frame
  #define CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_X       (106)
  #define CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_Y       (24)
  #define CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_W      (505)
  #define CONDITIONAL_ACCEPT_WORKTIME_CA_FRM_H       (420)

  //work duration items
  #define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_X        (10)
  #define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_Y        (20)
  #define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_LW      (260)
  #define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_RW      (140)
  #define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_H         (35)
  
  #define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_X              (250)
  #define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_Y              (160)
  #define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_W             (160)
  #define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_H              (COMM_CTRL_H)

//change work time result
#define CONDITIONAL_ACCEPT_WORKTIME_RESULT_X        (COMM_ITEM_OX_IN_ROOT/2)
#define CONDITIONAL_ACCEPT_WORKTIME_RESULT_Y        (360)
#define CONDITIONAL_ACCEPT_WORKTIME_RESULT_W        (600)
#define CONDITIONAL_ACCEPT_WORKTIME_RESULT_H        (COMM_CTRL_H)



/* rect style */
/* font style */
/* others */
RET_CODE open_conditional_accept_worktime(u32 para1, u32 para2);

#endif


