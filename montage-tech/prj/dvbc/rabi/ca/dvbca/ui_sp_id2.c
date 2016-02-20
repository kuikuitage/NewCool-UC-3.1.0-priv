#include "ui_common.h"
//#include "sowell.h"
#include "ui_sp_id.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_TITLE,
  IDC_CA_INFO_HEAD,  
  IDC_CA_INFO_LIST_ENTITLE,
  SN_IDC_PRODUCE_ID,
  SN_IDC_OVER_TIME,
};



static cas_operators_info_t *p_sp_id = NULL;

RET_CODE ca_sp2_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 ca_sp2_list_keymap(u16 key);
RET_CODE ca_sp2_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
extern BOOL ui_is_smart_card_insert(void);

static RET_CODE ca_info_list_sp_id_update(control_t *p_list, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(p_list);
  u16 uni_str[OPERATOR_NAME_MAX_LENGTH] = {0};
  u16 sp_id = 0;
  DEBUG(DBG,INFO,"****CALL IN*****\n"); 
  DEBUG(DBG,INFO,"***size = %d",size);
  DEBUG(DBG,INFO,"***start = %d",start);
  DEBUG(DBG,INFO,"***cnt = %d",list_get_count(p_list));
  DEBUG(DBG,INFO,"plist4 = %x\n",p_list);
  if(ui_is_smart_card_insert() == FALSE)
  	return SUCCESS;
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      sp_id = p_sp_id->p_operator_info[i+start].operator_id;
      list_set_field_content_by_dec(p_list, 
        (u16)(start + i), 0, sp_id); 
      DEBUG(DBG,INFO,"product_id = %d\n",sp_id);
	  sp_id_name_get_unistr(uni_str, p_sp_id->p_operator_info[i+start].operator_name);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str); 
    }
  }
  //set falg true then we can into the operator info page by the key
  set_update_sp_flag(TRUE);
  return SUCCESS;
}



