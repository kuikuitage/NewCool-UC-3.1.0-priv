/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
//#include "sowell.h"
#include "ui_ca_info.h"
#include "ui_ca_public.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_BG,
  IDC_TITLE1,
  IDC_TITLE2,
  IDC_TITLE3,
  IDC_TITLE4,
  IDC_TITLE5,
  IDC_TITLE6,
  IDC_CA_INFO_HEAD1,
  IDC_CA_INFO_HEAD2,
  IDC_CA_INFO_HEAD3,
  IDC_CA_INFO_HEAD4,
  IDC_CA_INFO_HEAD5,
  IDC_CA_INFO_HEAD6,
  IDC_CA_INFO_LIST1,
  IDC_CA_INFO_LIST2,
  IDC_CA_INFO_LIST3,
  IDC_CA_INFO_LIST4,
  IDC_CA_INFO_LIST5,
  IDC_CA_INFO_LIST6,
  IDC_CA_INFO_TOTAL_IPP,
  IDC_CA_INFO_TOTAL_IPP_CONTENT,
};

enum prv_msg_t
{
  MSG_DEL_SET = MSG_USER_BEGIN + 200,
  MSG_DEL_ALL
};

enum del_status_t
{
  STATUS_DEL_NULL = 0,
  STATUS_DEL_FALSE,
  STATUS_DEL_TRUE,
};

static list_xstyle_t ca_info_item_rstyle =
{
	RSI_PBACK,
	RSI_COMM_CONT_SH,
	RSI_SELECT_F,
	RSI_SELECT_F,
	RSI_SELECT_F,
};


static list_xstyle_t ca_info_field_fstyle =
{
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
};

static list_xstyle_t ca_info_field_rstyle =
{
	RSI_PBACK,
	RSI_PBACK,
	RSI_IGNORE,
	RSI_IGNORE,
	RSI_IGNORE,
};

static list_field_attr_t ca_list_attr[CA_LIST_BG1_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
    CA_PLIST_BG_W / 6, 0, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
    CA_PLIST_BG_W / 6, CA_PLIST_BG_W / 6, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    CA_PLIST_BG_W / 6, 2 * CA_PLIST_BG_W / 6, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    CA_PLIST_BG_W / 6, 3 * CA_PLIST_BG_W / 6, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    CA_PLIST_BG_W / 6, 4 * CA_PLIST_BG_W / 6, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    CA_PLIST_BG_W / 6, 5 * CA_PLIST_BG_W / 6, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },
};

static list_field_attr_t ca_info_list_attr[CA_LIST_BG_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR| STL_CENTER| STL_VCENTER,
    CA_PLIST_BG_W / 3, 0, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR| STL_CENTER| STL_VCENTER,
    CA_PLIST_BG_W / 3, CA_PLIST_BG_W / 3, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR| STL_CENTER| STL_VCENTER,
    CA_PLIST_BG_W / 3, 2 * CA_PLIST_BG_W / 3, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },
};

static u8 curn_index = 0;
static u8 g_sub_curn_index = 0;
static u16 itemmenu1_btn_str[ITEMMENU_CNT] =
{
	IDS_CA_ACCREDIT_INFO, IDS_CA_SLOT, IDS_CA_IPPV, IDS_CA_EIGENVALUE
};
static u16 itemmenu2_btn_str[ITEMMENU_CNT1] =
{
	IDS_CA_PRODUCT_ID, IDS_CA_RECORDED, IDS_CA_AUTHORIZE_DEADLINE
};
static u16 itemmenu3_btn_str[ITEMMENU_CNT1] =
{
	IDS_CA_SLOT_ID, IDS_CA_SLOT_POINT, IDS_CA_CREDIT
};

static u16 itemmenu4_btn_str[ITEMMENU_CNT2] =
{
	IDS_CA_SLOT_ID, IDS_CA_PROGRAM_ID, IDS_CA_STATE, IDS_CA_RECORDED, IDS_CA_PRICE, IDS_CA_AUTHORIZE_DEADLINE,
};

static u16 itemmenu7_btn_str[ITEMMENU_CNT1] =
{
	IDS_CA_AREA_CODE, IDS_CA_BUOUQUET_ID, IDS_CA_EIGENVALUE_INFO
};

static product_entitles_info_t *p_entitle = NULL;
static ipps_info_t *p_ippv = NULL;
static burses_info_t *p_burse = NULL;
static product_detitles_info_t *p_detitle_info = NULL;
static u32 del_flag[CA_INFO_DETITLE_MAX_NUM];
static ipp_info_t g_temp_ipp_info[160];

