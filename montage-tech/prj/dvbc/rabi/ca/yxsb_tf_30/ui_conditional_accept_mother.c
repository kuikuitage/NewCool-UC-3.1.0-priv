/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_conditional_accept_mother.h"
#include "ui_ca_public.h"

enum ctrl_id
{
	IDC_TITLE_MBOX = 1,
	IDC_CAS_MOTHER_LIST,
	IDC_CAS_MOTHER_LIST_SBAR,
};

static list_xstyle_t mother_list_item_rstyle =
{
	RSI_PBACK,
	RSI_PBACK,
	RSI_SELECT_F,
	RSI_SELECT_F,
	RSI_SELECT_F
};

static list_xstyle_t mother_list_field_fstyle =
{
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
};

static list_xstyle_t mother_list_field_rstyle =
{
	RSI_PBACK,
	RSI_PBACK,
	RSI_IGNORE,
	RSI_IGNORE,
	RSI_IGNORE,
};

static list_field_attr_t mother_list_attr[CAS_MOTHER_LIST_FIELD_NUM] =
{
	{ LISTFIELD_TYPE_DEC | STL_CENTER| STL_VCENTER,
		(CAS_MOTHER_LIST_W / 5 - 15), 0, 0, &mother_list_field_rstyle,  &mother_list_field_fstyle},
	{ LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
		(CAS_MOTHER_LIST_W / 5), (CAS_MOTHER_LIST_W / 5 - 15), 0, &mother_list_field_rstyle,  &mother_list_field_fstyle },
	{ LISTFIELD_TYPE_UNISTR| STL_CENTER | STL_VCENTER,
		(CAS_MOTHER_LIST_W / 5), (2 * CAS_MOTHER_LIST_W / 5 -15), 0, &mother_list_field_rstyle,  &mother_list_field_fstyle},
	{ LISTFIELD_TYPE_DEC| STL_CENTER | STL_VCENTER,
		(CAS_MOTHER_LIST_W / 5), (3 * CAS_MOTHER_LIST_W / 5 - 15), 0, &mother_list_field_rstyle,  &mother_list_field_fstyle},
	{ LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
		(CAS_MOTHER_LIST_W / 5 + 10), (4 * CAS_MOTHER_LIST_W / 5 - 15), 0, &mother_list_field_rstyle,  &mother_list_field_fstyle},
};

static cas_operators_info_t *p_operator = NULL;
static chlid_card_status_info p_child_info[4] = {{0},};
static u8 child_info_count = 0;

comm_dlg_data_t dlg_data = 
{
	ROOT_ID_INVALID,
	DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
	(300 + (SYS_RIGHT_CONT_W - COMM_DLG_W)/2), (SCREEN_HEIGHT- COMM_DLG_H)/2 - 20,
	COMM_DLG_W, COMM_DLG_H + 40,
	IDS_CA_CARD_MANAGE,
	RSC_INVALID_ID,
	0,
};

u16 mother_list_keymap(u16 key);
RET_CODE mother_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

