/******************************************************************************/
/******************************************************************************/
/*!
 Include files
 */
#include "ui_common.h"
#include "commonData.h"
#if ENABLE_FILE_PLAY
#include "file_play_api.h"
//#include "ui_onmov_provider.h"
#include "IqyDataProvider.h"
#include "ui_video_iqy.h"
#include "ui_mute.h"
#include "lib_char.h"
#include "sys_status.h"
#include "common_filter.h"
#include "ui_small_list_v2.h"
#include "ui_epg_api.h"
#include "ui_volume.h"
#include "ui_iptv_api.h"
#include "ui_iptv_prot.h"

#include "sys_dbg.h"
#include "iqiyiTV_C.h"
#include "IqiyiLibPorcess.h"

/*!
 * Struct for ui video module
 */
typedef struct
{
    u16 url_cnt;
    u8  **pp_url_list;
} iqy_net_url_t;

typedef union 
{
    iqy_net_url_t net_url;
    u16 *p_file_name;
} iqy_vdo_url_t;

typedef struct
{
    u8  play_func;
    u32 seek_play_time;
    iqy_vdo_url_t vdo_url;
} iqy_vdo_waiting_context_t;

typedef struct
{
    /*!
     * Handle for file play api
     */
    u32                 m_handle;
    /*!
     * Handle for file play api subtitle
     */
    u32                 m_sub_handle;
    /*!
     * Handle for file play api subtitle
     */
    void                 *p_sub_rgn;

    u16                  p_file_name[MAX_FILE_PATH];
    /*!
     * Speed of current play
     */
    mul_fp_play_speed_t m_speed;
    /*!
     * save the play time for continue play second
     */
    u32                 play_time_s;

    u32                 seek_play_time;
    /*!
     * User stop, file will be checked to play continue
     */
    BOOL                usr_stop;

    /*!
     * Init variable
     */
    BOOL                inited;

    //BOOL                b_seek_play;
    /*!
     * Audio track array
     */
    mul_fp_audio_track_t *p_audio_track_arr;

    /*!
     * Current playing insert sub id, if user selects same on, do nothing. 
     */
    u8                   playing_insert_sub_id;

    iqy_vdo_waiting_context_t waiting_context;

    mul_fp_play_state_t   play_state;

}iqy_fp_c_priv;

/*!
 * Globle struct variable
 */
static iqy_fp_c_priv g_fp_iqy_priv = {0};


#define printf_ret(ret)   if(SUCCESS != ret)\
                          {\
                              UI_VIDEO_PRINF("[%s]: ret = %d in %d line\n", \
                                                __FUNCTION__, ret, __LINE__);\
                          }


//#define MAX_COLOR_PALETTE  (8)
//static u32 default_video_subt[MAX_COLOR_PALETTE] = 
//{
//   0x0, //alpha
//   0xFFFFFFFF, //white
//   0xFF0000FF, //red
//   0x000000FF, //black
//   0x00FF00FF, //green
//   0x0000FFFF, //blue
//   0x808080FF, //gray
//};

typedef enum
{
    API_VIDEO_EVENT_EOS = ((APP_FILEPLAY << 16) + 0),
    API_VIDEO_EVENT_UP_TIME,
    API_VIDEO_EVENT_START_BUFFERING,
    API_VIDEO_EVENT_RESOLUTION,
    API_VIDEO_EVENT_UNSUPPORTED_VIDEO,
    API_VIDEO_EVENT_TRICK_TO_BEGIN,
    API_VIDEO_EVENT_UNSUPPORTED_MEMORY,
    API_VIDEO_EVENT_UNSUPPORT_SEEK,
    API_VIDEO_EVENT_LOAD_MEDIA_EXIT,
    API_VIDEO_EVENT_LOAD_MEDIA_ERROR,
    API_VIDEO_EVENT_LOAD_MEDIA_SUCCESS,
    API_VIDEO_EVENT_STOP_CFM, // 12
    API_VIDEO_EVENT_UPDATE_BPS,
    API_VIDEO_EVENT_FINISH_BUFFERING,
    API_VIDEO_EVENT_REQUEST_CHANGE_SRC,
    API_VIDEO_EVENT_SET_PATH_FAIL,
    API_VIDEO_EVENT_LOAD_MEDIA_TIME,
    API_VIDEO_EVENT_FINISH_UPDATE_BPS,
    API_VIDEO_EVENT_MAX,
}fplay_api_evt_t;


