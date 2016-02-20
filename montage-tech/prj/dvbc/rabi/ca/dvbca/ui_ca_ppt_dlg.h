/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_PPV_H__
#define __UI_CA_PPV_H__

/* coordinate */
#define PPV_INFO_CONT_X        ((SCREEN_WIDTH - PPV_INFO_CONT_W) / 2)
#define PPV_INFO_CONT_Y        150//160
#define PPV_INFO_CONT_W        10
#define PPV_INFO_CONT_H        235//220

#define PPV_CONT_TITLE_X      50 + 25//50
#define PPV_CONT_TITLE_Y      0
#define PPV_CONT_TITLE_W      (PPV_INFO_CONT_W - 2 * PPV_CONT_TITLE_X)-80
#define PPV_CONT_TITLE_H      235//220

#define PPV_CONT_TITLE2_X      50
#define PPV_CONT_TITLE2_Y      (PPV_CONT_TITLE_Y + 60) + 15//(PPV_CONT_TITLE_Y + 60)
#define PPV_CONT_TITLE2_W      (PPV_INFO_CONT_W - 2 * PPV_CONT_TITLE2_X) - 80
#define PPV_CONT_TITLE2_H      50


#define PPV_TEXT_TITLE_X      100
#define PPV_TEXT_TITLE_Y      5 + 10//5
#define PPV_TEXT_TITLE_W      (PPV_CONT_TITLE_W - 2 * PPV_TEXT_TITLE_X)
#define PPV_TEXT_TITLE_H      35


//PPV info items
#define PPV_ACCEPT_CA_INFO_ITEM_CNT      3
#define PPV_ACCEPT_CA_INFO_ITEM_X        67 - 20//67
#define PPV_ACCEPT_CA_INFO_ITEM_Y        40 + 10//40

#define PPV_ACCEPT_CA_INFO_ITEM_LW0       150
#define PPV_ACCEPT_CA_INFO_ITEM_RW0       200
#define PPV_ACCEPT_CA_INFO_ITEM_LW1       150
#define PPV_ACCEPT_CA_INFO_ITEM_RW1       200
#define PPV_ACCEPT_CA_INFO_ITEM_LW2       150
#define PPV_ACCEPT_CA_INFO_ITEM_RW2       200

#define PPV_ACCEPT_CA_INFO_ITEM_H        35
#define PPV_ACCEPT_CA_INFO_ITEM_V_GAP    5

#define PPV_TEXT_BIT_X      140 - 30//140
#define PPV_TEXT_BIT_Y      160 + 20//160
#define PPV_TEXT_BIT_W      80
#define PPV_TEXT_BIT_H      25//35

#define PPV_INFO_ITEM_CNT    5
#define PPV_SET_BTN_CNT      2


/* rect style */
#define RSI_EMAIL_FRM       RSI_WINDOW_2
#define RSI_EMAIL_LIST       RSI_IGNORE
#define RSI_CA_PPV_BTN_HL        RSI_SN_SET_BUTTON_SELECT
#define RSI_CA_PPV_BTN_SH        RSI_SN_SET_BUTTON_SELECT
#define RSI_CA_PPV_N        RSI_WHITE//WHITE

/* font style */
#define FSI_CA_PPV_SH        FSI_INDEX3//WHITE//FSI_INDEX4//BLACK
#define FSI_CA_PPV_N     FSI_WHITE_24
#define FSI_CA_PPV_HL     FSI_WHITE_24
RET_CODE open_ppv_info(u32 para1, u32 para2);
RET_CODE open_ca_ippv_ppt_dlg(u32 para1, u32 para2);

#endif


