#include "ui_common.h"

#include "ui_ca_urgent.h"
#include "gb2312.h"
#include "dvbca_cas_interface.h"
#include "dvbca_types.h"

enum ppv_icon_ctrl_id
{
  IDC_URGENT_TITLE = 1,
  IDC_URGENT_CONTENT, 
};

RET_CODE ca_urgent_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_urgent()
{
	control_t *p_cont = NULL,*p_title = NULL,*p_content = NULL;
	u16 uni_str[DVBCA_NOTIFY_CONTENT_SIZE + 1];
	DVBCA_UINT8 *title = "title";
	DVBCA_UINT8 *content = "Content";
#ifndef WIN32
	if(!ui_is_fullscreen_menu(fw_get_focus_id()))
		return ERR_FAILURE;
#endif
	if(fw_find_root_by_id(ROOT_ID_CA_URGENT) != NULL)
		return ERR_FAILURE;
	p_cont = fw_create_mainwin(ROOT_ID_CA_URGENT,
	                              SN_URGENT_X, SN_URGENT_Y, 
	                              SN_URGENT_W, SN_URGENT_H,
	                              0, 0,
	                              OBJ_ATTR_INACTIVE, 0);
	ctrl_set_proc(p_cont, ca_urgent_cont_proc);
	ctrl_set_rstyle(p_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	if (p_cont == NULL)
	{
		return FALSE;
	}
	p_title = ctrl_create_ctrl( CTRL_TEXT, IDC_URGENT_TITLE, 
	                                    SN_URGENT_TITLE_X, SN_URGENT_TITLE_Y, 
	                                    SN_URGENT_TITLE_W, SN_URGENT_TITLE_H, 
	                                    p_cont, 0);
	text_set_font_style(p_title, FSI_URGENT_TXT, FSI_URGENT_TXT, FSI_URGENT_TXT);
	text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_title, TEXT_STRTYPE_UNICODE);
	gb2312_to_unicode(title,strlen((char*)title),uni_str,DVBCA_NOTIFY_CONTENT_SIZE);
	text_set_content_by_unistr(p_title, uni_str);

	p_content = ctrl_create_ctrl( CTRL_TEXT, IDC_URGENT_CONTENT, 
	                                    SN_URGENT_CONTENT_X, SN_URGENT_CONTENT_Y, 
	                                    SN_URGENT_CONTENT_W, SN_URGENT_CONTENT_H, 
	                                    p_cont, 0);
	text_set_font_style(p_content, FSI_URGENT_TXT, FSI_URGENT_TXT, FSI_URGENT_TXT);
	text_set_align_type(p_content, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
	gb2312_to_unicode(content,strlen((char*)content),uni_str,DVBCA_NOTIFY_CONTENT_SIZE);
	text_set_content_by_unistr(p_content, uni_str);

	ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
	return SUCCESS;
}

void close_ca_urgent(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_CA_URGENT);
}

static RET_CODE on_urgent_exit(control_t * p_ctrl,u16 msg,u32 para1,u32 para2)
{
  close_ca_urgent();
  fw_tmr_destroy(ROOT_ID_CA_URGENT, MSG_EXIT);

  return SUCCESS;
}

BEGIN_MSGPROC(ca_urgent_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_urgent_exit)
END_MSGPROC(ca_urgent_cont_proc, cont_class_proc)

