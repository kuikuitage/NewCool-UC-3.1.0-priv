#ifndef __UI_IPTV_API_H__
#define __UI_IPTV_API_H__
#include "ui_common.h"
#include "iptv_interface.h"
#include "xmTV.h"

#define APP_IPTV    APP_NVOD
typedef enum
{
    IPTV_EVT_INIT_SUCCESS = ((APP_IPTV << 16) + 0),
    IPTV_EVT_INIT_FAIL,
    IPTV_EVT_DEINIT_SUCCESS,
    IPTV_EVT_NEW_RES_NAME_ARRIVE,
    IPTV_EVT_GET_RES_NAME_FAIL,
    IPTV_EVT_NEW_RES_CATGRY_ARRIVE,
    IPTV_EVT_GET_RES_CATGRY_FAIL,
    IPTV_EVT_NEW_PAGE_VDO_ARRIVE,
    IPTV_EVT_GET_PAGE_VDO_FAIL,
    IPTV_EVT_NEW_SEARCH_VDO_ARRIVE,
    IPTV_EVT_GET_SEARCH_VDO_FAIL,
    IPTV_EVT_NEW_VDO_INFO_ARRIVE,
    IPTV_EVT_GET_VDO_INFO_FAIL,
    IPTV_EVT_NEW_INFO_URL_ARRIVE,
    IPTV_EVT_GET_INFO_URL_FAIL,
    IPTV_EVT_NEW_PLAY_URL_ARRIVE,
    IPTV_EVT_GET_PLAY_URL_FAIL,
    IPTV_EVT_NEW_RECMND_INFO_ARRIVE,
    IPTV_EVT_GET_RECMND_INFO_FAIL,
    IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE,
    IPTV_EVT_GET_SOURCE_FORMAT_FAIL,
    IPTV_EVT_NEW_SEARCH_WORD_ARRIVE,
    IPTV_EVT_GET_SEARCH_WORD_FAIL,
    IPTV_EVT_MAX
} iptv_api_evt_t;


typedef enum
{
    MSG_IPTV_EVT_BEGIN = MSG_EXTERN_BEGIN + 1400,
    MSG_IPTV_EVT_INIT_SUCCESS = MSG_IPTV_EVT_BEGIN,
    MSG_IPTV_EVT_INIT_FAIL,
    MSG_IPTV_EVT_DEINIT_SUCCESS,
    MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE,
    MSG_IPTV_EVT_GET_RES_NAME_FAIL,
    MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE,
    MSG_IPTV_EVT_GET_RES_CATGRY_FAIL,
    MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE,
    MSG_IPTV_EVT_GET_PAGE_VDO_FAIL,
    MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE,
    MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL,
    MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE,
    MSG_IPTV_EVT_GET_VDO_INFO_FAIL,
    MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE,
    MSG_IPTV_EVT_GET_INFO_URL_FAIL,
    MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE,
    MSG_IPTV_EVT_GET_RECMND_INFO_FAIL,
    MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE,
    MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL,
    MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE,
    MSG_IPTV_EVT_GET_PLAY_URL_FAIL,
    MSG_IPTV_EVT_NEW_SEARCH_WORD_ARRIVE,
    MSG_IPTV_EVT_GET_SEARCH_WORD_FAIL,
    MSG_IPTV_EVT_TIME_UPDATE,
    MSG_IPTV_EVT_END = MSG_IPTV_EVT_BEGIN + 50
} iptv_api_msg_t;

////////////////////////////////////
typedef enum
{
    IPTV_DP_STATE_UNINIT,
    IPTV_DP_STATE_INITING,
    IPTV_DP_STATE_INIT_SUCC,
    IPTV_DP_STATE_INIT_FAIL,
} iptv_dp_state_t;

typedef enum
{
	IPTV_INVALID_ID,
    IPTV_ID_IQY,
	IPTV_ID_XM,
} iptv_module_id_t;


typedef  struct
{
    u32 res_id;
    u16 *name;
} al_iptv_name_res_item_t;

typedef struct
{
    u16 total_res;
    al_iptv_name_res_item_t *resList;
} al_iptv_name_res_list_t;

/////////////////////////////////////
typedef struct
{
    u16 *name;
    u8  *key;
} al_iptv_tags_item_t;

typedef struct
{
    u16 *name;
    u16 subTagSize;
    u32 total_vdo;
    al_iptv_tags_item_t *subtag;
} al_iptv_filter_item_t;

typedef struct
{
	u32 cat_id;				//二级ID
    u16 *name;
    u8  *key;				//	USED by Filter ~ Search
    u32 total_vdo;			//	Temp inited with 1000
} al_iptv_cat_item_t;

typedef struct
{
    u32 res_id;
    u16 *res_name;
    u16 total_cat;
    u16 total_filter;
    al_iptv_cat_item_t *catList;
    al_iptv_filter_item_t *filter;
} al_iptv_cat_res_item_t;

typedef struct
{
    u16 total_res;
    al_iptv_cat_res_item_t *resList;
} al_iptv_cat_res_list_t;


typedef struct
{
    VDO_ID_t vdo_id;//which　one is used to save the play info
    u32 res_id;
    u16 *name;
    u8  *img_url;
    u8 b_single_page;
} al_iptv_vdo_item_t;