static RET_CODE mon_child_info_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 uni_str[32];
  u8 asc_str[64];
  u16 cnt = list_get_count(p_list);
  #ifdef WIN32
  //static chlid_card_status_info p_child_info_temp[4] = {{0},};
  p_child_info[0].operator_id = 2;
  p_child_info[0].is_child = 1;
  memcpy( p_child_info[0].parent_card_sn, "8000302100228520", 16 );
  p_child_info[0].delay_time = 720;
  p_child_info[0].last_feed_time[0] = 20;
  p_child_info[0].last_feed_time[1] = 20;
  p_child_info[0].last_feed_time[2] = 20;
  p_child_info[0].last_feed_time[3] = 20;
  p_child_info[0].last_feed_time[4] = 20;
  p_child_info[0].last_feed_time[5] = 20;
  p_child_info[0].last_feed_time[6] = 20;

  #endif
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      // id 
      list_set_field_content_by_dec(p_list, (u16)(start + i), 0, p_child_info[i].operator_id); 

      // state 
      switch(p_child_info[i].is_child)
      {
        case 0:
          str_asc2uni((u8 *)"mother", uni_str);
          break;
        case 1:
          str_asc2uni((u8 *)"child", uni_str);
          break;
        default:
          str_asc2uni((u8 *)"common", uni_str);
          break;
      }
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str);

      DEBUG(CAS,INFO,"@@@@@ parent_card_sn == %s\n", p_child_info[i].parent_card_sn);
      //mother card sn;
      memset(asc_str, 0, sizeof(asc_str));
      if (p_child_info[i].is_child == 1)
      {
        sprintf((char*)asc_str,"%s", p_child_info[i].parent_card_sn);
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 2, asc_str);
      }
      memset(asc_str, 0, sizeof(asc_str));
        
      DEBUG(CAS,INFO,"@@@@@ is_child = %d, delay_time = %d, is_can_feed = %d, last_feed_time:%d-%d-%d %d-%d-%d\n",
                                          p_child_info[i].is_child,
                                          p_child_info[i].delay_time,
                                          p_child_info[i].is_can_feed,
                                          p_child_info[i].last_feed_time[0] * 100 + p_child_info[i].last_feed_time[1],
                                          p_child_info[i].last_feed_time[2],
                                          p_child_info[i].last_feed_time[3],
                                          p_child_info[i].last_feed_time[4],
                                          p_child_info[i].last_feed_time[5],
                                          p_child_info[i].last_feed_time[6]);
      //interval time
      if (p_child_info[i].is_child == 1)
      {
        list_set_field_content_by_dec(p_list, (u16)(start + i), 3, p_child_info[i].delay_time); 
        DEBUG(CAS,INFO,"delay time:%d\n", p_child_info[i].delay_time);
        
        //last time
        memset(asc_str, 0, sizeof(asc_str));
        sprintf((char*)asc_str,"%04d.%02d.%02d-%02d:%02d:%02d",
                    p_child_info[i].last_feed_time[0] * 100 + p_child_info[i].last_feed_time[1],
                    p_child_info[i].last_feed_time[2],
                    p_child_info[i].last_feed_time[3],
                    p_child_info[i].last_feed_time[4],
                    p_child_info[i].last_feed_time[5],
                    p_child_info[i].last_feed_time[6]);
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 4, asc_str);
      }      
    }
  }
  return SUCCESS;
}

static RET_CODE on_mon_child_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

	//RET_CODE ret = para1;
	//u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
	DEBUG(CAS,INFO,"@@@@@child_info_count ==%d, p_operator->max_num == %d\n", child_info_count, p_operator->max_num);
	//if(curn_root_id == ROOT_ID_CONDITIONAL_ACCEPT_MOTHER)
	//p_child_info = (chlid_card_status_info *)para2;
	memcpy(&p_child_info[child_info_count], (chlid_card_status_info *)para2, sizeof(chlid_card_status_info));
	child_info_count++;

	if(child_info_count == p_operator->max_num)
	{
		list_set_count(p_ctrl, child_info_count, CAS_MOTHER_LIST_PAGE);
		list_set_focus_pos(p_ctrl, 0);
		mon_child_info_update(p_ctrl, 0, child_info_count, 0);
		ctrl_paint_ctrl(p_ctrl, TRUE);
	}

	return SUCCESS;
}

static RET_CODE on_operate_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u32 i =0;
	u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
	DEBUG(CAS,INFO,"@@@@@ on_operate_info curn_root_id:%d\n", curn_root_id);
	if(curn_root_id == ROOT_ID_CONDITIONAL_ACCEPT_MOTHER)
	{
		p_operator = (cas_operators_info_t *)para2;
		if(p_operator->max_num > 0)
		{    
			for(i = 0; i < p_operator->max_num; i++)
			{
				ui_ca_do_cmd(CAS_CMD_MON_CHILD_STATUS_GET, p_operator->p_operator_info[i].operator_id, 0);
			}
		}
	}
	return SUCCESS;
}