u16 iqy_file_play_evtmap(u32 event);
RET_CODE iqy_file_play_evtproc(u32 event, u32 para1, u32 para2);
static int _iqy_video_start(void);
static int iqy_fileplay_callback(mul_fp_play_event_t content, u32 para);
static BOOL iqy_video_get_if_ad();
/*=================================================================================================
                        Video controller internel function
=================================================================================================*/

static void _ui_file_play_set_play_state(iqy_fp_c_priv *priv, mul_fp_play_state_t state)
{
    priv->play_state = state;
}
static mul_fp_play_state_t _ui_file_play_get_play_state(iqy_fp_c_priv *priv)
{
    return priv->play_state;
}

static mul_fp_play_event_t _iqy_file_play_event_map(FILE_PLAY_EVENT_E evt_id)
{
	DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT[%d]\n",evt_id);
	switch(evt_id)
	{
		case FILE_PLAYBACK_SEQ_STOP:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_EOS\n");
			return MUL_PLAY_EVENT_EOS;
		}
		case FILE_PLAYBACK_SEQ_RECEIVE_VIDEO_INFO:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_UP_RESOLUTION\n");			
			return MUL_PLAY_EVENT_UP_RESOLUTION;
		}  
		case FILE_PLAYBACK_SEQ_GET_VPTS:
		{
			//DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_UP_TIME\n");		
			return MUL_PLAY_EVENT_UP_TIME;
		}
		case FILE_PLAYBACK_UNSUPPORT_VIDEO:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_UNSUPPORTED_VIDEO\n");
			return MUL_PLAY_EVENT_UNSUPPORTED_VIDEO;
		}
		case FILE_PLAYBACK_UNSUPPORT_AUDIO:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_UNSUPPORTED_AUDIO\n");
			return MUL_PLAY_EVENT_UNSUPPORTED_AUDIO;
		}
		case FILE_PLAYBACK_CHECK_TRICKPLAY:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_TRICK_TO_BEGIN\n");
			return MUL_PLAY_EVENT_TRICK_TO_BEGIN;
		}
		case FILE_PLAYBACK_UNSUPPORT_MEMORY:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_UNSUPPORT_MEMORY\n");
			return MUL_PLAY_EVENT_UNSUPPORT_MEMORY;
		}
		case FILE_PLAYBACK_NEW_SUB_DATA_RECEIVE:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_NEW_SUB_DATA_RECEIVE\n");
			return MUL_PLAY_EVENT_NEW_SUB_DATA_RECEIVE;
		}
		case FILE_PLAYBACK_UNSUPPORT_SEEK:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_EOS\n");
			return MUL_PLAY_EVENT_UNSUPPORT_SEEK;
		}
		case FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_LOAD_MEDIA_EXIT\n");
			return MUL_PLAY_EVENT_LOAD_MEDIA_EXIT;
		}
		case FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_LOAD_MEDIA_ERROR\n");
			return MUL_PLAY_EVENT_LOAD_MEDIA_ERROR;
		}
		case FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_LOAD_MEDIA_SUCCESS\n");
			return MUL_PLAY_EVENT_LOAD_MEDIA_SUCCESS;
		}
		case FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_FILE_ES_TASK_EXIT\n");
			return MUL_PLAY_EVENT_FILE_ES_TASK_EXIT;
		}
		case FILE_PLAYBACK_SEQ_START_BUFFERING:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_START_BUFFERING\n");
			return MUL_PLAY_EVENT_START_BUFFERING;
		}  
		case FILE_PLAYBACK_SEQ_FINISH_BUFFERING:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_FINISH_BUFFERING\n");
			return MUL_PLAY_EVENT_FINISH_BUFFERING;
		}  
		case FILE_PLAYBACK_SEQ_UPDATE_BPS:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_UPDATE_BPS\n");
			return MUL_PLAY_EVENT_UPDATE_BPS;
		}
		case FILE_PLAYBACK_SEQ_REQUEST_CHANGE_SRC:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_REQUEST_CHANGE_SRC\n");
			return MUL_PLAY_EVENT_REQUEST_CHANGE_SRC;
		}
		case FILE_PLAYBACK_SEQ_SET_PATH_FAIL:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_SET_PATH_FAIL\n");
			return MUL_PLAY_EVENT_SET_PATH_FAIL;
		}  
		case FILE_PLAYBACK_SEQ_LOAD_MEDIA_TIME:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_LOAD_MEDIA_TIME\n");
			return MUL_PLAY_EVENT_LOAD_MEDIA_TIME;
		}  
		case FILE_PLAYBACK_HD_UNSUPPORT:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_UNSUPPORTED_HD_VIDEO\n");
			return MUL_PLAY_EVENT_UNSUPPORTED_HD_VIDEO;
		}
