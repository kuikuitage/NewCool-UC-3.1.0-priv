#ifndef __IQY_IPTV_API_H__
#define __IQY_IPTV_API_H__
#include "iqiyiTV_C.h"
#include "ui_iptv_api.h"

extern void iqy_dp_init(void);
extern void iqy_dp_deinit(void);
extern void iqy_dp_register_msg(void);
extern void iqy_dp_unregister_msg(void);
extern void iqy_free_msg_data(u16 msg, u32 para1, u32 para2);
extern const VodDpInterface_t * ui_iqy_return_instance(void);


#endif
