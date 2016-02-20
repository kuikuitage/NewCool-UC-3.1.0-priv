
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_operate.h"
#include "ui_ca_public.h"

#define TR_CA_OPERATE_TEST   (0)

enum control_id
{
  IDC_INVALID = 0,

  IDC_BG,

  IDC_TITLE1,
  IDC_TITLE2,
  IDC_TITLE3,
  IDC_TITLE4,

  IDC_PROVIDER_INFO_HEAD1,
  IDC_PROVIDER_INFO_HEAD2,
  IDC_PROVIDER_INFO_HEAD3,
  IDC_PROVIDER_INFO_HEAD4,

  IDC_PROVIDER_INFO_LIST1,
  IDC_PROVIDER_INFO_LIST2,
  IDC_PROVIDER_INFO_LIST3,
  IDC_PROVIDER_INFO_LIST4,
};

static list_xstyle_t provider_info_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
};

static list_xstyle_t provider_info_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
  FSI_BLACK,
};

static list_xstyle_t provider_info_field_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
};

static list_field_attr_t provider_info_list1_attr[PROVIDER_LIST_FIELD3] =
{
  { LISTFIELD_TYPE_UNISTR,
    100, 100, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    200, 370, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    260, 660, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },
};

static list_field_attr_t provider_info_list2_attr[PROVIDER_LIST_FIELD4] =
{
  { LISTFIELD_TYPE_DEC,
    100, 65, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    64, 320, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    190, 490, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    190, 730, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },
};

static list_field_attr_t provider_info_list3_attr[PROVIDER_LIST_FIELD3] =
{
  { LISTFIELD_TYPE_UNISTR,
    100, 100, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    100, 420, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR,
    100, 730, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },
};

static list_field_attr_t provider_info_list4_attr[PROVIDER_LIST_FIELD5] =
{
  { LISTFIELD_TYPE_DEC | STL_VCENTER| STL_VCENTER,
    60, 60, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    260, 141, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    100, 428, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    60, 628, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_VCENTER | STL_VCENTER,
    100, 800, 0, &provider_info_field_rstyle,  &provider_info_field_fstyle },
};

static u8 curn_index = 0;
static u8 g_sub_curn_index = 0;
static product_entitles_info_t *p_entitle = NULL;
static product_entitles_info_t entitle_info = {0};
static ipps_info_t *p_ippv = NULL;
static burses_info_t *p_burse = NULL;
static ipp_total_record_info_t *p_consume = NULL;

static u16 menu_btn_str[PROVIDER_ITEMMENU_CNT] =
{
  IDS_GENERAL_PRO, IDS_IPP_PRO_INFO, IDS_SLOT_INFO, IDS_IPP_CONSUME,
};

static u16 list1_title_str[PROVIDER_LIST_FIELD3] =
{
  IDS_CA_PRODUCT_ID, IDS_COMMISION_STATE, IDS_COMMISION_DATE
};

static u16 list2_title_str[PROVIDER_LIST_FIELD4] =
{
  IDS_IPP_ID, IDS_TYPE, IDS_START_TIME4, IDS_END_TIME4
};

static u16 list3_title_str[PROVIDER_LIST_FIELD3] =
{
  IDS_CA_SLOT_ID, IDS_CA_CREDIT, IDS_REMAIN_NUMBER
};

static u16 list4_title_str[PROVIDER_LIST_FIELD5] =
{
  IDS_CA_NUMBER, IDS_TIME, IDS_CA_STATE, IDS_GLIDE_NUM, IDS_MONEY
};

u16 provider_info_keymap(u16 key);
RET_CODE provider_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 provider_info_button_keymap(u16 key);
RET_CODE provider_info_button_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 provider_info_list_keymap(u16 key);
RET_CODE provider_info_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE provider_info_list1_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
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
      sprintf((char*)asc_str,"%d",(int)entitle_info.p_entitle_info[i].product_id);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str);

      //list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1,
      //p_entitle->p_entitle_info[i + start].states?"No authorization":"Authorization OK");
      DEBUG(CAS,INFO,"provider_info_list1_update item[%d] states[%d]\n",i + start, p_entitle->p_entitle_info[i + start].states);
      memset(asc_str, 0, sizeof(asc_str));
      switch(entitle_info.p_entitle_info[i].states)
      {
        case 0:
          sprintf((char*)asc_str, "%s", "Authorization OK");
          break;

        case 1:
          sprintf((char*)asc_str, "%s", "Parameters error");
          break;

        case 2:
          sprintf((char*)asc_str, "%s", "No entitle info");
          break;

        case 3:
          sprintf((char*)asc_str, "%s", "Limited");
          break;

        case 4:
          sprintf((char*)asc_str, "%s", "Entitle closed");
          break;

        case 5:
          sprintf((char*)asc_str, "%s", "Entitle paused");
          break;

        default:
          break;
      }
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, asc_str);

      memset(asc_str, 0, sizeof(asc_str));