#ifdef WARRIORS_CHIP
		case FILE_PLAYBACK_SEQ_SWITCH_TV_SYS:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_SWITCH_TV_SYS\n");
			return MUL_PLAY_EVENT_SWITCH_TV_SYS;
		}
#endif
		case FILE_PLAYBACK_FINISH_UPDATE_BPS:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_FINISH_UPDATE_BPS\n");
			return MUL_PLAY_EVENT_FINISH_UPDATE_BPS;
		}  
		default:
		{
			DEBUG(IPLAY,INFO,"MUL_PLAY_EVENT_MAX\n");
			return MUL_PLAY_EVENT_MAX;
		}
	}
}


static void _iqy_file_play_callback(FILE_PLAY_EVENT_E evt_id, u32 param)
{
	iqy_fileplay_callback(_iqy_file_play_event_map(evt_id), param);
}

void _iqy_video_utf82uni(u8 *in, u16 *out, u16 out_size)
{
    u16 index = 0;
    //MT_ASSERT(out_size >= strlen(in));
    DEBUG(IPLAY, INFO, "\n");
    while(*in && index <= out_size)
    {
        if (*in >= 0x00 && *in <= 0x7f)
        {
            *out = (u16) *in;
        }
        else if ((*in & (0xff << 5)) == 0xc0)
        {
            u16 unicode = 0;

            unicode = (*in & 0x1F) << 6; 
            in++;
            
            unicode |= (*in)  & 0x3F; 

            *out = unicode;
        }
        else if ((*in & (0xff << 4))== 0xe0)
        {
            u16 unicode = 0;

            unicode = ((*in) & 0x1F) << 12; 
            in++;

            unicode |= ((*in) & 0x3F) << 6; 
            in++;

            unicode |= ((*in) & 0x3F); 

            *out = unicode;
        }

        in++;
        out++;
        index ++;
    }

    out = 0;
}
void _iqy_video_dump_utf8(u8 *utf8)
{
    u8 *p = utf8;
    DEBUG(IPLAY, INFO, "\n");
    UI_VIDEO_PRINF("[%s]:begin\n UTF8:", __FUNCTION__);
    while(*p)
    {
        UI_VIDEO_PRINF("[%02X]", *p);
        p++;
    }
    
    UI_VIDEO_PRINF("\n");
}

void _iqy_video_dump_unicode(u16 *p_uni)
{
    u8 *p = (u8 *)p_uni;
    int i = 0;
    DEBUG(IPLAY, INFO, "\n");
    UI_VIDEO_PRINF("[%s]:begin len:%d \n UNI:", __FUNCTION__, uni_strlen(p_uni));
    for (i = 0; i < uni_strlen(p_uni); i++)
    {
        UI_VIDEO_PRINF("[%02X]", *p);
        p++;
    }
    
    UI_VIDEO_PRINF("\n");
}
/*=================================================================================================
                        Video controller interface function
=================================================================================================*/

