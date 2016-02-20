/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_favorite.h"

#include "ui_rename.h"
#include "ui_keyboard_v2.h"

//static u16 fav_context_index[MAX_FAV_CNT];
//static u16 fav_group_index[MAX_FAV_CNT];

enum local_msg
{
  MSG_BLUE = MSG_LOCAL_BEGIN,
};

enum favorite_ctrl_id
{
  IDC_FAVORITE_LIST = 1,
  IDC_FAVORITE_SBAR,
  IDC_SMALL_LIST_HELP,
};

static list_xstyle_t favorite_list_item_rstyle =
{
  RSI_PBACK,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
};

static list_xstyle_t favorite_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};

static list_xstyle_t favorite_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t favorite_list_attr[FAVORITE_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    160, 20, 0, &favorite_list_field_rstyle,  &favorite_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    300, 180, 0, &favorite_list_field_rstyle,  &favorite_list_field_fstyle},
};

static u16 favorite_list_keymap(u16 key);

static RET_CODE favorite_cont_proc(control_t *cont, u16 msg, u32 para1,
                             u32 para2);
static RET_CODE favorite_list_proc(control_t *cont, u16 msg, u32 para1,
                             u32 para2);


static RET_CODE favorite_list_update(control_t* p_list, u16 start, u16 size,
                                     u32 context)
{
  u8 i;
  u16 cnt = list_get_count(p_list);
  u16 name[MAX_FAV_NAME_LEN] = {0};
  u8 asc_str[8];

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      sprintf(asc_str, "%d", (start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);
      sys_status_get_fav_name((u8)(start + i), name);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, name);
    }
  }

  return SUCCESS;
}

RET_CODE open_favorite(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list, *p_sbar,*p_help;
  u8 i;

#ifndef SPT_SUPPORT
  p_cont =
    ui_comm_root_create(ROOT_ID_FAVORITE, 0, COMM_BG_X, COMM_BG_Y,
      COMM_BG_W,  COMM_BG_H, 0, IDS_FAVORITE);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
 
  ctrl_set_proc(p_cont, favorite_cont_proc);

  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_FAVORITE_LIST,
    FAVORITE_LIST_X, FAVORITE_LIST_Y,
    FAVORITE_LIST_W, FAVORITE_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_FAVORITE_LIST, RSI_FAVORITE_LIST, RSI_FAVORITE_LIST);
  ctrl_set_keymap(p_list, favorite_list_keymap);
  ctrl_set_proc(p_list, favorite_list_proc);

  ctrl_set_mrect(p_list, FAVORITE_LIST_MID_L, FAVORITE_LIST_MID_T,
                    FAVORITE_LIST_MID_W+FAVORITE_LIST_MID_L, FAVORITE_LIST_MID_H+FAVORITE_LIST_MID_T);
  list_set_item_interval(p_list, FAVORITE_LIST_ITEM_VGAP);
  list_set_item_rstyle(p_list, &favorite_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, MAX_FAV_CNT, FAVORITE_LIST_PAGE);
  list_set_field_count(p_list, FAVORITE_LIST_FIELD, FAVORITE_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, favorite_list_update, 0);

  for (i = 0; i < FAVORITE_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(favorite_list_attr[i].attr),
      (u16)(favorite_list_attr[i].width),
      (u16)(favorite_list_attr[i].left),
      (u8)(favorite_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, favorite_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, favorite_list_attr[i].fstyle);
  }

  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_FAVORITE_SBAR,
    FAVORITE_SBAR_X, FAVORITE_SBAR_Y,
    FAVORITE_SBAR_WIDTH, FAVORITE_SBAR_HEIGHT,
    p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_FAVORITE_SBAR_BG,
                  RSI_FAVORITE_SBAR_BG, RSI_FAVORITE_SBAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_FAVORITE_SBAR_MID,
                     RSI_FAVORITE_SBAR_MID, RSI_FAVORITE_SBAR_MID);
  ctrl_set_mrect(p_sbar, 0, FAVORITE_SBAR_VERTEX_GAP,
                    FAVORITE_SBAR_WIDTH,
                    FAVORITE_SBAR_HEIGHT - FAVORITE_SBAR_VERTEX_GAP);
  list_set_scrollbar(p_list, p_sbar);

  favorite_list_update(p_list, list_get_valid_pos(p_list), FAVORITE_LIST_PAGE, 0);

  //create help bar
    p_help = ctrl_create_ctrl(CTRL_MBOX, IDC_SMALL_LIST_HELP,
     FAVORITE_LIST_X, FAVORITE_LIST_Y+FAVORITE_LIST_H+20,
    FAVORITE_LIST_W, 50, 
                            p_cont, 0);
  ctrl_set_rstyle(p_help, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  mbox_enable_icon_mode(p_help, TRUE);
  mbox_enable_string_mode(p_help, TRUE);
  mbox_set_count(p_help, 2, 2, 1);
  ctrl_set_mrect(p_help, 10, 0, FAVORITE_LIST_W - 20 +10, 50);
  mbox_set_item_interval(p_help,0, 0);
  mbox_set_string_fstyle(p_help, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_help, 0, 0);
  mbox_set_icon_align_type(p_help, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_help, 40, 0);
  mbox_set_string_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_help, MBOX_STRTYPE_STRID);

     mbox_set_content_by_icon(p_help, 0, IM_HELP_BLUE, IM_HELP_BLUE);
  mbox_set_content_by_strid(p_help, 0, IDS_RENAME);
  //mbox_set_content_by_icon(p_help, 1, IM_OK, IM_OK);
   mbox_set_content_by_icon(p_help, 1, IM_HELP_OK, IM_HELP_OK);
  mbox_set_content_by_strid(p_help, 1, IDS_ENTER_CHANNEL_LIST);

