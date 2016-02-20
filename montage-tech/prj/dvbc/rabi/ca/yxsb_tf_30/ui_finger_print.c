/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include "ui_common.h"
#include "ui_finger_print.h"
#include "ui_ca_public.h"

enum control_id
{
  IDC_FINGER_PRINT = 1,
  IDC_FINGER_PRINT_TXT,
};

//电视偏移，防止边界显示不出
#define X_OFF 30
#define Y_OFF 25
#define CA_FINGER_PRINT_TMOUT 20  //ms

extern RET_CODE get_invisible_finger_status(void);

RET_CODE finger_print_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE close_finger_print(void)
{
  if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
  {
    fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
  }

  return SUCCESS;
}

RET_CODE open_finger_print(u32 para1, u32 para2)
{
	control_t *p_cont, *p_ctrl, *p_mbox;  
	finger_msg_t *p_finger_info = (finger_msg_t *)para2;  
	u16 x, y, w, h;
	u16 comb_data[8];
	u16 postion_x,postion_y;

	// check for close
	if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
	{  	
		close_finger_print();
	} 

	if(!ui_is_fullscreen_menu(fw_get_focus_id()))
	{
		return SUCCESS;
	}

	// 字符串显示
	if (0 == p_finger_info->finger_property.m_byFontType)
	{
		if (p_finger_info->show_front_size == 1)
			w = 15*p_finger_info->data_len;
		else if (p_finger_info->show_front_size == 2)
			w = 11*p_finger_info->data_len+15;
		else
			w = 13*p_finger_info->data_len+15;

		h = 35;	
	}//点阵
	else if (1 == p_finger_info->finger_property.m_byFontType)
	{	  
		h = w = 150;
	}//矩阵
	else if (2 == p_finger_info->finger_property.m_byFontType)
	{	
		h = w = 350;
	}
	else
	{  	
		return SUCCESS;
	}
  
	if(p_finger_info->show_postion_x == 0)
		postion_x = rand() % 100;
	else
		postion_x = 10;

	if(p_finger_info->show_postion_y == 0)
		postion_y = rand() % 100;
	else
		postion_y = 10;
		
	x = (SCREEN_WIDTH - w) * ((postion_x * 1.0)/100);
	y = (SCREEN_HEIGHT- h) * ((postion_y * 1.0)/100);
	DEBUG(CAS,INFO,"rect[%d][%d][%d][%d]\n",x,y,w,h);
  
	//当为点阵或者矩阵显示时，在最边上直接移动x，y位置，因为背景为透明看不出坐标被移动
	if (0 != p_finger_info->finger_property.m_byFontType)
	{
		if (x == 0)	
			x = X_OFF;
		if (y == 0) 
			y = Y_OFF;

		if(x >= (SCREEN_WIDTH - (w + X_OFF)))
		{
			x = SCREEN_WIDTH - (w + X_OFF);
		}

		if(y >= (SCREEN_HEIGHT- (h + Y_OFF)))
		{
			y = SCREEN_HEIGHT- (h + Y_OFF);
		}
	}
	DEBUG(CAS,INFO,"rect[%d][%d][%d][%d]\n",x,y,w,h);

	/*测试用例:DP.DC.01.022 超级指纹匦朐诔禣SD之上 设置 STL_EX_TOPMOST*/
	p_cont = fw_create_mainwin(ROOT_ID_FINGER_PRINT,
						x, y, w, h,
						ROOT_ID_INVALID, 0, OBJ_ATTR_INACTIVE, STL_EX_TOPMOST);
	if (p_cont == NULL) 
	{  	
		OS_PRINTF("\n\n\n\n[ui_finger_print][%s %d] invoked!致命bug!\n", __FUNCTION__, __LINE__);
		return ERR_FAILURE;
	}
  
	ctrl_set_proc(p_cont, finger_print_proc);
  
  // 字符串显示 直接显示szCount内容或者cardid
  if (0 == p_finger_info->finger_property.m_byFontType)
  {		
	u8 font_size = 0;
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_FINGER_PRINT,
							  0, 0, w, h,
							  p_cont, 0);
	if (p_ctrl == NULL) 
	{  	
		OS_PRINTF("\n\n [ui_finger_print][%s %d] invoked! bug!!! \n\n \n", __FUNCTION__, __LINE__);
		return ERR_FAILURE;
	}
	g_rsc_config.p_rstyle_tab[RSI_COMMAN_BG].bg.value = 	p_finger_info->finger_property.m_dwBackgroundARGB;
	ctrl_set_rstyle(p_ctrl, RSI_FINGER_PRINT_FRAME, RSI_FINGER_PRINT_FRAME, RSI_FINGER_PRINT_FRAME);

	//OS_PRINTF("[ui_finger_print][%s %d]	p_finger_info->show_front_size =%d\n", __FUNCTION__, __LINE__,p_finger_info->show_front_size);	 
	if(p_finger_info->show_front_size == 1)
	{
	  font_size = 20;
	}
	else if(p_finger_info->show_front_size == 2)
	{		  
	  font_size = 15;
	}
	else
	{
	 font_size = 18;
	}

	g_rsc_config.p_fstyle_tab[FSI_WHITE].width = g_rsc_config.p_fstyle_tab[FSI_WHITE].height = font_size;
	g_rsc_config.p_fstyle_tab[FSI_WHITE].color = p_finger_info->finger_property.m_dwFontARGB;

	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);		
	{			
		u32 style = 0;
		if (y == 0)	
			style = style | STL_BOTTOM;			
		if (x == 0)			
			style = style | STL_RIGHT;//防止显示不全
		text_set_align_type(p_ctrl, style);
		text_set_offset(p_ctrl, 0, 0);
	}			
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
	if(p_finger_info)	  
	  text_set_content_by_ascstr(p_ctrl, (u8*)p_finger_info->data);
	else
	  text_set_content_by_ascstr(p_ctrl, (u8*)" ");

  }
  //点阵显示 10*10显示，点的形状大小不做要求
  else if (1 == p_finger_info->finger_property.m_byFontType)
  {
  	 u16 index;
	 p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_FINGER_PRINT,
		 							0, 0, w, h,
                                    p_cont, 0);
    ctrl_set_rstyle(p_mbox, RSI_TRANSPARENT,RSI_TRANSPARENT, RSI_TRANSPARENT);
	mbox_enable_icon_mode(p_mbox, FALSE);
	mbox_enable_string_mode(p_mbox, TRUE);
    mbox_set_count(p_mbox, 100, 10, 10);
    //mbox_set_item_rect(p_mbox, 0, 0, w, h, 0, 0);	
    mbox_set_item_rstyle(p_mbox, RSI_TRANSPARENT,RSI_TRANSPARENT, RSI_TRANSPARENT);	
	mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_UNICODE);
  
    g_rsc_config.p_fstyle_tab[FSI_WHITE].width = g_rsc_config.p_fstyle_tab[FSI_WHITE].height = 7;	
    g_rsc_config.p_fstyle_tab[FSI_WHITE].color = 0xFFFFFFFF;//白色		
	mbox_set_string_fstyle(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	mbox_set_string_offset(p_mbox, 0, 0);
	mbox_set_string_align_type(p_mbox, STL_VCENTER | STL_VCENTER);

	//string转为数字
	for (index = 0; index < 16; index++)
	{
		if (p_finger_info->data[index] >= '0' &&  p_finger_info->data[index] <= '9')	
			p_finger_info->data[index] -= '0';
		else if (p_finger_info->data[index] >= 'A' &&  p_finger_info->data[index] <= 'F')			
			p_finger_info->data[index] = p_finger_info->data[index] - 'A' + 10;
		else if(p_finger_info->data[index] >= 'a' &&  p_finger_info->data[index] <= 'f')
			p_finger_info->data[index] = p_finger_info->data[index] - 'a' + 10;;
	}
	
	//每一行组成一个新的U16数字
	for (index = 0; index < 8; index++)
	{
		comb_data[index] = (p_finger_info->data[index]<<4) + p_finger_info->data[index+8];		
		//OS_PRINTF("[ui_finger_print][%s %d]	 comb_data[%d]=%02x\n", __FUNCTION__, __LINE__,index,comb_data[index]);	
	}
	
    for (index = 0; index < 100; index++)
    {	  
    	u8 row,col;
		mbox_set_content_by_ascstr(p_mbox, index, (u8 *)" ");  //clear item  	
    	if (index > 90 || index < 9 || (index%10) == 9 || (index%10) == 0)
    	{   
			if (index == 0 || index == 9 || index == 90 || index == 99)
				mbox_set_content_by_ascstr(p_mbox, index, (u8 *)"X");//移植PDF上是'+'替换成"X"，因为周围不做要求
		}			
		else
		{//lint -e{734}		
			row = index/10;
			col = index%10;
		 //lint -e{676}
			if ( (comb_data[row-1] << (col-1)) &0x80 )
				mbox_set_content_by_ascstr(p_mbox, index, (u8 *)"+");//中间的点使用'+'，因为使用"*或者."看不清
		}
    }	
	
  }  
  //矩阵显示 16*16显示，字体背景颜色有要求
   else if (2 == p_finger_info->finger_property.m_byFontType)
   {
		u16 i, index;
		u8 data[2]={0};
		p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_FINGER_PRINT,
										0, 0, w, h,
										p_cont, 0);
		ctrl_set_rstyle(p_mbox, RSI_TRANSPARENT,RSI_TRANSPARENT, RSI_TRANSPARENT);
		
		mbox_enable_icon_mode(p_mbox, FALSE);
		mbox_enable_string_mode(p_mbox, TRUE);
		mbox_set_count(p_mbox, 256, 16, 16);
		//mbox_set_item_rect(p_mbox, 0, 0, w, h, 0, 0);		
		mbox_set_item_rstyle(p_mbox, RSI_TRANSPARENT,RSI_TRANSPARENT, RSI_TRANSPARENT);
		mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_UNICODE);
		
		g_rsc_config.p_fstyle_tab[FSI_WHITE].width = g_rsc_config.p_fstyle_tab[FSI_WHITE].height = 12;
		g_rsc_config.p_fstyle_tab[FSI_WHITE].color = p_finger_info->finger_property.m_dwFontARGB;

		mbox_set_string_fstyle(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
		mbox_set_string_offset(p_mbox, 0, 0);
		mbox_set_string_align_type(p_mbox, STL_VCENTER | STL_VCENTER);
		/* data len 指纹长度有12位和16位之分，此处直接根据指纹长度设置*/
		for (i = 0; i < p_finger_info->data_len; i++)
		{ 	
			index = p_finger_info->matrix_inner_pos[i];
			data[0] = p_finger_info->data[i];
			mbox_set_content_by_ascstr(p_mbox, ((index&0x0F)*16 + (index>>4)),			
			data);	
		}		
		//ctrl_set_attr(p_mbox, OBJ_STS_SHOW);
  }

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

static RET_CODE on_ca_finger_print_tmout(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    UI_PRINTF("**************Unavailable finger print time out!!! disable  finger display!!! \n");
    if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
    {
      fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
    }
    return SUCCESS;
}

BEGIN_MSGPROC(finger_print_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_ca_finger_print_tmout)
END_MSGPROC(finger_print_proc, cont_class_proc)


