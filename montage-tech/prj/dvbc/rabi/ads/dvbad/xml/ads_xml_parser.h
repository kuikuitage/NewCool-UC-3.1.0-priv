#include "sys_types.h"
#include "ads_api.h"
#define XML_GET_CHILD(node) node->childs
#define XML_GET_ROOT(doc)   doc->root
#define XML_FREE(x) if(x){free(x);x=NULL;}
#ifndef xmlChar
#define xmlChar unsigned char
#endif

typedef enum{
	ADS_CHANNEL_CURRENT_FOLLOW=1,
	ADS_CHANNEL_INFO,
}ADS_CHANNEL_TYPE;

typedef struct{
	DVBAD_POS_TYPE type;
	ADS_CHANNEL_TYPE	channel_type;
	xmlChar type_name[16];
	xmlChar name[8];
	DVBAD_ADS_PIC_FORMAT p_format;
	u16 x;
	u16 y;
	u16 w;
	u16 h;
	u16 duration;
	u16 version;
	u16 file_number;
	u8* text;
}ads_pic_attribute;

typedef enum{
	PIC_COMPRESSED_FLAG_NO = 0,
	PIC_COMPRESSED_FLAG_YES = 1,
}PIC_COMPRESSED_FLAG;

typedef struct{
	u16 pic_num;
	u16 pic_id;
	u32 pic_original_size;
	u16 pic_segment_length;
	u16 pic_segment_num;
	PIC_COMPRESSED_FLAG pic_compressed_flag;
	u32 pic_compressed_size;
}ads_pic_infomation;

typedef struct _Data_Node{
	u8 *p_ads_pic_data;
	ads_pic_attribute ads_pic_attribute_data;
	ads_pic_infomation p_pic_info;
	u16 text_size;
	struct _Data_Node *next;
}ads_data_node;

typedef struct{
	xmlChar ads_area_name[32];
	xmlChar ads_manufacturer_name[32];
	xmlChar rcvtype_name[32];
	xmlChar portion_name[32];
	u32 stb_start_id;
	u32 stb_end_id;
}ads_rcv_config;

typedef struct{
	ad_head_flash ad_head_data;
	bat_data ad_bat_data;
	ads_data_node dvbad_pic_attribute_node;
	ads_rcv_config dvbad_ads_rcv_config;
	u16 ads_pic_count;
}dvbad_ads_pic_data;

#define FORMAT_STR				((xmlChar*)"format")
#define POSITION_X_STR			((xmlChar*)"position_x")
#define POSITION_Y_STR			((xmlChar*)"position_y")
#define WIDTH_STR				((xmlChar*)"width")
#define HEIGHT_STR				((xmlChar*)"height")
#define DURATION_STR			((xmlChar*)"duration")
#define FILE_NUME_STR			((xmlChar*)"file_number")
#define FILE_VERSION_STR		((xmlChar*)"file_version")
#define STB_START_ID_STR		((xmlChar*)"stb_start_id")
#define STB_END_ID_STR			((xmlChar*)"stb_end_id")
#define LOGI_NAME_STR			((xmlChar*)"Logo")
#define AD_NAME_STR				((xmlChar*)"ad")
#define ATTRIBUTE_NAME_STR		((xmlChar*)"name")
#define BOOTLOGO_STR			((xmlChar*)"Bootlogo")
#define MAINMENU_STR			((xmlChar*)"MainMenu")
#define SUBMENUS_STR			((xmlChar*)"SubMenus")
#define STRIPE_STR				((xmlChar*)"Stripe")
#define RCV_CONFIGR_STR			((xmlChar*)"RCVconfig")
#define CFG_AREA_STR			((xmlChar*)"area")
#define CFG_MANUFACTURER_STR	((xmlChar*)"manufacturer") 
#define CFG_RCVTYPE_STR			((xmlChar*)"rcvtype") 
#define CFG_PORTION_STR			((xmlChar*)"portion") 
#define CFG_STB_START_ID_STR	((xmlChar*)"stb_start_id")
#define CFG_STV_END_ID_STR		((xmlChar*)"stb_end_id")
#define CURRENT_FOLLOW_STR		((xmlChar*)"current_follow")
#define CHANNER_INFO_STR		((xmlChar*)"channel_info")
#define CHANNER_TEXT_STR		((xmlChar*)"Text")
#define SUBMENU_MENU_NAME_STR	((xmlChar*)"Menu")
#define PIC_FORMAT_GIF_STR	((xmlChar*)"gif")
#define PIC_FORMAT_BMP_STR	((xmlChar*)"bmp")
#define PIC_FORMAT_JPG_STR	((xmlChar*)"jpg")
#define PIC_FORMAT_MPG_STR	((xmlChar*)"mpg")
#define PIC_FORMAT_TIF_STR	((xmlChar*)"tif")
#define PIC_FORMAT_PNG_STR	((xmlChar*)"png")
#define PIC_FORMAT_DIB_STR	((xmlChar*)"dib")
#define AD_TYPE_EPG_STR		((xmlChar*)"EPG")
#define AD_TYPE_CHLIST_STR	((xmlChar*)"CHLIST")
#define AD_TYPE_VOLUME_STR	((xmlChar*)"VOLUME")
#define AD_TYPE_EPG1_STR	((xmlChar*)"epg")
#define AD_TYPE_CHLIST1_STR	((xmlChar*)"chlist")
#define AD_TYPE_VOLUME1_STR	((xmlChar*)"volume")



RET_CODE ads_xml_parse(const xmlChar *p_buf, u32 buf_len, dvbad_ads_pic_data *p_dvbad_ads_pic_attribute_data);