static RET_CODE on_mon_child_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	u16 cur_pos = 0;
	dlg_ret_t dlg_ret = DLG_RET_NULL;
	cur_pos = list_get_focus_pos(p_list);
	if(p_child_info[cur_pos].is_child == 0)
	{
		OS_PRINTF("card type is_monther!\n");
		ui_ca_do_cmd(CAS_CMD_READ_FEED_DATA, p_child_info[cur_pos].operator_id,0);
	}
	else if(p_child_info[cur_pos].is_child == 1)
	{
		OS_PRINTF("card type is_child!\n");
		if(p_child_info[cur_pos].is_can_feed)
		{
			dlg_data.content = IDS_CA_INSERT_PARENT_CARD;
		}
		else
		{
			dlg_data.content = IDS_CA_NOT_TIME_FEED;
		}
		
		dlg_ret = ui_comm_dlg_open(&dlg_data);
		if(dlg_ret == DLG_RET_YES)
		{
			if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
			{
				ui_comm_dlg_close();
			}
			DEBUG(CAS,INFO,"@@@@@ on_conditional_accept_feed_btn_select get feed data, cur_pos == %d, p_child_info[cur_pos].operator_id == %d,is_can_feed == %d!!!\n",
			                        cur_pos,p_child_info[cur_pos].operator_id, p_child_info[cur_pos].is_can_feed);
			if(p_child_info[cur_pos].is_can_feed)
			{
				ui_ca_do_cmd(CAS_CMD_READ_FEED_DATA, p_child_info[cur_pos].operator_id, 0);//need_modify
			}
		}
	}
	else
	{
		OS_PRINTF("ERROR card type!\n");
	}
	return SUCCESS;
}



static RET_CODE on_mon_child_feed_data(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = (RET_CODE)para1;
	u16 cur_pos = 0;
	chlid_card_status_info *mother_card = (chlid_card_status_info *)para2;
	dlg_ret_t dlg_ret = DLG_RET_NULL;
	u8 curn_root_id = manage_get_curn_menu_attr()->root_id;

	cur_pos = list_get_focus_pos(p_ctrl);
	DEBUG(CAS,INFO,"@@@@@ on_mon_child_feed_data curn_root_id:%d, cur_pos : %d\n", curn_root_id, cur_pos);
	if(curn_root_id == ROOT_ID_CONDITIONAL_ACCEPT_MOTHER)
	{
		if(ret != SUCCESS)
		{
			dlg_data.content =IDS_CA_MOTHER_CARD_ERR;
			dlg_ret = ui_comm_dlg_open(&dlg_data);
			if(dlg_ret == DLG_RET_YES)
			{
				if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
				{
					ui_comm_dlg_close();
				}
			}
			return ret;
		}
		else
		{
			dlg_data.content =IDS_CA_FEED_INSERT_CHILD_CARD;
			dlg_ret = ui_comm_dlg_open(&dlg_data);
			if(dlg_ret == DLG_RET_YES)
			{
				ui_ca_do_cmd(CAS_CMD_MON_CHILD_FEED, (u32)mother_card, 0);
				if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
				{
					ui_comm_dlg_close();
				}
			}
		}
	}
	return SUCCESS;
}

static RET_CODE on_mon_child_feed_result(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = (RET_CODE)para1;
	dlg_ret_t dlg_ret = DLG_RET_YES;
	u8 curn_root_id = manage_get_curn_menu_attr()->root_id;
	DEBUG(CAS,INFO,"@@@@@ on_mon_child_feed_result curn_root_id:%d\n", curn_root_id);
	if(curn_root_id == ROOT_ID_CONDITIONAL_ACCEPT_MOTHER)
	{
		if(ret != SUCCESS)
		{ 
			dlg_data.content =IDS_CA_FEED_FAILED;
			if(ret == CAS_E_FEEDTIME_NOT_ARRIVE)
			{
				dlg_data.content =IDS_CA_FEED_TIME_NOT_ARRIVE;
			}
		}
		else
		{
			dlg_data.content =IDS_CA_FEED_SUCCESS;
		}
		
		dlg_ret = ui_comm_dlg_open(&dlg_data);
		if(dlg_ret == DLG_RET_YES)
		{
			if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
			{
				ui_comm_dlg_close();
			}
		}
	}
	return ret;
}

static RET_CODE on_mon_child_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  child_info_count = 0;
  manage_close_menu(ROOT_ID_CONDITIONAL_ACCEPT_MOTHER, 0, 0);
  return ERR_FAILURE;
}