static int iqy_fileplay_callback(mul_fp_play_event_t content, u32 para)
{
  event_t evt = {0};

  if  (iqy_video_get_if_ad())
  {
    switch(content)
    {
      default:
      case MUL_PLAY_EVENT_EOS:
      case MUL_PLAY_EVENT_UP_TIME:
      case MUL_PLAY_EVENT_START_BUFFERING:
      case MUL_PLAY_EVENT_UP_RESOLUTION:
      case MUL_PLAY_EVENT_UPDATE_BPS:
      case MUL_PLAY_EVENT_FINISH_BUFFERING:
      case MUL_PLAY_EVENT_REQUEST_CHANGE_SRC:
      case MUL_PLAY_EVENT_LOAD_MEDIA_TIME:
      case MUL_PLAY_EVENT_FINISH_UPDATE_BPS:
      case MUL_PLAY_EVENT_LOAD_MEDIA_ERROR:
      case MUL_PLAY_EVENT_SET_PATH_FAIL:	  	
	  	return SUCCESS;
      case MUL_PLAY_EVENT_UNSUPPORT_MEMORY:
      case MUL_PLAY_EVENT_UNSUPPORTED_VIDEO:
      case MUL_PLAY_EVENT_TRICK_TO_BEGIN:
      case MUL_PLAY_EVENT_LOAD_MEDIA_EXIT:
      case MUL_PLAY_EVENT_LOAD_MEDIA_SUCCESS:
      case MUL_PLAY_EVENT_FILE_ES_TASK_EXIT:
	  	break;
    }
  }


	
  switch(content)
  {
    case MUL_PLAY_EVENT_EOS:
    {
        evt.id = API_VIDEO_EVENT_EOS;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UP_TIME:
    {
        evt.id = API_VIDEO_EVENT_UP_TIME;
        evt.data1 = para;
        g_fp_iqy_priv.play_time_s = para / 1000;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_START_BUFFERING:
    {
        evt.id = API_VIDEO_EVENT_START_BUFFERING;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UP_RESOLUTION:
    {
        evt.id = API_VIDEO_EVENT_RESOLUTION;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UNSUPPORT_MEMORY:
    {
        evt.id = API_VIDEO_EVENT_UNSUPPORTED_MEMORY;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UNSUPPORTED_VIDEO:
    {
        evt.id = API_VIDEO_EVENT_UNSUPPORTED_VIDEO;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_TRICK_TO_BEGIN:
    {
        evt.id = API_VIDEO_EVENT_TRICK_TO_BEGIN;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
/*    case MUL_PLAY_EVENT_UNSUPPORT_SEEK:
    {
        evt.id = API_VIDEO_EVENT_UNSUPPORT_SEEK;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }*/
    case MUL_PLAY_EVENT_LOAD_MEDIA_EXIT:
    {
        evt.id = API_VIDEO_EVENT_LOAD_MEDIA_EXIT;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_LOAD_MEDIA_ERROR:
    {
        evt.id = API_VIDEO_EVENT_LOAD_MEDIA_ERROR;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_LOAD_MEDIA_SUCCESS:
    {
        evt.id = API_VIDEO_EVENT_LOAD_MEDIA_SUCCESS;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_FILE_ES_TASK_EXIT:
    {
        evt.id = API_VIDEO_EVENT_STOP_CFM;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UPDATE_BPS:
    {
        evt.id = API_VIDEO_EVENT_UPDATE_BPS;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_FINISH_BUFFERING:
    {
        evt.id = API_VIDEO_EVENT_FINISH_BUFFERING;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }    
    case MUL_PLAY_EVENT_REQUEST_CHANGE_SRC:
    {
        evt.id = API_VIDEO_EVENT_REQUEST_CHANGE_SRC;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_SET_PATH_FAIL:
    {
        evt.id = API_VIDEO_EVENT_SET_PATH_FAIL;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_LOAD_MEDIA_TIME:
    {
        evt.id = API_VIDEO_EVENT_LOAD_MEDIA_TIME;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_FINISH_UPDATE_BPS:
    {
        evt.id = API_VIDEO_EVENT_FINISH_UPDATE_BPS;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }    
    default:
    {
        break;
    }
  }
  return SUCCESS;

}



static void iqy_video_reset_speed(void)
{
    DEBUG(IPLAY, INFO, "\n");

    g_fp_iqy_priv.m_speed = MUL_PLAY_NORMAL_PLAY;
}

/*!
 * Get current play time
 */
static u32 iqy_video_get_play_time(void)
{
    DEBUG(IPLAY, INFO, "\n");

    return g_fp_iqy_priv.play_time_s;
}

/*!
 * Get current play state
 */
static int iqy_video_get_play_state(void)
{
	mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;
 	state = _ui_file_play_get_play_state(&g_fp_iqy_priv);
	DEBUG(IPLAY, INFO,"play status = %d\n", state);

	return (int)state;
}

/*
void iqy_video_switch_time(video_play_time_t *p_time, u32 sec)
{
    p_time->hour = (u8)(sec / 3600);
    p_time->min = (u8)((sec - p_time->hour * 3600) / 60);
    p_time->sec = (u8)(sec - p_time->hour * 3600 - p_time->min * 60);
}
*/
/*!
 * Get total time of video
 */
static u32 iqy_video_get_total_time_by_sec(void)
{
	u32                 seconds     = 0;
	IQY_CurrentPlayingVideoInfo v_info;
	int ret;
	DEBUG(IPLAY, INFO, "\n");
	ret = iqy_getCurrentPlayingVideoInfo(&v_info);
	if (ret != IQY_NORMAL)
	{
		DEBUG(IPLAY,ERR,"iqy_getCurrentPlayingVideoInfo failure!err[%d]\n",ret);
		return 0;
	}
	seconds = v_info.duration/1000 - 2;//leave 2s, for disable seek to eos
	DEBUG(IPLAY, INFO, "seconds[%d]\n",seconds);
	
	return seconds;
}

/*!
 * Get total ad time of video
 */
static u32 iqy_video_get_total_ad_time_by_sec(void)
{
	u32                 seconds     = 0;
	IQY_CurrentPlayingVideoInfo v_info;
	int ret;
	int i;
	DEBUG(IPLAY, INFO, "\n");
	ret = iqy_getCurrentPlayingVideoInfo(&v_info);
	if (ret != IQY_NORMAL)
	{
		DEBUG(IPLAY,ERR,"iqy_getCurrentPlayingVideoInfo failure!err[%d]\n",ret);
		return 0;
	}
	for (i=0;i<v_info.adDurationListSize;i++)
	{
		seconds += v_info.adDurationList[i]/1000 +3;
	}
	DEBUG(IPLAY, INFO, "seconds[%d]\n",seconds);
	
	return seconds;
}



/*!
 * Get resolution of video
 */
static void iqy_video_get_resolution(rect_size_t *p_rect)
{
	IQY_CurrentPlayingVideoInfo v_info;
	int ret;
	DEBUG(IPLAY, INFO, "\n");
	ret = iqy_getCurrentPlayingVideoInfo(&v_info);
	if (ret != IQY_NORMAL)
	{
		DEBUG(IPLAY,ERR,"iqy_getCurrentPlayingVideoInfo failure!err[%d]\n",ret);
		return;
	}
	p_rect->w = v_info.videoWidth/1000;
	p_rect->h = v_info.videoHeight;
	DEBUG(IPLAY, INFO, "resolution[%dX%d]\n",p_rect->w,p_rect->h);
}

/*!
 * Get vd of video
 */
static int iqy_video_get_current_format(void)
{
	IQY_CurrentPlayingVideoInfo v_info;
	int ret;
	DEBUG(IPLAY, INFO, "\n");
	ret = iqy_getCurrentPlayingVideoInfo(&v_info);
	if (ret != IQY_NORMAL)
	{
		DEBUG(IPLAY,ERR,"iqy_getCurrentPlayingVideoInfo failure!err[%d]\n",ret);
		return 0;
	}
	DEBUG(IPLAY, INFO, "vd[%d]\n",v_info.vd);	
	return v_info.vd;
}




static int iqy_video_seek(s32 time_sec)
{
	int ret;
	DEBUG(IPLAY, INFO, "time_sec[%d]\n",time_sec);
	ret =  iqy_seek(time_sec*1000);
	if (ret != IQY_NORMAL)
	{
		DEBUG(IPLAY,ERR,"iqy_seek failure!err [%d]\n",ret);
		return ERR_FAILURE;
	}
	return SUCCESS;
}

/*!
 * Pause or resume playing
 */
static int iqy_video_pause_resume(void)
{
	int ret;
	mul_fp_play_state_t _state;

	_state = iqy_video_get_play_state();	
	DEBUG(IPLAY,INFO,"pause status[%d]\n",_state);
	if (_state == MUL_PLAY_STATE_PLAY)
	{
		ret = iqy_pause();
		if (ret != IQY_NORMAL)
		{
			DEBUG(IPLAY,ERR,"iqy_pause failure err[%d]\r\n",ret);
			return ERR_FAILURE;
		}
		_ui_file_play_set_play_state(&g_fp_iqy_priv, MUL_PLAY_STATE_PAUSE);
	}
	else if (_state == MUL_PLAY_STATE_PAUSE)
	{
		ret = iqy_resume();
		if (ret != IQY_NORMAL)
		{
			DEBUG(IPLAY,ERR,"iqy_pause failure err[%d]\r\n",ret);
			return ERR_FAILURE;
		}
		_ui_file_play_set_play_state(&g_fp_iqy_priv, MUL_PLAY_STATE_PLAY);
	}
	else
	{
		return -1;
	}
	return SUCCESS;	
}

static void iqy_video_set_usr_stop_flag(BOOL flag)
{
    DEBUG(IPLAY, INFO, "\n");

  g_fp_iqy_priv.usr_stop = flag;
}



/*!
 * Stop playing
 */
static int iqy_video_stop(void)
{
	int ret;
	mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;

	state =	_ui_file_play_get_play_state(&g_fp_iqy_priv);
	DEBUG(IPLAY, INFO, "state = %d \n",state);	
	if((state != MUL_PLAY_STATE_NONE)&&(state != MUL_PLAY_STATE_STOP))
	{
		ret = iqy_stop();
		
		if (ret != IQY_NORMAL)
		{
			DEBUG(IPLAY,ERR,"iqy_stop failure!err[%d]\n",ret);
			return ERR_FAILURE;
		}
	}

	_ui_file_play_set_play_state(&g_fp_iqy_priv, MUL_PLAY_STATE_STOP);
	iqy_video_set_usr_stop_flag(TRUE);
	return SUCCESS;
 }





/*iqy need*/
static void iqy_player_event_callback(int event)
{
	DEBUG(IPLAY, INFO, "EVENT[%d]\n",event); 
}
static int iqy_video_play_by_url_ex(video_url_info_t *pp_url_list, 
												u32 url_cnt,
												u32 seek_play_time,
												int vd)
{
	int ret = SUCCESS;
	IQY_PlaySetup iqy_player;
	VDO_ID_t *id;

	MT_ASSERT(NULL != pp_url_list);
	seek_play_time *= 1000;
	DEBUG(IPLAY, INFO, "URL tvQid[%s], vid[%s],vd[%d]\n",pp_url_list[0].tvQid,pp_url_list[0].vid,vd);
	DEBUG(IPLAY, INFO, "seek time[%d] play mode[%d]\n",seek_play_time);
	if (MUL_PLAY_STATE_PLAY == _ui_file_play_get_play_state(&g_fp_iqy_priv))
	{
		iqy_switchVd(vd, seek_play_time);
	}
	else
	{
		memset(&iqy_player,0,sizeof(iqy_player));
		iqy_player.error = iqy_player_event_callback;
		memcpy(iqy_player.tvQid ,pp_url_list[0].tvQid,sizeof(iqy_player.tvQid));
		memcpy(iqy_player.vid ,pp_url_list[0].vid,sizeof(iqy_player.vid));
		iqy_player.startTime = seek_play_time;
		iqy_player.vd = vd;
		iqy_player.playstate = (playstate_callback)_iqy_file_play_callback;
		iqy_player.info.chnId	= iqy_get_current_channel_id();
             if(ui_iptv_get_flag_to_plid())
		    memcpy(iqy_player.info.plid,iqy_get_current_plid(),sizeof(iqy_player.info.plid));
		id = iqy_get_current_video_id();
		memcpy(iqy_player.info.qpid,id->qpId,sizeof(iqy_player.info.qpid));
		iqy_player.info.tvepin	= iqy_get_current_episode_num();
		DEBUG(IPLAY, INFO, "CollectingInfo chnid[%d],plid[%s],qpid[%s],tvepin[%d]\n",
			iqy_player.info.chnId,iqy_player.info.plid,iqy_player.info.qpid,iqy_player.info.tvepin);
		
		iqy_player.info.resulotion[0] ='\0';
		iqy_start(&iqy_player);
	}
	_ui_file_play_set_play_state(&g_fp_iqy_priv, MUL_PLAY_STATE_LOAD_MEDIA);	
	return ret;
}

/*iqy need*/
static int iqy_video_play_by_url(video_url_info_t *p_net_url, u32 seek_play_time,int vd)
{
    DEBUG(IPLAY, INFO, "\n");
    return iqy_video_play_by_url_ex(p_net_url, 1, seek_play_time, vd);
}

static int _iqy_video_start(void)
{
    int ret           = SUCCESS;
    DEBUG(IPLAY, INFO, "\n");

    g_fp_iqy_priv.play_time_s = g_fp_iqy_priv.seek_play_time;
    UI_VIDEO_PRINF("@@@_iqy_video_start play_time_s=%d\n", g_fp_iqy_priv.play_time_s);

    set_volume(sys_status_get_global_volume());
    iqy_video_set_usr_stop_flag(FALSE);
    ui_set_mute(ui_is_mute());

    /*If in play state, set speed as normal*/
    iqy_video_reset_speed();
    _ui_file_play_set_play_state(&g_fp_iqy_priv, MUL_PLAY_STATE_PLAY);	
    return ret;
}

static BOOL iqy_video_get_usr_stop_flag(void)
{
    DEBUG(IPLAY, INFO, "\n");

  return g_fp_iqy_priv.usr_stop;
}

static BOOL iqy_video_init_state(void)
{
  DEBUG(IPLAY, INFO, "\n");

  return g_fp_iqy_priv.inited;
}



/*!
 * Destroy file play module
 */
static int iqy_video_destroy(void)
{
	UI_VIDEO_PRINF("[%s]: start\n", __FUNCTION__);
	DEBUG(IPLAY, INFO, "\n");

	if(g_fp_iqy_priv.inited)
	{
		g_fp_iqy_priv.m_handle = 0;
		g_fp_iqy_priv.inited = FALSE;
		g_fp_iqy_priv.play_time_s = 0;
		fw_unregister_ap_evtmap(APP_FILEPLAY);
		fw_unregister_ap_evtproc(APP_FILEPLAY);
		fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_USB_FILEPLAY);
		fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_FILEPLAY_BAR);
		fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_VIDEO_PLAYER); 
		fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_MAINMENU);
		fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_SMALL_LIST);
		fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_PROG_BAR);
		fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_BACKGROUND);
		fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_LIVE_TV);
		fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_IPTV_PLAYER);
	}

	ui_epg_init();
#ifndef IMPL_NEW_EPG
	ui_epg_start();
#else
	ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif

	return SUCCESS;
}
/*!
 * Create file play module
 */
static int iqy_video_create(u8 play_func)
{
	int      ret  = SUCCESS;
	//mul_fp_attr_t attr = {0};
	//av_set_t      av_set = {0};

	(void)iqy_fileplay_callback;
	(void)iqy_video_get_usr_stop_flag;
	(void)iqy_video_init_state;
	DEBUG(IPLAY, INFO, "\n");
	
	ui_epg_stop();
	ui_epg_release();

	if(g_fp_iqy_priv.inited)
	{
		return SUCCESS;
	}
	//call back

	fw_register_ap_evtmap(APP_FILEPLAY, iqy_file_play_evtmap);
	fw_register_ap_evtproc(APP_FILEPLAY, iqy_file_play_evtproc);
	fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_VIDEO_PLAYER); 	  
	fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_MAINMENU);
	fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_SMALL_LIST);
	fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_PROG_BAR);
	fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_BACKGROUND);
	fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_LIVE_TV);
	fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_IPTV_PLAYER);
	g_fp_iqy_priv.inited = TRUE;
	return ret;
}




