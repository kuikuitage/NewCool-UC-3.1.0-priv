/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_CA_ORDER_IPP_RTIME_H__
#define __UI_CA_ORDER_IPP_RTIME_H__
#if 1
/* mainwin */
#define IPP_RTIME_IPP_CONT_X			((SCREEN_WIDTH-IPP_RTIME_IPP_CONT_W)/2)
#define IPP_RTIME_IPP_CONT_Y			((SCREEN_HEIGHT-IPP_RTIME_IPP_CONT_H)/2)
#define IPP_RTIME_IPP_CONT_W			(600)
#define IPP_RTIME_IPP_CONT_H			(410)

/* coordinate */
#define IPP_RTIME_IPP_X			0
#define IPP_RTIME_IPP_Y			0
#define IPP_RTIME_IPP_W		(600)
#define IPP_RTIME_IPP_H			410

#define IPP_RTIME_STRIP_X 			(4)
#define IPP_RTIME_STRIP_W 			(IPP_RTIME_IPP_W-8)
#define IPP_RTIME_STRIP_H 			(35)
#define IPP_RTIME_STRIP1_Y 			(36)

#define IPP_PROG_INFO_CONT_X 		(8)
#define IPP_PROG_INFO_CONT_Y 		(IPP_RTIME_STRIP1_Y +4)
#define IPP_PROG_INFO_CONT_W 		(IPP_RTIME_STRIP_W)
#define IPP_PROG_INFO_CONT_H 		(370)

#define IPP_PROG_INFO_COUNT 		(6)

#define IPP_PROG_ITM_W 200

#define FOR_RTIME__X 4 
#define FOR_RTIME_TIPS_CONT_X 4 
#define FOR_RTIME_TIPS_CONT_Y  (IPP_PROG_INFO_CONT_Y + IPP_PROG_INFO_CONT_H +4)
#define FOR_RTIME_TIPS_CONT_W IPP_RTIME_STRIP_W
#define FOR_RTIME_TIPS_CONT_H 40 

#define IPP_RTIME_ITM_X 0
#define IPP_RTIME_ITM_Y 5
#define IPP_RTIME_ITM_W 260
#define IPP_RTIME_ITM_H IPP_RTIME_STRIP_H
#define IPP_RTIME_ITM_PIN_W_L 200
#define IPP_RTIME_ITM_PIN_W_R 80
#endif

RET_CODE open_real_time_ipp(u32 para1, u32 para2);
RET_CODE on_ipp_pin_select_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE on_ca_real_time_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2);

#endif
