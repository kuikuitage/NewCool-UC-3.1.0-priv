/****************************************************************************

 ****************************************************************************/
/*!
 \file ui_keyboard_menu.c
   this file  implement the interfaces defined in  ui_keyboard_menu.h, and some internal used functions,
   such as control message process functions, keymap functions.
   All these functions are about create a keyboard menu.
 */
//#include "ui_prog_list.h"
#include "ui_keyboard_ctrl.h"

enum keyboard_ctrl_local_msg
{
  MSG_BLUE_CONFIRM = MSG_LOCAL_BEGIN + 800,
  MSG_MOVE_RIGHT,
  MSG_MOVE_LEFT
};


enum find_edit_control_id
{
  IDC_FIND_TTL = 1,
  IDC_FIND_EDIT,
  IDC_FIND_KEYBOARD,
  IDC_FIND_HELP,
  IDC_FIND_HELP_EDIT
};

/*!
   max character number in current page.
 */
static u8 max_num[FIND_KEYBOARD_TOTAL_PAGE] = { FIND_KEYBOARD_ITEM_COUNT, FIND_KEYBOARD_ITEM_COUNT, 11 };
/*!
   current page number.
 */
static u8 g_page = 0;
static u8 g_root_id = 0;
static u8 *str_id_key[2] = {"biss","crypto"};


static u16 find_econt_keymap(u16 key);
static u16 find_keyboard_keymap(u16 key);


u16 find_bc_keyboard_keymap(u16 key);
RET_CODE find_bc_keyboard_proc(control_t *p_list, u16 msg, u32 para1,
             u32 para2);
u16 edit_bc_keyboard_keymap(u16 key);
RET_CODE edit_bc_keyboard_proc(control_t *p_list, u16 msg, u32 para1,
             u32 para2);

static RET_CODE find_econt_proc(control_t *ctrl, u16 msg, u32 para1,
                                u32 para2);
static RET_CODE find_keyboard_proc(control_t *p_list, u16 msg, u32 para1,
       u32 para2);


BOOL ui_keyboard_enter_edit(control_t *p_parent, u8 cont_id)
{
  BOOL ret;
	control_t * p_keyboard_cont, *p_edit;
	p_keyboard_cont = ctrl_get_child_by_id(p_parent, cont_id);
	p_edit = ctrl_get_child_by_id(p_keyboard_cont, IDC_FIND_EDIT);
  ret = ebox_enter_edit(p_edit);
	return ret;
}

RET_CODE ui_keyboard_setfocus(control_t *p_parent, u8 cont_id)
{
  RET_CODE ret;
  control_t * p_keyboard_cont, *p_mbox;
	p_keyboard_cont = ctrl_get_child_by_id(p_parent, cont_id);
	p_mbox = ctrl_get_child_by_id(p_keyboard_cont, IDC_FIND_KEYBOARD);
  ret = ctrl_default_proc(p_mbox, MSG_GETFOCUS, 0, 0);
	return ret;
}

RET_CODE ui_keyboard_hl(control_t *p_parent, u8 cont_id)
{
  RET_CODE ret = SUCCESS;
  control_t * p_keyboard_cont;
	p_keyboard_cont = ctrl_get_child_by_id(p_parent, cont_id);

  ctrl_set_attr(p_keyboard_cont, OBJ_ATTR_ACTIVE);
	ctrl_paint_ctrl(p_keyboard_cont, TRUE);

  //ret = ctrl_process_msg(p_keyboard_cont, MSG_GETFOCUS, 0, 0);
	return ret;
}
void decimal_to_char(u8 *p_data,u8 *p_char,u8 para)
{
    //u8 str_len = 0;
    u8 i=0,str_low = 0;
	char swi_to_char[32] = {0};
    //str_len = sizeof(p_data);

    for(;i<2 * para;i++)
    {
      if(i%2 == 0)
      {
        str_low = p_data[i/2] >> 4;
      }
      else
      {
         str_low = p_data[i/2] & 0xF;
      }
       if(0x9 <str_low && str_low < 0x10)
       {
           swi_to_char[i] = str_low - 0xA + 'A';
       }
       if( str_low< 10)
       {
           swi_to_char[i] = str_low + '0';
       }
    }
    memcpy(p_char,swi_to_char,2 * para);
    *(p_char + i) = '\0';
}




