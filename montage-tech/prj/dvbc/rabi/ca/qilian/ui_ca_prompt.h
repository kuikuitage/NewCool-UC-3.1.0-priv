/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_PROMPT_H__
#define __UI_CA_PROMPT_H__

#define CA_PROMPT_X             SYS_RIGHT_CONT_X
#define CA_PROMPT_Y             SYS_RIGHT_CONT_H
#define CA_PROMPT_W             SYS_RIGHT_CONT_W
#define CA_PROMPT_H             SYS_RIGHT_CONT_H

#define CA_PROMPT_COLUMN0_W                     130
#define CA_PROMPT_COLUMN1_W                     120
#define CA_PROMPT_COLUMN2_W                     130
#define CA_PROMPT_COLUMN3_W                     120

#define CA_PROMPT_COLUMN_HGAP                   6
#define CA_PROMPT_ROW_VGAP                      10

//signal strength0
#define SIG_STRENGTH_NAME_X       90
#define SIG_STRENGTH_NAME_Y       370
#define SIG_STRENGTH_NAME_W       120
#define SIG_STRENGTH_NAME_H       30

#define SIG_STRENGTH_PERCENT_X    (SIG_STRENGTH_NAME_X + SIG_STRENGTH_NAME_W)
#define SIG_STRENGTH_PERCENT_Y    SIG_STRENGTH_NAME_Y
#define SIG_STRENGTH_PERCENT_W    100
#define SIG_STRENGTH_PERCENT_H    30

#define SIG_STRENGTH_PBAR_X    (SIG_STRENGTH_PERCENT_X + SIG_STRENGTH_PERCENT_W)
#define SIG_STRENGTH_PBAR_Y    (SIG_STRENGTH_NAME_Y + (SIG_STRENGTH_NAME_H - SIG_STRENGTH_PBAR_H)/2)
#define SIG_STRENGTH_PBAR_W       230
#define SIG_STRENGTH_PBAR_H       16

//signal strength1
#define SIG_SET_NAME_X       SIG_STRENGTH_NAME_X
#define SIG_SET_NAME_Y       (SIG_STRENGTH_NAME_Y+SIG_STRENGTH_NAME_H)
#define SIG_SET_NAME_W       120
#define SIG_SET_NAME_H       30

#define SIG_SET_PERCENT_X    (SIG_SET_NAME_X + SIG_SET_NAME_W)
#define SIG_SET_PERCENT_Y    SIG_SET_NAME_Y
#define SIG_SET_PERCENT_W    100
#define SIG_SET_PERCENT_H    30

#define SIG_SET_PBAR_X       (SIG_SET_PERCENT_X + SIG_SET_PERCENT_W)
#define SIG_SET_PBAR_Y       (SIG_SET_NAME_Y + (SIG_SET_NAME_H - SIG_SET_PBAR_H)/2)
#define SIG_SET_PBAR_W       230
#define SIG_SET_PBAR_H       16

//signal strength2
#define SIG_SNR_NAME_X       SIG_STRENGTH_NAME_X
#define SIG_SNR_NAME_Y       (SIG_SET_NAME_Y+SIG_STRENGTH_NAME_H)
#define SIG_SNR_NAME_W       120
#define SIG_SNR_NAME_H       30

#define SIG_SNR_PERCENT_X    (SIG_SNR_NAME_X + SIG_SNR_NAME_W)
#define SIG_SNR_PERCENT_Y    SIG_SNR_NAME_Y
#define SIG_SNR_PERCENT_W    100
#define SIG_SNR_PERCENT_H    30

#define SIG_SNR_PBAR_X       (SIG_SNR_PERCENT_X + SIG_SNR_PERCENT_W)
#define SIG_SNR_PBAR_Y       (SIG_SNR_NAME_Y + (SIG_SNR_NAME_H - SIG_SNR_PBAR_H)/2)
#define SIG_SNR_PBAR_W       230
#define SIG_SNR_PBAR_H       16

RET_CODE open_ca_prompt(u32 para1, u32 para2);

#endif

