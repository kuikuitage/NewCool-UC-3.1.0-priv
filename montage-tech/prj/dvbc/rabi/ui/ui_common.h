/****************************************************************************

 ****************************************************************************/
#ifndef __AP_COMMON_H__
#define __AP_COMMON_H__

#define V_KEY_HOT_XEXTEND		 V_KEY_BEEPER
#define V_KEY_HOT_XSAVE	        V_KEY_FAV
#define V_KEY_HOT_XDEL		        V_KEY_SLOW
#define V_KEY_HOT_XREFRESH		 V_KEY_INFO

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_magic.h"
#include "sys_devs.h"
#include "sys_cfg.h"

// c lib
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "iso_639_2.h"
#include "iconv_ext.h"


// driver
#include "common.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_misc.h"
#include "hal_uart.h"

#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"
#include "vdec.h"
#include "display.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "dmx.h"
#include "region.h"//
#include "display.h"//
#include "lib_memp.h"
#include "pdec.h"//
#include "lpower.h"
// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_base.h"

#include "service.h"
#include "dvb_svc.h"
#include "monitor_service.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "ts_packet.h"
#include "eit.h"
#ifndef IMPL_NEW_EPG
#include "epg_data4.h"
#endif
#include "cat.h"
#include "emm.h"
#include "ecm.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "avctrl1.h"

#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "ss_ctrl.h"
#include "smc_ctrl.h"
#include "mt_time.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"

#include "media_data.h"
//eva
#include "interface.h"
#include "eva.h"
#include "media_format_define.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"
#include "eva_filter_factory.h"
#include "common_filter.h"
#include "file_source_filter.h"
#include "ts_player_filter.h"
#include "av_render_filter.h"
#include "demux_filter.h"
#include "record_filter.h"
#include "file_sink_filter.h"
#include "pic_filter.h"
#include "pic_render_filter.h"
#include "mp3_player_filter.h"
#include "google_map_filter.h"
#ifdef IMPL_NEW_EPG
#include "epg_type.h"
#endif
#include "file_list.h"
#include "mmi.h"

#include "flinger.h"
#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"
#include "gui_roll.h"
#include "vf_ft.h"
#include "gui_pango.h"

#include "ctrl_base.h"
#include "ctrl_container.h"
#include "ctrl_textfield.h"
#include "ctrl_scrollbar.h"
#include "ctrl_progressbar.h"
#include "ctrl_matrixbox.h"
#include "ctrl_list.h"
#include "ctrl_combobox.h"
#include "ctrl_bitmap.h"
#include "ctrl_editbox.h"
#include "ctrl_common.h"
#include "ctrl_string.h"
#include "ctrl_numbox.h"
#include "ctrl_timebox.h"
#include "ctrl_ipbox.h"
#include "ctrl_setbox.h"

#include "framework.h"

#include "cas_ware.h"

#include "ota_dm_api.h"
#include "ota_public_def.h"
#include "ota_api.h"

// prj misc
#include "db_dvbs.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbc_util.h"
#include "mem_cfg.h"

// ap
#include "ap_framework.h"

//#include "ap_multi_pic.h"
#include "ap_uio.h"
#include "ap_playback.h"
#include "../ap/ap_twin_port.h"

#include "ap_scan.h"
#include "ap_dvbs_scan.h"
#ifndef IMPL_NEW_EPG
#include "ap_epg4.h"
#else
#include "epg_type.h"
#include "epg_database.h"
#include "epg_api.h"
#endif
#include "ap_signal_monitor.h"
//#include "ap_satcodx.h"
#include "ap_upgrade.h"
//#include "ap_satcodx.h"
#include "ap_time.h"
//#include "ap_ca.h"
#include "ap_cas.h"
#include "pic_api.h"//
#include "google_map_api.h"
//#include "ap_music.h"//
#include "ap_playback_i.h"
//#include "ap_multi_pic_i.h"
//#include "ap_ts_player.h"

//#include "ap_record.h"
//#include "ap_timeshift.h"
#include "ap_usb_upgrade.h"

#include "sys_status.h"
#include "db_conn_play.h"

// ui common
#include "ui_config.h"
#include "ui_desktop.h"
#include "ui_menu_manager.h"

#include "ui_comm_dlg.h"
#include "ui_comm_root.h"
#include "ui_comm_ctrl.h"
#include "ui_comm_pwdlg.h"

#include "ui_dbase_api.h"
#include "ui_systatus_api.h"
#include "ui_play_api.h"
#include "ui_scan_api.h"
#include "ui_epg_api.h"
#include "ui_music_api.h"
#include "ui_signal_api.h"
#include "ui_util_api.h"
#include "ui_time_api.h"
#include "ui_picture_api.h"
#include "ui_book_api.h"
#include "ui_ota_api.h"
#include "ui_upgrade_api.h"
#include "ui_ca_api.h"
//#include "ui_twin_port.h"
#ifdef ENABLE_CA
#include "ui_ca_public.h"
#include "cas_ware.h"
#endif
#ifdef WIN32
#include "ui_ca_public.h"
#include "cas_ware.h"
#endif

#ifdef ENABLE_ADS
#include "ads_ware.h"
#include "config_ads.h"
#include "ui_ads_display.h"
#include "ui_ad_api.h"
#endif


//#include "ui_pvr_api.h"
#include "ui_usb_api.h"
#include "ui_usb_music_fullscreen.h"
#ifndef WIN32
#include "netplay.h"
#endif
#include "ui_util.h"
#include "ui_video_player_gprot.h"
//#include "ui_dlna_external.h"

// resource
#include "bmp_id.h"
#include "str_id.h"
#include "language_id.h"
#include "font_id.h"
#include "palette_id.h"

#include "ui_gui_defines.h"
#include "hotplug.h"
#include "livetv_db.h"
#include "custom_url.h"
#include "vfs.h"
//customer define
#include "customer_config.h"

//sys dbg
#include "sys_dbg.h"
#endif