u16 ca_info_keymap(u16 key);
RET_CODE ca_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 ca_info_button_keymap(u16 key);
RET_CODE ca_info_button_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ca_info_list_keymap(u16 key);
RET_CODE ca_info_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static RET_CODE ca_info_entitle_info_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u8 asc_str[64];
  
  if(p_entitle == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      memset(asc_str, 0, sizeof(asc_str));
      sprintf((char*)asc_str,"%d",(int)p_entitle->p_entitle_info[i+start].product_id);
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str); 

      //list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, 
      //p_entitle->p_entitle_info[i + start].states?"No authorization":"Authorization OK"); 
      UI_PRINTF("provider_info_list1_update item[%d] states[%d]\n",i + start, p_entitle->p_entitle_info[i + start].states);
      memset(asc_str, 0, sizeof(asc_str));
      switch(p_entitle->p_entitle_info[i+start].can_tape)
      {
        case 0:
          sprintf((char*)asc_str, "%s", "FALSE");
          break;

        case 1:
          sprintf((char*)asc_str, "%s", "TRUE");
          break;
        default:
          break;
      }
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1, asc_str);

      memset(asc_str, 0, sizeof(asc_str));
      if(p_entitle->p_entitle_info[i+start].states == 0)
      {
        sprintf((char*)asc_str,"%04d.%02d.%02d",
                  p_entitle->p_entitle_info[i+start].expired_time[0] * 100 + 
                  p_entitle->p_entitle_info[i+start].expired_time[1], 
                  p_entitle->p_entitle_info[i+start].expired_time[2],
                  p_entitle->p_entitle_info[i+start].expired_time[3]);
      }
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 2, asc_str);  
    }
  }

  return SUCCESS;
}

static RET_CODE on_ca_info_entitle_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST1);

  p_entitle = (product_entitles_info_t *)para2;
  UI_PRINTF("@@@@@ p_entitle->max_num = %d\n", p_entitle->max_num);
  list_set_count(p_list, p_entitle->max_num, CA_LIST_BG_PAGE);
  if(p_entitle->max_num > 0)
  {
    list_set_focus_pos(p_list, 0);
  }
  
  ca_info_entitle_info_update(p_list, list_get_valid_pos(p_list), CA_LIST_BG_PAGE, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_list), TRUE);
  
  return SUCCESS;
}

static RET_CODE ca_info_slot_info_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u8 asc_str[64];

  if(p_burse == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      memset(asc_str, 0, sizeof(asc_str));
      sprintf((char*)asc_str,"%d",(int)p_burse->p_burse_info[i + start].record_index);
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str); 

      memset(asc_str, 0, sizeof(asc_str));
      sprintf((char*)asc_str,"%d",(int)p_burse->p_burse_info[i + start].balance);
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1,  asc_str); 

      memset(asc_str, 0, sizeof(asc_str));
      sprintf((char*)asc_str,"%lu", p_burse->p_burse_info[i + start].credit_limit);
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 2,  asc_str); 

    }
  }

  return SUCCESS;
}

static RET_CODE on_ca_info_slot_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST2);

  p_burse = (burses_info_t *)para2;
  UI_PRINTF("@@@@@ slot max num = %d\n", p_burse->max_num);
  list_set_count(p_list, p_burse->max_num, CA_LIST_BG_PAGE);
  if(p_burse->max_num > 0)
  {
    list_set_focus_pos(p_list, 0);
  }
  ca_info_slot_info_update(p_list, list_get_valid_pos(p_list), CA_LIST_BG_PAGE, 0);

  return SUCCESS;
}


static RET_CODE ca_info_ippv_info_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u8 asc_str[64];

  if(p_ippv == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {   
      //slot_id
      memset(asc_str, 0, sizeof(asc_str));
      sprintf((char*)asc_str, "%d", (p_ippv->p_ipp_info[i + start].burse_id));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str); 

      //product_id
      memset(asc_str, 0, sizeof(asc_str)); 
      sprintf((char*)asc_str, "%lu", (p_ippv->p_ipp_info[i + start].product_id));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1, asc_str); 

      //product state
      memset(asc_str, 0, sizeof(asc_str));
      switch(p_ippv->p_ipp_info[i + start].book_state_flag)
      {
        case 0:
          sprintf((char*)asc_str, "%s", "BOOKING");
          break;
        case 1:
          sprintf((char*)asc_str, "%s", "VIEWED");
          break;
        default:
          break;
      }
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 2, asc_str); 

      //recorded
      memset(asc_str, 0, sizeof(asc_str));
      switch(p_ippv->p_ipp_info[i + start].is_can_tape)
      {
        case 0:
          sprintf((char*)asc_str, "%s", "FALSE");
          break;
        case 1:
          sprintf((char*)asc_str, "%s", "TRUE");
          break;
        default:
          break;
      }
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 3, asc_str); 

      //price
      memset(asc_str, 0, sizeof(asc_str));
      sprintf((char*)asc_str, "%d", (p_ippv->p_ipp_info[i + start].price));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 4, asc_str); 

      //over data
      memset(asc_str, 0, sizeof(asc_str));
      sprintf((char*)asc_str,"%04d.%02d.%02d",p_ippv->p_ipp_info[i + start].expired_time[0] * 100 + 
              p_ippv->p_ipp_info[i + start].expired_time[1], 
              p_ippv->p_ipp_info[i + start].expired_time[2],
              p_ippv->p_ipp_info[i + start].expired_time[3]);     
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 5, asc_str);
    }
  }
  return SUCCESS;
}

