#ifndef __UI_VIDEO_API_H__
#define __UI_VIDEO_API_H__
#include "file_playback_sequence.h"
#include "iqiyiTV_C.h"

/*!
 * Debug on/off
 */
#define UI_VIDEO_PRINF OS_PRINTF

#ifdef WIN32
#define STRCMPI strcmpi
#else
#define STRCMPI strcasecmp
#endif
/*!
 *Preview x/y
 */
#define VIDEO_V_PREV_X  0
#define VIDEO_V_PREV_Y  0
#define VIDEO_V_PREV_W  1280
#define VIDEO_V_PREV_H  720

/*!
 *Preview offset
 */
#define VIDEO_V_PREV_OFFSET_X  160
#define VIDEO_V_PREV_OFFSET_Y  64


typedef enum
{
    /*!
     Play end event
     */
    MSG_VIDEO_EVENT_EOS = MSG_EXTERN_BEGIN + 1200,
    /*!
     Update play time
     */
    MSG_VIDEO_EVENT_UP_TIME,
    /*!
     Update video resolution
     */
    MSG_VIDEO_EVENT_UP_RESOLUTION,
    /*!
     File play bar close
     */
    MSG_VIDEO_EVENT_FULLSCREEN_CLOSE,
    /*!
     File play unsupport video event
     */
    MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO,
    /*!
     File play trick to begin
     */
    MSG_VIDEO_EVENT_TRICK_TO_BEGIN,
    /*!
     File play unsupport video memory
     */
    MSG_VIDEO_EVENT_UNSUPPORTED_MEMORY,
    /*!
     File play unsupport seek operation
     */
    MSG_VIDEO_EVENT_UNSUPPORT_SEEK,
    /*!
     File play load media user exit
     */
    MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT,
    /*!
     File play load media error
     */
    MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR,
    /*!
     File play load media success
     */
    MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS,
    /*!
     File es task exit
     */
    MSG_VIDEO_EVENT_STOP_CFM,
    /*!
     File play update network speed and loading percent, (X)Byte/s, (Y)%
     */
    MSG_VIDEO_EVENT_UPDATE_BPS,
    /*!
     File play network speed is low, request to change url src
     */
    MSG_VIDEO_EVENT_REQUEST_CHANGE_SRC,
    /*!
     File play set path failed
     */
    MSG_VIDEO_EVENT_SET_PATH_FAIL,
    /*!
     File play update loading media cost time  X(s)
     */
    MSG_VIDEO_EVENT_LOAD_MEDIA_TIME,
    /*!
     File play finish show loading bps
     */
    MSG_VIDEO_EVENT_FINISH_BUFFERING,
    /*!
    * notify upper layer that player has already finish updating bps
    */
    MSG_VIDEO_EVENT_FINISH_UPDATE_BPS,
    /*
    *pause av decoder and continue to buffer data
    */
    MSG_VIDEO_EVENT_START_BUFFERING,
    /*!
     Event max
     */
    MSG_VIDEO_EVENT_MAX,
}iqy_video_event_t;

typedef enum
{
    VIDEO_PLAY_FUNC_INVALID = 0,
    VIDEO_PLAY_FUNC_USB,
    VIDEO_PLAY_FUNC_ONLINE,
    VIDEO_PLAY_FUNC_ONLINE_IPTV
}iqy_video_play_func_t;



/*!
 *Struct for resolution
 */
typedef struct
{
    /*!
     * file play resolution pixel x
     */
    int pixel_x;
    /*!
     * file play resolution pixel y
     */
    int pixel_y;
}video_play_pixel_t;




/*!
 * Ui video control function
 */
#endif
