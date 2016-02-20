/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_operate.h"
#include "ui_ca_public.h"


enum control_id
{
  IDC_INVALID = 0,
  IDC_CA_OPE_MBOX,
  IDC_CA_PRO,
};

static list_xstyle_t ca_item_rstyle =
{
	RSI_PBACK,
	RSI_PBACK,
	RSI_SELECT_F,
	RSI_SELECT_F,
	RSI_SELECT_F,
};
static list_xstyle_t ca_field_fstyle =
{
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
};
static list_xstyle_t ca_field_rstyle =
{
	RSI_PBACK,
	RSI_PBACK,
	RSI_IGNORE,
	RSI_IGNORE,
	RSI_IGNORE,
};

static list_field_attr_t ca_list_attr[CA_OPE_LIST_FIELD] =
{
	{LISTFIELD_TYPE_DEC| STL_CENTER| STL_VCENTER,
		CA_INFO_PRODUCT_ID_W, CA_INFO_PRODUCT_ID_OFFSET, 0, &ca_field_rstyle,  &ca_field_fstyle },
	{LISTFIELD_TYPE_DEC| STL_CENTER| STL_VCENTER,
		CA_OPE_ID_W, CA_OPE_ID_OFFSET, 0, &ca_field_rstyle,  &ca_field_fstyle }
};

static cas_operators_info_t operators_info = {0};

u16 ui_comm_root_keymap(u16 key);
RET_CODE ui_comm_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ca_list_keymap(u16 key);
RET_CODE ca_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE on_ca_item_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u32 oper_id = 0;

	if(list_get_count(p_ctrl) > 0)
	{
		oper_id = list_get_field_content(p_ctrl, list_get_focus_pos(p_ctrl), 1);
		manage_open_menu(ROOT_ID_CA_INFO, oper_id, 0);
	}

	return SUCCESS;
}

static RET_CODE operate_plist_update(control_t*p_ctrl, u16 start, u16 size, u32 context)
{
	u16 i;
	u16 cnt = list_get_count(p_ctrl);

	for (i = 0; i < size; i++)
	{
		if (i + start < cnt)
		{
			list_set_field_content_by_dec(p_ctrl, (u16)(start + i), 0, (u16)(start + i)); 
			list_set_field_content_by_dec(p_ctrl, (u16)(start + i), 1, operators_info.p_operator_info[start + i].operator_id); 
		}
	}
	return SUCCESS;
}

static RET_CODE on_ca_operate_update(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"on_ca_operate_update[0x%x]\n",para2);
	memcpy(&operators_info, (void*)para2, sizeof(cas_operators_info_t));
	list_set_count(p_list, operators_info.max_num, CA_OPE_LIST_PAGE);

	operate_plist_update(p_list, list_get_valid_pos(p_list), CA_OPE_LIST_PAGE, 0);
	if(operators_info.max_num > 0)
	{
		list_set_focus_pos(p_list, 0);
	}
	
	ctrl_paint_ctrl(ctrl_get_root(p_list), TRUE);
	return SUCCESS;
}
RET_CODE open_ca_operate(u32 para1, u32 para2)
{
	control_t *p_cont,*p_list,*p_mbox;
	u8 i;

	u16 itemmenu_btn_str[CA_OPE_COUNT] =
	{
		IDS_LABLE, IDS_CA_OPERATOR
	};
	p_cont = ui_comm_right_root_create(ROOT_ID_CA_OPE, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
  
	//mbox item title
	p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_CA_OPE_MBOX,
				                    CA_OPE_MBOX_X, CA_OPE_MBOX_Y, 
				                    CA_OPE_MBOX_W, CA_OPE_MBOX_H,p_cont, 0);
	ctrl_set_rstyle(p_mbox, RSI_PBACK,RSI_PBACK,RSI_PBACK);
	ctrl_set_attr(p_mbox, OBJ_ATTR_ACTIVE);
	mbox_set_focus(p_mbox, 1);
	mbox_enable_string_mode(p_mbox, TRUE);
	mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);
	mbox_set_count(p_mbox, CA_OPE_COUNT, CA_OPE_COUNT, 1);
	mbox_set_string_fstyle(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	mbox_set_string_offset(p_mbox, CA_OPE_MBOX_OFFSET, 0);
	mbox_set_string_align_type(p_mbox, STL_LEFT| STL_VCENTER);
	for (i = 0; i < CA_OPE_COUNT; i++)
	{
		mbox_set_content_by_strid(p_mbox, i, itemmenu_btn_str[i]);
	}

	//LIST
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_CA_PRO,
	                   CA_OPE_LIST_BG_X, CA_OPE_LIST_BG_Y, CA_OPE_LIST_BG_W,CA_OPE_LIST_BG_H, p_cont, 0);
	ctrl_set_keymap(p_list, ca_list_keymap);
	ctrl_set_proc(p_list, ca_list_proc);
	ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	list_set_item_rstyle(p_list, &ca_item_rstyle);
	list_set_count(p_list, 0, CA_OPE_LIST_PAGE);
	list_set_field_count(p_list, CA_OPE_LIST_FIELD, CA_OPE_LIST_PAGE);
	list_set_focus_pos(p_list, 1);

	for (i = 0; i < CA_OPE_LIST_FIELD; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(ca_list_attr[i].attr), (u16)(ca_list_attr[i].width),
		                 (u16)(ca_list_attr[i].left), (u8)(ca_list_attr[i].top));
		list_set_field_rect_style(p_list, (u8)i, ca_list_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, ca_list_attr[i].fstyle);
	}

	ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);
	
	ui_ca_do_cmd(CAS_CMD_OPERATOR_INFO_GET, 0 ,0);
	return SUCCESS;
}

BEGIN_KEYMAP(ca_list_keymap, NULL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ca_list_keymap, NULL)

BEGIN_MSGPROC(ca_list_proc, list_class_proc)
	ON_COMMAND(MSG_SELECT, on_ca_item_select)
	ON_COMMAND(MSG_CA_OPE_INFO, on_ca_operate_update)
END_MSGPROC(ca_list_proc, list_class_proc)

 
