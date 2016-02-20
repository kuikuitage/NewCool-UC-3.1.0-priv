/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_PAIR_H__
#define __UI_CA_PAIR_H__

#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_CNT	3
#define SN_CONDITIONAL_ACCEPT_PAIR_HELP_RSC_CNT	15
//CA frame
#define SN_CONDITIONAL_ACCEPT_PAIR_CA_FRM_X       106
#define SN_CONDITIONAL_ACCEPT_PAIR_CA_FRM_Y       24
#define SN_CONDITIONAL_ACCEPT_PAIR_CA_FRM_W       505
#define SN_CONDITIONAL_ACCEPT_PAIR_CA_FRM_H       360

//work duration items
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_X        COMM_ITEM_OX_IN_ROOT/2
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_Y        COMM_ITEM_OY_IN_ROOT/2
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_LW       COMM_ITEM_LW
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_RW       RIGHT_ROOT_W - COMM_ITEM_LW - COMM_ITEM_OX_IN_ROOT
#define SN_CONDITIONAL_ACCEPT_PAIR_ITEM_H        35

#define SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_X        250
#define SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_Y        220
#define SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_W        160
#define SN_CONDITIONAL_ACCEPT_PAIR_OK_ITEM_H        COMM_CTRL_H

//change work time result
#define CONDITIONAL_ACCEPT_PAIR_RESULT_X        (COMM_ITEM_OX_IN_ROOT/2)
#define CONDITIONAL_ACCEPT_PAIR_RESULT_Y        (360)
#define CONDITIONAL_ACCEPT_PAIR_RESULT_W       (300)
#define CONDITIONAL_ACCEPT_PAIR_RESULT_H        (COMM_CTRL_H)

RET_CODE open_stb_card_pair_info(u32 para1, u32 para2);

#endif


