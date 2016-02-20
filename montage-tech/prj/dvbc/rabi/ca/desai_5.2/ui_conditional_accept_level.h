/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_LEVEL_H__
#define __UI_CONDITIONAL_ACCEPT_LEVEL_H__


  #define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT   (4)
  #define CONDITIONAL_ACCEPT_LEVEL_HELP_RSC_CNT	                  (15)
  #define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP    20
  //CA frm
  #define CONDITIONAL_ACCEPT_LEVEL_CA_FRM_X             (0)
  #define CONDITIONAL_ACCEPT_LEVEL_CA_FRM_Y             (0)
  #define CONDITIONAL_ACCEPT_LEVEL_CA_FRM_W            (RIGHT_ROOT_W)
  #define CONDITIONAL_ACCEPT_LEVEL_CA_FRM_H             (RIGHT_ROOT_H)
  
  //level control items
  #define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X        (COMM_ITEM_OX_IN_ROOT/2)
  #define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_Y        (COMM_ITEM_OY_IN_ROOT/2)
  #define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW      (COMM_ITEM_LW)
  #define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW      (RIGHT_ROOT_W - COMM_ITEM_LW - COMM_ITEM_OX_IN_ROOT)
  #define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H        (45)
  
  #define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_X        (250)
  #define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_Y        (130)
  #define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W       (160)
  #define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_H        (COMM_CTRL_H)


//change level result
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X         (COMM_ITEM_OX_IN_ROOT/2)
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_Y         (360)
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W        (600)
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H         (COMM_CTRL_H)


/* rect style */
/* font style */
/* others */
#define CONDITIONAL_ACCEPT_WATCH_LEVEL_TOTAL 9

RET_CODE open_conditional_accept_level(u32 para1, u32 para2);

#endif


