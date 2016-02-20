/*
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#if defined(__MINGW32__) || defined(__CYGWIN__)
#include <windows.h>
#endif
#ifdef __LINUX__
#include <stdlib.h>
#else
#include "mp_func_trans.h"
#endif
#include "stream/stream.h"
#ifdef CONFIG_DVDREAD
#include "stream/stream_dvd.h"
#endif
#ifdef CONFIG_DVDNAV
#include "stream/stream_dvdnav.h"
#endif
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"
#include "codec-cfg.h"
#include "osdep/timer.h"
//#include "osdep/priority.h"
#include "path.h"
//#include "mplayer.h"
#include "sub/font_load.h"
#include "sub/sub.h"
#include "libvo/video_out.h"
#include "cpudetect.h"
#include "help_mp.h"
#include "mp_msg.h"
//#include "parser-cfg.h"
//#include "sub/spudec.h"
//#include "version.h"
#include "sub/ass_mp.h"
#include "sub/vobsub.h"
//#include "sub/av_sub.h"
//#include "sub/sub_cc.h"
#include "libmpcodecs/dec_teletext.h"
#include "libavutil/intreadwrite.h"
#include "m_option.h"
#include "mpcommon.h"

double sub_last_pts = -303;

#ifdef CONFIG_ASS
ASS_Track* ass_track = 0; // current track to render
#endif

subtitle* vo_sub_last = NULL;
char *spudec_ifo;
int forced_subs_only;

//const char *mencoder_version = "MEncoder " VERSION;
//const char *mplayer_version  = "MPlayer "  VERSION;
#if 0
void print_version(const char* name)
{
    mp_msg(MSGT_CPLAYER, MSGL_INFO, MP_TITLE, name);

    /* Test for CPU capabilities (and corresponding OS support) for optimizing */
    GetCpuCaps(&gCpuCaps);
#if ARCH_X86
    mp_msg(MSGT_CPLAYER, MSGL_V,
           "CPUflags:  MMX: %d MMX2: %d 3DNow: %d 3DNowExt: %d SSE: %d SSE2: %d SSE3: %d SSSE3: %d SSE4: %d SSE4.2: %d AVX: %d\n",
           gCpuCaps.hasMMX, gCpuCaps.hasMMX2,
           gCpuCaps.has3DNow, gCpuCaps.has3DNowExt,
           gCpuCaps.hasSSE, gCpuCaps.hasSSE2, gCpuCaps.hasSSE3,
           gCpuCaps.hasSSSE3, gCpuCaps.hasSSE4, gCpuCaps.hasSSE42,
           gCpuCaps.hasAVX);
#if CONFIG_RUNTIME_CPUDETECT
    mp_msg(MSGT_CPLAYER, MSGL_V, "Compiled with runtime CPU detection.\n");
#else
    mp_msg(MSGT_CPLAYER, MSGL_V, "Compiled for x86 CPU with extensions:");
if (HAVE_MMX)
    mp_msg(MSGT_CPLAYER,MSGL_V," MMX");
if (HAVE_MMX2)
    mp_msg(MSGT_CPLAYER,MSGL_V," MMX2");
if (HAVE_AMD3DNOW)
    mp_msg(MSGT_CPLAYER,MSGL_V," 3DNow");
if (HAVE_AMD3DNOWEXT)
    mp_msg(MSGT_CPLAYER,MSGL_V," 3DNowExt");
if (HAVE_SSE)
    mp_msg(MSGT_CPLAYER,MSGL_V," SSE");
if (HAVE_SSE2)
    mp_msg(MSGT_CPLAYER,MSGL_V," SSE2");
if (HAVE_SSE3)
    mp_msg(MSGT_CPLAYER,MSGL_V," SSE3");
if (HAVE_SSSE3)
    mp_msg(MSGT_CPLAYER,MSGL_V," SSSE3");
if (HAVE_SSE4)
    mp_msg(MSGT_CPLAYER,MSGL_V," SSE4");
if (HAVE_SSE42)
    mp_msg(MSGT_CPLAYER,MSGL_V," SSE4.2");
if (HAVE_AVX)
    mp_msg(MSGT_CPLAYER,MSGL_V," AVX");
