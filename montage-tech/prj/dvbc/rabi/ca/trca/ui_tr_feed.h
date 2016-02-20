/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_TR_FEED_H__
#define __UI_TR_FEED_H__

#define TR_CA_FEED_CONT_PROMPT_X		(90)
#define TR_CA_FEED_CONT_PROMPT_Y		(130)
#define TR_CA_FEED_CONT_PROMPT_W		(780)
#define TR_CA_FEED_CONT_PROMPT_H		(500)

#define TR_CA_FEED_PROMPT_X				(100)
#define TR_CA_FEED_PROMPT_Y				(80)
#define TR_CA_FEED_PROMPT_W				(150)
#define TR_CA_FEED_PROMPT_H				(40)

#define TR_CA_FEED_SUPPORT_X			(350)
#define TR_CA_FEED_SUPPORT_Y			(TR_CA_FEED_PROMPT_Y)
#define TR_CA_FEED_SUPPORT_W			(260)
#define TR_CA_FEED_SUPPORT_H			(TR_CA_FEED_PROMPT_H)

#define TR_CA_FEED_HELP_ICON_X			(320)
#define TR_CA_FEED_HELP_ICON_Y			(TR_CA_FEED_CONT_PROMPT_H - TR_CA_FEED_HELP_ICON_H - 30)
#define TR_CA_FEED_HELP_ICON_W			(40)
#define TR_CA_FEED_HELP_ICON_H			(30)

#define TR_CA_FEED_HELP_TEXT_X			(TR_CA_FEED_HELP_ICON_X + TR_CA_FEED_HELP_ICON_W + TR_CA_FEED_BUTTON_GAP)
#define TR_CA_FEED_HELP_TEXT_Y			(TR_CA_FEED_HELP_ICON_Y)
#define TR_CA_FEED_HELP_TEXT_W			(100)
#define TR_CA_FEED_HELP_TEXT_H			(TR_CA_FEED_HELP_ICON_H)


//cont2
#define TR_CA_FEED_CONT_OPERATE_X		(TR_CA_FEED_CONT_PROMPT_X)
#define TR_CA_FEED_CONT_OPERATE_Y		(TR_CA_FEED_CONT_PROMPT_Y)
#define TR_CA_FEED_CONT_OPERATE_W		(TR_CA_FEED_CONT_PROMPT_W)
#define TR_CA_FEED_CONT_OPERATE_H		(TR_CA_FEED_CONT_PROMPT_H)

#define TR_CA_FEED_TIP_X				(240)
#define TR_CA_FEED_TIP_Y				(60)
#define TR_CA_FEED_TIP_W				(280)
#define TR_CA_FEED_TIP_H				(COMM_ITEM_H)

#define TR_CA_FEED_OK_X					(300)
#define TR_CA_FEED_OK_Y					(TR_CA_FEED_TIP_Y + TR_CA_FEED_TIP_H + TR_CA_FEED_BUTTON_GAP)
#define TR_CA_FEED_OK_W					(160)
#define TR_CA_FEED_OK_H					(COMM_ITEM_H)

#define TR_CA_FEED_CANCEL_X				(TR_CA_FEED_OK_X)
#define TR_CA_FEED_CANCEL_Y				(TR_CA_FEED_OK_Y + TR_CA_FEED_OK_H + TR_CA_FEED_BUTTON_GAP)
#define TR_CA_FEED_CANCEL_W				(TR_CA_FEED_OK_W)
#define TR_CA_FEED_CANCEL_H				(TR_CA_FEED_OK_H)

#define TR_CA_FEED_BUTTON_GAP			(30)



RET_CODE open_tr_feed(u32 para1, u32 para2);

#endif


