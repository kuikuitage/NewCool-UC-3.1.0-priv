/****************************************************************************

 ****************************************************************************/
#ifndef __AP_COMMON_H__
#define __AP_COMMON_H__

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_magic.h"
#include "sys_devs.h"
#include "sys_cfg.h"

//c lib
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
#include "gpe.h"
#include "hal_watchdog.h"
#include "display.h"

// mdl
#include "mdl.h"

#include "mmi.h"
#include "surface.h"
#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"
#include "gui_roll.h"
#include "gui_vfont.h"


#include "class_factory.h"
#include "mdl.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "data_manager.h"
#include "data_base.h"
#include "ss_ctrl.h"

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
#include "gui_script.h"

// prj misc
#include "mem_cfg.h"

// ap
#include "ap_framework.h"
#include "ap_uio.h"

#include "ui_config.h"
#include "ui_desktop.h"
#include "ui_menu_manager.h"

#include "ui_comm_root.h"
#include "ui_comm_ctrl.h"



// resource
#include "bmp_id.h"
#include "str_id.h"
#include "language_id.h"
#include "font_id.h"
#include "palette_id.h"

#include "ui_comm_dlg.h"
#include "ui_gui_defines.h"

//
#include "sys_status.h"


#endif

