/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_CURTAIN__
#define __UI_CA_CURTAIN__

#define CA_CURATIN_CONT_FULL_X  ((SCREEN_WIDTH-CA_CURATIN_CONT_FULL_W)/2)
#define CA_CURATIN_CONT_FULL_Y  50
#define CA_CURATIN_CONT_FULL_W  600
#define CA_CURATIN_CONT_FULL_H  150


RET_CODE open_ca_card_curtain(u32 para1, u32 para2);
void close_ca_card_curtain(void);
BOOL ui_is_curtain(void);

#endif