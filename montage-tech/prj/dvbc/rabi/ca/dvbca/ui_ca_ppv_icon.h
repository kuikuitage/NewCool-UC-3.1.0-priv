/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_PPV_ICON_H__
#define __UI_CA_PPV_ICON_H__

/* coordinate */
#define PPV_ICON_CONT_X     550
#define PPV_ICON_CONT_Y     90
#define PPV_ICON_CONT_W     105
#define PPV_ICON_CONT_H     31

#define PPV_ICON_TXT_X     2
#define PPV_ICON_TXT_Y     0
#define PPV_ICON_TXT_W     (PPV_ICON_CONT_W - PPV_ICON_TXT_X * 2)
#define PPV_ICON_TXT_H     28

/* rect style */
#define RSI_PPV_ICON_TXT   RSI_GREEN

/* font style */
#define FSI_PPV_ICON_TXT    FSI_WHITE

#define PPV_ICON_AUTOCLOSE_MS 10000

RET_CODE open_ca_ppv_icon(u32 para1, u32 para2);

void close_ca_ppv_icon(void);

#endif


