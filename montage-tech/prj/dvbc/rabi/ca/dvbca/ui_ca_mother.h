/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_MOTHER_H__
#define __UI_CA_MOTHER_H__

/* coordinate */
#define CA_MOTHER_CONT_X        ((SCREEN_WIDTH - CA_MOTHER_CONT_W) / 2)
#define CA_MOTHER_CONT_Y        ((SCREEN_HEIGHT - CA_MOTHER_CONT_H) / 2)
#define CA_MOTHER_CONT_W        SCREEN_WIDTH
#define CA_MOTHER_CONT_H        SCREEN_HEIGHT

//CA frame
#define ACCEPT_CA_MOTHER_FRM_X  SECOND_TITLE_X
#define ACCEPT_CA_MOTHER_FRM_Y  (SECOND_TITLE_Y + SECOND_TITLE_H + 8)
#define ACCEPT_CA_MOTHER_FRM_W  SECOND_TITLE_W
#define ACCEPT_CA_MOTHER_FRM_H  360

//CA info items
#define ACCEPT_CA_MOTHER_TITLE_CNT     4
#define ACCEPT_CA_MOTHER_ITEM_X        10
#define ACCEPT_CA_MOTHER_ITEM_Y        5

#define ACCEPT_CA_MOTHER_ITEM_LW0       260//200
#define ACCEPT_CA_MOTHER_ITEM_RW0       110//100
#define ACCEPT_CA_MOTHER_ITEM_LW1       260//110
#define ACCEPT_CA_MOTHER_ITEM_RW1       110
#define ACCEPT_CA_MOTHER_ITEM_LW2       260//250
#define ACCEPT_CA_MOTHER_ITEM_RW2       110//70
#define ACCEPT_CA_MOTHER_ITEM_LW3       260//250
#define ACCEPT_CA_MOTHER_ITEM_RW3       110//70
#define ACCEPT_CA_MOTHER_ITEM_H        35
#define ACCEPT_CA_MOTHER_ITEM_V_GAP    5

/* rect style */
#define RSI_CA_MOTHER_FRM       RSI_WINDOW_2


RET_CODE open_ca_mother(u32 para1, u32 para2);

#endif
