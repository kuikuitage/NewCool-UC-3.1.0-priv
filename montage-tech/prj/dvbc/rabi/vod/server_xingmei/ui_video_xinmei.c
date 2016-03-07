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
#include "xingmeiDataProvider.h"
#include "ui_video_xinmei.h"
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
#include "xmTV.h"
#include "xingmeiDataPorcess.h"

/*!
 * Struct for ui video module
 */
typedef struct
{
    u16 url_cnt;
    u8  **pp_url_list;
} xm_net_url_t;

typedef union 
{
    xm_net_url_t net_url;
    u16 *p_file_name;
} xm_vdo_url_t;

typedef struct
{
    u8  play_func;
    u32 seek_play_time;
    xm_vdo_url_t vdo_url;
} xm_vdo_waiting_context_t;

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

    xm_vdo_waiting_context_t waiting_context;

    mul_fp_play_state_t   play_state;

}xm_fp_c_priv;

/*!
 * Globle struct variable
 */
static xm_fp_c_priv g_fp_xm_priv = {0};


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


u16 xm_file_play_evtmap(u32 event);
RET_CODE xm_file_play_evtproc(u32 event, u32 para1, u32 para2);
static int _xm_video_start(void);
static int xm_fileplay_callback(mul_fp_play_event_t content, u32 para);
static BOOL xm_video_get_if_ad();
/*=================================================================================================
                        Video controller internel function
=================================================================================================*/

static void _ui_file_play_set_play_state(xm_fp_c_priv *priv, mul_fp_play_state_t state)
{
    priv->play_state = state;
}
static mul_fp_play_state_t _ui_file_play_get_play_state(xm_fp_c_priv *priv)
{
    return priv->play_state;
}

static mul_fp_play_event_t _xm_file_play_event_map(FILE_PLAY_EVENT_E evt_id)
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


static void _xm_file_play_callback(FILE_PLAY_EVENT_E evt_id, u32 param)
{
	xm_fileplay_callback(_xm_file_play_event_map(evt_id), param);
}

void _xm_video_utf82uni(u8 *in, u16 *out, u16 out_size)
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
void _xm_video_dump_utf8(u8 *utf8)
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

void _xm_video_dump_unicode(u16 *p_uni)
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