if (HAVE_CMOV)
    mp_msg(MSGT_CPLAYER,MSGL_V," CMOV");
    mp_msg(MSGT_CPLAYER,MSGL_V,"\n");
#endif /* CONFIG_RUNTIME_CPUDETECT */
#endif /* ARCH_X86 */
}

void init_vo_spudec(struct stream *stream, struct sh_video *sh_video, struct sh_sub *sh_sub)
{
    unsigned width, height;
    spudec_free(vo_spudec);
    vo_spudec = NULL;

    // we currently can't work without video stream
    if (!sh_video)
        return;

    if (spudec_ifo) {
        unsigned int palette[16];
       // current_module = "spudec_init_vobsub";
        if (vobsub_parse_ifo(NULL, spudec_ifo, palette, &width, &height, 1, -1, NULL) >= 0)
            vo_spudec = spudec_new_scaled(palette, width, height, NULL, 0);
    }

    width  = sh_video->disp_w;
    height = sh_video->disp_h;

#ifdef CONFIG_DVDREAD
    if (vo_spudec == NULL && stream->type == STREAMTYPE_DVD) {
      //  current_module = "spudec_init_dvdread";
        vo_spudec      = spudec_new_scaled(((dvd_priv_t *)(stream->priv))->cur_pgc->palette,
                                           width, height,
                                           NULL, 0);
    }
#endif

#ifdef CONFIG_DVDNAV
    if (vo_spudec == NULL && stream->type == STREAMTYPE_DVDNAV) {
        unsigned int *palette = mp_dvdnav_get_spu_clut(stream);
      //  current_module = "spudec_init_dvdnav";
        vo_spudec      = spudec_new_scaled(palette, width, height, NULL, 0);
    }
#endif

    if (vo_spudec == NULL) {
     //   current_module = "spudec_init_normal";
        vo_spudec      = spudec_new_scaled(NULL, width, height,
                                           sh_sub ? sh_sub->extradata : NULL,
                                           sh_sub ? sh_sub->extradata_len : 0);
        spudec_set_font_factor(vo_spudec, font_factor);
    }

    if (vo_spudec)
        spudec_set_forced_subs_only(vo_spudec, forced_subs_only);
}
#endif

static int is_text_sub(int type)
{
    return type == 't' || type == 'm' || type == 'a';
}

static int is_av_sub(int type)
{
    return type == 'b' || type == 'p' || type == 'x';
}

void update_subtitles(sh_video_t *sh_video, double refpts, demux_stream_t *d_dvdsub, int reset)
{
    double curpts = refpts - sub_delay;
    unsigned char *packet=NULL;
    int len;
    int type = d_dvdsub->sh ? ((sh_sub_t *)d_dvdsub->sh)->type : 'v';
    static subtitle subs;
    if (reset) {
        sub_clear_text(&subs, MP_NOPTS_VALUE);
        if (vo_sub) {
            set_osd_subtitle(NULL);
        }

#ifdef CONFIG_FFMPEG
        if (is_av_sub(type))
            reset_avsub(d_dvdsub->sh);
#endif
    }
    // find sub
    if (subdata) {
   //   printf("%s    %d\n",__func__,__LINE__);
  
        if (sub_fps==0) sub_fps = sh_video ? sh_video->fps : 25;
  //    printf("%s    %d    %f   [%f]   [%d]\n",__func__,__LINE__,sub_fps,curpts,subdata->sub_uses_time );        
       // current_module = "find_sub";
        if (refpts > sub_last_pts || refpts < sub_last_pts-1.0) {
            find_sub(subdata, curpts *
                     (subdata->sub_uses_time ? 100. : sub_fps));
            if (vo_sub) vo_sub_last = vo_sub;
            // FIXME! frame counter...
            sub_last_pts = refpts;
      }
    }


   // current_module=NULL;
}
#if 0
void update_teletext(sh_video_t *sh_video, demuxer_t *demuxer, int reset)
{
    int page_changed;

    if (!demuxer->teletext)
        return;

    //Also forcing page update when such ioctl is not supported or call error occured
    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_IS_CHANGED,&page_changed)!=VBI_CONTROL_TRUE)
        page_changed=1;

    if(!page_changed)
        return;

    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_GET_VBIPAGE,&vo_osd_teletext_page)!=VBI_CONTROL_TRUE)
        vo_osd_teletext_page=NULL;
    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_GET_HALF_PAGE,&vo_osd_teletext_half)!=VBI_CONTROL_TRUE)
        vo_osd_teletext_half=0;
    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_GET_MODE,&vo_osd_teletext_mode)!=VBI_CONTROL_TRUE)
        vo_osd_teletext_mode=0;
    if(teletext_control(demuxer->teletext,TV_VBI_CONTROL_GET_FORMAT,&vo_osd_teletext_format)!=VBI_CONTROL_TRUE)
        vo_osd_teletext_format=0;
    vo_osd_changed(OSDTYPE_TELETEXT);

    teletext_control(demuxer->teletext,TV_VBI_CONTROL_MARK_UNCHANGED,NULL);
}
#endif

