#ifndef __ADS__API__HH__
#define __ADS__API__HH__

#define	DVBAD_FILTER_MAX_LEN		(15)
#define IN

#ifndef DVBAD_Semaphore
#define DVBAD_Semaphore	unsigned int
#endif

typedef struct{
	BOOL valid_flag;
	u16 stream_id;
	u16 network_id;
	u16 service_id;
	u32 frequency;
	u8 modulation;
	u32 symbol_rate;
}bat_data;

typedef struct{
	u16 ad_pic_num;
	u8 version_number;
	u16 AD_version;
	BOOL save_all_pic_flag;
	u32 buff_size;
	char sum;
}ad_head_flash;

typedef enum{
	ADS_PIC_FORMAT_GIF = 1,
	ADS_PIC_FORMAT_PNG,
	ADS_PIC_FORMAT_DIB,
	ADS_PIC_FORMAT_BMP,
	ADS_PIC_FORMAT_JPG,
	ADS_PIC_FORMAT_MPG,
	ADS_PIC_FORMAT_TIF,
	ADS_PIC_FORMAT_UNKNOW,
}DVBAD_ADS_PIC_FORMAT;

typedef enum 
{
	DVBAD_STB_LOG=1,       //开机广告
	DVBAD_STB_EPG=2,          
	DVBAD_STB_VOLUME = 3,
	DVBAD_STB_MAINMENU = 4,
	DVBAD_STB_CHLIST = 5,
	DVBAD_STB_CURRENT_FOLLOW = 6,
	DVBAD_STB_CHANNEL_INFO = 7,
	DVBAD_STB_POS_UNKNOWN,
}DVBAD_POS_TYPE;

//pic show info
typedef struct{
	DVBAD_POS_TYPE 	pos_type;
	DVBAD_ADS_PIC_FORMAT	pic_type;
	u16			x;
	u16			y;
	u16			w;
	u16			h;
	u32			data_len;
	u32			pic_id;
	u8			*pic_data;
}dvbad_show_pic_info;

//osd position
typedef enum _EN_DVBAD_OSD_SHOW_POS
{
	SHOW_TOP = 0,					
	SHOW_BOTTOM,					
	SHOW_TOP_BOTTOM,				
	SHOW_LEFT,					
	SHOW_RIGHT,					
	SHOW_LEFT_RIGHT,				
	SHOW_UNKNOWN_TYPE,
}dvbad_osd_show_pos;

typedef enum 
{
	DVBAD_AV_VIDEO_AVI=1,
	DVBAD_AV_AUDIO_MP3=2,  //暂不支持
	DVBAD_AV_VIDEO_TS=3,
	DVBAD_AV_VIDEO_M2V=4,
	DVBAD_AV_VIDEO_MPG=5,
	DVBAD_AV_UNKNOWN,
}dvbad_av_type;

//av show info
typedef struct 
{
	DVBAD_POS_TYPE	enPosType;
	dvbad_av_type	enAvType;
	u16				usStartX;
	u16				usStartY;
	u16				usWidth;
	u16				usHeigth;
	u32				uiDataLen;
	u8				aucElementId[4];
	u8* 				pucAvData;
}dvbad_av_show_info;

//osd show info
typedef struct _ST_DVBAD_OSD_SHOW_INFO
{
	dvbad_osd_show_pos		enDisplayPos;
	u8				aucBackgroundColor[4];
	u8				aucDisplayColor[4];
	u16				usContentSize;
	u16				usGetBufferSize;
	u8* 			pucDisplayBuffer;
	u8				ucFontType;
	u8				ucFontSize;
	u8				ucShowTimes;
}dvbad_osd_show_info;


typedef enum 
{
	DVBAD_FUN_ERR_PARA,
	DVBAD_FUN_NO_ELEMENT,
	DVBAD_FUN_NOT_SUPPORT,
	DVBAD_FUN_FLASH_NOT_READY,
	DVBAD_FUN_FILTER_COMPELET,
	DVBAD_FUN_OK,
}ST_AD_DVBAD_FUNC_TYPE;

typedef struct _ST_DVBAD_PROGRAM_PARA
{
	u16 usNetworkId;
	u16 usTsId;
	u16 usServiceId;
}dvbad_program_para;

typedef struct _ST_DVBAD_OSD_PROGRAM_SHOW_INFO
{
	dvbad_osd_show_info stTextShow[15];	
	u8 aucFlag[15];
	u8 ucOsdNum;
}dvbad_osd_program_show_info;


typedef struct 
{
  u8 ucLogType;
  u32 uiLogoSize;
}dvbad_log_save_info;

typedef struct _DVBAD_PROGRAM_SHOW_INFO
{
	dvbad_show_pic_info stPictureShow;
	dvbad_av_show_info stAvPlay;
	u8 aucFlag[2];
}dvbad_program_show_info;

typedef void (* FilterNotifyFunction)(u16 usPid, IN u8* pucDataBuffer, u16 usDataLen);

//过滤器过滤条件
typedef struct _ST_DVBAD_DVBAD_FILTER_INFO
{
	u16 filter_id;//8
	u16 usChannelPid;//16
	u8 ucFilterLen;//8
	u8 aucFilter[15];//15*8
	u8 aucMask[15];//15*8
	u8 ucWaitSeconds;
	FilterNotifyFunction pfDataNotifyFunction;
}ST_ADS_DVBAD_FILTER_INFO;

typedef enum{
	DVBAD_NEED_UPDATE_YES = 1,
	DVBAD_NEED_UPDATE_NO,
	DVBAD_UPDATE_FAILURE,
}DVBAD_UPDATE_STATUS;

#endif