RET_CODE open_conditional_accept_mother(u32 para1, u32 para2)
{
	control_t  *p_cont, *p_mbox, *p_list = NULL;
	u8 i=0;
	u16 stxt_title[CAS_MOTHER_TITLE_CNT] =
	{ 
		IDS_CA_OPERATOR,
		IDS_CA_CARD_TYPE,
		IDS_CA_PARENT_CARD_ID,
		IDS_CA_FEED_DURATION,
		IDS_CA_LAST_FEED_TIME
	};
	child_info_count = 0;

	p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_MOTHER, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);

	if (p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	//ctrl_set_keymap(p_cont, conditional_accept_mother_cont_keymap);
	//ctrl_set_proc(p_cont, conditional_accept_mother_cont_proc);
 

	//titles
	p_mbox = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_TITLE_MBOX,
	                          CAS_MOTHER_TITLE_MBOX_X, CAS_MOTHER_TITLE_MBOX_Y,
	                          CAS_MOTHER_TITLE_MBOX_W,CAS_MOTHER_TITLE_MBOX_H,
	                          p_cont, 0);
	ctrl_set_rstyle(p_mbox, RSI_PBACK,RSI_PBACK, RSI_PBACK);
	mbox_set_focus(p_mbox, 1);
	mbox_enable_string_mode(p_mbox, TRUE);
	mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);
	mbox_set_count(p_mbox, CAS_MOTHER_TITLE_CNT, CAS_MOTHER_TITLE_CNT, 1);
	mbox_set_string_fstyle(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	mbox_set_string_offset(p_mbox, CAS_MOTHER_TITLE_OFFSET, 0);
	mbox_set_string_align_type(p_mbox, STL_CENTER| STL_VCENTER);

	for (i = 0; i < CAS_MOTHER_TITLE_CNT; i++)
	{
		mbox_set_content_by_strid(p_mbox, i, stxt_title[i]);
	}

	//create feed list
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_CAS_MOTHER_LIST,
	                 CAS_MOTHER_LIST_X, CAS_MOTHER_LIST_Y,
	                 CAS_MOTHER_LIST_W, CAS_MOTHER_LIST_H,
	                 p_cont, 0);
	ctrl_set_rstyle(p_list,RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_keymap(p_list, mother_list_keymap);
	ctrl_set_proc(p_list, mother_list_proc);
	list_set_item_rstyle(p_list, &mother_list_item_rstyle);
	list_set_count(p_list, 0, CAS_MOTHER_LIST_PAGE);
	list_set_field_count(p_list, CAS_MOTHER_LIST_FIELD_NUM, CAS_MOTHER_LIST_PAGE);
	list_set_focus_pos(p_list, 0);
	list_set_update(p_list, mon_child_info_update, 0);

	for (i = 0; i < CAS_MOTHER_LIST_FIELD_NUM; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(mother_list_attr[i].attr), (u16)(mother_list_attr[i].width),
		                    (u16)(mother_list_attr[i].left), (u8)(mother_list_attr[i].top));
		list_set_field_rect_style(p_list, (u8)i, mother_list_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, mother_list_attr[i].fstyle);
	}

	ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

	ui_ca_do_cmd(CAS_CMD_OPERATOR_INFO_GET, 0, 0);

  return SUCCESS;
}

BEGIN_KEYMAP(mother_list_keymap, NULL)
	ON_EVENT(V_KEY_OK, MSG_SELECT)  
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)  
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)   
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)   
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)      
	ON_EVENT(V_KEY_MENU, MSG_EXIT)  
	ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(mother_list_keymap, NULL)

BEGIN_MSGPROC(mother_list_proc, list_class_proc)
	ON_COMMAND(MSG_EXIT, on_mon_child_exit)
	ON_COMMAND(MSG_EXIT_ALL, on_mon_child_exit)
	ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_mon_child_info)
	ON_COMMAND(MSG_CA_OPE_INFO, on_operate_info)
	ON_COMMAND(MSG_CA_READ_FEED_DATA, on_mon_child_feed_data)
	ON_COMMAND(MSG_CA_MON_CHILD_FEED, on_mon_child_feed_result)
	ON_COMMAND(MSG_SELECT, on_mon_child_list_select)
END_MSGPROC(mother_list_proc, list_class_proc)

