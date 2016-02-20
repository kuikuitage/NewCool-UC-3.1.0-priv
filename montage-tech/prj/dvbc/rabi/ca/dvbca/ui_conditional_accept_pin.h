/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_PIN_H__
#define __UI_CONDITIONAL_ACCEPT_PIN_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_PIN_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_PIN_CONT_W) / 2)
#define CONDITIONAL_ACCEPT_PIN_CONT_Y          ((SCREEN_HEIGHT - CONDITIONAL_ACCEPT_PIN_CONT_H) / 2)
#define CONDITIONAL_ACCEPT_PIN_CONT_W          SCREEN_WIDTH
#define CONDITIONAL_ACCEPT_PIN_CONT_H          SCREEN_HEIGHT

//CA frame
#define CONDITIONAL_ACCEPT_PIN_CA_FRM_X  SECOND_TITLE_X
#define CONDITIONAL_ACCEPT_PIN_CA_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 8)
#define CONDITIONAL_ACCEPT_PIN_CA_FRM_W  SECOND_TITLE_W
#define CONDITIONAL_ACCEPT_PIN_CA_FRM_H  360

//pin modify items
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT      4
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X        10
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_Y        20
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW       140
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW       140
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H        35
#define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP    4

#define CONDITIONAL_ACCEPT_PIN_OK_ITEM_X        150
#define CONDITIONAL_ACCEPT_PIN_OK_ITEM_Y        160
#define CONDITIONAL_ACCEPT_PIN_OK_ITEM_W        50
#define CONDITIONAL_ACCEPT_PIN_OK_ITEM_H        35

//change pin result
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_X        50
#ifndef CONDITIONAL_ACCEPT_CHANGE_RESULT_Y
//#define CONDITIONAL_ACCEPT_CHANGE_RESULT_Y        200
#endif
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_W        260
#define CONDITIONAL_ACCEPT_CHANGE_RESULT_H        90

/* rect style */

/* font style */

/* others */

RET_CODE open_cas_accept_pin(u32 para1, u32 para2);

#endif


