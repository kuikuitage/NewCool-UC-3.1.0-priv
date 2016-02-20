/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_ENTITLE_INFO_H__
#define __UI_CA_ENTITLE_INFO_H__

//mbox
#define ENTITLE_INFO_MBOX_X           (5)
#define ENTITLE_INFO_MBOX_Y           (5)
#define ENTITLE_INFO_MBOX_W          (SYS_RIGHT_CONT_W-2*ENTITLE_INFO_MBOX_X)
#define ENTITLE_INFO_MBOX_H           (COMM_ITEM_H)

//plist
#define ENTITLE_INFO_PLIST_X         (0)
#define ENTITLE_INFO_PLIST_Y         (ENTITLE_INFO_MBOX_Y + ENTITLE_INFO_MBOX_H)
#define ENTITLE_INFO_PLIST_W        (SYS_RIGHT_CONT_W)
#define ENTITLE_INFO_PLIST_H         (SYS_RIGHT_CONT_H - ENTITLE_INFO_PLIST_Y - 30)

//plist rect
#define ENTITLE_INFO_LIST_MIDL         (10)
#define ENTITLE_INFO_LIST_MIDT         (10)
#define ENTITLE_INFO_LIST_MIDW        (ENTITLE_INFO_PLIST_W - 2*ENTITLE_INFO_LIST_MIDL)
#define ENTITLE_INFO_LIST_MIDH         (ENTITLE_INFO_PLIST_H - 2*ENTITLE_INFO_LIST_MIDT)
#define ENTITLE_INFO_LIST_VGAP         (12)

//other
#define ENTITLE_INFO_LIST_PAGE         (12)
#define ENTITLE_INFO_LIST_FIELD        (2)

//product id & entitle time
#define ENTITLE_INFO_PRODUCT_ID_ITEM_OFFSET 40
#define ENTITLE_INFO_ITME_COUNT           2


/* rect style */
RET_CODE open_ca_entitle_info(u32 para1, u32 para2);

#endif
