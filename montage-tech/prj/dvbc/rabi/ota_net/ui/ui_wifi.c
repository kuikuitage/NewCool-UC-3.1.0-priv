/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_wifi.h"
#include "wifi.h"
#include "net_svc.h"
//#include "ui_network_config.h"
#include "ui_wifi_link_info.h"

enum wifi_local_msg
{
  MSG_REFRESH = MSG_LOCAL_BEGIN + 200,
  MSG_ADD_WIFI,
  MSG_REFRESH_WIFI_SIGNAL,
};
 
enum wifi_ctrl_id
{
  IDC_WIFI_TITLE  = 1,
  IDC_CONNECT_STATUS_CONT,
  IDC_CONNECT_STATUS_LEFT,
  IDC_CONNECT_STATUS_RIGHT,
  IDC_CONNECT_STATUS,
  IDC_WIFI_LIST_CONT,
  IDC_WIFI_LIST_BAR,
  IDC_WIFI_LIST,
    
};

static list_xstyle_t wifi_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_ITEM_1,
  RSI_PBACK,
  RSI_LIST_ITEM_1,
};

static list_xstyle_t wifi_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_xstyle_t wifi_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_field_attr_t wifi_play_list_attr[WIFI_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
    45, 0, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR |STL_CENTER,
    600, 50, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
  { LISTFIELD_TYPE_ICON | STL_CENTER,
    100, 650, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
  { LISTFIELD_TYPE_ICON | STL_CENTER,
    100, 750, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
  { LISTFIELD_TYPE_STRID| STL_CENTER,
    100, 850, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
};

ethernet_device_t* p_wifi_dev = NULL;
static u32 wifi_ap_count = 0;
static wifi_info_t p_wifi_inf = {{0},{0}};
static net_conn_stats_t wifi_connt_stats;
static u16 g_conn_focus = INVALID_ID;
static u16 g_select_focus = INVALID_ID;
static u16 g_connecting_focus = INVALID_ID;
extern void on_config_ip_update(ethernet_device_t * p_dev);
extern net_conn_stats_t ui_get_net_connect_status(void);
extern void * ui_get_net_svc_instance(void);

u16 wifi_cont_keymap(u16 key);
u16 wifi_list_keymap(u16 key);

RET_CODE wifi_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE wifi_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 get_wifi_select_focus()
{
  return g_select_focus;
}

u16 get_wifi_conn_focus()
{
  return g_conn_focus;
}

void set_wifi_conn_focus(u16 focus)
{
  g_conn_focus = focus;
}

u16 get_wifi_connecting_focus()
{
  return g_connecting_focus;
}

static control_t *create_wifi_menu(void)
{
  control_t *p_menu = NULL;
  
  p_menu = fw_create_mainwin(ROOT_ID_WIFI,
                         WIFI_CONT_X, WIFI_CONT_Y, WIFI_CONT_W, WIFI_CONT_H,
                         ROOT_ID_SUBMENU, 0, OBJ_ATTR_ACTIVE, 0);
  
  ctrl_set_rstyle(p_menu, RSI_BOX3, RSI_BOX3, RSI_BOX3);
  ctrl_set_keymap(p_menu, wifi_cont_keymap);
  ctrl_set_proc(p_menu, wifi_cont_proc);

  return p_menu;
}

 //create wifi title
static void create_wifi_title(control_t *p_menu)
{
  control_t *p_title = NULL;
  
  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_WIFI_TITLE,
                             WIFI_TITLE_X, WIFI_TITLE_Y, WIFI_TITLE_W, WIFI_TITLE_H,
                             p_menu, 0);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_WIFI_MANAGER);
  
}

void paint_wifi_status(BOOL is_conn, BOOL is_paint)
{
  control_t *p_status = NULL;
  control_t *root = fw_find_root_by_id(ROOT_ID_WIFI);
  
  if(root == NULL)
  {
    return ;
  }
  
  p_status = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_WIFI, IDC_CONNECT_STATUS_CONT), IDC_CONNECT_STATUS);
  if(is_conn)
  {
    text_set_content_by_strid(p_status, IDS_WIFI_DEV_CONNECT);
  }
  else
  {
    text_set_content_by_strid(p_status, IDS_WIFI_DEV_NOT_CONNECT);
  }
  
  if(is_paint)
  {
    ctrl_paint_ctrl(p_status, TRUE);
  }
}

