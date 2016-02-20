/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_CARD_INFO_H__
#define __UI_CA_CARD_INFO_H__

//CA info items
#define ACCEPT_INFO_CARD_INFO_ITEM_CNT      4

#define ACCEPT_INFO_CA_INFO_ITEM_X        60
#define ACCEPT_INFO_CA_INFO_ITEM_Y        60
#define ACCEPT_INFO_CA_INFO_ITEM_LW       ((SYS_RIGHT_CONT_W-2*ACCEPT_INFO_CA_INFO_ITEM_X)/3)
#define ACCEPT_INFO_CA_INFO_ITEM_RW       (SYS_RIGHT_CONT_W-2*ACCEPT_INFO_CA_INFO_ITEM_X-ACCEPT_INFO_CA_INFO_ITEM_LW)
#define ACCEPT_INFO_CA_INFO_ITEM_H        60

#define ACCEPT_CA_INFO_ITEM_V_GAP    0


RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif


