#ifndef __XM_IPTV_API_H__
#define __XM_IPTV_API_H__
#include "xmTV.h"
#include "ui_iptv_api.h"

extern void xm_dp_init(void);
extern void xm_dp_deinit(void);
extern void xm_dp_register_msg(void);
extern void xm_dp_unregister_msg(void);
extern void xm_free_msg_data(u16 msg, u32 para1, u32 para2);
extern const VodDpInterface_t * ui_xm_return_instance(void);


#endif