extern void gb2312_2_utf16le(char* inbuf, int in_len, char* outbuf, int out_len);

static void create_wifi_status(control_t *p_menu)
{
  control_t *p_status_cont = NULL, *p_title_left = NULL, *p_status = NULL;
  u16 utf16le_title[] = {0x8fde, 0x63a5,  0x72b6, 0x6001, 0xff1a, 0}; // ����״̬:
  
  //status container
  p_status_cont =
			ctrl_create_ctrl(CTRL_CONT, IDC_CONNECT_STATUS_CONT, WIFI_STATUS_CONT_X,
			WIFI_STATUS_CONT_Y, WIFI_STATUS_CONT_W,WIFI_STATUS_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_status_cont, RSI_BOX2, RSI_BOX2, RSI_BOX2);

/*
  //left arrow
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_CONNECT_STATUS_LEFT,
                             WIFI_STATUSL_X, WIFI_STATUSL_Y,
                             WIFI_STATUSL_W, WIFI_STATUSL_H,
                             p_status_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_ARROW_L);

  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_CONNECT_STATUS_RIGHT,
                             WIFI_STATUSR_X, WIFI_STATUSR_Y,
                             WIFI_STATUSR_W, WIFI_STATUSR_H,
                             p_status_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_ARROW_R);
*/
  //
  p_title_left = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_STATUS_LEFT,
                               WIFI_STATUS_LEFT_X, WIFI_STATUS_LEFT_Y, 
                               WIFI_STATUS_LEFT_W, WIFI_STATUS_LEFT_H,
                               p_status_cont, 0);
  
  ctrl_set_rstyle(p_title_left, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_title_left, STL_CENTER| STL_VCENTER);
  text_set_font_style(p_title_left, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
  text_set_content_type(p_title_left, TEXT_STRTYPE_UNICODE);
  text_set_content_by_unistr(p_title_left, utf16le_title);
  
  //status
  p_status = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_STATUS, WIFI_STATUS_X,
                             WIFI_STATUS_Y, WIFI_STATUS_W,
                             WIFI_STATUS_H, p_status_cont, 0);
  ctrl_set_rstyle(p_status, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_status, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
  text_set_align_type(p_status, STL_CENTER | STL_VCENTER);

  wifi_connt_stats = ui_get_net_connect_status();
  paint_wifi_status(wifi_connt_stats.is_wifi_conn, FALSE);
  
}

void paint_list_field_is_connect(u16 index, BOOL is_connect, BOOL is_paint)
{
  control_t *p_list = NULL;
  control_t *root = fw_find_root_by_id(ROOT_ID_WIFI);
  
  if(root == NULL || index == INVALID_ID)
  {
    return ;
  }
  
  p_list = ui_comm_root_get_ctrl(ROOT_ID_WIFI, IDC_WIFI_LIST);
  if(is_connect)
  {
    list_set_field_content_by_strid(p_list, index, 4, IDS_CONNECT);
  }
  else
  {
    list_set_field_content_by_strid(p_list, index, 4, 0);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_list, TRUE);
  }
}

