#include "ui_common.h"

#include "ui_volume_usb.h"
#include "ui_mute.h"
#include "ui_usb_music.h"
#include "ui_usb_picture.h"
#include "ui_usb_music_fullscreen.h"


enum local_msg
{
  MSG_AUDIO = MSG_LOCAL_BEGIN + 900,
  MSG_PARENT,
};
enum control_id
{
  IDC_INVALID = 0,
  IDC_ICON,
  IDC_BAR,
};

//static dvbs_prog_node_t curn_prog;
//BOOL is_display_pg = TRUE;
//BOOL is_no_audio_usb = FALSE;
static u8 curn_volume;

u16 volume_usb_keymap(u16 key);
RET_CODE volume_usb_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 volume_bar_usb_keymap(u16 key);
RET_CODE volume_bar_usb_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);


void set_volume_usb(u8 volume)
{    
  avc_setvolume_1(class_get_handle_by_id(AVC_CLASS_ID), volume); 
  curn_volume = volume;
}


RET_CODE open_volume_usb(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_frm;
  control_t *p_bar = NULL, *p_icon = NULL;
  u8 volume;

 // is_no_audio_usb =(BOOL) para2;

  if(ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  /* get volume */
  curn_volume = sys_status_get_global_volume();
  volume = curn_volume;

  
  switch(para2)
  {
	case V_KEY_VDOWN:
      if(volume > 0)
      {
        volume--;
        /* set volume */
        set_volume_usb(volume);
      }
      break;
	case V_KEY_VUP:
      if(volume < VOLUME_USB_MAX)
      {
        volume++;
        /* set volume */
        set_volume_usb(volume);
      }
    default:
      /* do nothing */;
  }
  p_cont = fw_create_mainwin(ROOT_ID_VOLUME_USB,
                             VOLUME_USB_L, VOLUME_USB_T,
                             VOLUME_USB_W, VOLUME_USB_H,
                             (u8)para1, 0,
                             OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_POPUP_BG,
                  RSI_POPUP_BG, RSI_POPUP_BG);
  ctrl_set_proc(p_cont, volume_usb_proc);
  ctrl_set_keymap(p_cont, volume_usb_keymap);

  p_frm = ctrl_create_ctrl(CTRL_CONT, 0,
                           VOLUME_BAR_USB_CONT_X, VOLUME_BAR_USB_CONT_Y,
                           VOLUME_BAR_USB_CONT_W, VOLUME_BAR_USB_CONT_H,
                           p_cont, 0);

  ctrl_set_rstyle(p_frm,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG);

  p_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_ICON,
                           VOLUME_BAR_USB_ICON_X, VOLUME_BAR_USB_ICON_Y,
                           VOLUME_BAR_USB_ICON_W, VOLUME_BAR_USB_ICON_H,
                           p_frm, 0);
  bmap_set_content_by_id(p_icon, IM_ICON_SOUND);                     

  p_bar = ctrl_create_ctrl(CTRL_PBAR, IDC_BAR,
                           VOLUME_BAR_USB_X, VOLUME_BAR_USB_Y,
                           VOLUME_BAR_USB_W, VOLUME_BAR_USB_H,
                           p_frm, 0);
  ctrl_set_rstyle(p_bar, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_mrect(p_bar, 0, 0, VOLUME_BAR_USB_W, VOLUME_BAR_USB_H);
  pbar_set_rstyle(p_bar, RSI_PROGRESS_BAR_VOLUME_MID, RSI_PBACK, RSI_PROGRESS_BAR_VOLUME_MID);
  pbar_set_count(p_bar, 0, VOLUME_USB_MAX, VOLUME_USB_MAX);
  pbar_set_direction(p_bar, 1);
  pbar_set_workmode(p_bar, 0, 4);
  pbar_set_current(p_bar, volume);
  ctrl_set_proc(p_bar, volume_bar_usb_proc);
  ctrl_set_keymap(p_bar, volume_bar_usb_keymap);

  ctrl_default_proc(p_bar, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  return SUCCESS;
}


RET_CODE close_volume_usb(void)
{
	sys_status_set_global_volume(curn_volume);
	sys_status_save();
  manage_close_menu(ROOT_ID_VOLUME_USB, 0, 0);
  return SUCCESS;
}


static RET_CODE on_exit_volume_usb(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_volume_usb();
  return SUCCESS;
}

static RET_CODE on_change_volume_usb(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 volume = 0;

  if(fw_find_root_by_id(ROOT_ID_IPTV_PLAYER) == NULL)
  {
    if(para1 == V_KEY_F3 || para1 == V_KEY_SAT)
    {
        OS_PRINTF("###f3, f4 only for iptvplayer###\n");
        return SUCCESS;
    }
  }
  volume = curn_volume;

  if(msg == MSG_INCREASE)
  {
    if(volume < VOLUME_USB_MAX)
    {
      volume++;
    }
  }
  else
  {
    if(volume > 0)
    {
      volume--;
    }
  }
  /* set volume */
  set_volume_usb(volume);
  pbar_class_proc(p_ctrl, msg, para1, para2);
  ctrl_paint_ctrl(p_ctrl,TRUE);

  return SUCCESS;
}

#if 0

static RET_CODE on_audio_usb(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 vkey = para1;
   if(!is_no_audio_usb)
  {
      manage_open_menu(ROOT_ID_AUDIO_SET, vkey, 0);
  }
  return SUCCESS;
}
#endif

static RET_CODE on_exit_volume_usb_soon(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  fw_notify_parent(ROOT_ID_VOLUME_USB, NOTIFY_T_KEY, FALSE, para1, 0, 0);
  close_volume_usb();
  return SUCCESS;
}

static RET_CODE on_pause_volume_usb(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root = NULL;
  p_root = fw_find_root_by_id(ROOT_ID_VIDEO_PLAYER);

  if(fw_get_first_active_id() == ROOT_ID_VIDEO_PLAYER)
  {
     fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_PAUSE, 0, 0);
     close_volume_usb();
  }
  return SUCCESS;
}

BEGIN_KEYMAP(volume_usb_keymap, NULL)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_EXIT, MSG_EXIT)
ON_EVENT(V_KEY_INFO,MSG_EXIT)
ON_EVENT(V_KEY_HOT_XEXTEND, MSG_PARENT)
ON_EVENT(V_KEY_AUDIO,MSG_PARENT)
ON_EVENT(V_KEY_UP, MSG_PARENT)
ON_EVENT(V_KEY_DOWN, MSG_PARENT)
ON_EVENT(V_KEY_PAGE_UP, MSG_PARENT)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PARENT)
ON_EVENT(V_KEY_SAT, MSG_PARENT)
ON_EVENT(V_KEY_YELLOW, MSG_PARENT)
ON_EVENT(V_KEY_RED, MSG_PARENT)
ON_EVENT(V_KEY_BLUE, MSG_PARENT)
ON_EVENT(V_KEY_FAV, MSG_PARENT)
ON_EVENT(V_KEY_OK, MSG_PARENT)
ON_EVENT(V_KEY_STOP, MSG_PARENT)
ON_EVENT(V_KEY_TVRADIO, MSG_PARENT)
END_KEYMAP(volume_usb_keymap, NULL)

BEGIN_MSGPROC(volume_usb_proc, cont_class_proc)
ON_COMMAND(MSG_EXIT, on_exit_volume_usb)
//ON_COMMAND(MSG_AUDIO, on_audio_usb)
ON_COMMAND(MSG_PARENT, on_exit_volume_usb_soon)
END_MSGPROC(volume_usb_proc, cont_class_proc)

BEGIN_KEYMAP(volume_bar_usb_keymap, NULL)
ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
ON_EVENT(V_KEY_VUP, MSG_INCREASE)
//ON_EVENT(V_KEY_F3, MSG_DECREASE)
//ON_EVENT(V_KEY_SAT, MSG_INCREASE)
ON_EVENT(V_KEY_PAUSE, MSG_PAUSE)
END_KEYMAP(volume_bar_usb_keymap, NULL)

BEGIN_MSGPROC(volume_bar_usb_proc, pbar_class_proc)
ON_COMMAND(MSG_INCREASE, on_change_volume_usb)
ON_COMMAND(MSG_DECREASE, on_change_volume_usb)
ON_COMMAND(MSG_PAUSE, on_pause_volume_usb)
END_MSGPROC(volume_bar_usb_proc, pbar_class_proc)