static RET_CODE iqy_video_on_loadmedia_error(u32 event, u32 para1, u32 para2)
{
    DEBUG(IPLAY, INFO, "\n");

    _ui_file_play_set_play_state(&g_fp_iqy_priv, MUL_PLAY_STATE_NONE);
    return ERR_NOFEATURE;
}

static RET_CODE iqy_video_on_loadmedia_success(u32 event, u32 para1, u32 para2)
{
    DEBUG(IPLAY, INFO, "\n");

    _iqy_video_start();
    return ERR_NOFEATURE;
}

RET_CODE iqy_video_on_loadmedia_exit(u32 event, u32 para1, u32 para2)
{
    DEBUG(IPLAY, INFO, "\n");

    _ui_file_play_set_play_state(&g_fp_iqy_priv, MUL_PLAY_STATE_NONE);
    return ERR_NOFEATURE;
}

static RET_CODE iqy_video_on_stop_cfm(u32 event, u32 para1, u32 para2)
{
    DEBUG(IPLAY, INFO, "\n");

      _ui_file_play_set_play_state(&g_fp_iqy_priv, MUL_PLAY_STATE_NONE);
    return ERR_NOFEATURE;
}

static RET_CODE iqy_video_on_eos(u32 event, u32 para1, u32 para2)
{
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;
    DEBUG(IPLAY, INFO, "\n");

    state = _ui_file_play_get_play_state(&g_fp_iqy_priv);

      _ui_file_play_set_play_state(&g_fp_iqy_priv, MUL_PLAY_STATE_NONE);
    if (state == MUL_PLAY_STATE_STOP)
    {
        return SUCCESS;
    }
    else
    {
        return ERR_NOFEATURE;
    }
}

