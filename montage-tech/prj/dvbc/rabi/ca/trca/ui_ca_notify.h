/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_NOTIFY_H__
#define __UI_CA_NOTIFY_H__

#define CA_NOTIFY_X                                 ((SCREEN_WIDTH - CA_NOTIFY_W)/2)
#define CA_NOTIFY_Y                                 ((SCREEN_HEIGHT - CA_NOTIFY_H)/2)
#define CA_NOTIFY_W                                 (810)
#define CA_NOTIFY_H                                 (430)

#define CA_NOTIFY_CONT_X                            (0)
#define CA_NOTIFY_CONT_Y                            (0)
#define CA_NOTIFY_CONT_W                            (CA_NOTIFY_W - CA_NOTIFY_CONT_X*2)
#define CA_NOTIFY_CONT_H                            (CA_NOTIFY_H - CA_NOTIFY_CONT_Y*2)

#define CA_NOTIFY_MES_TYPE_X                        (0)
#define CA_NOTIFY_MES_TYPE_Y                        (CA_NOTIFY_MES_VGAP)
#define CA_NOTIFY_MES_TYPE_W                        (200)
#define CA_NOTIFY_MES_TYPE_H                        (30)

#define CA_NOTIFY_MES_TITLE_X                       (20)
#define CA_NOTIFY_MES_TITLE_Y                       (CA_NOTIFY_MES_TYPE_X + CA_NOTIFY_MES_TYPE_H)
#define CA_NOTIFY_MES_TITLE_W                       (CA_NOTIFY_W - CA_NOTIFY_MES_TITLE_X*2)
#define CA_NOTIFY_MES_TITLE_H                       (30)

#define CA_NOTIFY_MES_CONTENT_X                     (CA_NOTIFY_MES_TITLE_X)
#define CA_NOTIFY_MES_CONTENT_Y                     (CA_NOTIFY_MES_TITLE_Y + CA_NOTIFY_MES_TITLE_H + CA_NOTIFY_MES_VGAP)
#define CA_NOTIFY_MES_CONTENT_W                     (CA_NOTIFY_W - CA_NOTIFY_MES_CONTENT_X*2)
#define CA_NOTIFY_MES_CONTENT_H                     (290)

#define CA_NOTIFY_RECEIVE_TIME_TITLE_X				((CA_NOTIFY_W - CA_NOTIFY_HELP_WITH)/2)
#define CA_NOTIFY_RECEIVE_TIME_TITLE_Y				(CA_NOTIFY_MES_CONTENT_Y + CA_NOTIFY_MES_CONTENT_H + CA_NOTIFY_MES_VGAP)
#define CA_NOTIFY_RECEIVE_TIME_TITLE_W				(120)
#define CA_NOTIFY_RECEIVE_TIME_TITLE_H				(30)

#define CA_NOTIFY_RECEIVE_TIME_CONTENT_X			(CA_NOTIFY_RECEIVE_TIME_TITLE_X + CA_NOTIFY_RECEIVE_TIME_TITLE_W)
#define CA_NOTIFY_RECEIVE_TIME_CONTENT_Y			(CA_NOTIFY_RECEIVE_TIME_TITLE_Y)
#define CA_NOTIFY_RECEIVE_TIME_CONTENT_W			(200)
#define CA_NOTIFY_RECEIVE_TIME_CONTENT_H			(CA_NOTIFY_RECEIVE_TIME_TITLE_H)

#define CA_NOTIFY_NEW_TITLE_X						(CA_NOTIFY_RECEIVE_TIME_CONTENT_X + CA_NOTIFY_RECEIVE_TIME_CONTENT_W)
#define CA_NOTIFY_NEW_TITLE_Y						(CA_NOTIFY_RECEIVE_TIME_TITLE_Y)
#define CA_NOTIFY_NEW_TITLE_W						(120)
#define CA_NOTIFY_NEW_TITLE_H						(CA_NOTIFY_RECEIVE_TIME_TITLE_H)

#define CA_NOTIFY_NEW_CONTENT_X						(CA_NOTIFY_NEW_TITLE_X + CA_NOTIFY_NEW_TITLE_W)
#define CA_NOTIFY_NEW_CONTENT_Y						(CA_NOTIFY_RECEIVE_TIME_TITLE_Y)
#define CA_NOTIFY_NEW_CONTENT_W						(60)
#define CA_NOTIFY_NEW_CONTENT_H						(CA_NOTIFY_RECEIVE_TIME_TITLE_H)

#define CA_NOTIFY_IMPORT_TITLE_X					(CA_NOTIFY_NEW_CONTENT_X + CA_NOTIFY_NEW_CONTENT_W)
#define CA_NOTIFY_IMPORT_TITLE_Y					(CA_NOTIFY_RECEIVE_TIME_TITLE_Y)
#define CA_NOTIFY_IMPORT_TITLE_W					(120)
#define CA_NOTIFY_IMPORT_TITLE_H					(CA_NOTIFY_RECEIVE_TIME_TITLE_H)

#define CA_NOTIFY_IMPORT_CONTENT_X					(CA_NOTIFY_IMPORT_TITLE_X + CA_NOTIFY_IMPORT_TITLE_W)
#define CA_NOTIFY_IMPORT_CONTENT_Y					(CA_NOTIFY_RECEIVE_TIME_TITLE_Y)
#define CA_NOTIFY_IMPORT_CONTENT_W					(60)
#define CA_NOTIFY_IMPORT_CONTENT_H					(CA_NOTIFY_RECEIVE_TIME_TITLE_H)

#define CA_NOTIFY_HELP_WITH	(CA_NOTIFY_IMPORT_CONTENT_W + CA_NOTIFY_IMPORT_TITLE_W +\
							 CA_NOTIFY_NEW_CONTENT_W + CA_NOTIFY_NEW_TITLE_W +\
							 CA_NOTIFY_RECEIVE_TIME_CONTENT_W + CA_NOTIFY_RECEIVE_TIME_TITLE_W)


#define CA_NOTIFY_MES_VGAP                    		(10)

RET_CODE open_ca_notify(u32 para1, u32 para2);

#endif

