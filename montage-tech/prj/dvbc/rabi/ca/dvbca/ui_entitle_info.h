/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_ENTITLE_INFO_H__
#define __UI_CA_ENTITLE_INFO_H__

/*cont*/
#define CA_CONT_X          0
#define CA_CONT_Y          ((SCREEN_HEIGHT - CA_CONT_H) / 2)
#define CA_CONT_W          SCREEN_WIDTH
#define CA_CONT_H          SCREEN_HEIGHT

//TILTLE
#define ENTITLE_TITLE_X       WHOLE_SCR_TITLE_X
#define ENTITLE_TITLE_Y       WHOLE_SCR_TITLE_Y
#define ENTITLE_TITLE_W      WHOLE_SCR_TITLE_W
#define ENTITLE_TITLE_H       35

//plist
#define CA_PLIST_BG_X  ENTITLE_TITLE_X
#define CA_PLIST_BG_Y  (RIGHT_BG_Y + CA_MBOX_H)
#define CA_PLIST_BG_W  ENTITLE_TITLE_W
#define CA_PLIST_BG_H  (RIGHT_BG_H - CA_MBOX_H)

//mbox
#define CA_MBOX_X           ENTITLE_TITLE_X
#define CA_MBOX_Y           RIGHT_BG_Y
#define CA_MBOX_W          ENTITLE_TITLE_W
#define CA_MBOX_H           40
#define CA_MBOX_MID_X   20
#define CA_MBOX_MID_W  (CA_MBOX_W - 100)
#define CA_MBOX_MID_H   CA_MBOX_H

//plist rect
#define CA_LIST_BG_MIDL         4
#define CA_LIST_BG_MIDT         4//10
#define CA_LIST_BG_MIDW        (CA_PLIST_BG_W-CA_LIST_BG_MIDL*2)//(CA_PLIST_BG_W-20)
#define CA_LIST_BG_MIDH        (CA_PLIST_BG_H-CA_LIST_BG_MIDT*2)//(CA_PLIST_BG_H-20)
#define CA_LIST_BG_VGAP         3///2
//other
#define CA_INFO_LIST_PAGE         11
#define CA_INFO_ENTITLE_FIELD        2

#define CA_ITME_COUNT           2



/* rect style */
RET_CODE open_dvb_ca_entitle_info(u32 para1,u32 para2);

#endif