control_t *ui_keyboard_cb_create(control_t *parent,
                                  u8 cont_id,
                                  u16 x,
                                  u16 y,
                                  u16 w,
                                  u16 h,
                                  u8 root_id,
                                  u32 para1,
                                  u32 para2)
{
	control_t *p_econt;
	control_t *p_help,*p_help_edit;
	control_t *p_edit, *p_keyboard;
	u16 i = 0;
	u16 index = 0,ca_status=0;
	u8 temp[2] = "A";
      	char data_rec[16] = {0};
         u8 *str_id[FIND_BC_HELP_TOT] = {"Delete", "Send","Left","Right"};
	char p_char[33] = {0};
	u16 bmp_id_ca[2] = {IM_INFO_ICON_B_1,IM_INFO_ICON_CW_1};
    u16 bmp_id[FIND_BC_HELP_TOT] = {
      IM_HELP_RED,
      IM_HELP_BLUE,
      IM_HELP_GREEN,
      IM_HELP_YELLOW
    };
	//u8 bmp_id_edit[2] = {IM_EPG_COLORBUTTON_GREEN,IM_EPG_COLORBUTTON_YELLOW};
	 ca_status = para1 & 0xFF;
        memcpy(data_rec,(u8 *)para2,para1>>8);
        decimal_to_char(data_rec, p_char,para1>>8);
	g_page = 0;
	g_root_id = root_id;

	p_econt = ctrl_create_ctrl(CTRL_CONT, cont_id,
    x, y, w, h, parent, 0);
  ctrl_set_rstyle(p_econt, RSI_FIND_EDIT_CONT, RSI_FIND_EDIT_CONT, RSI_FIND_EDIT_CONT);
  //ctrl_set_keymap(p_econt, find_econt_keymap);
 // ctrl_set_proc(p_econt, find_econt_proc);

  p_edit = ctrl_create_ctrl(CTRL_EBOX, IDC_FIND_EDIT,
                            FIND_EDIT_X, FIND_KEYBOARD_HGAP,
                            (w - 2 * FIND_EDIT_X), FIND_EDIT_H,
                            p_econt, 0);
  ctrl_set_rstyle(p_edit, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG);
  ebox_set_maxtext(p_edit, CB_KEY_LENGTH);
  ebox_set_worktype(p_edit, EBOX_WORKTYPE_SHIFT);
  ebox_set_align_type(p_edit, STL_LEFT| STL_VCENTER);
  ebox_set_content_by_ascstr(p_edit, p_char);
  ebox_enter_edit(p_edit);
  #if 0
  ctrl_set_keymap(p_edit,edit_bc_keyboard_keymap);
  ctrl_set_proc(p_edit, edit_bc_keyboard_proc);
 #endif
  //keyboard

  p_keyboard = ctrl_create_ctrl(CTRL_MBOX, IDC_FIND_KEYBOARD,
  FIND_KEYBOARD_BC_X, FIND_KEYBOARD_BC_Y,
  FIND_KEYBOARD_BC_W,
  FIND_KEYBOARD_BC_H,
  p_econt, 0);
  ctrl_set_rstyle(p_keyboard, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_keyboard, find_bc_keyboard_keymap);
  ctrl_set_proc(p_keyboard, find_bc_keyboard_proc);
  mbox_enable_string_mode(p_keyboard, TRUE);
  mbox_set_count(p_keyboard, FIND_KEYBOARD_CB_TOT, FIND_KEYBOARD_CB_COL, FIND_KEYBOARD_CB_ROW);
  ctrl_set_mrect(p_keyboard, 0, 0, FIND_KEYBOARD_BC_W,
  	   CB_KEY_HEIGHT);
  mbox_set_item_interval(p_keyboard,FIND_KEYBOARD_BC_HGAP,FIND_KEYBOARD_BC_VGAP);
  mbox_set_item_rstyle(p_keyboard,
                      RSI_ITEM_1_HL, RSI_ITEM_1_SH, RSI_ITEM_1_SH);
  mbox_set_string_fstyle(p_keyboard, FSI_KEYBOARD_ITEM_HL, FSI_KEYBOARD_ITEM_HL, FSI_KEYBOARD_ITEM_HL);

  mbox_set_icon_offset(p_keyboard, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_keyboard, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_keyboard, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_keyboard, STL_CENTER | STL_VCENTER);

  for (index = 0; index < 6; index++)
  {
    temp[0] = (u8)('A' + (u8)index);
    mbox_set_content_by_ascstr(p_keyboard, index, temp);
  }

   ctrl_set_related_id(p_keyboard,
                        0,                                         /* left */
                        (u8)(IDC_FIND_EDIT),             /* up */
                        0,                                         /* right */
                       (u8)(IDC_FIND_EDIT));                  /* down */
  //help
  p_help = ctrl_create_ctrl(CTRL_MBOX, IDC_FIND_HELP,
    FIND_HELP_BC_X,
      FIND_HELP_BC_Y,
    (w - 2 * FIND_HELP_BC_X) , FIND_HELP_BC_H,
    p_econt, 0);
  ctrl_set_rstyle(p_help, RSI_FIND_HELP, RSI_FIND_HELP, RSI_FIND_HELP);
  mbox_enable_icon_mode(p_help, TRUE);
  mbox_enable_string_mode(p_help, TRUE);
  mbox_set_count(p_help, FIND_BC_HELP_TOT, FIND_BC_HELP_COL, FIND_HELP_ROW);
  ctrl_set_mrect(p_help, 0, 0, (w - 2 * FIND_HELP_BC_X), FIND_HELP_BC_H);
  mbox_set_item_interval(p_help, FIND_HELP_HGAP, FIND_HELP_VGAP);
  mbox_set_item_rstyle(p_help, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_string_fstyle(p_help, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_help, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_help, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_help, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_help, MBOX_STRTYPE_UNICODE);

  for (i = 0; i < FIND_BC_HELP_TOT; i++)
  {
    mbox_set_content_by_icon(p_help, (u8)i, bmp_id[i], bmp_id[i]);
    mbox_set_content_by_ascstr(p_help, (u8)i, str_id[i]);

   }

  p_help_edit = ctrl_create_ctrl(CTRL_MBOX, IDC_FIND_HELP_EDIT,
    FIND_HELP_BC_X + w - 2 * FIND_HELP_BC_X -135,
      FIND_KEYBOARD_BC_Y - 3,
       135, 35,
    p_econt, 0);
  ctrl_set_rstyle(p_help_edit, RSI_FIND_HELP, RSI_FIND_HELP, RSI_FIND_HELP);
  mbox_enable_icon_mode(p_help_edit, TRUE);
  mbox_enable_string_mode(p_help_edit, TRUE);
  mbox_set_count(p_help_edit, 1, 1, 1);
  ctrl_set_mrect(p_help_edit, 0, 0, 135, 35);
  mbox_set_item_interval(p_help_edit, FIND_HELP_HGAP, 5);
  mbox_set_item_rstyle(p_help_edit, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_string_fstyle(p_help_edit, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_help_edit, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_help_edit, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_help_edit, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_help_edit, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_help_edit, MBOX_STRTYPE_UNICODE);
  if(ca_status == 0)
  {
    mbox_set_content_by_icon(p_help_edit, (u8)0, bmp_id_ca[0], bmp_id_ca[0]);
    mbox_set_content_by_ascstr(p_help_edit, 0, str_id_key[0]);
   }
   else if(ca_status == 5)
   {
   mbox_set_content_by_icon(p_help_edit, (u8)0, bmp_id_ca[1], bmp_id_ca[1]);
   mbox_set_content_by_ascstr(p_help_edit, 0, str_id_key[1]);
   }


   return p_econt;
}



control_t *ui_keyboard_create(control_t *parent,
                                  u8 cont_id,
                                  u16 x,
                                  u16 y,
                                  u16 w,
                                  u16 h,
                                  u8 root_id)
{
  control_t *p_econt;
  control_t *p_help;
  control_t *p_edit, *p_keyboard;
  u16 i = 0;
  u16 index;
  u8 temp[2] = "A";
  u16 bmp_id[FIND_HELP_TOT] = {IM_HELP_RED, IM_HELP_BLUE, IM_HELP_YELLOW};
  u16 str_id[FIND_HELP_TOT] = {IDS_DELETE, IDS_SELECT, IDS_CANCEL};

  g_page = 0;
  g_root_id = root_id;

  p_econt = ctrl_create_ctrl(CTRL_CONT, cont_id,
                            x, y, w, h, parent, 0);
  ctrl_set_rstyle(p_econt, RSI_FIND_EDIT_CONT, RSI_FIND_EDIT_CONT, RSI_FIND_EDIT_CONT);
  ctrl_set_keymap(p_econt, find_econt_keymap);
  ctrl_set_proc(p_econt, find_econt_proc);

  //input edit
  p_edit = ctrl_create_ctrl(CTRL_EBOX, IDC_FIND_EDIT,
                          FIND_EDIT_X, FIND_EDIT_Y,
                          (w - 2 * FIND_EDIT_X), FIND_EDIT_H,
                          p_econt, 0);
  ctrl_set_rstyle(p_edit, RSI_KEYBOARD_BAR_V2, RSI_KEYBOARD_BAR_V2, RSI_KEYBOARD_BAR_V2);
  ebox_set_maxtext(p_edit, DB_DVBS_MAX_NAME_LENGTH);
  ebox_set_font_style(p_edit, FSI_WHITE, FSI_WHITE, FSI_WHITE);

  //keyboard
  p_keyboard = ctrl_create_ctrl(CTRL_MBOX, IDC_FIND_KEYBOARD,
                                ((w - FIND_KEYBOARD_W) >> 1), FIND_KEYBOARD_Y, 
                                FIND_KEYBOARD_W, FIND_KEYBOARD_H,
                                p_econt, 0);
  ctrl_set_rstyle(p_keyboard, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_keyboard, find_keyboard_keymap);
  ctrl_set_proc(p_keyboard, find_keyboard_proc);
  //mbox_enable_icon_mode(p_keyboard, TRUE);
  mbox_enable_string_mode(p_keyboard, TRUE);
  mbox_set_count(p_keyboard, FIND_KEYBOARD_TOT, FIND_KEYBOARD_COL, FIND_KEYBOARD_ROW);
  ctrl_set_mrect(p_keyboard, 0, 0, FIND_KEYBOARD_W, FIND_KEYBOARD_H);
  mbox_set_item_interval(p_keyboard, FIND_KEYBOARD_HGAP, FIND_KEYBOARD_VGAP);
  mbox_set_item_rstyle(p_keyboard, RSI_KEYBOARD_BUTTON_HL, RSI_KEYBOARD_BUTTON_SH, RSI_KEYBOARD_BUTTON_SH);
  mbox_set_string_fstyle(p_keyboard, FSI_BLACK, FSI_WHITE, FSI_WHITE);
  mbox_set_icon_offset(p_keyboard, 0, 0);
  mbox_set_icon_align_type(p_keyboard, STL_CENTER | STL_VCENTER); 
  mbox_set_string_offset(p_keyboard, 12, 0);
  mbox_set_string_align_type(p_keyboard, STL_CENTER | STL_VCENTER);
  for (index = 0; index < FIND_KEYBOARD_TOT; index++)
  {
    temp[0] = (u8)(' ' + (u8)index);
    mbox_set_content_by_ascstr(p_keyboard, index, temp);
  }

  //help
  p_help = ctrl_create_ctrl(CTRL_MBOX, IDC_FIND_HELP,
                          FIND_HELP_X, (h - HELP_ITEM_H - HELP_CONT_POP_DLG_V_GAP), 
                          (w - (FIND_HELP_X + FIND_HELP_X)), FIND_HELP_H,
                          p_econt, 0);
  ctrl_set_rstyle(p_help, RSI_FIND_HELP, RSI_FIND_HELP, RSI_FIND_HELP);
  mbox_enable_icon_mode(p_help, TRUE);
  mbox_enable_string_mode(p_help, TRUE);
  mbox_set_count(p_help, FIND_HELP_TOT, FIND_HELP_COL, FIND_HELP_ROW);
  ctrl_set_mrect(p_help, 0, 0, (w - 2 * FIND_HELP_X), FIND_HELP_H);
  mbox_set_item_interval(p_help, FIND_HELP_HGAP, FIND_HELP_VGAP);
  mbox_set_item_rstyle(p_help,
                      RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_string_fstyle(p_help, FSI_WHITE, FSI_WHITE, FSI_WHITE);

  mbox_set_icon_offset(p_help, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_help, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_help, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_help, MBOX_STRTYPE_STRID);

  for (i = 0; i < FIND_HELP_TOT; i++)
  {
    mbox_set_content_by_icon(p_help, (u8)i, bmp_id[i], bmp_id[i]);
    mbox_set_content_by_strid(p_help, (u8)i, str_id[i]);
  }

  return p_econt;
}

static void keyboard_load_data(struct control *p_ctrl)
{
  u16 index, i;
  u8 temp[2] = "A", numbers;

  for (index = 0; index < FIND_KEYBOARD_ITEM_COUNT; index++)
  {
    numbers = 0;
    for (i = 0; i < g_page; i++)
    {
      numbers += max_num[g_page - 1];
    }
    temp[0] = (u8)(' ' + (u8)index + numbers);
    mbox_set_content_by_ascstr(p_ctrl, index, temp);
  }

  ctrl_paint_ctrl(p_ctrl, TRUE);
}

static void edit_notify_root(u16 msg, u32 para1, u32 para2)
{
  control_t *p_root;
  msgproc_t proc;

  p_root = fw_find_root_by_id(g_root_id);
  if(p_root != NULL)
  {
    proc = ctrl_get_proc(p_root);
    proc(p_root, msg, para1, para2);
  }
}

// ON COMM ROOT
static RET_CODE on_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_close_all_mennus();

  return SUCCESS;
}

static RET_CODE on_find_bc_keyboard_input(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  u16 focus = 0;
  s32 ret = SUCCESS;
  control_t *p_edit;

   focus = mbox_get_focus(p_ctrl);

	p_edit = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_FIND_EDIT);
       if(msg == MSG_CHAR)
      {
	{
 	ret = ebox_class_proc(p_edit, (u16)(MSG_CHAR | (focus + 'A')), 0, 0);
	 }
       }
	else if(msg == MSG_NUMBER)
	{
	 ret = ebox_class_proc(p_edit, (u16)(MSG_CHAR | (para1 -5 + '0' )), 0, 0);
	 }
	edit_notify_root(MSG_INPUT_CHANGED, (u32)ebox_get_content(p_edit), 0);

	return ret;
}


static RET_CODE on_find_keyboard_input(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  u16 focus;
  s32 ret = SUCCESS;
  u8 numbers, i;
	control_t *p_edit;

  focus = mbox_get_focus(p_ctrl);

	p_edit = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_FIND_EDIT);

	for(numbers= 0, i = 0; i < g_page; i++)
	{
	  numbers += max_num[i];
	}

	ret = ebox_class_proc(p_edit, (u16)(MSG_CHAR | (focus + ' ' + numbers)), 0, 0);

	edit_notify_root(MSG_INPUT_CHANGED, (u32)ebox_get_content(p_edit), 0);

	return ret;
}


