/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_terrestrial_full_search.h"



enum control_id
{
  IDC_FULL_SEARCH_INVALID = 0,
  IDC_FULL_SEARCH_START_FREQUENCY,
  IDC_FULL_SEARCH_END_FREQUENCY,
  IDC_FULL_SEARCH,
  IDC_HELP_CONT,
};

enum full_search_local_msg
{
  MSG_SEARCH = MSG_LOCAL_BEGIN + 238,
};

static u32 channel_min = 0;
static u32 channel_max = 0;


u16 full_search_keymap(u16 key);
RET_CODE full_search_proc(control_t *p_btn, u16 msg, u32 para1, u32 para2);
RET_CODE full_search_frequency_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static BOOL _ui_full_search_c_init(void)
{
#ifdef DTMB_PROJECT_FULL_SEARCH
  u8 full_scan_cnt = 94;
#else
  u8 full_scan_cnt = FULL_SCAN_CNT;
#endif
  dvbs_tp_node_t tp[full_scan_cnt];
  sat_node_t sat_node = {0};
  control_t *p_start_freq = NULL, *p_end_freq = NULL;
  u16 i = 0;
  u32 start_frequency = 0, end_frequency = 0;
  BOOL lcn_enable = FALSE;

  sys_status_get_status(BS_LCN_RECEIVED, &lcn_enable);
  memset(tp, 0, sizeof(dvbs_tp_node_t) * full_scan_cnt);
  //sys_status_get_country_set(&search_info);
  p_start_freq = ui_comm_root_get_ctrl(ROOT_ID_TERRESTRIAL_FULL_SEARCH, IDC_FULL_SEARCH_START_FREQUENCY);
  p_end_freq = ui_comm_root_get_ctrl(ROOT_ID_TERRESTRIAL_FULL_SEARCH, IDC_FULL_SEARCH_END_FREQUENCY);

  start_frequency = ui_comm_numedit_get_num(p_start_freq);
  end_frequency = ui_comm_numedit_get_num(p_end_freq);

  tp[0].freq = channel_min;
  for(i = 1; i < full_scan_cnt; i++)
  {
    if(tp[i-1].freq == 467000)
		tp[i].freq = 474000;
	else
    	tp[i].freq = tp[i-1].freq + 8000;
  }
  
  if(start_frequency <= end_frequency) 
  {
    ui_scan_param_init();
    ui_scan_add_dvbt_sat(FALSE, &sat_node);
    ui_scan_param_add_sat(&sat_node);

    for(i = 0; i < full_scan_cnt; i++)
    {
      //ui_dbase_get_tp_by_pos(g_tp_vid, &tp, i);
      if((tp[i].freq >= start_frequency) && (tp[i].freq <= end_frequency))
      {
        tp[i].sym = DTMB_HN_SYMBOL_DEFAULT;
        tp[i].sat_id = sat_node.id;
        tp[i].nim_type = NIM_DTMB;
        ui_scan_param_add_tp(&tp[i]);
      }
    }
    if(TRUE == lcn_enable)
    {
      ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_ALL_TP);
    }
    else
    {
      ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
    }
  }
  else
  {
     ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);	 
     ui_comm_numedit_set_num(p_start_freq, channel_min);
     ui_comm_numedit_set_num(p_end_freq, channel_max);
     
     ctrl_paint_ctrl(p_start_freq, TRUE);
     ctrl_paint_ctrl(p_end_freq, TRUE);
  }
  
  return TRUE;
}


static RET_CODE _ui_full_search_v_frequency_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_start_freq = NULL, *p_end_freq = NULL;
  u32 start_frequency = 0, end_frequency = 0;

  ui_comm_num_proc(p_ctrl, msg, para1, para2);
  p_ctrl = ctrl_get_parent(p_ctrl);
  p_start_freq = ui_comm_root_get_ctrl(ROOT_ID_TERRESTRIAL_FULL_SEARCH, IDC_FULL_SEARCH_START_FREQUENCY);
  p_end_freq = ui_comm_root_get_ctrl(ROOT_ID_TERRESTRIAL_FULL_SEARCH, IDC_FULL_SEARCH_END_FREQUENCY);

  start_frequency = ui_comm_numedit_get_num(p_start_freq);
  end_frequency = ui_comm_numedit_get_num(p_end_freq);

  if(start_frequency > end_frequency)
  {
     ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);	 
     ui_comm_numedit_set_num(p_start_freq, channel_min);
     ui_comm_numedit_set_num(p_end_freq, channel_max);
     
     ctrl_paint_ctrl(p_start_freq, TRUE);
     ctrl_paint_ctrl(p_end_freq, TRUE);
  }

  return SUCCESS;
}


