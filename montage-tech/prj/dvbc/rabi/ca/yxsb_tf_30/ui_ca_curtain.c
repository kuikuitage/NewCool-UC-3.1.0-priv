/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_public.h"
#include "ui_ca_curtain.h"

enum control_id
{
	IDC_INVALID = 0,
};

BOOL g_is_curtain = FALSE;
BOOL ui_is_curtain(void)
{
	return g_is_curtain;
}

RET_CODE open_ca_card_curtain(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  
	u8 focus = fw_get_focus_id();
	g_is_curtain = TRUE;
	if(fw_find_root_by_id(ROOT_ID_CA_CARD_CURTAIN) != NULL)
	{
		UI_PRINTF("\n ROOT_ID_CA_CARD_CURTAIN already opened............!!! \n");
		return ERR_FAILURE;
	}
	if(!ui_is_fullscreen_menu(focus))
	{
		return SUCCESS;
	}
	p_cont = fw_create_mainwin(ROOT_ID_CA_CARD_CURTAIN,
	                       CA_CURATIN_CONT_FULL_X, CA_CURATIN_CONT_FULL_Y,
	                       CA_CURATIN_CONT_FULL_W, CA_CURATIN_CONT_FULL_H,
	                       ROOT_ID_INVALID, 0,
	                       OBJ_ATTR_INACTIVE, 0);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_rstyle(p_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
 
	ctrl_paint_ctrl(p_cont, FALSE);
	return SUCCESS;
}

void close_ca_card_curtain(void)
{
	g_is_curtain = FALSE;
	manage_close_menu(ROOT_ID_CA_CARD_CURTAIN, 0, 0);
}

