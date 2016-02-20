/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_SN_CA_CARD_ADT_MG_H__
#define __UI_SN_CA_CARD_ADT_MG_H__

/* coordinate */
//xuxin
#define SN_SCREEN_WIDTH 					  640
#define SN_SCREEN_HEIGHT					  480

#ifndef SN_SUBMENU_CONT_X 
#define SN_SUBMENU_CONT_X          (((SN_SCREEN_WIDTH) - (SUBMENU_CONT_W)) / 2)
#endif
#ifndef SN_SUBMENU_CONT_Y
#define SN_SUBMENU_CONT_Y          (((SN_SCREEN_HEIGHT) - (SUBMENU_CONT_H)) / 2)
#endif
#define SN_SUBMENU_CONT_W          SN_SCREEN_WIDTH
#define SN_SUBMENU_CONT_H          SN_SCREEN_HEIGHT
//xuxin end
#define SUBMENU_CNT             12
#define SN_CARD_ADT_CNT          8

//advertisement window
#define SUBMENU_AD_X     SUB_AD_X
#define SUBMENU_AD_Y     SUB_AD_Y
#define SUBMENU_AD_W    SUB_AD_W
#define SUBMENU_AD_H     SUB_AD_H

//preview window
#define SUBMENU_PREV_X     SUB_PREV_X
#define SUBMENU_PREV_Y     SUB_PREV_Y
#define SUBMENU_PREV_W    SUB_PREV_W
#define SUBMENU_PREV_H     SUB_PREV_H

//second title
#define SUBMENU_SECOND_TITLE_X  SECOND_TITLE_X
#define SUBMENU_SECOND_TITLE_Y  SECOND_TITLE_Y
#define SUBMENU_SECOND_TITLE_W SECOND_TITLE_W
#define SUBMENU_SECOND_TITLE_H  SECOND_TITLE_H

//COL 1 CONT
#define SUBMENU_COL1_CONT_X   SUBMENU_SECOND_TITLE_X
#define SUBMENU_COL1_CONT_Y   RIGHT_BG_Y
#define SUBMENU_COL1_CONT_W  166
#define SUBMENU_COL1_CONT_H   RIGHT_BG_H

//COL 2 CONT
#ifndef SUBMENU_COL2_CONT_X 
#define SUBMENU_COL2_CONT_X   ((SUBMENU_SECOND_TITLE_X) + (SUBMENU_SECOND_TITLE_W) - (SUBMENU_COL2_CONT_W))
#endif
#define SUBMENU_COL2_CONT_Y   SUBMENU_COL1_CONT_Y
#define SUBMENU_COL2_CONT_W  SUBMENU_COL1_CONT_W
#define SUBMENU_COL2_CONT_H   SUBMENU_COL1_CONT_H
#ifndef SUBMENU_COL1_BTN_X 
#define SUBMENU_COL1_BTN_X   ((SUBMENU_COL1_CONT_X) + ((SUBMENU_COL1_CONT_W) - (SUBMENU_BTN_W))/2)
#endif
#ifndef SUBMENU_COL2_BTN_X 
#define SUBMENU_COL2_BTN_X   ((SUBMENU_COL2_CONT_X) + ((SUBMENU_COL1_CONT_W) - (SUBMENU_BTN_W))/2)
#endif
#define SUBMENU_BTN_Y   (SUBMENU_COL1_CONT_Y + 10)
#define SUBMENU_BTN_W  126
#define SUBMENU_BTN_H   35

#define SUBMENU_BTN_V_GAP       18

RET_CODE open_dvb_ca_info(u32 para1, u32 para2);

#endif