static RET_CODE on_find_bc_keyboard(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  s32 ret = SUCCESS;
  ret = mbox_class_proc(p_ctrl, msg, para1, para2);
  return ret;
}
#if 0
static RET_CODE on_find_bc_up_down_keyboard(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  s32 ret = SUCCESS;
  control_t *p_ctrl_edit;
  ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0);
  p_ctrl_edit = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_FIND_EDIT);
  ret = ctrl_default_proc(p_ctrl_edit, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_ctrl,TRUE);
  ctrl_paint_ctrl(p_ctrl_edit,TRUE);
  return ret;
}
#endif
static RET_CODE on_find_keyboard_up(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  u16 focus;
  s32 ret = SUCCESS;

  focus = mbox_get_focus(p_ctrl);

  if (focus < FIND_KEYBOARD_COL) /* focus on top line */
  {
    if (g_page == 0)               //firset page
    {
      /* pass to root container */
      ret = ERR_NOFEATURE;
    }
    else
    {
      g_page--;
      focus += FIND_KEYBOARD_COL * (FIND_KEYBOARD_ROW- 1);
      while (focus > (max_num[g_page] - 1))
      {
        focus -= FIND_KEYBOARD_COL;
      }

      mbox_set_focus(p_ctrl, focus);
      keyboard_load_data(p_ctrl);
    }
  }
  else
  {
    ret = mbox_class_proc(p_ctrl, msg, para1, para2);
  }

  return ret;
}

