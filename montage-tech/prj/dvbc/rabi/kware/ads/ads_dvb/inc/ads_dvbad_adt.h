
#ifndef __ADS_DVBAD_ADT_H__
#define __ADS_DVBAD_ADT_H__

typedef enum{
	GET_AD_START,
	GET_AD_FAILURE,
	GET_AD_TRUE,
}get_ad_flag;

RET_CODE ads_adt_dvbad_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id);

#endif