static RET_CODE wifi_list_update(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  u16 i = 0;
  u8 asc_str[32] = {0};
  u16 temp = 0;
  wifi_ap_info_t ap_info = {{0}};
  u16 cnt = 0;

  sys_status_get_wifi_info(&p_wifi_inf);
  wifi_connt_stats = ui_get_net_connect_status();
  cnt = list_get_count(p_list);
  OS_PRINTF("###wifi list update cnt == %d ####\n", cnt);
  if(cnt == INVALID_ID || cnt == 0)
  {
    OS_PRINTF("####cnt is 65535 or 0, is invalidid list not update ,will return###\n");
    return ERR_FAILURE;
  }
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf(asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);
      /* NAME */
      #ifndef WIN32
      wifi_ap_info_get(p_wifi_dev,(u16)(start + i),&ap_info);
      #endif
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1, ap_info.essid);
      OS_PRINTF(" ####ui list update, Qual_level %3d,   Essid %s ####\n", ap_info.qual_level,ap_info.essid);
      /* LOCK ICON */
      if(ap_info.EncrypType != IW_ENC_NONE)
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 2, IM_HELP_LOCK);
   //     OS_PRINTF("@@the %s wifi sigal encrypttype is %d###\n",ap_info.essid, ap_info.EncrypType);
      }
      else
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 2, 0);
      }
      /*connect status*/
      if(strcmp(p_wifi_inf.essid, ap_info.essid) == 0 && strcmp("", p_wifi_inf.essid) != 0)
      {
        g_connecting_focus = (u16)(start + i);
        if(wifi_connt_stats.is_wifi_conn)
        {
          g_conn_focus = (u16)(start + i);
          paint_list_field_is_connect(g_conn_focus, 1, FALSE);
          OS_PRINTF("########ssdata essid is%s , and search wifi ap essid  is:%s#####",p_wifi_inf.essid,ap_info.essid);
        }
        else
        {
          g_conn_focus = (u16)(start + i);
          paint_list_field_is_connect(g_conn_focus, 0, FALSE);
          OS_PRINTF("########ssdata essid is%s , and search wifi ap essid  is:%s#####",p_wifi_inf.essid,ap_info.essid);
        }
      }
      else
      {
        paint_list_field_is_connect((u16)(start + i), 0, FALSE);
      }
      
       /*SIGNAL  ICON */
       temp = ap_info.qual_level;
       temp = temp/10;
      
       switch(temp)
       {
          case 1:
          case 2:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_1);
            break;
          case 3:
          case 4:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_2);
            break; 
          case 5:
          case 6:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3,IM_SIGNAL_3);
            break;  
          case 7:
          case 8:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_4);
            break;
          case 9:
          case 10:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_5);
            break;
           default:
            break;
          
       }
      
    }

  }

  return SUCCESS;
}

static void create_wifi_list(control_t *p_menu)
{
  control_t *p_list_sbar = NULL, *p_list = NULL;
  u16 i;  
  
  //scrollbar
  p_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_WIFI_LIST_BAR, WIFI_LIST_BAR_X,
    WIFI_LIST_BAR_Y, WIFI_LIST_BAR_W, WIFI_LIST_BAR_H, p_menu, 0);
  ctrl_set_rstyle(p_list_sbar, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_list_sbar, 1);
  sbar_set_direction(p_list_sbar, 0);
  sbar_set_mid_rstyle(p_list_sbar, RSI_OTT_SCROLL_BAR, RSI_OTT_SCROLL_BAR,
                     RSI_OTT_SCROLL_BAR);

  //list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_WIFI_LIST, WIFI_LIST_X,
    WIFI_LIST_Y, WIFI_LIST_W, WIFI_LIST_H, p_menu, 0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_list, wifi_list_keymap);
  ctrl_set_proc(p_list, wifi_list_proc);
 
  ctrl_set_mrect(p_list, WIFI_LIST_MIDL, WIFI_LIST_MIDT,
                    WIFI_LIST_MIDW+WIFI_LIST_MIDL, WIFI_LIST_MIDH+WIFI_LIST_MIDT);
  list_set_item_interval(p_list,WIFI_LCONT_LIST_VGAP);
  list_set_item_rstyle(p_list, &wifi_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, (u16)wifi_ap_count, WIFI_LIST_ITEM_NUM_ONE_PAGE);
  list_set_field_count(p_list, WIFI_LIST_FIELD, WIFI_LIST_ITEM_NUM_ONE_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, wifi_list_update, 0);
  ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

  for (i = 0; i < WIFI_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(wifi_play_list_attr[i].attr), (u16)(wifi_play_list_attr[i].width),
                        (u16)(wifi_play_list_attr[i].left), (u8)(wifi_play_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, wifi_play_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, wifi_play_list_attr[i].fstyle);
  }
  list_set_scrollbar(p_list, p_list_sbar);
  wifi_list_update(p_list, list_get_valid_pos(p_list), WIFI_LIST_ITEM_NUM_ONE_PAGE, 0);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); 
  
}