static RET_CODE on_find_keyboard_down(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  u16 focus;
  s32 ret = SUCCESS;

  focus = mbox_get_focus(p_ctrl);

  if ((focus + FIND_KEYBOARD_COL) > (max_num[g_page] - 1)) //page down
  {
    if (g_page == (FIND_KEYBOARD_TOTAL_PAGE - 1))               /* focus on bottom line */
    {
      /* pass to root container */
      ret = ERR_NOFEATURE;
    }
    else
    {
      g_page++;
      focus = focus % FIND_KEYBOARD_COL;
      mbox_set_focus(p_ctrl, focus);
      keyboard_load_data(p_ctrl);
    }
  }
  else
  {
    mbox_class_proc(p_ctrl, msg, para1, para2);
  }

  return ret;
}



static RET_CODE on_find_keyboard_left(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  u16 focus;
  s32 ret = SUCCESS;

  focus = mbox_get_focus(p_ctrl);

	if((g_page == (FIND_KEYBOARD_TOTAL_PAGE - 1))&&((focus == max_num[FIND_KEYBOARD_TOTAL_PAGE - 1] - 1)))
  {
    //mbox_set_focus_draw(ctrl, max_num[KEYBOARD_TOTAL_PAGE - 1]/KEYBOARD_MBOX_COL*KEYBOARD_MBOX_COL);
    //break;
  }
  else
  {
    ret = mbox_class_proc(p_ctrl, msg, para1, para2);
  }

  return ret;
}