#if TR_CA_OPERATE_TEST
	sprintf((char*)asc_str,"2015.12.30--2016.12.30");
#else
      if(entitle_info.p_entitle_info[i].states == 0)
      {
        sprintf((char*)asc_str,"%04d.%02d.%02d--%04d.%02d.%02d",
                  entitle_info.p_entitle_info[i].start_time[0] * 100 +
                  entitle_info.p_entitle_info[i].start_time[1],
                  entitle_info.p_entitle_info[i].start_time[2],
                  entitle_info.p_entitle_info[i].start_time[3],
                  entitle_info.p_entitle_info[i].expired_time[0] * 100 +
                  entitle_info.p_entitle_info[i].expired_time[1],
                  entitle_info.p_entitle_info[i].expired_time[2],
                  entitle_info.p_entitle_info[i].expired_time[3]);
      }
#endif	  
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2, asc_str);
    }
  }
  return SUCCESS;
}

static RET_CODE provider_info_list2_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[64];

  if(p_ippv == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
    #if TR_CA_OPERATE_TEST
	list_set_field_content_by_dec(ctrl, (u16)(start + i), 0, (start + i));
	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, ((start + i)%2)?(u8*)"ippt":(u8*)"ippv");
	sprintf((char*)asc_str,"2015.12.30-23:59:59");
	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2, asc_str);
	sprintf((char*)asc_str,"2016.12.30-23:59:59");
	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);
	#else
      list_set_field_content_by_dec(ctrl, (u16)(start + i), 0, p_ippv->p_ipp_info[i + start].index);

      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, p_ippv->p_ipp_info[i + start].type?(u8*)"ippt":(u8*)"ippv");

      sprintf((char*)asc_str,"%04d.%02d.%02d-%02d:%02d:%02d",p_ippv->p_ipp_info[i + start].start_time[0] * 100 +
              p_ippv->p_ipp_info[i + start].start_time[1],
              p_ippv->p_ipp_info[i + start].start_time[2],
              p_ippv->p_ipp_info[i + start].start_time[3],
              p_ippv->p_ipp_info[i + start].start_time[4],
              p_ippv->p_ipp_info[i + start].start_time[5],
              p_ippv->p_ipp_info[i + start].start_time[6]);
       list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2, asc_str);

      sprintf((char*)asc_str,"%04d.%02d.%02d-%02d:%02d:%02d",p_ippv->p_ipp_info[i + start].expired_time[0] * 100 +
              p_ippv->p_ipp_info[i + start].expired_time[1],
              p_ippv->p_ipp_info[i + start].expired_time[2],
              p_ippv->p_ipp_info[i + start].expired_time[3],
              p_ippv->p_ipp_info[i + start].expired_time[4],
              p_ippv->p_ipp_info[i + start].expired_time[5],
              p_ippv->p_ipp_info[i + start].expired_time[6]);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);
	#endif
    }
  }
  return SUCCESS;
}

static RET_CODE provider_info_list3_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[64];

  if(p_burse == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
    #if TR_CA_OPERATE_TEST
 	sprintf((char*)asc_str,"%d",(i+start));
 	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str);
 	
 	sprintf((char*)asc_str,"999999999");
 	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1,  asc_str);
 	
 	sprintf((char*)asc_str,"666666");
 	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2,  asc_str);
	#else
      sprintf((char*)asc_str,"%d",(int)p_burse->p_burse_info[i + start].record_index);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str);

      sprintf((char*)asc_str,"%d",(int)p_burse->p_burse_info[i + start].credit_limit);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1,  asc_str);

      sprintf((char*)asc_str,"%d",(int)p_burse->p_burse_info[i + start].cash_value);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2,  asc_str);
	 #endif
    }
  }
  return SUCCESS;
}