typedef struct
{
    u32 total_page;
    u32 page_num;

    u32 total_vdo;
    u16 vdo_cnt;
    al_iptv_vdo_item_t *vdoList;
} al_iptv_vdo_list_t;

//////////////////////////////////////////////////
typedef struct
{
    u16 *name;
} al_iptv_orgn_item_t;

typedef struct
{
    u8  total_format;
    int  *formatList;
} al_iptv_source_format_t;

typedef struct
{
    u16 tvsets; 
    u32 res_id;
    u32 time;
    VDO_ID_t vdo_id;

    u16 *name;
    u16 *area;
    u16 *director;
    u16 *actor;
    u16 *score;
    u16 *description;
    u16 *attr;
    u16 *years;
    u8  *img_url;
    u16 orgn_cnt;
    al_iptv_source_format_t *format;
    al_iptv_orgn_item_t *orgnList;
} al_iptv_vdo_info_t;
typedef struct
{
      VDO_ID_t id;
      u16 *title;
      int cat_id;
      char *img;
}iptv_recmd_channel_t;

typedef struct
{
   int number;
   iptv_recmd_channel_t *recmd;
}al_iptv_recomand_info_t;
//////////////////////////////////////////////////
typedef struct
{	
	//For xinmei
	//播放地址名
	char name[20];
	//播放码率
	int hdtv;
	//播放地址
	char playUrl[256];
	//频道时移地址
	char timeshiftUrl[256];
	
	//For iqy
    char  tvQid[IPTV_STRING_ID_LEN];                //剧集奇谱ID
    char  vid[IPTV_STRING_ID_LEN];                   //播放码流id
    u16 *urltitle;
} al_iptv_info_url_item_t;

typedef struct
{
    u32 page_count;
    u32 page_total;
    u32 count;
    al_iptv_info_url_item_t *urlList;
} al_iptv_info_url_list_t;

typedef struct
{
    char *collect;
    char  tvQid[IPTV_STRING_ID_LEN];                //剧集奇谱ID
    char  vid[IPTV_STRING_ID_LEN];                   //播放码流id
    u16 *urltitle;
}al_iptv_play_origin_item_t;

typedef struct
{
    u16 *origin;
    u32 url_count;
    al_iptv_play_origin_item_t *play_origin_info;
} al_iptv_play_origin_info_t;


typedef struct pp_info
{
	int id;
	char name[XM_TAG_MAX];//播放地址名
	int hdtv;//播放码率
	char playUrl[XM_LEN_URL];//播放地址
	char timeshiftUrl[XM_LEN_URL];

    char  tvQid[IPTV_STRING_ID_LEN];                //剧集奇谱ID
    char  vid[IPTV_STRING_ID_LEN];                   //播放码流id
}al_iptv_play_url_info_t;

//////////////////////////////////////////////////
typedef struct
{
    u16 total_url;
    al_iptv_play_url_info_t  *pp_urlList;
} al_iptv_play_url_list_t;
///////////////////////////////////////////////////




//////////////////////////////////////////////////
typedef struct
{
    u16 *word;
}al_iptv_word_name_t;

typedef struct
{
    u8  total_word;
    al_iptv_word_name_t  *word_list;
} al_iptv_search_word_t;
//////////////////////////////////////////////////




void ui_iptv_free_msg_data(u16 msg, u32 para1, u32 para2);
inline u16 ui_iptv_get_dp_state(void);
inline void ui_iptv_set_dp_state(u16 state);
void ui_iptv_dp_init(void);
void ui_iptv_dp_deinit(void);
void ui_iptv_dp_del(void);
void ui_iptv_register_msg(void);
void ui_iptv_unregister_msg(void);
void ui_iptv_get_res_list(void);
void ui_iptv_get_catgry_list(void);
void ui_iptv_get_video_list(u32 res_id, u16 *cat_name, u8 *key, u32 page_num);
void ui_iptv_get_video_list_for_filter(u32 res_id, u16 *cat_name, u8 *key, u32 page_num);
void ui_iptv_search_set_res_id(u32 res_id);
void ui_iptv_search_set_keyword(u16 *keyword);
void ui_iptv_search_word(void);
void ui_iptv_search(u32 page_num, u16 index);
void ui_iptv_get_video_info(VDO_ID_t * vdo_id, u8 cat_id);
void ui_iptv_get_info_url(VDO_ID_t * vdo_id, u8 cat_id, u16 *origin, u32 page_num, u32 page_size);
void ui_iptv_get_recmnd_info(VDO_ID_t * vdo_id, u32 res_id, u16 *area);
void ui_iptv_get_play_url(IPTV_API_FORMAT_T format,u8 *tvQid,u8 *vid,int episode_num);
void ui_iptv_get_play_format(u8 *tvQid,u8 *vid,int episode_num);
void ui_iptv_set_page_size(u16 page_size);
int ui_iptv_vdo_idntfy_cmp(u32 unidntf_code);
void ui_iptv_dp_set_iptvId(u8 iptv_id);
inline iptv_module_id_t ui_iptv_dp_get_iptvId(void);
inline const VodPlayerInterface_t * ui_iptv_get_player_instance(void);

extern volatile u32 vdo_identify_code;

#endif
