/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_URGENT_H__
#define __UI_CA_URGENT_H__

#define SN_URGENT_X				120
#define SN_URGENT_Y				140
#define SN_URGENT_W  			400
#define SN_URGENT_H   			200
#define SN_URGENT_TITLE_X		0
#define SN_URGENT_TITLE_Y		10
#define SN_URGENT_TITLE_W		SN_URGENT_W
#define SN_URGENT_TITLE_H		30
#define SN_URGENT_CONTENT_X		0
#define SN_URGENT_CONTENT_Y		(SN_URGENT_TITLE_Y + SN_URGENT_TITLE_H + 5)
#define SN_URGENT_CONTENT_W		SN_URGENT_W
#define SN_URGENT_CONTENT_H		(SN_URGENT_H - (SN_URGENT_CONTENT_Y))
#define FSI_URGENT_TXT    FSI_WHITE


RET_CODE open_ca_urgent();

void close_ca_urgent(void);

#endif