static int xm_fileplay_callback(mul_fp_play_event_t content, u32 para)
{
  event_t evt = {0};

  if  (xm_video_get_if_ad())
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
        g_fp_xm_priv.play_time_s = para / 1000;
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



static void xm_video_reset_speed(void)
{
    DEBUG(IPLAY, INFO, "\n");

    g_fp_xm_priv.m_speed = MUL_PLAY_NORMAL_PLAY;
}

/*!
 * Get current play time
 */
static u32 xm_video_get_play_time(void)
{
	return 0;
}

/*!
 * Get current play state
 */
static int xm_video_get_play_state(void)
{
	return 0;
}

/*
void xm_video_switch_time(video_play_time_t *p_time, u32 sec)
{
    p_time->hour = (u8)(sec / 3600);
    p_time->min = (u8)((sec - p_time->hour * 3600) / 60);
    p_time->sec = (u8)(sec - p_time->hour * 3600 - p_time->min * 60);
}
*/
/*!
 * Get total time of video
 */
static u32 xm_video_get_total_time_by_sec(void)
{
	return 0;
}

/*!
 * Get total ad time of video
 */
static u32 xm_video_get_total_ad_time_by_sec(void)
{
	return 0;
}



/*!
 * Get resolution of video
 */
static void xm_video_get_resolution(rect_size_t *p_rect)
{
	return;
}

/*!
 * Get vd of video
 */
static int xm_video_get_current_format(void)
{
	return 0;
}




static int xm_video_seek(s32 time_sec)
{
	return 0;
}

/*!
 * Pause or resume playing
 */
static int xm_video_pause_resume(void)
{
	return 0;
}

static void xm_video_set_usr_stop_flag(BOOL flag)
{
    DEBUG(IPLAY, INFO, "\n");

  g_fp_xm_priv.usr_stop = flag;
}



/*!
 * Stop playing
 */
static int xm_video_stop(void)
{
	return 0;
}





/*iqy need*/
static void xm_player_event_callback(int event)
{
	DEBUG(IPLAY, INFO, "EVENT[%d]\n",event); 
}
static int xm_video_play_by_url_ex(video_url_info_t *pp_url_list, 
												u32 url_cnt,
												u32 seek_play_time,
												int vd)
{
	return 0;
}

/*iqy need*/
static int xm_video_play_by_url(video_url_info_t *p_net_url, u32 seek_play_time,int vd)
{
    DEBUG(IPLAY, INFO, "\n");
    return xm_video_play_by_url_ex(p_net_url, 1, seek_play_time, vd);
}

static int _xm_video_start(void)
{
    int ret           = SUCCESS;
    DEBUG(IPLAY, INFO, "\n");

    g_fp_xm_priv.play_time_s = g_fp_xm_priv.seek_play_time;
    UI_VIDEO_PRINF("@@@_xm_video_start play_time_s=%d\n", g_fp_xm_priv.play_time_s);

    set_volume(sys_status_get_global_volume());
    xm_video_set_usr_stop_flag(FALSE);
    ui_set_mute(ui_is_mute());

    /*If in play state, set speed as normal*/
    xm_video_reset_speed();
    _ui_file_play_set_play_state(&g_fp_xm_priv, MUL_PLAY_STATE_PLAY);	
    return ret;
}

static BOOL xm_video_get_usr_stop_flag(void)
{
    DEBUG(IPLAY, INFO, "\n");

  return g_fp_xm_priv.usr_stop;
}

static BOOL xm_video_init_state(void)
{
  DEBUG(IPLAY, INFO, "\n");

  return g_fp_xm_priv.inited;
}



/*!
 * Destroy file play module
 */
static int xm_video_destroy(void)
{
	return 0;
}
/*!
 * Create file play module
 */
static int xm_video_create(u8 play_func)
{
	return 0;
}




static RET_CODE xm_video_on_loadmedia_error(u32 event, u32 para1, u32 para2)
{
    DEBUG(IPLAY, INFO, "\n");

    _ui_file_play_set_play_state(&g_fp_xm_priv, MUL_PLAY_STATE_NONE);
    return ERR_NOFEATURE;
}

static RET_CODE xm_video_on_loadmedia_success(u32 event, u32 para1, u32 para2)
{
    DEBUG(IPLAY, INFO, "\n");

    _xm_video_start();
    return ERR_NOFEATURE;
}

RET_CODE xm_video_on_loadmedia_exit(u32 event, u32 para1, u32 para2)
{
    DEBUG(IPLAY, INFO, "\n");

    _ui_file_play_set_play_state(&g_fp_xm_priv, MUL_PLAY_STATE_NONE);
    return ERR_NOFEATURE;
}

static RET_CODE xm_video_on_stop_cfm(u32 event, u32 para1, u32 para2)
{
    DEBUG(IPLAY, INFO, "\n");

      _ui_file_play_set_play_state(&g_fp_xm_priv, MUL_PLAY_STATE_NONE);
    return ERR_NOFEATURE;
}

static RET_CODE xm_video_on_eos(u32 event, u32 para1, u32 para2)
{
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;
    DEBUG(IPLAY, INFO, "\n");

    state = _ui_file_play_get_play_state(&g_fp_xm_priv);

      _ui_file_play_set_play_state(&g_fp_xm_priv, MUL_PLAY_STATE_NONE);
    if (state == MUL_PLAY_STATE_STOP)
    {
        return SUCCESS;
    }
    else
    {
        return ERR_NOFEATURE;
    }
}

static BOOL xm_video_get_if_ad()
{
	return 0;
}

static VodPlayerInterface_t iqyPlayerInterface;

const VodPlayerInterface_t* GetXingMeiPlayerInterface(void)
{
	(void)_xm_file_play_callback;
	(void)xm_player_event_callback;
	(void)xm_video_get_usr_stop_flag;
	(void)xm_video_init_state;
	if (!iqyPlayerInterface.create)
	{
		iqyPlayerInterface.create 		= xm_video_create;
		iqyPlayerInterface.destory 		= xm_video_destroy;
		iqyPlayerInterface.getCurrentFormat = xm_video_get_current_format;
		iqyPlayerInterface.getCurrentPlayTime = xm_video_get_play_time;
		iqyPlayerInterface.getPlayState 	= xm_video_get_play_state;
		iqyPlayerInterface.getResolution	= xm_video_get_resolution;
		iqyPlayerInterface.getTotalAdTime = xm_video_get_total_ad_time_by_sec;
		iqyPlayerInterface.getTotalTimeBySec = xm_video_get_total_time_by_sec;
		iqyPlayerInterface.ifPlayingAd   	= xm_video_get_if_ad;
		iqyPlayerInterface.pause_resume	 = xm_video_pause_resume;
		iqyPlayerInterface.playUrl 		= xm_video_play_by_url;
		iqyPlayerInterface.playUrlEx		= xm_video_play_by_url_ex;
		iqyPlayerInterface.seek		= xm_video_seek;
		iqyPlayerInterface.videoStop	= xm_video_stop;
	}
	return &iqyPlayerInterface;
}

BEGIN_AP_EVTMAP(xm_file_play_evtmap)
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

END_AP_EVTMAP(xm_file_play_evtmap)

BEGIN_AP_EVTPROC(xm_file_play_evtproc)
ON_EVENTPROC(API_VIDEO_EVENT_LOAD_MEDIA_ERROR, xm_video_on_loadmedia_error)
ON_EVENTPROC(API_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, xm_video_on_loadmedia_success)
ON_EVENTPROC(API_VIDEO_EVENT_LOAD_MEDIA_EXIT, xm_video_on_loadmedia_exit)
ON_EVENTPROC(API_VIDEO_EVENT_STOP_CFM, xm_video_on_stop_cfm)
ON_EVENTPROC(API_VIDEO_EVENT_EOS, xm_video_on_eos)
ON_EVENTPROC(API_VIDEO_EVENT_SET_PATH_FAIL,xm_video_on_loadmedia_error)
END_AP_EVTPROC(xm_file_play_evtproc)
#endif