static RET_CODE provider_info_list4_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 cnt = list_get_count(ctrl);
  u8 asc_str[64];

  if(p_consume == NULL)
  {
    return ERR_FAILURE;
  }

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
#if TR_CA_OPERATE_TEST
	 	list_set_field_content_by_dec(ctrl, (u16)(start + i), 0, (i+start));
	 	sprintf((char*)asc_str,"2015.12.12-12:12:12");
	 	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, asc_str);
	 
	 	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2, ((i+start)%2)?(u8*)"charge":(u8*)"consume");
	 
	 	sprintf((char*)asc_str,"%d",(i+start)*9);
	 	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);
	 
	 	sprintf((char*)asc_str,"%d",(i+start)*8);
	 	list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 4, asc_str);

#else
      list_set_field_content_by_dec(ctrl, (u16)(start + i), 0, p_consume->record_info[i + start].index);
      sprintf((char*)asc_str,"%04d.%02d.%02d-%02d:%02d:%02d",p_consume->record_info[i + start].start_time[0] * 100 +
              p_consume->record_info[i + start].start_time[1],
              p_consume->record_info[i + start].start_time[2],
              p_consume->record_info[i + start].start_time[3],
              p_consume->record_info[i + start].start_time[4],
              p_consume->record_info[i + start].start_time[5],
              p_consume->record_info[i + start].start_time[6]);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 1, asc_str);

      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2, p_consume->record_info[i + start].state_flag?(u8*)"charge":(u8*)"consume");

      sprintf((char*)asc_str,"%d",(int)p_consume->record_info[i + start].runningNum);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);

      sprintf((char*)asc_str,"%d",(int)p_consume->record_info[i + start].value);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 4, asc_str);
#endif	  
    }
  }
  return SUCCESS;
}

