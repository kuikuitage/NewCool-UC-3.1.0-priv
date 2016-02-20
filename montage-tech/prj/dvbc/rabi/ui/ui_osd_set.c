/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_osd_set.h"

enum control_id
{
  IDC_INVALID = 0,
#if ENABLE_TTX_SUBTITLE
  IDC_SUBTITLE,
  //IDC_INSERTER,
#endif
  IDC_TIMEOUT,
  IDC_TRANSPARENCY
};

static u16 osd_set_cont_keymap(u16 key);


static RET_CODE osd_set_select_proc(control_t *ctrl, u16 msg, u32 para1,
                                    u32 para2);

RET_CODE open_osd_set(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[OSD_SET_ITEM_CNT];
  u8 i;
  osd_set_t osd_set;

#ifndef SPT_SUPPORT
  u16 y;
#if ENABLE_TTX_SUBTITLE
  u8 j;
  u16 stxt [OSD_SET_ITEM_CNT] =
  { IDS_SUBTITLE_DISPLAY, /*IDS_VBI_INSERTER,*/
     IDS_OSD_TIMEOUT, IDS_OSD_TRANSPARENCY };
  u8 opt_cnt [OSD_SET_ITEM_CNT] = { 2,/* 2,*/ 0, 0 };
  u16 opt_data[OSD_SET_ITEM_CNT][2] = {
    { IDS_OFF, IDS_ON },
    /*{ IDS_OFF, IDS_ON },*/
    {     0 },
    {     0 },
  };
  u16 droplist_page[OSD_SET_ITEM_CNT] = {0, /*0,*/ 5, 5};
#else
  u16 stxt [OSD_SET_ITEM_CNT] =
  { IDS_OSD_TIMEOUT, IDS_OSD_TRANSPARENCY };
  u8 opt_cnt [OSD_SET_ITEM_CNT] = { 0, 0 };
  u16 droplist_page[OSD_SET_ITEM_CNT] = {5, 5};
#endif
#endif

#ifndef SPT_SUPPORT
  p_cont =
    ui_comm_root_create(ROOT_ID_OSD_SET, 0, COMM_BG_X, COMM_BG_Y, 
    COMM_BG_W,  COMM_BG_H, 0, IDS_OSD_SET);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, osd_set_cont_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  y = OSD_SET_ITEM_Y;
  for (i = 0; i < OSD_SET_ITEM_CNT; i++)
  {
#if ENABLE_TTX_SUBTITLE
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_SUBTITLE + i),
#else
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_TIMEOUT + i),
#endif
                                        OSD_SET_ITEM_X, y,
                                        OSD_SET_ITEM_LW,
                                        OSD_SET_ITEM_RW);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], osd_set_select_proc);

#if ENABLE_TTX_SUBTITLE
    switch (i)
    {
      case 1:  // 2 IDC_TIMEOUT
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
                                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
        ui_comm_select_set_num_range(p_ctrl[i], 3, 10, 1, 0, NULL);
        break;
      case 2: // 3 IDC_TRANSPARENCY
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
                                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
        ui_comm_select_set_num_range(p_ctrl[i], 0, 40, 10, 0, NULL);
        break;
      default:
        ui_comm_select_set_param(p_ctrl[i], TRUE,
                                   CBOX_WORKMODE_STATIC, opt_cnt[i],
                                   CBOX_ITEM_STRTYPE_STRID, NULL);
        for (j = 0; j < opt_cnt[i]; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, opt_data[i][j]);
        }
    }
#else
    switch (i)
    {
      case 0:
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
                                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
        ui_comm_select_set_num_range(p_ctrl[i], 3, 10, 1, 0, NULL);
        break;
      case 1:
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
                                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
        ui_comm_select_set_num_range(p_ctrl[i], 0, 40, 10, 0, NULL);
        break;
      default:
        break;
    }
#endif
    ui_comm_select_create_droplist(p_ctrl[i], droplist_page[i]);

    ctrl_set_related_id(p_ctrl[i],
                        0,                                     /* left */
                        (u8)((i - 1 +
                              OSD_SET_ITEM_CNT) %
                             OSD_SET_ITEM_CNT + 1),            /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % OSD_SET_ITEM_CNT + 1)); /* down */

    y += OSD_SET_ITEM_H + OSD_SET_ITEM_V_GAP;
  }
#else
#endif
  /* set focus according to current info */
  sys_status_get_osd_set(&osd_set);

#if ENABLE_TTX_SUBTITLE
  ui_comm_select_set_focus(p_ctrl[0], osd_set.enable_subtitle);
 // ui_comm_select_set_focus(p_ctrl[1], osd_set.enable_vbinserter);
  ui_comm_select_set_num(p_ctrl[1], osd_set.timeout);
  ui_comm_select_set_num(p_ctrl[2], osd_set.transparent);
#else
  ui_comm_select_set_num(p_ctrl[0], osd_set.timeout);
  ui_comm_select_set_num(p_ctrl[1], osd_set.transparent);
#endif

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_osd_set_select_change(control_t *ctrl, u16 msg, u32 para1,
                                    u32 para2)
{
  u8 focus = (u8)(para2);
  osd_set_t osd_set;
  u8 ctrl_id = ctrl_get_ctrl_id(ctrl_get_parent(ctrl));

  // check for initialize
  if(!ctrl_is_onfocus(ctrl))
  {
    return SUCCESS;
  }

  sys_status_get_osd_set(&osd_set);
  switch (ctrl_id)
  {
#if ENABLE_TTX_SUBTITLE
  case IDC_SUBTITLE:
    osd_set.enable_subtitle = focus;
    break;
  #if 0
  case IDC_INSERTER:
    osd_set.enable_vbinserter = focus;
    ui_enable_vbi_insert((BOOL)focus);
    break;
  #endif
#endif
  case IDC_TIMEOUT:
    osd_set.timeout = focus;
    break;
  case IDC_TRANSPARENCY:
    osd_set.transparent = focus;
    gdi_set_global_alpha((100 - focus) * 255 / 100);        
    break;
  default:
    MT_ASSERT(0);
    return ERR_FAILURE;
  }

  sys_status_set_osd_set(&osd_set);
  sys_status_save();

  if(ctrl_id == IDC_TRANSPARENCY)
  {
    ctrl_paint_ctrl(ctrl_get_root(ctrl), TRUE);
  }

  return SUCCESS;
}

BEGIN_KEYMAP(osd_set_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(osd_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(osd_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_osd_set_select_change)
END_MSGPROC(osd_set_select_proc, cbox_class_proc)                             