void remove_some_unvalue_ipp_info(ipps_info_t * p_ipp)
{
  u32 i = 0;
  u16 k = 0;

  memset((void *)g_temp_ipp_info, 0, sizeof(ipp_info_t) * 160);
  for(i = 0; i < p_ipp->max_num; i++)
  {
    if(p_ipp->p_ipp_info[i].price != 0)
    {
      memcpy(&g_temp_ipp_info[i - k], &p_ipp->p_ipp_info[i], sizeof(ipp_info_t));
    }
    else
    {
      k++;
    }
  }
  p_ipp->max_num = p_ipp->max_num - k;

  for(i = 0; i < p_ipp->max_num; i++)
  {
    memcpy(&p_ipp->p_ipp_info[i], &g_temp_ipp_info[i], sizeof(ipp_info_t));
  }
}

static RET_CODE on_ca_info_ippv_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl = NULL;
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST3);
  u8 asc_str[64];
  UI_PRINTF("enter on_provider_info_ippv\n");
  p_ippv = (ipps_info_t *)para2;

  //remove some ipp info
  remove_some_unvalue_ipp_info(p_ippv);
  list_set_count(p_list, p_ippv->max_num, CA_LIST_BG_PAGE);
  if(p_ippv->max_num > 0)
  {
    list_set_focus_pos(p_list, 0);
  }
  UI_PRINTF("@@@@@ p_ippv max num = %d\n", p_ippv->max_num);
  
  ca_info_ippv_info_update(p_list, list_get_valid_pos(p_list), CA_LIST_BG_PAGE, 0);
  
  //total ippv
  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str,"%d", p_ippv->max_num);     
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_TOTAL_IPP_CONTENT);
  text_set_content_by_ascstr(p_ctrl, asc_str);

  return SUCCESS;
}

static RET_CODE on_ca_info_aclist_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  //RET_CODE ret = para1;
  u8 asc_str[64];
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST6);
  u32 *p_aclist;
  p_aclist = (u32 *)para2;
  UI_PRINTF("@@@@@ on_ca_info_aclist_info, %X, %ld, %ld , %ld\n", p_aclist, p_aclist[3], p_aclist[4], p_aclist[5]);

  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str, "%lu", p_aclist[0]);
  list_set_field_content_by_ascstr(p_list, 0, 0, asc_str); 

  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str, "%lu", p_aclist[1]);
  list_set_field_content_by_ascstr(p_list, 0, 1, asc_str); 
  
  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str , "%lu", p_aclist[3]);
  list_set_field_content_by_ascstr(p_list, 0, 2, asc_str);
  
  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str , "%lu", p_aclist[4]);
  list_set_field_content_by_ascstr(p_list, 1, 2, asc_str); 
  
  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str , "%lu", p_aclist[5]);
  list_set_field_content_by_ascstr(p_list, 2, 2, asc_str); 
  
  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str , "%lu", p_aclist[6]);
  list_set_field_content_by_ascstr(p_list, 3, 2, asc_str); 
  
  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str , "%lu", p_aclist[7]);
  list_set_field_content_by_ascstr(p_list, 4, 2, asc_str); 

  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str , "%lu", p_aclist[8]);
  list_set_field_content_by_ascstr(p_list, 5, 2, asc_str); 

  memset(asc_str, 0, sizeof(asc_str));
  sprintf((char*)asc_str , "%lu", p_aclist[9]);
  list_set_field_content_by_ascstr(p_list, 6, 2, asc_str); 

  return SUCCESS;
}

static RET_CODE on_ca_info_del_one(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u16 index = 0;
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST5);
  if(ctrl_get_attr(p_list) == OBJ_STS_SHOW)
  {
    if((0 != p_detitle_info->max_num) && (0xFFFF !=p_detitle_info->max_num))
    {
      index = list_get_focus_pos(p_list);
      if(del_flag[index] == STATUS_DEL_FALSE)
      {
        list_set_field_content_by_icon(p_list, index, 2, IM_TV_DEL);
        del_flag[index] = STATUS_DEL_TRUE;
      }
      else if(del_flag[index] == STATUS_DEL_TRUE)
      {
         list_set_field_content_by_icon(p_list, index, 2, 0);
        del_flag[index] = STATUS_DEL_FALSE;
      }
    }
    ctrl_paint_ctrl(ctrl_get_root(p_list), TRUE);
  }
  return SUCCESS;
}


