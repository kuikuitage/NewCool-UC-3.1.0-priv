#include "ui_common.h"
#include "ui_ca_order_ipp_rtime.h"
#include "ui_finger_print.h"
#include "ui_ca_public.h"

#include "ui_ca_buy_taping.h"
#include "ui_ca_order_ipp_rtime.h"

enum control_id
{
  IDC_INVALID = 4,
  IDC_CA_IPP_BUY_TAPING,
  IDC_CA_IPP_BUY_NO_TAPING,
};

static u8 g_curn_index = IDC_CA_IPP_BUY_TAPING;


u16 tf_ipp_buy_taping_cont_keymap(u16 key);
RET_CODE tf_ipp_buy_taping_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);



RET_CODE open_ipp_buy_taping(u32 para1, u32 para2)
{
	control_t *p_ctrl, *p_cont;
	OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);




	p_cont = fw_create_mainwin(ROOT_ID_CA_IPP_BUY_TAPING,
								 IPP_RTIME_IPP_CONT_X, IPP_RTIME_IPP_CONT_Y,
								 IPP_RTIME_IPP_CONT_W, IPP_RTIME_IPP_CONT_H,
								 0, 0,
								 OBJ_ATTR_ACTIVE, 0);
		if (p_cont == NULL)
		{
			return FALSE;
		}
	ctrl_set_rstyle(p_cont,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG);
	ctrl_set_keymap(p_cont, tf_ipp_buy_taping_cont_keymap);
	ctrl_set_proc(p_cont, tf_ipp_buy_taping_cont_proc);
	OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);


	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_IPP_BUY_TAPING),
								  10, 
								  140,
								  580, 
								  50,
								  p_cont, 0);
	OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);				
					ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
					text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
					//text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
					text_set_offset(p_ctrl, COMM_CTRL_OX, 0);
					text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
					text_set_content_by_strid(p_ctrl,IDS_CA_IPP_BUY_TAPING);


	ctrl_set_related_id(p_ctrl,
				0,									   /* left */
				(u8)IDC_CA_IPP_BUY_NO_TAPING, 		  /* up */
				0,									   /* right */
				(u8)IDC_CA_IPP_BUY_NO_TAPING);/* down */

	OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_IPP_BUY_NO_TAPING),
									  10, 
									  210,
									  580, 
									  50,
									  p_cont, 0);

OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);				
						ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
						text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
						text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
						text_set_offset(p_ctrl, COMM_CTRL_OX, 0);
						text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
						text_set_content_by_strid(p_ctrl,IDS_CA_IPP_BUY_NO_TAPING);

OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);

ctrl_set_related_id(p_ctrl,
				0,									   /* left */
				(u8)IDC_CA_IPP_BUY_TAPING, 		  /* up */
				0,									   /* right */
				(u8)IDC_CA_IPP_BUY_TAPING);/* down */

ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CA_IPP_BUY_TAPING), MSG_GETFOCUS, 0, 0);
ctrl_paint_ctrl(p_cont, FALSE);
OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);

return SUCCESS;					
}


static RET_CODE on_ipp_channel_up_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont = NULL;

	p_cont = fw_find_root_by_id(ROOT_ID_CA_IPP_BUY_TAPING);
OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);	
	switch(msg)
	{
		case MSG_FOCUS_UP:
		case MSG_FOCUS_DOWN:
		  if(IDC_CA_IPP_BUY_TAPING == g_curn_index)
			{
			OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);
			//	ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
				ctrl_process_msg(ctrl_get_child_by_id(p_cont, IDC_CA_IPP_BUY_NO_TAPING), MSG_GETFOCUS, 0, 0);
  				ctrl_process_msg(ctrl_get_child_by_id(p_cont, IDC_CA_IPP_BUY_TAPING), MSG_LOSTFOCUS, 0, 0);
				g_curn_index = IDC_CA_IPP_BUY_NO_TAPING;
			}
			else
			{
			OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);
				//ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
				ctrl_process_msg(ctrl_get_child_by_id(p_cont, IDC_CA_IPP_BUY_TAPING), MSG_GETFOCUS, 0, 0);
  				ctrl_process_msg(ctrl_get_child_by_id(p_cont, IDC_CA_IPP_BUY_NO_TAPING), MSG_LOSTFOCUS, 0, 0);
				g_curn_index = IDC_CA_IPP_BUY_TAPING;
			}
			break;
	//	case MSG_FOCUS_DOWN:
	//		manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_DOWN, 0);
	//		break;
		default:
			OS_PRINTF("error message in on_ipp_channel_up_down\n");
	}
	ctrl_paint_ctrl(p_cont, TRUE);
	return SUCCESS;
}


static RET_CODE on_ipp_buy_select_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

	OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);

	control_t *p_temp;
	u8 temp_ctrl_id = IDC_CA_IPP_BUY_TAPING;
		
	p_temp=fw_find_root_by_id(ROOT_ID_CA_BOOK_IPP_REAL_TIME);

	if(IDC_CA_IPP_BUY_TAPING == g_curn_index)
		{		
			temp_ctrl_id = IDC_CA_IPP_BUY_TAPING;
		}
	else
		{
			temp_ctrl_id = IDC_CA_IPP_BUY_NO_TAPING;
		}
	//manage_close_menu(ROOT_ID_CA_IPP_BUY_TAPING, 0, 0);
	OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);

	on_ipp_pin_select_ok(p_ctrl, 0, (u32)temp_ctrl_id, 0);
	OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);
	manage_close_menu(ROOT_ID_CA_IPP_BUY_TAPING, 0, 0);
	OS_PRINTF("%s ,%d \n",__FUNCTION__,__LINE__);
	return SUCCESS;
}


static RET_CODE on_ipp_buy_pating_exit(control_t *p_ctrl,	u16 msg, u32 para1, u32 para2)
{
	manage_close_menu(ROOT_ID_CA_IPP_BUY_TAPING, 0, 0);
	return SUCCESS;
}



BEGIN_KEYMAP(tf_ipp_buy_taping_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN) 
	ON_EVENT(V_KEY_OK, MSG_SELECT)

	ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(tf_ipp_buy_taping_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tf_ipp_buy_taping_cont_proc, ui_comm_root_proc) 
	ON_COMMAND(MSG_FOCUS_UP, on_ipp_channel_up_down)
	ON_COMMAND(MSG_FOCUS_DOWN, on_ipp_channel_up_down)
	ON_COMMAND(MSG_SELECT, on_ipp_buy_select_ok)

	ON_COMMAND(MSG_EXIT, on_ipp_buy_pating_exit)
END_MSGPROC(tf_ipp_buy_taping_cont_proc, ui_comm_root_proc)