RET_CODE open_ca_operate(u32 para1, u32 para2)
{
  control_t *p_cont,*p_btn[PROVIDER_ITEMMENU_CNT],*p_list,*p_mbox1,*p_mbox2,*p_mbox3,*p_mbox4;
  u8 i;
  u16 x, y;

  p_cont = ui_comm_right_root_create(ROOT_ID_CA_OPE, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, provider_info_keymap);
  ctrl_set_proc(p_cont, provider_info_proc);
  
  //title
  for(i=0; i<PROVIDER_ITEMMENU_CNT; i++)
  {
    x = (PROVIDER_BTN_X + (PROVIDER_BTN_W + PROVIDER_BTN_V_GAP)*i);
    y = PROVIDER_BTN_Y;
    p_btn[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_TITLE1+ i),
                              x, y, PROVIDER_BTN_W,PROVIDER_BTN_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_btn[i],RSI_ITEM_8_BG,RSI_OPT_BTN_H,RSI_ITEM_8_BG);
    ctrl_set_keymap(p_btn[i], provider_info_button_keymap);
    ctrl_set_proc(p_btn[i], provider_info_button_proc);
    text_set_font_style(p_btn[i], FSI_BLACK, FSI_BLACK, FSI_BLACK);
    text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], menu_btn_str[i]);
  }

  //LIST1
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_PROVIDER_INFO_LIST1,
                           PROVIDER_PLIST_BG_X, PROVIDER_PLIST_BG_Y, PROVIDER_PLIST_BG_W,PROVIDER_PLIST_BG_H, p_cont, 0);
  
  ctrl_set_keymap(p_list, provider_info_list_keymap);
  ctrl_set_proc(p_list, provider_info_list_proc);
  ctrl_set_sts(p_list,OBJ_STS_SHOW);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_item_rstyle(p_list, &provider_info_item_rstyle);
  list_set_count(p_list, CAS_MAX_PRODUCT_ENTITLE_NUM_320, PROVIDER_LIST_PAGE);
  list_set_field_count(p_list, PROVIDER_LIST_FIELD3, PROVIDER_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, provider_info_list1_update, 0);
  
  for (i = 0; i < PROVIDER_LIST_FIELD3; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(provider_info_list1_attr[i].attr), (u16)(provider_info_list1_attr[i].width),
                        (u16)(provider_info_list1_attr[i].left), (u8)(provider_info_list1_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, provider_info_list1_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, provider_info_list1_attr[i].fstyle);
  }
  //provider_info_list1_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);
  
  //LIST2
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_PROVIDER_INFO_LIST2,
                           PROVIDER_PLIST_BG_X, PROVIDER_PLIST_BG_Y, PROVIDER_PLIST_BG_W,PROVIDER_PLIST_BG_H, p_cont, 0);
  
  ctrl_set_keymap(p_list, provider_info_list_keymap);
  ctrl_set_proc(p_list, provider_info_list_proc);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_sts(p_list, OBJ_STS_HIDE);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_item_rstyle(p_list, &provider_info_item_rstyle);
  list_set_count(p_list, 6, PROVIDER_LIST_PAGE);
  list_set_field_count(p_list, PROVIDER_LIST_FIELD4, PROVIDER_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, provider_info_list2_update, 0);
  for (i = 0; i < PROVIDER_LIST_FIELD4; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(provider_info_list2_attr[i].attr), (u16)(provider_info_list2_attr[i].width),
                        (u16)(provider_info_list2_attr[i].left), (u8)(provider_info_list2_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, provider_info_list2_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, provider_info_list2_attr[i].fstyle);
  }
  //provider_info_list2_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  //LIST3
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_PROVIDER_INFO_LIST3,
                           PROVIDER_PLIST_BG_X, PROVIDER_PLIST_BG_Y, PROVIDER_PLIST_BG_W,PROVIDER_PLIST_BG_H, p_cont, 0);
  
  ctrl_set_keymap(p_list, provider_info_list_keymap);
  ctrl_set_proc(p_list, provider_info_list_proc);
  ctrl_set_sts(p_list, OBJ_STS_HIDE);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_item_rstyle(p_list, &provider_info_item_rstyle);
  list_set_count(p_list, 8, PROVIDER_LIST_PAGE);
  list_set_field_count(p_list, PROVIDER_LIST_FIELD3, PROVIDER_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, provider_info_list3_update, 0);

  for (i = 0; i < PROVIDER_LIST_FIELD3; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(provider_info_list3_attr[i].attr), (u16)(provider_info_list3_attr[i].width),
                        (u16)(provider_info_list3_attr[i].left), (u8)(provider_info_list3_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, provider_info_list3_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, provider_info_list3_attr[i].fstyle);
  }
  //provider_info_list3_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  //LIST4
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_PROVIDER_INFO_LIST4,
                           PROVIDER_PLIST_BG_X, PROVIDER_PLIST_BG_Y, PROVIDER_PLIST_BG_W,PROVIDER_PLIST_BG_H, p_cont, 0);
  
  ctrl_set_keymap(p_list, provider_info_list_keymap);
  ctrl_set_proc(p_list, provider_info_list_proc);
  ctrl_set_sts(p_list, OBJ_STS_HIDE);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_item_rstyle(p_list, &provider_info_item_rstyle);
  list_set_count(p_list, 8, PROVIDER_LIST_PAGE);
  list_set_field_count(p_list, PROVIDER_LIST_FIELD5, PROVIDER_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_select_item(p_list, 0);
  list_set_update(p_list, provider_info_list4_update, 0);

  for (i = 0; i < PROVIDER_LIST_FIELD5; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(provider_info_list4_attr[i].attr), (u16)(provider_info_list4_attr[i].width),
                        (u16)(provider_info_list4_attr[i].left), (u8)(provider_info_list4_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, provider_info_list4_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, provider_info_list4_attr[i].fstyle);
  }
  //provider_info_list4_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  //mbox1
  p_mbox1 = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_PROVIDER_INFO_HEAD1,
                            PROVIDER_MBOX_BTN_X,
                            PROVIDER_MBOX_BTN_Y,
                            PROVIDER_MBOX_BTN_W,
                            PROVIDER_MBOX_BTN_H,
                            p_cont, 0);
  
  ctrl_set_rstyle(p_mbox1, RSI_PBACK,RSI_ITEM_1_SH, RSI_PBACK);
  ctrl_set_sts(p_mbox1, OBJ_STS_SHOW);
  mbox_set_focus(p_mbox1, 1);
  mbox_enable_string_mode(p_mbox1, TRUE);
  mbox_set_content_strtype(p_mbox1, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox1, PROVIDER_LIST_FIELD3, PROVIDER_LIST_FIELD3, 1);
  mbox_set_string_fstyle(p_mbox1, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_offset(p_mbox1, 0, 0);
  mbox_set_string_align_type(p_mbox1, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < PROVIDER_LIST_FIELD3; i++)
  {
    mbox_set_content_by_strid(p_mbox1, i, list1_title_str[i]);
  }

  //mbox2
  p_mbox2 = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_PROVIDER_INFO_HEAD2,
                            PROVIDER_MBOX_BTN_X,
                            PROVIDER_MBOX_BTN_Y,
                            PROVIDER_MBOX_BTN_W,
                            PROVIDER_MBOX_BTN_H,
                            p_cont, 0);
  
  ctrl_set_rstyle(p_mbox2, RSI_PBACK,RSI_PBACK, RSI_PBACK);
  ctrl_set_sts(p_mbox2, OBJ_STS_HIDE);
  mbox_set_focus(p_mbox2, 0);
  mbox_enable_string_mode(p_mbox2, TRUE);
  mbox_set_content_strtype(p_mbox2, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox2, PROVIDER_LIST_FIELD4, PROVIDER_LIST_FIELD4, 1);
  mbox_set_string_fstyle(p_mbox2, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_offset(p_mbox2, 0, 0);
  mbox_set_string_align_type(p_mbox2, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < PROVIDER_LIST_FIELD4; i++)
  {
    mbox_set_content_by_strid(p_mbox2, i, list2_title_str[i]);
  }

  //mbox3
  p_mbox3 = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_PROVIDER_INFO_HEAD3,
                            PROVIDER_MBOX_BTN_X,
                            PROVIDER_MBOX_BTN_Y,
                            PROVIDER_MBOX_BTN_W,
                            PROVIDER_MBOX_BTN_H,
                            p_cont, 0);
  
  ctrl_set_rstyle(p_mbox3, RSI_PBACK,RSI_PBACK, RSI_PBACK);
  ctrl_set_sts(p_mbox3, OBJ_STS_HIDE);
  mbox_set_focus(p_mbox3, 0);
  mbox_enable_string_mode(p_mbox3, TRUE);
  mbox_set_content_strtype(p_mbox3, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox3, PROVIDER_LIST_FIELD3, PROVIDER_LIST_FIELD3, 1);
  mbox_set_string_fstyle(p_mbox3, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_offset(p_mbox3, 0, 0);
  mbox_set_string_align_type(p_mbox3, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < PROVIDER_LIST_FIELD3; i++)
  {
    mbox_set_content_by_strid(p_mbox3, i, list3_title_str[i]);
  }

  //mbox4
  p_mbox4 = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_PROVIDER_INFO_HEAD4,
                            PROVIDER_MBOX_BTN_X,
                            PROVIDER_MBOX_BTN_Y,
                            PROVIDER_MBOX_BTN_W,
                            PROVIDER_MBOX_BTN_H,
                            p_cont, 0);
  
  ctrl_set_rstyle(p_mbox4, RSI_PBACK,RSI_PBACK, RSI_PBACK);
  ctrl_set_sts(p_mbox4, OBJ_STS_HIDE);
  mbox_set_focus(p_mbox4, 0);
  mbox_enable_string_mode(p_mbox4, TRUE);
  mbox_set_content_strtype(p_mbox4, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox4, PROVIDER_LIST_FIELD5, PROVIDER_LIST_FIELD5, 1);
  mbox_set_string_fstyle(p_mbox4, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_offset(p_mbox4, 0, 0);
  mbox_set_string_align_type(p_mbox4, STL_VCENTER | STL_VCENTER);
  for (i = 0; i < PROVIDER_LIST_FIELD5; i++)
  {
    mbox_set_content_by_strid(p_mbox4, i, list4_title_str[i]);
  }

  ctrl_default_proc(p_btn[g_sub_curn_index], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, (0<<16)|(0+PROVIDER_LIST_PAGE) , 0);
  ui_ca_do_cmd(CAS_CMD_IPPV_INFO_GET, 0 , 0);
  ui_ca_do_cmd(CAS_CMD_BURSE_INFO_GET, 0 , 0);
  ui_ca_do_cmd(CAS_CMD_IPP_REC_INFO_GET, 0 , 0);

  return SUCCESS;
}