static RET_CODE _ui_full_search_v_full_search(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{ 
  if(_ui_full_search_c_init())
  {
    manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DTMB_FULL, 0);
  }
  
  return SUCCESS;
}

RET_CODE open_terrestrial_full_search(u32 para1, u32 para2)
{
  control_t *p_cont = NULL, *p_ctrl[FULL_SEARCH_ITEM_CNT] = {NULL};
  u8 i = 0;
  u16 y, stxt[FULL_SEARCH_ITEM_CNT] =
  {
    IDS_FREQUENCY_START, IDS_FREQUENCY_END, IDS_FULL_SCAN
  };

  #ifdef	DTMB_PROJECT_FULL_SEARCH
    channel_min = 115000;
	channel_max = SEARCH_FREQ_MAX;
  #else
	channel_min = SEARCH_FREQ_MIN;
	channel_max = SEARCH_FREQ_MAX;
  #endif


  p_cont = ui_comm_right_root_create(ROOT_ID_TERRESTRIAL_FULL_SEARCH, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, full_search_keymap);
  ctrl_set_proc(p_cont, full_search_proc);

  y = FULL_SEARCH_ITEM_Y;
  for(i = 0; i < FULL_SEARCH_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_FULL_SEARCH_START_FREQUENCY + i),
                                           FULL_SEARCH_ITEM_X, y, 
                                           FULL_SEARCH_ITEM_LW, FULL_SEARCH_ITEM_RW);
		MT_ASSERT(NULL!=p_ctrl[i]);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], full_search_frequency_select_proc);
        ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC, channel_min, channel_max, SEARCH_FREQ_BIT, 0);
		ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MHZ);
		ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
        ui_comm_numedit_set_num(p_ctrl[i], channel_min);
        break;
        
      case 1:
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_FULL_SEARCH_START_FREQUENCY + i),
                                           FULL_SEARCH_ITEM_X, y, 
                                           FULL_SEARCH_ITEM_LW, FULL_SEARCH_ITEM_RW);
		MT_ASSERT(NULL!=p_ctrl[i]);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], full_search_frequency_select_proc);
        ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC, channel_min, channel_max, SEARCH_FREQ_BIT, 0);
		ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MHZ);
		ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
        ui_comm_numedit_set_num(p_ctrl[i], channel_max);
        break;
        
      case 2:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_FULL_SEARCH_START_FREQUENCY + i),
                                    FULL_SEARCH_ITEM_X, y, 
                                    (FULL_SEARCH_ITEM_LW + FULL_SEARCH_ITEM_RW), FULL_SEARCH_ITEM_H, p_cont, 0);
		MT_ASSERT(NULL!=p_ctrl[i]);
        ctrl_set_rstyle(p_ctrl[i], RSI_FULL_SEARCH_BTN_SH, RSI_FULL_SEARCH_BTN_HL, RSI_FULL_SEARCH_BTN_SH);
        text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_BLACK, FSI_WHITE);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_align_type(p_ctrl[i], STL_LEFT | STL_VCENTER);
        text_set_offset(p_ctrl[i], COMM_CTRL_OX, 0);
        text_set_content_by_strid(p_ctrl[i], stxt[i]);
        break;
  
      default:
        break;
     }
    ctrl_set_related_id(p_ctrl[i],
                        0,                                                                /* left */
                        (u8)((i - 1 + FULL_SEARCH_ITEM_CNT) % FULL_SEARCH_ITEM_CNT + 1),  /* up */
                        0,                                                                /* right */
                        (u8)((i + 1) % FULL_SEARCH_ITEM_CNT + 1));                        /* down */

    y += FULL_SEARCH_ITEM_H + FULL_SEARCH_ITEM_V_GAP;
  }
  #ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
  #endif

  /* default focus on first option */
  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


BEGIN_KEYMAP(full_search_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(full_search_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(full_search_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, _ui_full_search_v_full_search)
END_MSGPROC(full_search_proc, ui_comm_root_proc)

BEGIN_MSGPROC(full_search_frequency_select_proc, ui_comm_num_proc)
  //ON_COMMAND(MSG_FOCUS_LEFT, _ui_full_search_v_frequency_change)
  //ON_COMMAND(MSG_FOCUS_RIGHT, _ui_full_search_v_frequency_change)
  ON_COMMAND(MSG_SELECT, _ui_full_search_v_frequency_change)
END_MSGPROC(full_search_frequency_select_proc, ui_comm_num_proc)