static RET_CODE on_find_keyboard_right(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  u16 focus;
  s32 ret = SUCCESS;

  focus = mbox_get_focus(p_ctrl);

  if((g_page == (FIND_KEYBOARD_TOTAL_PAGE - 1))&&((focus == max_num[FIND_KEYBOARD_TOTAL_PAGE - 1]/FIND_KEYBOARD_COL*FIND_KEYBOARD_COL)))
  {
    //mbox_set_focus_draw(ctrl, max_num[KEYBOARD_TOTAL_PAGE - 1] - 1);
    //break;
  }
  else
  {
    ret = mbox_class_proc(p_ctrl, msg, para1, para2);
  }

  return ret;
}

static RET_CODE on_find_keyboard_backspace(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  u16 focus;
  s32 ret = SUCCESS;
	control_t *p_edit;

  focus = mbox_get_focus(p_ctrl);

	p_edit = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_FIND_EDIT);

	ret = ebox_class_proc(p_edit, msg, 0, 0);

  edit_notify_root(MSG_INPUT_CHANGED, (u32)ebox_get_content(p_edit), 0);

  return ret;
}

static RET_CODE on_find_keyboard_edit_move(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  control_t *ctrl_edit;
  //s8 offset = (s8)(msg == MSG_MOVE_LEFT? -1 : 1);
  ctrl_edit = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_FIND_EDIT);
  MT_ASSERT(ctrl_edit != NULL);
  // ebox_shift_focus(ctrl_edit, offset);
  return SUCCESS;
}