static void create_wifi_helpbar(control_t *p_menu)
{

  comm_help_data_t help_data =
  {
    3,3,
    {IDS_REFRESH,IDS_OK,IDS_MANUALLY_ADD},
    {IM_COLORBUTTON_RED,IM_HELP_OK,IM_COLORBUTTON_GREEN},
  };
  ui_comm_help_create(&help_data, p_menu);
  ui_comm_help_move_pos(p_menu, 200, -30, 600, 20, 42);
  
}
ethernet_device_t *get_wifi_dev_handle()
{
  return p_wifi_dev;
}

void ui_wifi_info_get()
{
  p_wifi_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
  if(p_wifi_dev == NULL)
  {
    OS_PRINTF("@@@p_wifi_dev == NULL@@@\n");
    return ;
  }
  {
    service_t *p_server = NULL;
    net_svc_cmd_para_t net_svc_para;
    net_svc_t *p_net_svc = NULL;

    p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
    if(p_net_svc->net_svc_is_busy(p_net_svc) == TRUE)
    {
      ui_comm_cfmdlg_open_ex(NULL, IDS_NETWORK_BUSY, NULL, 2000);
      OS_PRINTF("###########net service is busy ,will be return#########\n");
      return ;
    }
    net_svc_para.p_eth_dev = p_wifi_dev;
    p_server = (service_t *)ui_get_net_svc_instance();
    
    p_server->do_cmd(p_server, NET_DO_WIFI_SERACH, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  }

}

static void popup_searching_win()
{
  comm_dlg_data_t search_dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_SEARCH_WIFI,
    0,
  };
  ui_comm_dlg_open_ex(&search_dlg_data);
}

RET_CODE ui_open_wifi(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  //wifi info get
  ui_wifi_info_get();
  
  //create menu
  p_menu = create_wifi_menu();
  
  //create title
  create_wifi_title(p_menu);
  
  //create connect status control
  create_wifi_status(p_menu);

  //create wifi list
  create_wifi_list(p_menu);

  //create help bar
  create_wifi_helpbar(p_menu);

  fw_tmr_create(ROOT_ID_WIFI, MSG_REFRESH_WIFI_SIGNAL, 30 * SECOND, TRUE);
  
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  popup_searching_win();
  
  return SUCCESS;
}

static RET_CODE on_wifi_exit(control_t *p_cont,
                              u16 msg, u32 para1, u32 para2)
{

  ui_comm_dlg_close_ex();
 // retry_cnt = 0;
  fw_tmr_destroy(ROOT_ID_WIFI, MSG_REFRESH_WIFI_SIGNAL);
  manage_close_menu(ROOT_ID_WIFI, para1, para2);
  return SUCCESS;
}

static RET_CODE on_wifi_destory(control_t *p_cont,
                              u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("####wifi destory###\n");
  g_conn_focus = INVALID_ID;
  g_select_focus = INVALID_ID;
  g_connecting_focus = INVALID_ID;
  
  return ERR_NOFEATURE;
}