static RET_CODE on_ca_info_del_all(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u16 i;
  u16 del_cnt = 0;
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST5);
  if((0 != p_detitle_info->max_num) && (0xFFFF != p_detitle_info->max_num))
  {
    for(i=0; i<CA_INFO_DETITLE_MAX_NUM; i++)
    {
      if(del_flag[i] == STATUS_DEL_TRUE)
        del_cnt ++;
    }
    if(del_cnt == p_detitle_info->max_num)
    {
      for (i = 0; i < p_detitle_info->max_num; i++)
      {
        list_set_field_content_by_icon(p_list, i, 2, 0);
        del_flag[i] = STATUS_DEL_FALSE;
      }
    }
    else
    {
      for (i = 0; i < p_detitle_info->max_num; i++)
      {
        list_set_field_content_by_icon(p_list, i, 2, IM_TV_DEL);
        del_flag[i] = STATUS_DEL_TRUE;
      }
    }
  }
  ctrl_paint_ctrl(ctrl_get_root(p_list), TRUE);
  return SUCCESS;
}

static RET_CODE on_provider_info_accept_notify(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u32 event_id = 0;
  UI_PRINTF("enter on_provider_info_accept_notify\n");
  switch(msg)
  {
    case MSG_CA_INIT_OK:
      ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, (0<<16)|(0+CA_LIST_BG_PAGE), 0);
      ui_ca_do_cmd(CAS_CMD_IPPV_INFO_GET, 0, 0);
      ui_ca_do_cmd(CAS_CMD_BURSE_INFO_GET, 0, 0);
      ui_ca_do_cmd(CAS_CMD_IPP_REC_INFO_GET, 0, 0);
      break;

    case MSG_CA_EVT_NOTIFY:
      event_id = para2;
      if(event_id == CAS_S_ADPT_CARD_REMOVE)
      {
        ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, (0<<16)|(0+CA_LIST_BG_PAGE), 0);
        ui_ca_do_cmd(CAS_CMD_IPPV_INFO_GET, 0, 0);
        ui_ca_do_cmd(CAS_CMD_BURSE_INFO_GET, 0, 0);
        ui_ca_do_cmd(CAS_CMD_IPP_REC_INFO_GET, 0, 0);
      }
      break;

	 default:
	 	break;
  }

  return SUCCESS;
}