void char_to_decimal(u8 *p_char,u8 *p_data)
{
    u8 str_len = 0;
    u8 i=0,str_high = 0,str_low=0;
	char swi_to_data[16] = {0};

    str_len = strlen(p_char);

    for(;i<str_len;i++)
    {
         if(i%2 == 0)
       {
         if('A' <=  p_char[i] && p_char[i] <= 'F')
         {
           str_high = (p_char[i] - 'A' + 0xA) << 4;
           swi_to_data[i/2] = str_high | 0x0F;
         }
         else if('0' <= p_char[i] && p_char[i] <= '9')
          {
	  str_high = (p_char[i] - '0') << 4;
           swi_to_data[i/2] = str_high | 0x0F;
          }
       }
       else
       {
          if('A' <=  p_char[i] && p_char[i] <= 'F')
         {
           str_low = (p_char[i] - 'A' + 0xA);
           swi_to_data[i/2] = swi_to_data[i/2] & (str_low | 0xF0);
         }
         else if('0' <= p_char[i] && p_char[i] <= '9')
          {
	  str_low = (p_char[i] - '0');
           swi_to_data[i/2] = swi_to_data[i/2] & (str_low | 0xF0);
          }
       }
    }
    memcpy(p_data,swi_to_data,str_len/2);
}
  play_param_t p_play = {0};

