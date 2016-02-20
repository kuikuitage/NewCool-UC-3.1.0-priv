/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_INFO_H__
#define __UI_CONDITIONAL_ACCEPT_INFO_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_INFO_CONT_X          ((SCREEN_WIDTH - CONDITIONAL_ACCEPT_INFO_ITEM_LW - CONDITIONAL_ACCEPT_INFO_ITEM_RW) / 2)
#define CONDITIONAL_ACCEPT_INFO_CONT_Y          200
#define CONDITIONAL_ACCEPT_INFO_CONT_W          COMM_BG_W
#define CONDITIONAL_ACCEPT_INFO_CONT_H          COMM_BG_H

//CA info items
#define CONDITIONAL_ACCEPT_INFO_ITEM_CNT      4

#define CONDITIONAL_ACCEPT_INFO_ITEM_X        CONDITIONAL_ACCEPT_INFO_CONT_X
#define CONDITIONAL_ACCEPT_INFO_ITEM_Y        CONDITIONAL_ACCEPT_INFO_CONT_Y
#define CONDITIONAL_ACCEPT_INFO_ITEM_LW       300
#define CONDITIONAL_ACCEPT_INFO_ITEM_RW       600
#define CONDITIONAL_ACCEPT_INFO_ITEM_H        COMM_CTRL_H

#define ACCEPT_CA_OPER_INFO_ITEM_V_GAP    20

#define CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH     80
/* rect style */

/* font style */

/* others */

RET_CODE open_conditional_accept_info(u32 para1, u32 para2);

#endif