static RET_CODE on_change_focus_to_list(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont = p_ctrl->p_parent;
  control_t *p_next_ctrl = NULL;
  u16 id;
  id = ctrl_get_ctrl_id(p_ctrl);

  switch(id)
  {
    case IDC_TITLE1:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST1);
      break;
      
    case IDC_TITLE2:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST2);
      break;
      
    case IDC_TITLE3:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST3);
      break;
    case IDC_TITLE4:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST6);
      break;
	default:
	  break;
  }
  
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
  
  
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_next_ctrl, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_ca_info_list_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl = NULL;
  
  p_cont = ctrl_get_parent(p_list);
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,(IDC_TITLE1 + curn_index));      
      break;
      
    case MSG_FOCUS_LEFT:
      p_next_ctrl = ctrl_get_child_by_id(p_cont,(IDC_TITLE1 + curn_index));
      break;

    case MSG_FOCUS_UP:
    case MSG_FOCUS_DOWN:
    case MSG_PAGE_UP:
    case MSG_PAGE_DOWN:
      list_class_proc(p_list, msg, para1, para2);
      break;
      
    default:
      break;
  }

  if(p_next_ctrl != NULL)
  {
    ctrl_set_sts(p_next_ctrl, OBJ_STS_SHOW);
    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_list), TRUE);
    ctrl_paint_ctrl(p_next_ctrl, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_ca_item_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl, *p_mbox1, *p_mbox2, *p_mbox3, *p_mbox6;
  control_t *p_list1, *p_list2, *p_list3, *p_list6, *p_total_ipp, *p_total_ipp_content;
  u16 num = 0;
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      curn_index++;
      if(curn_index >3)
      {
        curn_index = 0;
      }
      break;
    case MSG_FOCUS_LEFT:
      curn_index--;
      if(curn_index >3)
      {
      curn_index = 3;
      }
      break;
    default:
      break;
  }
  p_cont = p_ctrl->p_parent;
  p_next_ctrl = ctrl_get_child_by_id(p_cont,(IDC_TITLE1 + curn_index));
  num = (IDC_TITLE1 + curn_index);
  p_list1 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST1);
  p_list2 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST2);
  p_list3 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST3);
  p_list6 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_LIST6);
  p_total_ipp = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_TOTAL_IPP);
  p_total_ipp_content = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_TOTAL_IPP_CONTENT);
  p_mbox1 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_HEAD1);
  p_mbox2 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_HEAD2);
  p_mbox3 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_HEAD3);
  p_mbox6 = ctrl_get_child_by_id(p_cont,IDC_CA_INFO_HEAD6); 
  if(num == IDC_TITLE1)
  {
    ctrl_set_sts(p_mbox2, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox3, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox6, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox1, OBJ_STS_SHOW);
    ctrl_set_sts(p_list2, OBJ_STS_HIDE);
    ctrl_set_sts(p_list3, OBJ_STS_HIDE);
    ctrl_set_sts(p_list6, OBJ_STS_HIDE);
    ctrl_set_sts(p_list1, OBJ_STS_SHOW);
    ctrl_set_sts(p_total_ipp, OBJ_STS_HIDE);
    ctrl_set_sts(p_total_ipp_content, OBJ_STS_HIDE);
    //ui_comm_help_create2((comm_help_data_t2 *)&ca_info_help_data, p_cont, 0);    
    ctrl_paint_ctrl(p_cont, TRUE);
  }
  else if(num == IDC_TITLE2)
  {
    ctrl_set_sts(p_mbox1, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox3, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox6, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox2, OBJ_STS_SHOW);
    ctrl_set_sts(p_list1, OBJ_STS_HIDE);
    ctrl_set_sts(p_list3, OBJ_STS_HIDE);
    ctrl_set_sts(p_list6, OBJ_STS_HIDE);
    ctrl_set_sts(p_list2, OBJ_STS_SHOW);
    ctrl_set_sts(p_total_ipp, OBJ_STS_HIDE);
    ctrl_set_sts(p_total_ipp_content, OBJ_STS_HIDE);
    //ui_comm_help_create2((comm_help_data_t2 *)&ca_info_help_data, p_cont, 0);    
    ctrl_paint_ctrl(p_cont, TRUE);
  }
  else if(num == IDC_TITLE3)
  {
    ctrl_set_sts(p_mbox1, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox2, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox6, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox3, OBJ_STS_SHOW);
    ctrl_set_sts(p_list1, OBJ_STS_HIDE);
    ctrl_set_sts(p_list2, OBJ_STS_HIDE);
    ctrl_set_sts(p_list6, OBJ_STS_HIDE);
    ctrl_set_sts(p_list3, OBJ_STS_SHOW);
    ctrl_set_sts(p_total_ipp, OBJ_STS_SHOW);
    ctrl_set_sts(p_total_ipp_content, OBJ_STS_SHOW);
    //ui_comm_help_create2((comm_help_data_t2 *)&ca_info_help_data, p_cont, 0);    
    ctrl_paint_ctrl(p_cont, TRUE);
  }
  else if(num == IDC_TITLE4)
  {
    ctrl_set_sts(p_mbox1, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox2, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox3, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox6, OBJ_STS_SHOW);
    ctrl_set_sts(p_list1, OBJ_STS_HIDE);
    ctrl_set_sts(p_list2, OBJ_STS_HIDE);
    ctrl_set_sts(p_list3, OBJ_STS_HIDE);
    ctrl_set_sts(p_list6, OBJ_STS_SHOW);
    ctrl_set_sts(p_total_ipp, OBJ_STS_HIDE);
    ctrl_set_sts(p_total_ipp_content, OBJ_STS_HIDE);
    //ui_comm_help_create2((comm_help_data_t2 *)&ca_info_help_data, p_cont, 0);    
    ctrl_paint_ctrl(p_cont, TRUE);
  }
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_next_ctrl, TRUE);
  return SUCCESS;
}