static RET_CODE on_provider_info_list_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 id;
  u16 valid_pos = 0;
  control_t *p_cont = p_ctrl->p_parent;
  control_t *p_list = NULL;
  id = ctrl_get_ctrl_id(p_ctrl);

  switch(id)
  {
    case IDC_TITLE1:
      p_list = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST1);
      break;

    case IDC_TITLE2:
      p_list = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST2);
      break;

    case IDC_TITLE3:
      p_list = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST3);
      break;

    case IDC_TITLE4:
      p_list = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST4);
      break;

    default:
      break;
  }

  list_class_proc(p_list, msg, para1, para2);

  if(id == IDC_TITLE1)
  {
    p_entitle = NULL;
    valid_pos = list_get_valid_pos(p_list);
    DEBUG(CAS,INFO,"on_provider_info_list_msg valid_pos[%d]\n",valid_pos);
    ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, (valid_pos<<16)|(valid_pos+PROVIDER_LIST_PAGE) , 0 );
  }
  else
  {
    ctrl_paint_ctrl(p_list, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_provider_item_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_next_ctrl, *p_mbox1, *p_mbox2, *p_mbox3,*p_mbox4,*p_list1,*p_list2,*p_list3,*p_list4;
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

  p_list1 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST1);
  p_list2 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST2);
  p_list3 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST3);
  p_list4 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_LIST4);
  p_mbox1 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_HEAD1);
  p_mbox2 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_HEAD2);
  p_mbox3 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_HEAD3);
  p_mbox4 = ctrl_get_child_by_id(p_cont,IDC_PROVIDER_INFO_HEAD4);

  if(num == IDC_TITLE1)
  {
    ctrl_set_sts(p_mbox2, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox3, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox4, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox1, OBJ_STS_SHOW);

    ctrl_set_sts(p_list2, OBJ_STS_HIDE);
    ctrl_set_sts(p_list3, OBJ_STS_HIDE);
    ctrl_set_sts(p_list4, OBJ_STS_HIDE);
    ctrl_set_sts(p_list1, OBJ_STS_SHOW);

    ctrl_paint_ctrl(p_list1, TRUE);
    ctrl_paint_ctrl(p_mbox1, TRUE);
  }
  else if(num == IDC_TITLE2)
  {
    ctrl_set_sts(p_mbox1, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox3, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox4, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox2, OBJ_STS_SHOW);

    ctrl_set_sts(p_list1, OBJ_STS_HIDE);
    ctrl_set_sts(p_list3, OBJ_STS_HIDE);
    ctrl_set_sts(p_list4, OBJ_STS_HIDE);
    ctrl_set_sts(p_list2, OBJ_STS_SHOW);

    ctrl_paint_ctrl(p_list2, TRUE);
    ctrl_paint_ctrl(p_mbox2, TRUE);
  }
  else if(num == IDC_TITLE3)
  {
    ctrl_set_sts(p_mbox1, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox2, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox4, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox3, OBJ_STS_SHOW);

    ctrl_set_sts(p_list1, OBJ_STS_HIDE);
    ctrl_set_sts(p_list2, OBJ_STS_HIDE);
    ctrl_set_sts(p_list4, OBJ_STS_HIDE);
    ctrl_set_sts(p_list3, OBJ_STS_SHOW);

    ctrl_paint_ctrl(p_list3, TRUE);
    ctrl_paint_ctrl(p_mbox3, TRUE);
  }
  else if(num == IDC_TITLE4)
  {
    ctrl_set_sts(p_mbox1, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox2, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox3, OBJ_STS_HIDE);
    ctrl_set_sts(p_mbox4, OBJ_STS_SHOW);

    ctrl_set_sts(p_list1, OBJ_STS_HIDE);
    ctrl_set_sts(p_list2, OBJ_STS_HIDE);
    ctrl_set_sts(p_list3, OBJ_STS_HIDE);
    ctrl_set_sts(p_list4, OBJ_STS_SHOW);

    ctrl_paint_ctrl(p_list4, TRUE);
    ctrl_paint_ctrl(p_mbox4, TRUE);
  }

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_next_ctrl, TRUE);
  return SUCCESS;
}

