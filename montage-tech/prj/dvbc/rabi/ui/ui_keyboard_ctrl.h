/****************************************************************************

 ****************************************************************************/
#ifndef _UI_KEYBOARD_CTRL_H_
#define _UI_KEYBOARD_CTRL_H_

#include "ui_common.h"

#define FIND_ITEM_V_GAP  6         
#define FIND_ITEM_H_GAP  4

#define FIND_TTL_X 0
#define FIND_TTL_Y FIND_ITEM_V_GAP+10
#define FIND_TTL_H  32//24

#define FIND_EDIT_X  (20)
#define FIND_EDIT_Y  (20)
#define FIND_EDIT_H  (36)

#define FIND_KEY_W          (32)
#define FIND_KEY_H          (32)
#define FIND_KEYBOARD_X  24//46
#define FIND_KEYBOARD_Y (FIND_EDIT_Y + FIND_EDIT_H + 20)//126
#define FIND_KEYBOARD_W  384//((FIND_KEY_W + FIND_KEYBOARD_HGAP) * FIND_KEYBOARD_COL - FIND_KEYBOARD_HGAP)//292
#define FIND_KEYBOARD_H  344//((FIND_KEY_H + FIND_KEYBOARD_VGAP) * FIND_KEYBOARD_ROW - FIND_KEYBOARD_VGAP)//344

#define FIND_KEYBOARD_BC_X 46
#define FIND_KEYBOARD_BC_Y FIND_KEYBOARD_HGAP + FIND_EDIT_H +14
#define FIND_KEYBOARD_BC_W  FIND_KEYBOARD_W+CB_KEY_ADD_W - 180 
#define FIND_KEYBOARD_BC_H CB_KEY_HEIGHT

#define FIND_KEYBOARD_HGAP 18//20
#define FIND_KEYBOARD_VGAP 20

#define FIND_KEYBOARD_BC_HGAP 15
#define FIND_KEYBOARD_BC_VGAP 5

#define FIND_HELP_BC_X  10//25
#define FIND_HELP_BC_Y 106
#define FIND_HELP_BC_H 35//28

#define FIND_HELP_X  10//25
#define FIND_HELP_H HELP_ITEM_H//28

#define FIND_HELP_HGAP 0
#define FIND_HELP_VGAP 0

#define CB_KEY_LENGTH 32
#define CB_KEY_ADD_W 146
#define CB_KEY_HEIGHT 30
/*rstyle*/
#define RSI_FIND_EDIT_CONT  RSI_POPUP_BG
#define RSI_FIND_CAPS_TXT  RSI_PBACK
#define RSI_FIND_HELP RSI_PBACK
#define RSI_FIND_KEYBOARD RSI_SUBMENU_DETAIL_BG

/*fstyle*/
#define FSI_FIND_TEXT            FSI_BLACK     //font styel of single satellite information edit
 

/*others*/
#define FIND_EDIT_MAX_LEN     16
#define FIND_HELP_TOT 3
#define FIND_HELP_COL 3

#define FIND_BC_HELP_TOT 4
#define FIND_BC_HELP_COL 4


#define FIND_HELP_ROW 1
#define FIND_KEYBOARD_TOT 42
#define FIND_KEYBOARD_COL 6
#define FIND_KEYBOARD_ROW 7

#define FIND_KEYBOARD_CB_TOT 6
#define FIND_KEYBOARD_CB_COL 6
#define FIND_KEYBOARD_CB_ROW 1

#define FIND_KEYBOARD_ITEM_COUNT				(FIND_KEYBOARD_COL*FIND_KEYBOARD_ROW)
#define FIND_KEYBOARD_TOTAL_PAGE			3

control_t *ui_keyboard_cb_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 w,
                                  u16 h,
                                  u8 root_id,
                                  u32 para1,
                                  u32 para2);


control_t *ui_keyboard_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 w,
                                  u16 h,
                                  u8 root_id);
BOOL ui_keyboard_enter_edit(control_t *p_parent, u8 cont_id);

RET_CODE ui_keyboard_setfocus(control_t *p_parent, u8 cont_id);

RET_CODE ui_keyboard_hl(control_t *p_parent, u8 cont_id);

#endif