RET_CODE open_ca_info(u32 para1, u32 para2)
{
	control_t *p_cont,*p_btn[ITEMMENU_CNT],*p_list,*p_mbox1,*p_mbox2,*p_mbox3, *p_mbox6, *p_ctrl;
	u8 i;
	u16 x;
	u32 op_id = (u32)para1;

	p_cont = ui_comm_right_root_create(ROOT_ID_CA_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, ca_info_keymap);
	ctrl_set_proc(p_cont, ca_info_proc);

	p_entitle = NULL;
	p_ippv = NULL;
	p_burse = NULL;
	p_detitle_info = NULL;
	curn_index = 0;
    
	//title 
	for(i=0; i<ITEMMENU_CNT; i++)
	{
		x = ITEMMENU_BTN_X + (ITEMMENU_BTN_W + ITEMMENU_BTN_V_GAP) * i;
		p_btn[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_TITLE1+ i),
			                      x, ITEMMENU_BTN_Y, ITEMMENU_BTN_W,ITEMMENU_BTN_H,
			                      p_cont, 0);
		ctrl_set_rstyle(p_btn[i],RSI_ITEM_8_BG,RSI_OPT_BTN_H,RSI_ITEM_8_BG);
		ctrl_set_keymap(p_btn[i], ca_info_button_keymap);
		ctrl_set_proc(p_btn[i], ca_info_button_proc);
		text_set_font_style(p_btn[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
		text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
		text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_btn[i], itemmenu1_btn_str[i]);
	}

	//mbox1
	p_mbox1 = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_CA_INFO_HEAD1,
	                    CA_MBOX_BTN_X,CA_MBOX_BTN_Y,CA_MBOX_BTN_W,CA_MBOX_BTN_H,p_cont, 0);
	ctrl_set_rstyle(p_mbox1, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_sts(p_mbox1, OBJ_STS_SHOW);
	mbox_set_focus(p_mbox1, 1);
	mbox_enable_string_mode(p_mbox1, TRUE);
	mbox_set_content_strtype(p_mbox1, MBOX_STRTYPE_STRID);
	mbox_set_count(p_mbox1, CA_ITME_COUNT, CA_ITME_COUNT, 1);
	ctrl_set_mrect(p_mbox1, 0, 0, CA_MBOX_BTN_W, CA_MBOX_BTN_H);
	mbox_set_string_fstyle(p_mbox1, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	mbox_set_string_offset(p_mbox1, 0, 0);
	mbox_set_string_align_type(p_mbox1, STL_CENTER | STL_VCENTER);
	for (i = 0; i < CA_ITME_COUNT; i++)
	{
		mbox_set_content_by_strid(p_mbox1, i, itemmenu2_btn_str[i]);
	}
  
	//mbox2
	p_mbox2 = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_CA_INFO_HEAD2,
	                    CA_MBOX_BTN_X,CA_MBOX_BTN_Y,CA_MBOX_BTN_W,CA_MBOX_BTN_H,p_cont, 0);
	ctrl_set_rstyle(p_mbox2, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_sts(p_mbox2, OBJ_STS_HIDE);
	mbox_set_focus(p_mbox2, 0);
	mbox_enable_string_mode(p_mbox2, TRUE);
	mbox_set_content_strtype(p_mbox2, MBOX_STRTYPE_STRID);
	mbox_set_count(p_mbox2, CA_ITME_COUNT, CA_ITME_COUNT, 1);
	ctrl_set_mrect(p_mbox2, 0, 0, CA_MBOX_BTN_W, CA_MBOX_BTN_H);
	mbox_set_string_fstyle(p_mbox2, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	mbox_set_string_offset(p_mbox2, 0, 0);
	mbox_set_string_align_type(p_mbox2, STL_CENTER | STL_VCENTER);
	for (i = 0; i < CA_ITME_COUNT; i++)
	{
		mbox_set_content_by_strid(p_mbox2, i, itemmenu3_btn_str[i]);
	}
  
	//mbox3
	p_mbox3 = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_CA_INFO_HEAD3,
	                        CA_MBOX_BTN_X,CA_MBOX_BTN_Y,CA_MBOX_BTN_W,CA_MBOX_BTN_H,p_cont, 0);
	ctrl_set_rstyle(p_mbox3, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_sts(p_mbox3, OBJ_STS_HIDE);
	mbox_set_focus(p_mbox3, 0);
	mbox_enable_string_mode(p_mbox3, TRUE);
	mbox_set_content_strtype(p_mbox3, MBOX_STRTYPE_STRID);
	mbox_set_count(p_mbox3, CA_ITME_COUNT1, CA_ITME_COUNT1, 1);
	ctrl_set_mrect(p_mbox3, 0, 0, CA_MBOX_BTN_W, CA_MBOX_BTN_H);
	mbox_set_string_fstyle(p_mbox3, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	mbox_set_string_offset(p_mbox3, 0, 0);
	mbox_set_string_align_type(p_mbox3, STL_CENTER | STL_VCENTER);
	for (i = 0; i < ITEMMENU_CNT2; i++)
	{
		mbox_set_content_by_strid(p_mbox3, i, itemmenu4_btn_str[i]);
	}
  
	//mbox6
	p_mbox6 = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_CA_INFO_HEAD6,
	                        CA_MBOX_BTN_X,CA_MBOX_BTN_Y,CA_MBOX_BTN_W,CA_MBOX_BTN_H,p_cont, 0);
	ctrl_set_rstyle(p_mbox6, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_sts(p_mbox6, OBJ_STS_HIDE);
	mbox_set_focus(p_mbox6, 0);
	mbox_enable_string_mode(p_mbox6, TRUE);
	mbox_set_content_strtype(p_mbox6, MBOX_STRTYPE_STRID);
	mbox_set_count(p_mbox6, CA_ITME_COUNT, CA_ITME_COUNT, 1);
	ctrl_set_mrect(p_mbox6, 0, 0, CA_MBOX_BTN_W, CA_MBOX_BTN_H);
	mbox_set_string_fstyle(p_mbox6, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	mbox_set_string_offset(p_mbox6, 0, 0);
	mbox_set_string_align_type(p_mbox6, STL_CENTER | STL_VCENTER);
	for (i = 0; i < ITEMMENU_CNT1; i++)
	{
		mbox_set_content_by_strid(p_mbox6, i, itemmenu7_btn_str[i]);
	}
  
	//LIST1
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_CA_INFO_LIST1,
	                       CA_PLIST_BG_X, CA_PLIST_BG_Y, CA_PLIST_BG_W,CA_PLIST_BG_H, p_cont, 0);
	ctrl_set_keymap(p_list, ca_info_list_keymap);
	ctrl_set_proc(p_list, ca_info_list_proc);
	ctrl_set_sts(p_list, OBJ_STS_SHOW);
	ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	list_enable_select_mode(p_list, TRUE);
	list_set_select_mode(p_list, LIST_SINGLE_SELECT);
	list_set_item_rstyle(p_list, &ca_info_item_rstyle);
	list_set_count(p_list, 0, CA_LIST_BG_PAGE);
	list_set_field_count(p_list, CA_LIST_BG_FIELD, CA_LIST_BG_PAGE);
	list_set_focus_pos(p_list, 0);
	list_select_item(p_list, 0);
	list_set_update(p_list, ca_info_entitle_info_update, 0);
 
	for (i = 0; i < CA_LIST_BG_FIELD; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(ca_info_list_attr[i].attr), (u16)(ca_info_list_attr[i].width),
		                    (u16)(ca_info_list_attr[i].left), (u8)(ca_info_list_attr[i].top));
		list_set_field_rect_style(p_list, (u8)i, ca_info_list_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, ca_info_list_attr[i].fstyle);
	}
  //ca_info_list1_update(p_list, list_get_valid_pos(p_list), CA_LIST_BG_PAGE, 0);
  
  //LIST2
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_CA_INFO_LIST2,
	                   CA_PLIST_BG_X, CA_PLIST_BG_Y, CA_PLIST_BG_W,CA_PLIST_BG_H, p_cont, 0);
	ctrl_set_keymap(p_list, ca_info_list_keymap);
	ctrl_set_proc(p_list, ca_info_list_proc);
	ctrl_set_sts(p_list, OBJ_STS_HIDE);
	ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	list_enable_select_mode(p_list, TRUE);
	list_set_select_mode(p_list, LIST_SINGLE_SELECT);
	list_set_item_rstyle(p_list, &ca_info_item_rstyle);
	list_set_count(p_list, 0, CA_LIST_BG_PAGE);
	list_set_field_count(p_list, CA_LIST_BG_FIELD, CA_LIST_BG_PAGE);
	list_set_focus_pos(p_list, 0);
	list_select_item(p_list, 0);
	list_set_update(p_list, ca_info_slot_info_update, 0);

	for (i = 0; i < CA_LIST_BG_FIELD; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(ca_info_list_attr[i].attr), (u16)(ca_info_list_attr[i].width),
		                (u16)(ca_info_list_attr[i].left), (u8)(ca_info_list_attr[i].top));
		list_set_field_rect_style(p_list, (u8)i, ca_info_list_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, ca_info_list_attr[i].fstyle);
	}
  //ca_info_list1_update(p_list, list_get_valid_pos(p_list), CA_LIST_BG_PAGE, 0);
  
	//LIST3
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_CA_INFO_LIST3,
	                   CA_PLIST_BG_X, CA_PLIST_BG_Y, CA_PLIST_BG_W,CA_PLIST_BG_H - 50, p_cont, 0);
	ctrl_set_keymap(p_list, ca_info_list_keymap);
	ctrl_set_proc(p_list, ca_info_list_proc);
	ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_sts(p_list, OBJ_STS_HIDE);
	list_enable_select_mode(p_list, TRUE);
	list_set_select_mode(p_list, LIST_SINGLE_SELECT);
	list_set_item_rstyle(p_list, &ca_info_item_rstyle);
	list_set_count(p_list, 0, CA_LIST_BG_PAGE);
	list_set_field_count(p_list, CA_LIST_BG1_FIELD, CA_LIST_BG_PAGE);
	list_set_focus_pos(p_list, 0);
	list_select_item(p_list, 0);
	list_set_update(p_list, ca_info_ippv_info_update, 0);
	for (i = 0; i < CA_LIST_BG1_FIELD; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(ca_list_attr[i].attr), (u16)(ca_list_attr[i].width),
		                (u16)(ca_list_attr[i].left), (u8)(ca_list_attr[i].top));
		list_set_field_rect_style(p_list, (u8)i, ca_list_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, ca_list_attr[i].fstyle);
	}
	//ca_info_list2_update(p_list, list_get_valid_pos(p_list), CA_LIST_BG_PAGE, 0);

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_INFO_TOTAL_IPP),
	                      CA_TOTAL_IPP_X, CA_TOTAL_IPP_Y, CA_TOTAL_IPP_LW, CA_TOTAL_IPP_H, p_cont, 0);
	//ctrl_set_rstyle(p_ctrl, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_CA_TOTAL_IPP);
	ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_INFO_TOTAL_IPP_CONTENT),
	                      CA_TOTAL_IPP_X + CA_TOTAL_IPP_LW + CA_TOTAL_IPP_GAP, 
	                      CA_TOTAL_IPP_Y, CA_TOTAL_IPP_RW, CA_TOTAL_IPP_H, p_cont, 0);
	//ctrl_set_rstyle(p_ctrl, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER| STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE); 
	ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
  
	//LIST6
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_CA_INFO_LIST6,
	                       CA_PLIST_BG_X, CA_PLIST_BG_Y, CA_PLIST_BG_W,CA_PLIST_BG_H, p_cont, 0);
	ctrl_set_keymap(p_list, ca_info_list_keymap);
	ctrl_set_proc(p_list, ca_info_list_proc);
	ctrl_set_sts(p_list, OBJ_STS_HIDE);
	ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	list_enable_select_mode(p_list, TRUE);
	list_set_select_mode(p_list, LIST_SINGLE_SELECT);
	list_set_item_rstyle(p_list, &ca_info_item_rstyle);
	list_set_count(p_list, 7, CA_LIST_BG_PAGE);
	list_set_field_count(p_list, CA_LIST_BG_FIELD, CA_LIST_BG_PAGE);
	list_set_focus_pos(p_list, 0);
	//list_select_item(p_list, 0);
 
	for (i = 0; i < CA_LIST_BG_FIELD; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(ca_info_list_attr[i].attr), (u16)(ca_info_list_attr[i].width),
		                    (u16)(ca_info_list_attr[i].left), (u8)(ca_info_list_attr[i].top));
		list_set_field_rect_style(p_list, (u8)i, ca_info_list_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, ca_info_list_attr[i].fstyle);
	}

	//ui_comm_help_create2((comm_help_data_t2 *)&ca_info_help_data, p_cont, 0);
	ctrl_default_proc(p_btn[g_sub_curn_index], MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);
  
	ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, op_id, 0);
	ui_ca_do_cmd(CAS_CMD_BURSE_INFO_GET, op_id, 0);
	ui_ca_do_cmd(CAS_CMD_IPPV_INFO_GET, op_id, 0);
	ui_ca_do_cmd(CAS_CMD_ACLIST_INFO_GET, op_id, 0);
	
	return SUCCESS;
}


