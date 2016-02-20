/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_ALERT_MESS__
#define __UI_ALERT_MESS__

/* coordinate */
#define ALERT_MENU_X            SYS_RIGHT_CONT_X 
#define ALERT_MENU_Y            SYS_RIGHT_CONT_Y
#define ALERT_MENU_W            SYS_RIGHT_CONT_W 
#define ALERT_MENU_H            SYS_RIGHT_CONT_H 

//items
#define ALERT_MBOX_X        40
#define ALERT_MBOX_Y        (5)
#define ALERT_MBOX_W        (ALERT_MENU_W - 2*ALERT_MBOX_X)
#define ALERT_MBOX_H        COMM_ITEM_H


//plist
#define ALERT_LIST_X        (30)
#define ALERT_LIST_Y        (ALERT_MBOX_Y+ALERT_MBOX_H)
#define ALERT_LIST_W        (ALERT_MENU_W - 90)
#define ALERT_LIST_H        (ALERT_RECEIVED_HEAD_Y - ALERT_LIST_Y)

//plist rect
#define ALERT_LIST_MIDL         10
#define ALERT_LIST_MIDT         10
#define ALERT_LIST_MIDW        (ALERT_LIST_W - ALERT_LIST_MIDL * 3)
#define ALERT_LIST_MIDH         (ALERT_LIST_H - ALERT_LIST_MIDT * 2)
#define ALERT_LIST_VGAP         10

//list scroll bar
#define ALERT_SBAR_X       (ALERT_LIST_X + ALERT_LIST_W - ALERT_LIST_VGAP - ALERT_SBAR_W)
#define ALERT_SBAR_Y       (ALERT_LIST_Y + ALERT_LIST_MIDT)
#define ALERT_SBAR_W       8
#define ALERT_SBAR_H       ALERT_LIST_MIDH

//received head
#define ALERT_RECEIVED_HEAD_X    ALERT_LIST_X+20
#define ALERT_RECEIVED_HEAD_Y    570
#define ALERT_RECEIVED_HEAD_W    80 
#define ALERT_RECEIVED_HEAD_H    30

//received
#define ALERT_RECEIVED_X        (ALERT_RECEIVED_HEAD_X + ALERT_RECEIVED_HEAD_W)
#define ALERT_RECEIVED_Y        ALERT_RECEIVED_HEAD_Y
#define ALERT_RECEIVED_W        100
#define ALERT_RECEIVED_H        ALERT_RECEIVED_HEAD_H

//reset head
#define ALERT_RESET_HEAD_X      (ALERT_RECEIVED_X + ALERT_RECEIVED_W)
#define ALERT_RESET_HEAD_Y      ALERT_RECEIVED_HEAD_Y
#define ALERT_RESET_HEAD_W      120
#define ALERT_RESET_HEAD_H      ALERT_RECEIVED_HEAD_H

//reset
#define ALERT_RESET_X          (ALERT_RESET_HEAD_X+ALERT_RESET_HEAD_W)
#define ALERT_RESET_Y          ALERT_RECEIVED_HEAD_Y
#define ALERT_RESET_W          100
#define ALERT_RESET_H          ALERT_RECEIVED_HEAD_H

#define ALERT_LIST_PAGE         11
#define ALERT_ITEM_OFFSET       5

//del one
#define ALERT_DEL_ICON_X      (ALERT_RESET_X + ALERT_RESET_W + 30)
#define ALERT_DEL_ICON_Y      (ALERT_RECEIVED_HEAD_Y)
#define ALERT_DEL_ICON_W      (50)
#define ALERT_DEL_ICON_H      (ALERT_RECEIVED_HEAD_H)

#define ALERT_DEL_TEXT_X      (ALERT_DEL_ICON_X + ALERT_DEL_ICON_W + 20)
#define ALERT_DEL_TEXT_Y      (ALERT_RECEIVED_HEAD_Y)
#define ALERT_DEL_TEXT_W      (100)
#define ALERT_DEL_TEXT_H      (ALERT_RECEIVED_HEAD_H)

//del all
#define ALERT_DEL_ALL_ICON_X      (ALERT_DEL_TEXT_X + ALERT_DEL_TEXT_W + 40)
#define ALERT_DEL_ALL_ICON_Y      (ALERT_RECEIVED_HEAD_Y)
#define ALERT_DEL_ALL_ICON_W      (50)
#define ALERT_DEL_ALL_ICON_H      (ALERT_RECEIVED_HEAD_H)

#define ALERT_DEL_ALL_TEXT_X      (ALERT_DEL_ALL_ICON_X + ALERT_DEL_ALL_ICON_W + 20)
#define ALERT_DEL_ALL_TEXT_Y      (ALERT_RECEIVED_HEAD_Y)
#define ALERT_DEL_ALL_TEXT_W      (200)
#define ALERT_DEL_ALL_TEXT_H      (ALERT_RECEIVED_HEAD_H)


//others

#define ALERT_LIST_FIELD        5

#define ALERT_ITME_COUNT      5

#define ALERT_MAX_NUMB          100

RET_CODE open_alert_mess(u32 para1, u32 para2);

#endif




