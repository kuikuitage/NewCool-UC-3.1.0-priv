#ifndef  __XINGMEI_DATA_PROVIDER__
#define  __XINGMEI_DATA_PROVIDER__
#include "xingmeiDataProvider.h"


int xingmei_iptv_dp_init(XM_DP_HDL_T * hdl);
int xingmei_iptv_dp_deinit(XM_DP_HDL_T * hdl);
VDO_ID_t *xingmei_get_current_video_id(void);
int	xingmei_get_current_channel_id(void);
char* xingmei_get_current_plid(void);
int xingmei_get_current_episode_num(void);


#endif



