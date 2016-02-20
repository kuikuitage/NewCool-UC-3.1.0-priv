/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_CHARACTER_CODE_H__
#define __UI_CA_CHARACTER_CODE_H__

/* coordinate */
#define ACCEPT_CHARACTER_CONT_X        ((SCREEN_WIDTH - ACCEPT_CHARACTER_CONT_W) / 2)
#define ACCEPT_CHARACTER_CONT_Y        ((SCREEN_HEIGHT - ACCEPT_CHARACTER_CONT_H) / 2)
#define ACCEPT_CHARACTER_CONT_W        SCREEN_WIDTH
#define ACCEPT_CHARACTER_CONT_H        SCREEN_HEIGHT

//CA frame
#define ACCEPT_CHARACTER_CODE_X  0
#define ACCEPT_CHARACTER_CODE_Y  (0 + 0 + 8)
#define ACCEPT_CHARACTER_CODE_W  10
#define ACCEPT_CHARACTER_CODE_H  360

//CA info items
#define ACCEPT_CHARACTER_CODE_NUB_X  5
#define ACCEPT_CHARACTER_CODE_NUB_Y  5
#define ACCEPT_CHARACTER_CODE_NUB_W  (ACCEPT_CHARACTER_CODE_W - 2 * ACCEPT_CHARACTER_CODE_NUB_X)
#define ACCEPT_CHARACTER_CODE_NUB_H  100

/* rect style */
#define RSI_CA_MOTHER_FRM       RSI_PBACK

#define CARD_EIGEN_VALU_MAX_LEN    32

RET_CODE open_ca_character_code(u32 para1, u32 para2);

#endif