static RET_CODE on_find_keyboard_confirm(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
         s32 ret = SUCCESS;
	control_t *p_edit;
         u8 p_ascstr[33] = {0};
	u8 p_data_key[16] = {0};
	u16 *uni_str = NULL;
	u8 char_len = 0;
	u16 pg_id = 0;
         //BOOL p_status=0;
	dvbs_prog_node_t pg = {0};
         dvbs_tp_node_t tp= {0};
         if (( pg_id = sys_status_get_curn_group_curn_prog_id()) == INVALIDID)
       {
         return ERR_FAILURE;
        }
       load_play_paramter_by_pgid(&p_play, pg_id);

        db_dvbs_get_pg_by_id(pg_id, &pg);
        db_dvbs_get_tp_by_id(pg.tp_id, &tp);

	p_edit = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_FIND_EDIT);

	ctrl_process_msg(p_edit, MSG_LOSTFOCUS, 0, 0);

	edit_notify_root(MSG_HIDE_KEYBOARD, (u32)ebox_get_content(p_edit), 0);

         uni_str = ebox_get_content(p_edit);

         char_len = uni_strlen(uni_str);

	str_nuni2asc(p_ascstr,uni_str, char_len);

         char_to_decimal(p_ascstr,p_data_key);

         //sys_status_get_status(BS_BISS_TYPE, &p_status);



          #if ENABLE_BISS_KEY
            if(pg.ca_system_id == 0x2600)
            {
             if(char_len == 16)
             {
             sys_status_set_key(BSDATA_BLOCK_ID, tp.freq, pg.s_id,p_data_key, char_len/2);
             pti_set_cw(p_data_key,0,8);
             }
	     }


          if(pg.ca_system_id >= 0x0d00 && pg.ca_system_id <= 0x0dff)
	  {
	    #if ENABLE_BISS_KEY
	    sys_status_set_key(CRYPTO_BLOCK_ID,  tp.freq, pg.s_id,p_data_key, char_len/2);
            #endif
	  #if ENABLE_CRYPTO_KEY
	  {
	   cmd_t cmd;
          cmd.id = PB_CMD_PLAY;

	   memcpy(p_play.pg_info.crypto_key,p_data_key,char_len/2);
	   p_play.pg_info.crypto_key_length = char_len/2;
          cmd.data1 = (u32)&p_play;
          cmd.data2 = 1;
         ap_frm_do_command(APP_PLAYBACK, &cmd);
         return ret;
	  }
	  #endif
	}
	#endif
         //manage_close_menu(ROOT_ID_CA_BC,0,0);
	return ret;
}

#if 0
static RET_CODE on_find_keyboard_cancel(control_t *p_ctrl, u16 msg, u32 para1,
									u32 para2)
{
  u16 focus;
  s32 ret = SUCCESS;

  focus = mbox_get_focus(p_ctrl);

	return ret;
}
#endif

static RET_CODE on_find_econt_select(control_t *ctrl, u16 msg, u32 para1,
                                  u32 para2)
{
	control_t *p_edit;

  //p_list = ctrl_get_child_by_id(ctrl_get_parent(ctrl), IDC_FIND_LIST);
	p_edit = ctrl_get_child_by_id(ctrl, IDC_FIND_EDIT);

	ctrl_default_proc(p_edit, MSG_LOSTFOCUS, 0, 0);
	ctrl_paint_ctrl(p_edit, TRUE);
	//ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
	//ctrl_paint_ctrl(p_list, TRUE);

	return SUCCESS;
}

static RET_CODE on_find_econt_exit(control_t *ctrl, u16 msg, u32 para1,
                                  u32 para2)
{
	return manage_close_menu(ROOT_ID_FIND, 0, 0);
}


BEGIN_KEYMAP(find_econt_keymap, NULL)
	ON_EVENT(V_KEY_YELLOW, MSG_YES)
	ON_EVENT(V_KEY_BLUE, MSG_EXIT)
END_KEYMAP(find_econt_keymap, NULL)

BEGIN_MSGPROC(find_econt_proc, cont_class_proc)
	ON_COMMAND(MSG_YES, on_find_econt_select)
	ON_COMMAND(MSG_EXIT, on_find_econt_exit)
