/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_pair.h"
#include "ui_ca_public.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

enum ca_ca_info_id
{
  IDC_STB_CARD_PAIR_STATE = 1,
  IDC_STB_CARD_PAIR_STATE_CONTENT,

  IDC_STB_CARD_PAIR_ID,
  IDC_STB_CARD_PAIR_ID_CONTENT,
};
 
static list_xstyle_t pair_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
};

static list_xstyle_t pair_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t pair_list_field_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t pair_list_attr =
{
    LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    320, 10, 0, 
    &pair_list_field_rstyle,  &pair_list_field_fstyle

};

u16 conditional_accept_pair_cont_keymap(u16 key);
RET_CODE conditional_accept_pair_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE open_conditional_accept_pair(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl;
  u8 i = 0;
  u16 x=0, y=0, w=0, h=0;
  u32 text_type = 0, align_type = 0;
  u16 str[CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_CNT] =
  { 
    IDS_CA_PAIR_STATE,  IDS_CA_PLAT_ID
  };


							   
   p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_PAIR, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_pair_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_pair_cont_proc);
  
  //CA pair
  
  x = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_X;
  y = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_Y;
  h = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_H;
  for(i=0; i<4; i++)
  {
    switch(i%2)
    {      
      case 0:
        x = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_X;
        y = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_Y+i/2*(CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_H+CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_V_GAP);
        w = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_LW;
        text_type = TEXT_STRTYPE_STRID;
        align_type = STL_CENTER | STL_VCENTER;
        break;

      case 1:
        x = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_X+CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_LW+CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_H_GAP;
        w = CONDITIONAL_ACCEPT_PAIR_CA_PAIR_ITEM_RW;
        text_type = TEXT_STRTYPE_UNICODE;
        align_type = STL_LEFT| STL_VCENTER;
        break;

	  default:
	  	break;
    }
    if(i == 3)
    {
      p_ctrl = ctrl_create_ctrl(CTRL_LIST, (u8)(IDC_STB_CARD_PAIR_STATE + i),
                              x, y, w, PAIR_LIST_H, p_cont, 0);
      //ctrl_set_rstyle(p_ctrl, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);  
      ctrl_set_mrect(p_ctrl, PAIR_LIST_MIDL, PAIR_LIST_MIDT, PAIR_LIST_MIDL + PAIR_LIST_MIDW, PAIR_LIST_MIDT + PAIR_LIST_MIDH);
      list_set_item_rstyle(p_ctrl, &pair_item_rstyle);
      list_set_count(p_ctrl, 0, PAIR_LIST_PAGE);

      list_set_field_count(p_ctrl, PAIR_LIST_PAGE, PAIR_LIST_PAGE);
      list_set_focus_pos(p_ctrl, 0);
      
      list_set_field_attr(p_ctrl, 0, (u32)(pair_list_attr.attr), (u16)(pair_list_attr.width),
                          (u16)(pair_list_attr.left), (u8)(pair_list_attr.top));
      list_set_field_rect_style(p_ctrl, 0, pair_list_attr.rstyle);
      list_set_field_font_style(p_ctrl, 0, pair_list_attr.fstyle);
    }
    else
    {
      p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_STB_CARD_PAIR_STATE + i),
                              x, y, w, h, p_cont, 0);
      ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
      text_set_align_type(p_ctrl, align_type);
      text_set_content_type(p_ctrl, text_type);                        
    }

    if(i%2==0)
    {
      text_set_content_by_strid(p_ctrl, str[i/2]);
    }
  }
  
  //ui_comm_help_create2((comm_help_data_t2*)&ca_card_pair_help_data, p_cont, FALSE);

  ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_STB_CARD_PAIR_ID_CONTENT), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  ui_ca_do_cmd(CAS_CMD_PAIRE_STATUS_GET, 0, 0);
  return SUCCESS;
}


static void conditional_accept_pair_set_content(control_t *p_cont, u32 result, u32 stb_id_arr)
{
  u32 i = 0, k = 0;
  control_t *p_state = ctrl_get_child_by_id(p_cont, IDC_STB_CARD_PAIR_STATE_CONTENT);
  
  control_t *p_stb_id_content = ctrl_get_child_by_id(p_cont, IDC_STB_CARD_PAIR_ID_CONTENT);
  card_stb_paired_list_t *p_card_stb_paired_list = (card_stb_paired_list_t *)stb_id_arr;
  u16 start = 0;
  u8 asc_str[64];
  if(p_card_stb_paired_list == NULL)
  {
    return;
  }
  UI_PRINTF("@@@@@ state: %d, %x \n", p_card_stb_paired_list->paired_status, p_card_stb_paired_list->STB_list);
  //ui_comm_static_set_content_by_ascstr(p_name, (u8 *)"Unknown");
  //ui_comm_static_set_content_by_ascstr(p_state, (u8 *)result);
  switch(p_card_stb_paired_list->paired_status)
  {
    case CAS_EVT_NONE:
      text_set_content_by_ascstr(p_state, (u8 *)"The card is paired with the current STB\n");
      break;
    case CAS_E_ILLEGAL_CARD:
      text_set_content_by_ascstr(p_state,(u8 *) "Card invalid\n");
      break;
    case CAS_E_CARD_PAIROTHER:
      text_set_content_by_ascstr(p_state, (u8 *)"The card is paired with another STB\n");
      break;
    case CAS_E_CARD_DIS_PARTNER:
      text_set_content_by_ascstr(p_state, (u8 *)"The card is not paired with any STB\n");
      break;
    default:
      text_set_content_by_ascstr(p_state,(u8 *)"Unknown");
      break;
  }
  UI_PRINTF("@@@@@ conditional_accept_pair_set_content paired_num:%d\n", p_card_stb_paired_list->paired_num);
  if(p_card_stb_paired_list->paired_num > 0)
  {
    list_set_count(p_stb_id_content, p_card_stb_paired_list->paired_num , PAIR_LIST_PAGE);
    for (i = 0; i < PAIR_LIST_PAGE; i++)
    {
      if (i + start < p_card_stb_paired_list->paired_num)
      {
        memset(asc_str, 0, sizeof(asc_str));
        sprintf((char*)asc_str, "%.2X%.2X%.2X%.2X%.2X%.2X", p_card_stb_paired_list->STB_list[6 * k], 
                  p_card_stb_paired_list->STB_list[6 * k + 1], p_card_stb_paired_list->STB_list[6 * k + 2],
                  p_card_stb_paired_list->STB_list[6 * k + 3], p_card_stb_paired_list->STB_list[6 * k + 4],
                  p_card_stb_paired_list->STB_list[6 * k + 5]);
                  /*
        for(j = 0; j < 6; j++)
        {
          sprintf((char*)asc_str + j * 2, "%X", p_card_stb_paired_list->STB_list[j + 6 * k]);
        }
        */
        list_set_field_content_by_ascstr(p_stb_id_content, (u16)(start + i), 0, asc_str); 
        k++;
      }
    }
  }
  
  
}

static RET_CODE on_conditional_accept_pair_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  UI_PRINTF("@@@@@ on_conditional_accept_pair_update\n");
  conditional_accept_pair_set_content(p_cont, para1, para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}
BEGIN_KEYMAP(conditional_accept_pair_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_pair_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_pair_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_PAIRE_INFO, on_conditional_accept_pair_update)
END_MSGPROC(conditional_accept_pair_cont_proc, ui_comm_root_proc)