static RET_CODE on_provider_info_entitle(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_PROVIDER_INFO_LIST1);

  p_entitle = (product_entitles_info_t *)para2;

  if(p_entitle != NULL)
  {
    memcpy(&entitle_info, p_entitle, sizeof(product_entitles_info_t));
  }

  provider_info_list1_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);

  return SUCCESS;
}
static RET_CODE on_provider_info_ippv(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_PROVIDER_INFO_LIST2);

  p_ippv = (ipps_info_t *)para2;

#if TR_CA_OPERATE_TEST
  list_set_count(p_list, PROVIDER_LIST_PAGE, PROVIDER_LIST_PAGE);
#else
  list_set_count(p_list, p_ippv->max_num, PROVIDER_LIST_PAGE);
#endif
  provider_info_list2_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);

  return SUCCESS;
}

static RET_CODE on_provider_info_burse(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_PROVIDER_INFO_LIST3);

  p_burse = (burses_info_t *)para2;
#if TR_CA_OPERATE_TEST
  list_set_count(p_list, PROVIDER_LIST_PAGE, PROVIDER_LIST_PAGE);
#else
  list_set_count(p_list, p_burse->max_num, PROVIDER_LIST_PAGE);
#endif
  provider_info_list3_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);

  return SUCCESS;
}