static RET_CODE on_wifi_search_ok(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  
  wifi_ap_count = para1;
  OS_PRINTF("@@@@#######at wifi ui, after search get ap count == %d@@\n", wifi_ap_count);
  /* if(wifi_ap_count < 0 || wifi_ap_count == 0)
  {
   while(retry_cnt < 3)
    {
      OS_PRINTF("@@@@after search wifi count <= 0, will retry search@@\n");
      retry_search_wifi();
      retry_cnt ++;
    }
  }
  else
  {
    OS_PRINTF("@@@@wifi search cnt > 0 , will end retry search@@\n");
    retry_cnt = 0;
  }*/
  OS_PRINTF("@@@@wifi count == %d@@\n",wifi_ap_count);  
  list_set_focus_pos(p_list, 0);
  list_set_count(p_list, wifi_ap_count, WIFI_LIST_ITEM_NUM_ONE_PAGE);
  list_set_update(p_list, wifi_list_update, 0);
  wifi_list_update(p_list, list_get_valid_pos(p_list), WIFI_LIST_ITEM_NUM_ONE_PAGE, 0);

  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
  OS_PRINTF("@@@on_wifi_search_ok@@@\n");
  ctrl_paint_ctrl(p_list->p_parent, TRUE);
  ui_comm_dlg_close_ex();

  return SUCCESS;
}

static RET_CODE on_wifi_list_select(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0;

  if(wifi_ap_count < 0 || wifi_ap_count == 0)
  {
    OS_PRINTF("#######wifi ap count <=0, will return####\n");
    return ERR_FAILURE;
  }
  focus = list_get_focus_pos(p_list);

  if(65535 == focus)
  {
    focus = 0;
    list_set_focus_pos(p_list, 0);
    ctrl_paint_ctrl(p_list->p_parent, TRUE);
  }
  g_select_focus = focus;
  manage_open_menu(ROOT_ID_WIFI_LINK_INFO, 0, (u32)focus);//para1 means connect wifi
  return SUCCESS;
}

static RET_CODE on_wifi_list_add(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  manage_open_menu(ROOT_ID_WIFI_LINK_INFO, 1, 0);//para1 ==1 means  add wifi
  return SUCCESS;
}

static RET_CODE on_wifi_list_refresh(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  ui_wifi_info_get();
  list_set_focus_pos(p_list, 0);
  list_set_count(p_list, wifi_ap_count, WIFI_LIST_ITEM_NUM_ONE_PAGE);
  list_set_update(p_list, wifi_list_update, 0);
  wifi_list_update(p_list, list_get_valid_pos(p_list), WIFI_LIST_ITEM_NUM_ONE_PAGE, 0);
  OS_PRINTF("@@@on_wifi_list_refresh@@@\n");
  ctrl_paint_ctrl(p_list->p_parent, TRUE);
  
  return SUCCESS;
}

void on_wifi_list_ext_update(void)
{
  control_t *p_list = NULL;
  p_list = ui_comm_root_get_ctrl(ROOT_ID_WIFI, IDC_WIFI_LIST);

  mtos_printk("lou 222222222222 g_connecting_focus==%d g_connect_focus==%d\n",g_connecting_focus,g_conn_focus);
  list_set_count(p_list, wifi_ap_count, WIFI_LIST_ITEM_NUM_ONE_PAGE);
  list_set_update(p_list, wifi_list_update, 0);
  wifi_list_update(p_list, list_get_valid_pos(p_list), WIFI_LIST_ITEM_NUM_ONE_PAGE, 0);
  list_set_focus_pos(p_list, g_conn_focus);
  OS_PRINTF("@@@on_wifi_list_ext_update@@@\n");
  ctrl_paint_ctrl(p_list->p_parent, TRUE);
 }


void paint_wifi_list_field_not_connect(control_t *p_list)
{
  u16 i;
  u16 cnt;
  u16 start;
  
  start = list_get_valid_pos(p_list);
  cnt = list_get_count(p_list);
  for (i = 0; i < WIFI_LIST_ITEM_NUM_ONE_PAGE; i++)
  {
    if (i + start < cnt)
    {
      paint_list_field_is_connect((u16)(start + i), FALSE, TRUE);
    }
  }
  ctrl_paint_ctrl(p_list, TRUE);
}