int select_audio(demuxer_t* demuxer, int audio_id, char* audio_lang)
{
    if (audio_id == -1 && audio_lang)
        audio_id = demuxer_audio_track_by_lang(demuxer, audio_lang);
    if (audio_id == -1)
        audio_id = demuxer_default_audio_track(demuxer);
    if (audio_id != -1) // -1 (automatic) is the default behaviour of demuxers
        demuxer_switch_audio(demuxer, audio_id);
    if (audio_id == -2) { // some demuxers don't yet know how to switch to no sound
        demuxer->audio->id = -2;
        demuxer->audio->sh = NULL;
    }
    return demuxer->audio->id;
}

/* Parse -noconfig common to both programs */
int disable_system_conf=0;
int disable_user_conf=0;
#ifdef CONFIG_GUI
int disable_gui_conf=0;
#endif /* CONFIG_GUI */

/* Disable all configuration files */
static void noconfig_all(void)
{
    disable_system_conf = 1;
    disable_user_conf = 1;
#ifdef CONFIG_GUI
    disable_gui_conf = 1;
#endif /* CONFIG_GUI */
}

m_config_t *mconfig;
#if 0
int cfg_inc_verbose(m_option_t *conf)
{
    ++verbose;
    return 0;
}

int cfg_include(m_option_t *conf, const char *filename)
{
    return m_config_parse_config_file(mconfig, filename, 0);
}
#endif

const m_option_t noconfig_opts[] = {
    {"all", noconfig_all, CONF_TYPE_FUNC, CONF_GLOBAL|CONF_NOCFG|CONF_PRE_PARSE, 0, 0, NULL},
    {"system", &disable_system_conf, CONF_TYPE_FLAG, CONF_GLOBAL|CONF_NOCFG|CONF_PRE_PARSE, 0, 1, NULL},
    {"user", &disable_user_conf, CONF_TYPE_FLAG, CONF_GLOBAL|CONF_NOCFG|CONF_PRE_PARSE, 0, 1, NULL},
#ifdef CONFIG_GUI
    {"gui", &disable_gui_conf, CONF_TYPE_FLAG, CONF_GLOBAL|CONF_NOCFG|CONF_PRE_PARSE, 0, 1, NULL},
#endif /* CONFIG_GUI */
    {NULL, NULL, 0, 0, 0, 0, NULL}
};

/**
 * Code to fix any kind of insane defaults some OS might have.
 * Currently mostly fixes for insecure-by-default Windows.
 */
static void sanitize_os(void)
{
#if defined(__MINGW32__) || defined(__CYGWIN__)
    HMODULE kernel32 = GetModuleHandle("Kernel32.dll");
    BOOL WINAPI (*setDEP)(DWORD) = NULL;
    BOOL WINAPI (*setDllDir)(LPCTSTR) = NULL;
    if (kernel32) {
        setDEP = GetProcAddress(kernel32, "SetProcessDEPPolicy");
        setDllDir = GetProcAddress(kernel32, "SetDllDirectoryA");
    }
    if (setDEP) setDEP(3);
    if (setDllDir) setDllDir("");
    // stop Windows from showing all kinds of annoying error dialogs
    SetErrorMode(0x8003);
    // request 1ms timer resolution
    timeBeginPeriod(1);
#endif
}
#if 0