END_MSGPROC(find_econt_proc, cont_class_proc)

#if 0
BEGIN_KEYMAP(edit_bc_keyboard_keymap, NULL)
	ON_EVENT(V_KEY_YELLOW, MSG_YES)
	ON_EVENT(V_KEY_BLUE, MSG_EXIT)
END_KEYMAP(edit_bc_keyboard_keymap, NULL)

BEGIN_MSGPROC(edit_bc_keyboard_proc, cont_class_proc)
	ON_COMMAND(MSG_YES, on_find_econt_select)
	ON_COMMAND(MSG_EXIT, on_find_econt_exit)
END_MSGPROC(edit_bc_keyboard_proc, cont_class_proc)
#endif
BEGIN_KEYMAP(find_keyboard_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_OK, MSG_CHAR)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_RED, MSG_BACKSPACE)
	ON_EVENT(V_KEY_BLUE, MSG_BLUE_CONFIRM)
	ON_EVENT(V_KEY_YELLOW, MSG_CANCEL)

END_KEYMAP(find_keyboard_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(find_keyboard_proc, mbox_class_proc)
	ON_COMMAND(MSG_CHAR, on_find_keyboard_input)
	ON_COMMAND(MSG_FOCUS_UP, on_find_keyboard_up)
	ON_COMMAND(MSG_FOCUS_DOWN, on_find_keyboard_down)
	ON_COMMAND(MSG_FOCUS_LEFT, on_find_keyboard_left)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_find_keyboard_right)
	ON_COMMAND(MSG_BACKSPACE, on_find_keyboard_backspace)
	ON_COMMAND(MSG_CANCEL, on_exit_all)
	ON_COMMAND(MSG_BLUE_CONFIRM, on_find_keyboard_confirm)
END_MSGPROC(find_keyboard_proc, mbox_class_proc)


BEGIN_KEYMAP(find_bc_keyboard_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_OK, MSG_CHAR)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_RED, MSG_BACKSPACE)
	ON_EVENT(V_KEY_BLUE, MSG_BLUE_CONFIRM)
	ON_EVENT(V_KEY_YELLOW, MSG_MOVE_RIGHT)
       ON_EVENT(V_KEY_GREEN, MSG_MOVE_LEFT)
	ON_EVENT(V_KEY_0, MSG_NUMBER)
       ON_EVENT(V_KEY_1, MSG_NUMBER)
       ON_EVENT(V_KEY_2, MSG_NUMBER)
       ON_EVENT(V_KEY_3, MSG_NUMBER)
       ON_EVENT(V_KEY_4, MSG_NUMBER)
       ON_EVENT(V_KEY_5, MSG_NUMBER)
       ON_EVENT(V_KEY_6, MSG_NUMBER)
       ON_EVENT(V_KEY_7, MSG_NUMBER)
       ON_EVENT(V_KEY_8, MSG_NUMBER)
       ON_EVENT(V_KEY_9, MSG_NUMBER)
END_KEYMAP(find_bc_keyboard_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(find_bc_keyboard_proc, mbox_class_proc)
      	ON_COMMAND(MSG_NUMBER, on_find_bc_keyboard_input)
	ON_COMMAND(MSG_CHAR, on_find_bc_keyboard_input)
	ON_COMMAND(MSG_FOCUS_UP, on_find_bc_keyboard)
	ON_COMMAND(MSG_FOCUS_DOWN, on_find_bc_keyboard)
	ON_COMMAND(MSG_FOCUS_LEFT, on_find_bc_keyboard)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_find_bc_keyboard)
	ON_COMMAND(MSG_BACKSPACE, on_find_keyboard_backspace)
    ON_COMMAND(MSG_MOVE_RIGHT, on_find_keyboard_edit_move)
    ON_COMMAND(MSG_MOVE_LEFT, on_find_keyboard_edit_move)
	ON_COMMAND(MSG_CANCEL, on_exit_all)
	//#if ENABLE_BISS_KEY
	//ON_COMMAND(MSG_SWITCH, on_switch_key_type)
	//#endif
	ON_COMMAND(MSG_BLUE_CONFIRM, on_find_keyboard_confirm)
END_MSGPROC(find_bc_keyboard_proc, mbox_class_proc)


