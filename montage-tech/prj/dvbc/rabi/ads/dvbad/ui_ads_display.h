
#ifndef _UI_ADS_DISPLAY_H
#define _UI_ADS_DISPLAY_H


#define MAX_ROLL_OSD_LENGTH 256
#define MAX_ROLL_OSD_NUM 15

/*
//5、调用广告系统函数返回值类型定义
typedef enum 
{
	DVBAD_FUN_ERR_PARA,
	DVBAD_FUN_NO_ELEMENT,
	DVBAD_FUN_NOT_SUPPORT,
	DVBAD_FUN_FLASH_NOT_READY,
	DVBAD_FUN_OK,
}ST_AD_DS_FUNC_TYPE;
*/
//6、时间格式
typedef struct 
{
	u8 	ucHour;
	u8 	ucMinute;
	u8	ucSecond;
}ST_AD_DS_TIME_TYPE;

//7、日期时间格式
typedef struct 
{
	u16	usYear;
	u8	ucMonth;
	u8	ucDay;
	u8 	ucHour;
	u8 	ucMinute;
	u8	ucSecond;
}ST_AD_DS_DATE_TIME_INFO;  
 

typedef struct 
{
  BOOL b_SaveFlag;
  u8 osd_display_buffer[MAX_ROLL_OSD_LENGTH + 1];	
}ST_ADS_OSD_INFO;

BOOL ui_get_ads_osd_status(void);
BOOL ui_get_full_scr_ad_status(void);

void ui_set_full_scr_ad_status(BOOL status);

void ui_ads_osd_roll_stop(void);
void ui_ads_osd_roll(void);

RET_CODE ui_adv_dvbad_display(ads_info_t *p_ads_info);
void ads_dvbad_set_type(ads_ad_type_t type, u32 * cmd, ads_info_t * p_cfg);
void ui_get_unauth_program_ad(void);
RET_CODE on_ads_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#endif