BEGIN_KEYMAP(ca_info_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(ca_info_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_info_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_CA_ENT_INFO, on_ca_info_entitle_info)
	ON_COMMAND(MSG_CA_BUR_INFO, on_ca_info_slot_info)
	ON_COMMAND(MSG_CA_IPV_INFO, on_ca_info_ippv_info)
	ON_COMMAND(MSG_CA_ACLIST_INFO, on_ca_info_aclist_info)
	ON_COMMAND(MSG_CA_EVT_NOTIFY, on_provider_info_accept_notify)
	ON_COMMAND(MSG_CA_INIT_OK, on_provider_info_accept_notify)
	ON_COMMAND(MSG_DEL_SET, on_ca_info_del_one)
	ON_COMMAND(MSG_DEL_ALL, on_ca_info_del_all)
END_MSGPROC(ca_info_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ca_info_button_keymap, NULL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(ca_info_button_keymap, NULL)

BEGIN_MSGPROC(ca_info_button_proc, text_class_proc)
	ON_COMMAND(MSG_FOCUS_LEFT, on_ca_item_change_focus)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_ca_item_change_focus)
	ON_COMMAND(MSG_FOCUS_DOWN, on_change_focus_to_list)
	ON_COMMAND(MSG_FOCUS_UP, on_change_focus_to_list)
END_MSGPROC(ca_info_button_proc, text_class_proc)

BEGIN_KEYMAP(ca_info_list_keymap, NULL)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)    
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ca_info_list_keymap, NULL)

BEGIN_MSGPROC(ca_info_list_proc, list_class_proc)
	ON_COMMAND(MSG_FOCUS_LEFT, on_ca_info_list_msg)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_ca_info_list_msg)
	ON_COMMAND(MSG_FOCUS_UP, on_ca_info_list_msg)
	ON_COMMAND(MSG_FOCUS_DOWN, on_ca_info_list_msg)
	ON_COMMAND(MSG_PAGE_DOWN, on_ca_info_list_msg)
	ON_COMMAND(MSG_PAGE_UP, on_ca_info_list_msg)
END_MSGPROC(ca_info_list_proc, list_class_proc)

