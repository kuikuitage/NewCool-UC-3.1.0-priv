/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_DVB_CA_CARD_INFO_H__
#define __UI_DVB_CA_CARD_INFO_H__

/* coordinate */
#define ACCEPT_INFO_CONT_X          ((SCREEN_WIDTH - ACCEPT_INFO_CONT_W) / 2)
#define ACCEPT_INFO_CONT_Y          ((SCREEN_HEIGHT - ACCEPT_INFO_CONT_H) / 2)
#define ACCEPT_INFO_CONT_W          SCREEN_WIDTH
#define ACCEPT_INFO_CONT_H          SCREEN_HEIGHT

//CA frame
#define ACCEPT_INFO_CA_FRM_X  SECOND_TITLE_X
#define ACCEPT_INFO_CA_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 8)
#define ACCEPT_INFO_CA_FRM_W  SECOND_TITLE_W
#define ACCEPT_INFO_CA_FRM_H  360

//CA info items
#define ACCEPT_INFO_CARD_INFO_ITEM_CNT      10
#define ACCEPT_INFO_CA_INFO_ITEM_X        (COMM_ITEM_OX_IN_ROOT/2)
#define ACCEPT_INFO_CARDFO_ITEM_Y        5

#define ACCEPT_INFO_CA_INFO_ITEM_LW0       220//200//170
#define ACCEPT_INFO_CA_INFO_ITEM_RW0       200//160
#define ACCEPT_INFO_CA_INFO_ITEM_LW1       220//160
#define ACCEPT_INFO_CA_INFO_ITEM_RW1       200//170
#define ACCEPT_INFO_CA_INFO_ITEM_LW2       220//145
#define ACCEPT_INFO_CA_INFO_ITEM_RW2       200//195
#define ACCEPT_INFO_CA_INFO_ITEM_LW3       220//160
#define ACCEPT_INFO_CA_INFO_ITEM_RW3       200//170
#define ACCEPT_INFO_CA_INFO_ITEM_LW4       220//170
#define ACCEPT_INFO_CA_INFO_ITEM_RW4       200//160
#define ACCEPT_INFO_CA_INFO_ITEM_LW5       220//170
#define ACCEPT_INFO_CA_INFO_ITEM_RW5       200//160
#define ACCEPT_INFO_CA_INFO_ITEM_LW6       220//170
#define ACCEPT_INFO_CA_INFO_ITEM_RW6       200//160
#define ACCEPT_INFO_CA_INFO_ITEM_LW7       220//190
#define ACCEPT_INFO_CA_INFO_ITEM_RW7       200//140
#define ACCEPT_INFO_CA_INFO_ITEM_LW8       220//200
#define ACCEPT_INFO_CA_INFO_ITEM_RW8       200//130

#define ACCEPT_INFO_CA_INFO_ITEM_H        35
#define ACCEPT_CA_INFO_ITEM_V_GAP    5 

/* rect style */

/* font style */

/* others */

RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif


