/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_OPERATOR_INFO_H__
#define __UI_OPERATOR_INFO_H__

//CA info items
#define OPERATOR_INFO_ITEM_CNT      (4)

#define OPERATOR_INFO_ITEM_X          (COMM_ITEM_OX_IN_ROOT/2)
#define OPERATOR_INFO_ITEM_Y          (COMM_ITEM_OY_IN_ROOT/2)
#define OPERATOR_INFO_ITEM_LW       (COMM_ITEM_LW - 150)
#define OPERATOR_INFO_ITEM_RW       (RIGHT_ROOT_W - OPERATOR_INFO_ITEM_LW - COMM_ITEM_OX_IN_ROOT)
#define OPERATOR_INFO_ITEM_H         (COMM_CTRL_H)

#define ACCEPT_CA_OPER_INFO_ITEM_V_GAP    (20)

#define OPERATOR_INFO_CA_INFO_MAX_LENGTH     (80)
/* rect style */
/* font style */
/* others */

RET_CODE open_ca_operator_info(u32 para1, u32 para2);

#endif


