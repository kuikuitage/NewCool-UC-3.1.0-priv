/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_ANTIAUTH_INFO_H__
#define __UI_CA_ANTIAUTH_INFO_H__

//piar info items
#define CA_BURSE_INFO_ITEM_CNT      4

#define CA_BURSE_INFO_ITEM_X        (COMM_ITEM_OX_IN_ROOT/2)
#define CA_BURSE_INFO_ITEM_Y        (COMM_ITEM_OY_IN_ROOT/2)
#define CA_BURSE_INFO_ITEM_LW      (COMM_ITEM_LW)
#define CA_BURSE_INFO_ITEM_RW      (RIGHT_ROOT_W - COMM_ITEM_LW - COMM_ITEM_OX_IN_ROOT)
#define CA_BURSE_INFO_ITEM_H        COMM_CTRL_H

#define CA_BURSE_INFO_ITEM_V_GAP    20


/* rect style */
/* font style */
/* others */

RET_CODE open_ca_antiauth_info(u32 para1, u32 para2);

#endif

