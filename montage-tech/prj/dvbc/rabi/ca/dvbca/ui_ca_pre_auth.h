/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_PRE_AUTH_H__
#define __UI_CA_PRE_AUTH_H__

/* coordinate */
#define PRE_AUTH_CONT_X     550
#define PRE_AUTH_CONT_Y     125
#define PRE_AUTH_CONT_W     105
#define PRE_AUTH_CONT_H     30

#define PRE_AUTH_TXT_X     2
#define PRE_AUTH_TXT_Y     0
#define PRE_AUTH_TXT_W     (PRE_AUTH_CONT_W - PRE_AUTH_TXT_X * 2)
#define PRE_AUTH_TXT_H     28

/* rect style */
#define RSI_PRE_AUTH_TXT   RSI_GREEN

/* font style */
#define FSI_PRE_AUTH_TXT    FSI_WHITE

#define PRE_AUTH_AUTOCLOSE_MS 3000


RET_CODE open_ca_pre_auth(u32 para1, u32 para2);

void close_ca_pre_auth(void);

#endif


