/********************************************************************************************/
/********************************************************************************************/
#ifndef __AV_RENDER_FILTER_H_
#define __AV_RENDER_FILTER_H_

/*!
  av render config
  */
typedef struct tag_av_render_config
{
  /*!
    video pid
    */
  u16 video_pid;
  /*!
    audio pid
    */
  u16 audio_pid;
  /*!
    pcr pid
    */
  u16 pcr_pid;
  /*!
    vdec_src_fmt_t
    */
  u8 video_fmt;
  /*!
    adec_src_fmt_vsb_t
    */
  u8 audio_fmt;
   /*!
    encrypt_flag
    */
  u32 encrypt_flag : 16;
 
  /*!
    audio_key_len
    */
  u32 audio_key_odd_len : 4;
  /*!
    audio_key_len
    */
  u32 audio_key_even_len : 4;
  /*!
    video_key_len
    */
  u32 video_key_odd_len : 4;
  /*!
    video_key_len
    */
  u32 video_key_even_len : 4;
  /*!
    audio odd key
    */
  u8 audio_key_odd[CAS_CH_KEY_MAX];
  /*!
    audio even key
    */
  u8 audio_key_even[CAS_CH_KEY_MAX];
  /*!
    video odd key
    */
  u8 video_key_odd[CAS_CH_KEY_MAX];
  /*!
    video even key
    */
  u8 video_key_even[CAS_CH_KEY_MAX];
}av_render_config_t;

/*!
  av render filter command define
  */
typedef enum tag_av_render_filter_cmd
{
  /*!
    config video/audio/pcr pid
    */
  AV_RENDER_CFG,
  /*!
    end
    */
  AV_RENDER_CFG_END
}av_render_filter_cmd_t;

/*!
  create a video render filter instance
  filter ID:AV_RENDER_FILTER
  \return return the instance of filter
  */
ifilter_t * av_render_filter_create(void *p_para);

/*!
  av_render_trick_mode
  */
void av_render_trick_mode(handle_t _this, u32 is_normal);

/*!
  av_render_av_reset
  */
void av_render_av_reset(handle_t _this);

/*!
  av_render_set_play_mode
  */
void av_render_set_play_mode(handle_t _this, u8 play_mode);
  
#endif // End for __AV_RENDER_FILTER_H_