/**
 * Initialization code to be run at the very start, must not depend
 * on option values.
 */
void common_preinit(void)
{
    sanitize_os();
    InitTimer();
    srand(GetTimerMS());

    mp_msg_init();
}

/**
 * Initialization code to be run after command-line parsing.
 */
int common_init(void)
{
#if (defined(__MINGW32__) || defined(__CYGWIN__)) && defined(CONFIG_WIN32DLL)
    set_path_env();
#endif

#ifdef CONFIG_PRIORITY
    set_priority();
#endif

    if (codec_path)
        set_codec_path(codec_path);

    /* Check codecs.conf. */
    if (!codecs_file || !parse_codec_cfg(codecs_file)) {
        char *conf_path = get_path("codecs.conf");
        if (!parse_codec_cfg(conf_path)) {
            if (1) {
                if (!parse_codec_cfg(NULL)) {
                    free(conf_path);
                    return 0;
                }
                mp_msg(MSGT_CPLAYER, MSGL_V, "Using built-in default codecs.conf.\n");
            }
        }
        free(conf_path);
    }

    // check font
#ifdef CONFIG_FREETYPE
    init_freetype();
#endif
#ifdef CONFIG_FONTCONFIG
    if (font_fontconfig <= 0)
#endif
    {
#ifdef CONFIG_BITMAP_FONT
        if (font_name) {
            vo_font = read_font_desc(font_name, font_factor, verbose>1);
            if (!vo_font)
                mp_msg(MSGT_CPLAYER,MSGL_ERR,MSGTR_CantLoadFont,
                       filename_recode(font_name));
        } else {
            // try default:
            char *desc_path = get_path("font/font.desc");
            vo_font = read_font_desc(desc_path, font_factor, verbose>1);
            free(desc_path);
            if (!vo_font)
                vo_font = read_font_desc(MPLAYER_DATADIR "/font/font.desc", font_factor, verbose>1);
        }
        if (sub_font_name)
            sub_font = read_font_desc(sub_font_name, font_factor, verbose>1);
        else
            sub_font = vo_font;
#endif
    }

    vo_init_osd();

#ifdef CONFIG_ASS
    ass_library = ass_init();
#endif
    return 1;
}
#endif

/// Returns a_pts
double calc_a_pts(sh_audio_t *sh_audio, demux_stream_t *d_audio)
{
    double a_pts;
    if(!sh_audio || !d_audio)
        return MP_NOPTS_VALUE;
    // first calculate the end pts of audio that has been output by decoder
    a_pts = sh_audio->pts;
    // If we cannot get any useful information at all from the demuxer layer
    // just count the decoded bytes. This is still better than constantly
    // resetting to 0.
    if (sh_audio->pts_bytes && a_pts == MP_NOPTS_VALUE &&
        !d_audio->pts && !sh_audio->i_bps)
        a_pts = 0;
    if (a_pts != MP_NOPTS_VALUE)
        // Good, decoder supports new way of calculating audio pts.
        // sh_audio->pts is the timestamp of the latest input packet with
        // known pts that the decoder has decoded. sh_audio->pts_bytes is
        // the amount of bytes the decoder has written after that timestamp.
        a_pts += sh_audio->pts_bytes / (double) sh_audio->o_bps;
    else {
        // Decoder doesn't support new way of calculating pts (or we're
        // being called before it has decoded anything with known timestamp).
        // Use the old method of audio pts calculation: take the timestamp
        // of last packet with known pts the decoder has read data from,
        // and add amount of bytes read after the beginning of that packet
        // divided by input bps. This will be inaccurate if the input/output
        // ratio is not constant for every audio packet or if it is constant
        // but not accurately known in sh_audio->i_bps.

        a_pts = d_audio->pts;
        // ds_tell_pts returns bytes read after last timestamp from
        // demuxing layer, decoder might use sh_audio->a_in_buffer for bytes
        // it has read but not decoded
        if (sh_audio->i_bps)
            a_pts += (ds_tell_pts(d_audio) - sh_audio->a_in_buffer_len) /
                     (double)sh_audio->i_bps;
    }
    return a_pts;
}
