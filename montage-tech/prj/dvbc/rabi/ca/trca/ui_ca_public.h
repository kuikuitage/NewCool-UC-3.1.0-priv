/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_PUBLIC_H__
#define __UI_CA_PUBLIC_H__

typedef enum
{
  ROOT_ID_CA_CARD_INFO = ROOT_ID_CA_START,
  ROOT_ID_TR_LEVEL,
  ROOT_ID_TR_PIN,
  ROOT_ID_TR_WORKTIME,
  ROOT_ID_CA_OPE,
  ROOT_ID_TR_FEED,
  ROOT_ID_ALERT_MESS,
  ROOT_ID_CA_PROMPT,
  ROOT_ID_NEW_MAIL,
  ROOT_ID_EMAIL_MESS,
  ROOT_ID_CA_IPP,
  ROOT_ID_FINGER_PRINT,
  ROOT_ID_CA_NOTIFY,
}ca_ui_root_id;


typedef struct
{
u32 index;  //index 
u32 message_type;  // 0:mail  1:announce 2:alert mail 3:alert announce
}prompt_type_t;


enum background_menu_ctrl_id
{
  /* value 1 for TITLE_ROLL*/
  IDC_ROLL_TITLE = 1,
  IDC_BG_MENU_CA_ROLL_TOP = 2,
  IDC_BG_MENU_CA_ROLL_BOTTOM = 3,
  IDC_BG_MENU_CA_SENIOR_PREVIEW = 6,
  IDC_BG_MENU_CA_ROLL_FULLSCREEN = 12,
  IDC_BG_MENU_CA_ROLL_HELP = 13,
};

void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2);
RET_CODE on_ca_rolling_show(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE on_ca_rolling_hide(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
BOOL ui_get_smart_card_state(void);
RET_CODE ui_desktop_proc_cas(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);
u16 ui_desktop_keymap_cas(u16 key);
BOOL ui_set_smart_card_insert(BOOL status);
BOOL ui_is_smart_card_insert(void);
RET_CODE on_ca_level_update(control_t *p_cont, u16 msg, u32 para1, u32 para2);

RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);
void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id);
RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);

RET_CODE open_ca_rolling_menu(u32 para1, u32 para2);
extern void set_uio_status(BOOL is_disable);
#endif