static void paint_wifi_signal(control_t *p_list)
{
  u16 i;
  u16 temp;
  wifi_ap_info_t ap_info;
  u16 cnt;
  u16 start;
  
  start = list_get_valid_pos(p_list);
  cnt = list_get_count(p_list);
  for (i = 0; i < WIFI_LIST_ITEM_NUM_ONE_PAGE; i++)
  {
    if (i + start < cnt)
    {
     
      #ifndef WIN32
      wifi_ap_info_get(p_wifi_dev,(u16)(start + i),&ap_info);
      #endif
       temp = ap_info.qual_level;
       temp = temp/10;
      
       switch(temp)
       {
          case 1:
          case 2:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_1);
            break;
          case 3:
          case 4:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_2);
            break; 
          case 5:
          case 6:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3,IM_SIGNAL_3);
            break;  
          case 7:
          case 8:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_4);
            break;
          case 9:
          case 10:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_5);
            break;
           default:
            break;
          
       }
      
    }

  }
  OS_PRINTF("@@@@###########paint_wifi_signal###########@@@\n");
  ctrl_paint_ctrl(p_list, TRUE);
  
}


static RET_CODE on_wifi_list_signal_update(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  paint_wifi_signal(p_list);
  
  return SUCCESS;
}

static RET_CODE on_connect_wifi_ok(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ethernet_device_t * p_wifi_dev = get_wifi_dev_handle();

  OS_PRINTF("###connect wifi successfully ####\n");

  if(!is_wifi_static_ip())
  {
    OS_PRINTF("####wifi connect ok, dhcp####\n");
    on_config_ip_update(p_wifi_dev);//update wifi dhcp ipaddress
  }
  else
  {
    OS_PRINTF("####wifi connect ok, static ip####\n");
  }
  ui_comm_dlg_close_ex();
  //manage_close_menu(ROOT_ID_WIFI_LINK_INFO, 0, 0);

  return SUCCESS;

}

static RET_CODE on_dis_connect_wifi(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("############### dis connect wifi #####################\n");

  if(fw_find_root_by_id(ROOT_ID_WIFI))
  {
   ui_comm_dlg_close_ex();
  }
  return SUCCESS;

}

static RET_CODE on_connect_wifi_fail(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("############### connect wifi failed#####################\n");

  wifi_connt_stats = ui_get_net_connect_status();
  wifi_connt_stats.is_wifi_conn = FALSE;
  ui_set_net_connect_status(wifi_connt_stats);
  
  if(fw_find_root_by_id(ROOT_ID_WIFI))
  {
   ui_comm_dlg_close_ex();
  }
  
  return SUCCESS;
}


BEGIN_KEYMAP(wifi_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  
END_KEYMAP(wifi_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(wifi_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_wifi_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_wifi_exit)
  ON_COMMAND(MSG_DESTROY, on_wifi_destory)
  ON_COMMAND(MSG_WIFI_AP_CONNECT, on_connect_wifi_ok)
  ON_COMMAND(MSG_WIFI_AP_DISCONNECT, on_dis_connect_wifi)
  ON_COMMAND(MSG_WIFI_AP_CONNECT_FAIL, on_connect_wifi_fail)
END_MSGPROC(wifi_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(wifi_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_RED,MSG_REFRESH)
  ON_EVENT(V_KEY_GREEN,MSG_ADD_WIFI)
END_KEYMAP(wifi_list_keymap, NULL)

BEGIN_MSGPROC(wifi_list_proc, list_class_proc)
  ON_COMMAND(MSG_GET_WIFI_AP_CNT, on_wifi_search_ok)
  ON_COMMAND(MSG_SELECT, on_wifi_list_select)
  ON_COMMAND(MSG_REFRESH, on_wifi_list_refresh)
  ON_COMMAND(MSG_ADD_WIFI, on_wifi_list_add)
  ON_COMMAND(MSG_REFRESH_WIFI_SIGNAL, on_wifi_list_signal_update)
  
END_MSGPROC(wifi_list_proc, list_class_proc)