RET_CODE open_dvb_ca_sp_id_list2(u32 para1, u32 para2)
{
  //mbox item title
  #define SN_DVB_CA_MBOX_X                   (5)
  #define SN_DVB_CA_MBOX_Y                   (5)
  #define SN_DVB_CA_MBOX_W                  (SYS_RIGHT_CONT_W-2*SN_DVB_CA_MBOX_X)
  #define SN_DVB_CA_MBOX_H                   (COMM_ITEM_H)

  #define SN_DVB_CA_PLIST_BG_X               (0)
  #define SN_DVB_CA_PLIST_BG_Y               (SN_DVB_CA_MBOX_Y + SN_DVB_CA_MBOX_H)
  #define SN_DVB_CA_PLIST_BG_W               (SYS_RIGHT_CONT_W - COMM_ITEM_OX_IN_ROOT/2)
  #define SN_DVB_CA_PLIST_BG_H               (SYS_RIGHT_CONT_H-SN_DVB_CA_PLIST_BG_Y-30)

  #define SN_DVB_CA_LIST_BG_VGAP             6
  
  #define SN_SUB_MENU_HELP_RSC_CNT       13

  control_t *p_cont, *p_list, *p_mbox;//,*p_btn, *p_ctrl
  u8 i;
  u16 itemmenu_btn_str[DVB_CA_ITME_COUNT] =
  {
    IDS_CA_OPERATOR, IDS_CA_SP_NAME1
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
  static list_field_attr_t ca_info_list_entitle_attr[DVB_CA_INFO_ENTITLE_FIELD] =
  {
    { LISTFIELD_TYPE_DEC| STL_LEFT | STL_VCENTER,
      80, 25, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },

    { LISTFIELD_TYPE_UNISTR| STL_LEFT | STL_VCENTER,
      300, 500, 0, &ca_info_field_rstyle,  &ca_info_field_fstyle },
  };
  //set flag false then can't into operator info page only if operator list update successly
  set_update_sp_flag(FALSE);
  
  // create container
  p_cont = ui_comm_right_root_create(ROOT_ID_SP_LIST2, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ca_sp2_info_proc);

  //mbox item title
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_CA_INFO_HEAD,
                            SN_DVB_CA_MBOX_X,SN_DVB_CA_MBOX_Y,
                            SN_DVB_CA_MBOX_W, SN_DVB_CA_MBOX_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_attr(p_mbox, OBJ_ATTR_ACTIVE);
  mbox_set_focus(p_mbox, 1);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox, DVB_CA_ITME_COUNT, DVB_CA_ITME_COUNT, 1);
  mbox_set_string_fstyle(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_offset(p_mbox, 0, 0);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  for (i = 0; i < DVB_CA_ITME_COUNT; i++)
  {
    mbox_set_content_by_strid(p_mbox, i, itemmenu_btn_str[i]);
  }
  
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_CA_INFO_LIST_ENTITLE,
                           SN_DVB_CA_PLIST_BG_X,SN_DVB_CA_PLIST_BG_Y,//CA_PLIST_BG_X, CA_PLIST_BG_Y, 
                           SN_DVB_CA_PLIST_BG_W,SN_DVB_CA_PLIST_BG_H, p_cont, 0);
  ctrl_set_keymap(p_list, ca_sp2_list_keymap);
  ctrl_set_proc(p_list, ca_sp2_list_proc);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  list_set_item_rstyle(p_list, &ca_info_item_rstyle);
  list_set_count(p_list, 0, DVB_CA_INFO_LIST_PAGE);
  list_set_field_count(p_list, DVB_CA_INFO_ENTITLE_FIELD, DVB_CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, ca_info_list_sp_id_update, 0);
 
  for (i = 0; i < DVB_CA_INFO_ENTITLE_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(ca_info_list_entitle_attr[i].attr),(u16)(ca_info_list_entitle_attr[i].width),(u16)(ca_info_list_entitle_attr[i].left),(u8)(ca_info_list_entitle_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i,ca_info_list_entitle_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i,ca_info_list_entitle_attr[i].fstyle);
  }  

  #ifndef WIN32
  ui_ca_do_cmd(CAS_CMD_OPERATOR_INFO_GET, 0, 0);
  #endif
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  DEBUG(DBG,INFO,"[dvbcas] %s>>>>>>>>>>>>> %d ! \n",__func__,__LINE__);
  return SUCCESS;
}

static RET_CODE on_ca_info_sp_id(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_CA_INFO_LIST_ENTITLE);

  p_sp_id= (cas_operators_info_t *)para2;
  list_set_count(p_list, p_sp_id->max_num, DVB_CA_INFO_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  ca_info_list_sp_id_update(p_list, list_get_valid_pos(p_list), DVB_CA_INFO_LIST_PAGE, 0);

  ctrl_paint_ctrl(p_list, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_sp_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  RET_CODE ret = SUCCESS;
  u16 old_focus;
  //when operator list unupdated,we can't into the operator info page
  if(get_update_sp_flag() == FALSE)
  {
    return ERR_FAILURE;
  }
  old_focus = list_get_focus_pos(p_list);
  ret = list_class_proc(p_list, msg, para1, para2);
  my_sp_id = p_sp_id->p_operator_info[old_focus].operator_id;
  DEBUG(DBG,INFO,"p_sp_id select = %d\n",my_sp_id);
  manage_open_menu(ROOT_ID_CA_ENTITLE_INFO, 0, 0);
  return SUCCESS;
}

BEGIN_MSGPROC(ca_sp2_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_OPE_INFO, on_ca_info_sp_id)
//  ON_COMMAND(MSG_SAVE, on_ca_info_save)
END_MSGPROC(ca_sp2_info_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ca_sp2_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(ca_sp2_list_keymap, NULL)

BEGIN_MSGPROC(ca_sp2_list_proc, list_class_proc)
  ON_COMMAND(MSG_YES, on_sp_list_select)
END_MSGPROC(ca_sp2_list_proc, list_class_proc)