static BOOL iqy_video_get_if_ad()
{
	int status;
	status = iqy_getCurrentStatus();
	if (status == TYPE_STATUS_ADVERTISING)
		return TRUE;
	
	return FALSE;
}

static VodPlayerInterface_t iqyPlayerInterface;

const VodPlayerInterface_t* GetIqyPlayerInterface(void)
{
	if (!iqyPlayerInterface.create)
	{
		iqyPlayerInterface.create 		= iqy_video_create;
		iqyPlayerInterface.destory 		= iqy_video_destroy;
		iqyPlayerInterface.getCurrentFormat = iqy_video_get_current_format;
		iqyPlayerInterface.getCurrentPlayTime = iqy_video_get_play_time;
		iqyPlayerInterface.getPlayState 	= iqy_video_get_play_state;
		iqyPlayerInterface.getResolution	= iqy_video_get_resolution;
		iqyPlayerInterface.getTotalAdTime = iqy_video_get_total_ad_time_by_sec;
		iqyPlayerInterface.getTotalTimeBySec = iqy_video_get_total_time_by_sec;
		iqyPlayerInterface.ifPlayingAd   	= iqy_video_get_if_ad;
		iqyPlayerInterface.pause_resume	 = iqy_video_pause_resume;
		iqyPlayerInterface.playUrl 		= iqy_video_play_by_url;
		iqyPlayerInterface.playUrlEx		= iqy_video_play_by_url_ex;
		iqyPlayerInterface.seek		= iqy_video_seek;
		iqyPlayerInterface.videoStop	= iqy_video_stop;
	}
	return &iqyPlayerInterface;
}

