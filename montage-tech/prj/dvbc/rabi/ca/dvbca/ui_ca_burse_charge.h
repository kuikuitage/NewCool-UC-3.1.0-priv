/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_BURSE_CHARGE_H__
#define __UI_CA_BURSE_CHARGE_H__

/* coordinate */
#define BURSE_INFO_CONT_X        ((SCREEN_WIDTH - BURSE_INFO_CONT_W) / 2)
#define BURSE_INFO_CONT_Y        200
#define BURSE_INFO_CONT_W        COMM_ROOT_W
#define BURSE_INFO_CONT_H        100

#define BURSE_CONT_TITLE_X      50
#define BURSE_CONT_TITLE_Y      0
#define BURSE_CONT_TITLE_W      (BURSE_INFO_CONT_W - 2 * BURSE_CONT_TITLE_X)
#define BURSE_CONT_TITLE_H      100

#define BURSE_CONT_TITLE2_X      50
#define BURSE_CONT_TITLE2_Y      60
#define BURSE_CONT_TITLE2_W      (BURSE_INFO_CONT_W - 2 * BURSE_CONT_TITLE2_X)
#define BURSE_CONT_TITLE2_H      50


#define BURSE_CONT_FULL_X  ((SCREEN_WIDTH-BURSE_CONT_FULL_W)/2)
#define BURSE_CONT_FULL_Y  ((SCREEN_HEIGHT-BURSE_CONT_FULL_H)/2 -20)
#define BURSE_CONT_FULL_W  350
#define BURSE_CONT_FULL_H  100

#define BURSE_TITLE_FULL_X  0
#define BURSE_TITLE_FULL_Y  0
#define BURSE_TITLE_FULL_W  BURSE_CONT_FULL_W
#define BURSE_TITLE_FULL_H  40

#define BURSE_CONTENT_FULL_X  10
#define BURSE_CONTENT_FULL_Y  (BURSE_TITLE_FULL_Y+BURSE_TITLE_FULL_H+5)
#define BURSE_CONTENT_FULL_W  (BURSE_CONT_FULL_W - 2*BURSE_CONTENT_FULL_X)
#define BURSE_CONTENT_FULL_H  50

#define BURSE_TIME_OUT 10000
RET_CODE open_ca_burse_dlg(u32 para1, u32 para2);
void close_ca_burse_dlg(void);

#endif