static RET_CODE on_provider_info_consume(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_PROVIDER_INFO_LIST4);

  p_consume = (ipp_total_record_info_t *)para2;

#if TR_CA_OPERATE_TEST
  list_set_count(p_list, (u16)PROVIDER_LIST_PAGE, PROVIDER_LIST_PAGE);
#else
  list_set_count(p_list, (u16)p_consume->max_num, PROVIDER_LIST_PAGE);
#endif
  provider_info_list4_update(p_list, list_get_valid_pos(p_list), PROVIDER_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);

  return SUCCESS;
}

static RET_CODE on_provider_info_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  p_entitle = NULL;
  p_ippv = NULL;
  p_burse = NULL;
  p_consume = NULL;
  u8 root_id;
  switch(msg)
  {
    case MSG_EXIT:
	  root_id = ctrl_get_ctrl_id(p_ctrl);
   
      manage_close_menu(ROOT_ID_CA_OPE, para1, para2);
	  if(ROOT_ID_XSYS_SET==fw_get_focus_id())
	  	swtich_to_sys_set(root_id, 0);
      break;

    case MSG_EXIT_ALL:
      ui_close_all_mennus();
      break;

    default:
      break;
  }

  return SUCCESS;
}

static RET_CODE on_provider_info_accept_notify(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u32 event_id = 0;

  switch(msg)
  {
    case MSG_CA_INIT_OK:
      ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, (0<<16)|(0+PROVIDER_LIST_PAGE) , 0);
      ui_ca_do_cmd(CAS_CMD_IPPV_INFO_GET, 0 , 0);
      ui_ca_do_cmd(CAS_CMD_BURSE_INFO_GET, 0 , 0);
      ui_ca_do_cmd(CAS_CMD_IPP_REC_INFO_GET, 0 , 0);
      break;

    case MSG_CA_EVT_NOTIFY:
      event_id = para2;
      if(event_id == CAS_S_ADPT_CARD_REMOVE)
      {
        ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, (0<<16)|(0+PROVIDER_LIST_PAGE) , 0);
        ui_ca_do_cmd(CAS_CMD_IPPV_INFO_GET, 0 , 0);
        ui_ca_do_cmd(CAS_CMD_BURSE_INFO_GET, 0 , 0);
        ui_ca_do_cmd(CAS_CMD_IPP_REC_INFO_GET, 0 , 0);
      }
      break;

      default:
        break;
  }

  return SUCCESS;
}


BEGIN_KEYMAP(provider_info_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(provider_info_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(provider_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_ENT_INFO, on_provider_info_entitle)
  ON_COMMAND(MSG_CA_IPV_INFO, on_provider_info_ippv)
  ON_COMMAND(MSG_CA_BUR_INFO, on_provider_info_burse)
  ON_COMMAND(MSG_CA_IPP_REC_INFO, on_provider_info_consume)
  ON_COMMAND(MSG_CA_INIT_OK, on_provider_info_accept_notify)
END_MSGPROC(provider_info_proc, ui_comm_root_proc)

BEGIN_KEYMAP(provider_info_button_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(provider_info_button_keymap, NULL)

BEGIN_MSGPROC(provider_info_button_proc, text_class_proc)
  ON_COMMAND(MSG_FOCUS_LEFT, on_provider_item_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_provider_item_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_provider_info_list_msg)
  ON_COMMAND(MSG_PAGE_UP, on_provider_info_list_msg)
  ON_COMMAND(MSG_EXIT, on_provider_info_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_provider_info_exit)
END_MSGPROC(provider_info_button_proc, text_class_proc)

BEGIN_KEYMAP(provider_info_list_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(provider_info_list_keymap, NULL)

BEGIN_MSGPROC(provider_info_list_proc, list_class_proc)

END_MSGPROC(provider_info_list_proc, list_class_proc)


