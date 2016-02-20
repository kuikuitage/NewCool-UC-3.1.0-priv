#ifndef __ADS__DRV_DVBAD__HH__
#define __ADS__DRV_DVBAD__HH__
#include "ads_api.h"

void dvbad_ads_client_init();
ST_AD_DVBAD_FUNC_TYPE DVBAD_SetFilter(ST_ADS_DVBAD_FILTER_INFO *pstFilterInfo);
BOOL dvb_ad_nim_lock(u32 freq, u32 symbolrate, u16 qammode);
BOOL dvbad_set_filter_bat();

RET_CODE get_AD_version(u16 *param);

#endif

