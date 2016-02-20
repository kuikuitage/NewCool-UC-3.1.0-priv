/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
//#include "sowell.h"
#include "ui_entitle_info.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_TITLE,
  IDC_CA_INFO_HEAD,  
  IDC_CA_INFO_LIST_ENTITLE,
  SN_IDC_PRODUCE_ID,
  SN_IDC_OVER_TIME,
};



static product_entitles_info_t *p_entitle = NULL;

RET_CODE ca_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 ca_info_list_keymap(u16 key);
RET_CODE ca_info_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE ca_info_list_entitle_update(control_t *p_list, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(p_list);
  DEBUG(DBG,INFO,"##################CNT = %d################\n",list_get_count(p_list));
  u8 asc_str[64] = {0};
  u32 product_id = 0;
  DEBUG(DBG,INFO,"****CALL IN*****\n"); 
  DEBUG(DBG,INFO,"***size = %d",size);
  DEBUG(DBG,INFO,"***start = %d",start);
  DEBUG(DBG,INFO,"***cnt = %d",list_get_count(p_list));
  DEBUG(DBG,INFO,"plist4 = %x\n",p_list);
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
        product_id = p_entitle->p_entitle_info[i + start].product_id;
        list_set_field_content_by_dec(p_list, (u16)(start + i), 0, product_id); 
        DEBUG(DBG,INFO,"##################CNT = %d################\n",list_get_count(p_list));
        DEBUG(DBG,INFO,"product_id = %d\n",product_id);
        sprintf((char *)asc_str,"%04d/%02d/%02d  ~  %04d/%02d/%02d ",
                p_entitle->p_entitle_info[i + start].start_time[0] + 2000, 
                p_entitle->p_entitle_info[i + start].start_time[1],
                p_entitle->p_entitle_info[i + start].start_time[2],
                p_entitle->p_entitle_info[i + start].expired_time[0] + 2000, 
                p_entitle->p_entitle_info[i + start].expired_time[1],
                p_entitle->p_entitle_info[i + start].expired_time[2]);
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1, asc_str); 
	  DEBUG(DBG,INFO,"**************asc_str = %s************\n",asc_str);
    }
  }
  return SUCCESS;
}



RET_CODE open_dvb_ca_entitle_info(u32 para1, u32 para2)
{
  //mbox item title
  #define SN_CA_MBOX_X                   (5)
  #define SN_CA_MBOX_Y                   (5)
  #define SN_CA_MBOX_W                   (SYS_RIGHT_CONT_W-2*SN_CA_MBOX_X)
  #define SN_CA_MBOX_H                   (COMM_ITEM_H)


  #define SN_CA_PLIST_BG_X               (0)
  #define SN_CA_PLIST_BG_Y               ((SN_CA_MBOX_Y) + (SN_CA_MBOX_H))
  #define SN_CA_PLIST_BG_W               (SYS_RIGHT_CONT_W - COMM_ITEM_OX_IN_ROOT/2)
  #define SN_CA_PLIST_BG_H               (SYS_RIGHT_CONT_H - SN_CA_PLIST_BG_Y - 30)

  #define SN_CA_LIST_BG_VGAP             6
  #define SN_SUB_MENU_HELP_RSC_CNT       13

  control_t *p_cont, *p_list, *p_mbox;
  u8 i;
  u16 itemmenu_btn_str[CA_ITME_COUNT] =
  {
    IDS_CA_PROGRAM_ID_2, IDS_ENTITLE_TIME
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
  DEBUG(DBG,INFO,"[dvbcas] %s<<<<<<<<<<<<< %d ! \n",__func__,__LINE__);  
  static list_field_attr_t ca_info_list_entitle_attr[CA_INFO_ENTITLE_FIELD] =
  {
    { LISTFIELD_TYPE_DEC| STL_LEFT | STL_VCENTER,
      80, 25, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

    { LISTFIELD_TYPE_UNISTR| STL_LEFT | STL_VCENTER,
      400, 360, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },
  };
DEBUG(DBG,INFO,"2---****************open_dvb_ca_entitle_info***************\n");  
  // create container
  p_cont = ui_comm_right_root_create(ROOT_ID_CA_ENTITLE_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ca_info_proc);
 
  //mbox item title
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_CA_INFO_HEAD,
                            SN_CA_MBOX_X,SN_CA_MBOX_Y,
                            SN_CA_MBOX_W, SN_CA_MBOX_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_attr(p_mbox, OBJ_ATTR_ACTIVE);
  mbox_set_focus(p_mbox, 1);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox, CA_ITME_COUNT, CA_ITME_COUNT, 1);
  mbox_set_string_fstyle(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_offset(p_mbox, 0, 0);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  for (i = 0; i < CA_ITME_COUNT; i++)
  {
    mbox_set_content_by_strid(p_mbox, i, itemmenu_btn_str[i]);
  }
  
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_CA_INFO_LIST_ENTITLE,
                           SN_CA_PLIST_BG_X,SN_CA_PLIST_BG_Y,
                           SN_CA_PLIST_BG_W,SN_CA_PLIST_BG_H, p_cont, 0);
  ctrl_set_keymap(p_list, ca_info_list_keymap);
  ctrl_set_proc(p_list, ca_info_list_proc);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  list_set_item_rstyle(p_list, &ca_info_item_rstyle);
  list_set_count(p_list, 0, CA_INFO_LIST_PAGE);
  list_set_field_count(p_list, CA_INFO_ENTITLE_FIELD, CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, ca_info_list_entitle_update, 0); 
  for (i = 0; i < CA_INFO_ENTITLE_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i,(u32)(ca_info_list_entitle_attr[i].attr),(u16)(ca_info_list_entitle_attr[i].width),(u16)(ca_info_list_entitle_attr[i].left),(u8)(ca_info_list_entitle_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i,ca_info_list_entitle_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i,ca_info_list_entitle_attr[i].fstyle);
  }  

  #ifndef WIN32
  ui_ca_do_cmd(CAS_CMD_ENTITLE_INFO_GET, 0, 0);
  #endif
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  DEBUG(DBG,INFO,"[dvbcas] %s>>>>>>>>>>>>> %d ! \n",__func__,__LINE__);
  return SUCCESS;
}

static RET_CODE on_ca_info_entitle(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST_ENTITLE);

  p_entitle = (product_entitles_info_t *)para2;
  list_set_count(p_list, p_entitle->max_num, CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  ca_info_list_entitle_update(p_list, list_get_valid_pos(p_list), CA_INFO_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  return SUCCESS;
}

BEGIN_MSGPROC(ca_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_ENT_INFO, on_ca_info_entitle)
//  ON_COMMAND(MSG_SAVE, on_ca_info_save)
END_MSGPROC(ca_info_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ca_info_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(ca_info_list_keymap, NULL)

BEGIN_MSGPROC(ca_info_list_proc, list_class_proc)

END_MSGPROC(ca_info_list_proc, list_class_proc)