#else
#endif
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}

static RET_CODE on_favorite_rename_update(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list;
  u16 *uni_str = (u16 *)para1;
  u16 focus;

  p_list = ctrl_get_child_by_id(p_ctrl, IDC_FAVORITE_LIST);
  MT_ASSERT(p_list != NULL);

  focus = list_get_focus_pos(p_list);
  list_set_field_content_by_unistr(p_list, focus, 1, uni_str);

  list_draw_item_ext(p_list, focus, TRUE);
  ctrl_paint_ctrl(p_list, TRUE);

  sys_status_set_fav_name((u8)focus, uni_str);
  sys_status_save();

  return SUCCESS;

}

static RET_CODE on_favorite_rename_check(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list;
  u16 *uni_str = (u16 *)para1;
  u16 name[MAX_FAV_NAME_LEN] = {0};
  u16 focus;
  u8 i = 0;

  p_list = ctrl_get_child_by_id(p_ctrl, IDC_FAVORITE_LIST);
  MT_ASSERT(p_list != NULL);

  focus = list_get_focus_pos(p_list);

  for(i = 0; i < MAX_FAV_CNT; i++)
  {
    sys_status_get_fav_name(i, name);

    if(!uni_strcmp(name, uni_str) && (i != focus))
    {
      ctrl_process_msg(p_list,MSG_GETFOCUS , 0, 0);
      return ERR_NOFEATURE;
    }
  }

  return SUCCESS;
}

static RET_CODE on_favorite_list_yes(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  s16 focus;
   u8 is_radio = 0;
  u16 pgid;
  u32 context;
   u8 curn_mode = CURN_MODE_NONE;
  u32 group_index;


  focus = list_get_focus_pos(p_ctrl);

  //sys_status_get_curn_prog_in_group(1 + DB_DVBS_MAX_SAT + focus,
      //sys_status_get_curn_prog_mode(), &pgid, &context);

  group_index= 1 + DB_DVBS_MAX_SAT + focus;

  //if(pgid == INVALIDID)
 //{
   //ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_FAV_PROG, NULL, 0);
    //return ERR_FAILURE;
 //}

    curn_mode = sys_status_get_curn_prog_mode();
    if(curn_mode == CURN_MODE_TV)
    {
      is_radio = 0;
    }
    else if(curn_mode == CURN_MODE_RADIO)
    {
      is_radio = 1;
   }
   else
   {
      ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_PROG, NULL, 0);
      return ERR_FAILURE;
   }

  sys_status_get_curn_prog_in_group(group_index, curn_mode, &pgid, &context);

  if(pgid == INVALIDID)
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_FAV_PROG, NULL, 0);
    return ERR_FAILURE;
  }

  return manage_open_menu(ROOT_ID_PROG_LIST,is_radio ,group_index);
}

static RET_CODE on_favorite_list_rename(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
 RET_CODE ret = SUCCESS;
  rename_param_t rename;
  u16 focus;

  //open a dialog for rename.
  rename.max_len = (MAX_FAV_NAME_LEN - 8);
  rename.uni_str = (u16 *)list_get_field_content(p_ctrl, list_get_focus_pos(p_ctrl), 1);
  rename.type = KB_INPUT_TYPE_SENTENCE;

  focus = list_get_focus_pos(p_ctrl);
  list_select_item(p_ctrl, focus);
  ctrl_paint_ctrl(p_ctrl, TRUE);

  ret = manage_open_menu(ROOT_ID_RENAME, (u32)&rename, ROOT_ID_FAVORITE);

  return SUCCESS;
}

static RET_CODE on_favorite_list_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 focus;

  focus = list_get_focus_pos(p_ctrl);
  list_unselect_item(p_ctrl, focus);
  ctrl_paint_ctrl(p_ctrl, TRUE);

  list_class_proc(p_ctrl, msg, para1, para2);

  return SUCCESS;
}


BEGIN_MSGPROC(favorite_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_RENAME_UPDATE, on_favorite_rename_update)
	ON_COMMAND(MSG_RENAME_CHECK, on_favorite_rename_check)
END_MSGPROC(favorite_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(favorite_list_keymap, NULL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_BLUE, MSG_BLUE)
   ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(favorite_list_keymap, NULL)

BEGIN_MSGPROC(favorite_list_proc, list_class_proc)
	ON_COMMAND(MSG_BLUE, on_favorite_list_rename)
	ON_COMMAND(MSG_YES, on_favorite_list_yes)
	ON_COMMAND(MSG_FOCUS_UP, on_favorite_list_change)
   ON_COMMAND(MSG_FOCUS_DOWN, on_favorite_list_change)
	ON_COMMAND(MSG_PAGE_UP, on_favorite_list_change)
   ON_COMMAND(MSG_PAGE_DOWN, on_favorite_list_change)
END_MSGPROC(favorite_list_proc, list_class_proc)