BEGIN_AP_EVTMAP(iqy_file_play_evtmap)
CONVERT_EVENT(API_VIDEO_EVENT_EOS, MSG_VIDEO_EVENT_EOS)
CONVERT_EVENT(API_VIDEO_EVENT_UP_TIME, MSG_VIDEO_EVENT_UP_TIME)
CONVERT_EVENT(API_VIDEO_EVENT_START_BUFFERING, MSG_VIDEO_EVENT_START_BUFFERING)
CONVERT_EVENT(API_VIDEO_EVENT_RESOLUTION, MSG_VIDEO_EVENT_UP_RESOLUTION)
CONVERT_EVENT(API_VIDEO_EVENT_UNSUPPORTED_VIDEO, MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO)
CONVERT_EVENT(API_VIDEO_EVENT_UNSUPPORTED_MEMORY, MSG_VIDEO_EVENT_UNSUPPORTED_MEMORY)
CONVERT_EVENT(API_VIDEO_EVENT_TRICK_TO_BEGIN, MSG_VIDEO_EVENT_TRICK_TO_BEGIN)
CONVERT_EVENT(API_VIDEO_EVENT_UNSUPPORT_SEEK, MSG_VIDEO_EVENT_UNSUPPORT_SEEK)
CONVERT_EVENT(API_VIDEO_EVENT_LOAD_MEDIA_EXIT, MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT)
CONVERT_EVENT(API_VIDEO_EVENT_LOAD_MEDIA_ERROR, MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR)
CONVERT_EVENT(API_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS)
CONVERT_EVENT(API_VIDEO_EVENT_STOP_CFM, MSG_VIDEO_EVENT_STOP_CFM)
CONVERT_EVENT(API_VIDEO_EVENT_UPDATE_BPS, MSG_VIDEO_EVENT_UPDATE_BPS)
CONVERT_EVENT(API_VIDEO_EVENT_REQUEST_CHANGE_SRC, MSG_VIDEO_EVENT_REQUEST_CHANGE_SRC)
CONVERT_EVENT(API_VIDEO_EVENT_SET_PATH_FAIL, MSG_VIDEO_EVENT_SET_PATH_FAIL)
CONVERT_EVENT(API_VIDEO_EVENT_LOAD_MEDIA_TIME, MSG_VIDEO_EVENT_LOAD_MEDIA_TIME)
CONVERT_EVENT(API_VIDEO_EVENT_FINISH_BUFFERING, MSG_VIDEO_EVENT_FINISH_BUFFERING)
CONVERT_EVENT(API_VIDEO_EVENT_FINISH_UPDATE_BPS, MSG_VIDEO_EVENT_FINISH_UPDATE_BPS)

END_AP_EVTMAP(iqy_file_play_evtmap)

BEGIN_AP_EVTPROC(iqy_file_play_evtproc)
ON_EVENTPROC(API_VIDEO_EVENT_LOAD_MEDIA_ERROR, iqy_video_on_loadmedia_error)
ON_EVENTPROC(API_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, iqy_video_on_loadmedia_success)
ON_EVENTPROC(API_VIDEO_EVENT_LOAD_MEDIA_EXIT, iqy_video_on_loadmedia_exit)
ON_EVENTPROC(API_VIDEO_EVENT_STOP_CFM, iqy_video_on_stop_cfm)
ON_EVENTPROC(API_VIDEO_EVENT_EOS, iqy_video_on_eos)
ON_EVENTPROC(API_VIDEO_EVENT_SET_PATH_FAIL,iqy_video_on_loadmedia_error)
END_AP_EVTPROC(iqy_file_play_evtproc)
#endif

