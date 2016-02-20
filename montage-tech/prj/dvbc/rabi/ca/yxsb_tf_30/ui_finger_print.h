/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_FINGER_PRINT_H__
#define __UI_FINGER_PRINT_H__

#define FINGER_PRINT_X   0
#define FINGER_PRINT_Y   0
#define FINGER_PRINT_W   220
#define FINGER_PRINT_H   30

//finger
#define FINGER_PRINT_TEXT_X  0
#define FINGER_PRINT_TEXT_Y  0
#define FINGER_PRINT_TEXT_W  FINGER_PRINT_W
#define FINGER_PRINT_TEXT_H  FINGER_PRINT_H

#define FINGER_PRINT_TIMEOUT 20000

/* rect style */
#define RSI_FINGER_PRINT_FRAME RSI_PBACK

#define RSI_FINGER_PRINT_TXT RSI_IGNORE

/*font style*/
#define FSI_FINGER FSI_WHITE

RET_CODE open_finger_print (u32 para1, u32 para2);

RET_CODE close_finger_print(void);

#endif

