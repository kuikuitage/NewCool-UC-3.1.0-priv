/********************************************************************************************/
/********************************************************************************************/
#ifndef __VOD_INTERFACE__H__
#define __VOD_INTERFACE__H__
#ifdef __cplusplus
extern "C" {
#endif

#include "commonData.h"
#include "common.h"
#include "file_playback_sequence.h"

#define IPTV_STRING_LONG     (2048)
#define IPTV_STRING_SHORT   (128)
#define IPTV_STRING_ID_LEN  (48)

#define IPTV_MAX_FORMAT_COUNT   (6)

#define MAX_KEYWORD_LENGTH                        (32)

typedef  enum{

	GET_IPTV_CATEGORY,
	GET_IPTV_AREA,
	GET_IPTV_CATEGORY_TYPE,
	GET_IPTV_NEW_PAGE,
	GET_IPTV_RECOMMEND_INFO,
	GET_IPTV_VIDEO_INFO,
	GET_IPTV_PLAY_URL,
	GET_IPTV_FORMAT_TYPE,
	GET_IPTV_PLAYBACK_INFO,
	GET_IPTV_INIT ,///////add for IIPTV_initDataProvider to a thread 
	GET_IPTV_DEINIT,
	GET_IPTV_SEARCH_WORD,//fuxl add for IIPTV_getSearchWord to search operate

	GET_IPTV_INVALID_TYPE,

}IPTV_VIDEO_CMD_TYPE;

typedef enum {
	IPTV_DATA_FAIL=0,
	IPTV_DATA_SUCCESS=1,
	IPTV_DATA_MAX,  
}IPTV_DATA_STATUS;



/* video ID*/
typedef struct VDO_ID_S{
	int	   	res_id;
	int 	cat_id;
	int     program_id;
	
	char    qpId[IPTV_STRING_ID_LEN];                 	//专辑奇谱（视频/专辑/来源）ID for iqy
	char    tvQid[IPTV_STRING_ID_LEN];                	//剧集奇谱ID for iqy
	char    sourceCode[IPTV_STRING_SHORT];   			//来源code
	int     type;                               		//专辑类型（0：视频，1：专辑）调用此接口传进来的那个qpId的数据类型，不是返回结果中的qpId的数据类型
}VDO_ID_t;//external ID


/*
* Return value:
*       zero:  do it
*       none-zero:  ignore it
*/
typedef int (*ignore_callback)(u32 para);

typedef enum
{
    IPTV_QUERY_MODE_CATGRY,
    IPTV_QUERY_MODE_PINYIN,
    IPTV_QUERY_MODE_SCREENING,
} IPTV_QUERY_MODE_T;

typedef enum
{
    IPTV_PAGE_SORT_DEFAULT,
    IPTV_PAGE_SORT_NEWEST,
    IPTV_PAGE_SORT_HOTTEST,
    IPTV_PAGE_SORT_SCORE,
    IPTV_PAGE_SORT_ROMDAM,
}IPTV_PAGE_SORT_TYPE;

typedef enum
{
    IPTV_EVENT_INVAILID,
    IPTV_INIT_SUCCESS,
    IPTV_INIT_FAIL,
    IPTV_CATEGORY_ARRIVAL,
    IPTV_AREA_INFO_ARRIVAL,
    IPTV_CATEGORY_TYPE_ARRIVAL,
    IPTV_NEW_PAGE_ARRIVAL,
    IPTV_VIDEO_INFO_ARRIVAL,
    IPTV_PLAY_URL_ARRIVAL,
    IPTV_FORMAT_TYPE_ARRIVAL,
    IPTV_PLAYBACK_INFO_ARRIVAL,
    IPTV_RECOMMEND_INFO_ARRIVAL,
    IPTV_SEARCH_WORD_ARRIVAL,//fuxl add for search word
    IPTV_DEINIT_SUCCESS,
}IPTV_EVENT_TYPE;


typedef enum
{
	IPTV_API_FORMAT_NORMAL 	= 1,
	IPTV_API_FORMAT_HIGH 		,
	IPTV_API_FORMAT_SUPER 	,
	IPTV_API_FORMAT_TOPSPEED ,
	IPTV_API_FORMAT_720P 		 ,
	IPTV_API_FORMAT_1080P 	 ,
	IPTV_API_FORMAT_DEFAULT 	,
}IPTV_API_FORMAT_T;


/*
* the parameter of IIPTV_updatePage()
*/
typedef struct{
	int 					cat_id;//一级分类ID
	int 					cat_id2;// 二级分类ID
	char 					tags[IPTV_STRING_LONG];
	/*
	* used for search, co-operate with types
	*/
	char 					keys[IPTV_STRING_SHORT];
	char 					types[IPTV_STRING_SHORT];  // name
	/*
	*title first char "rzdf" = "让子弹飞"
	*/
	char 					pinyin[IPTV_STRING_SHORT];
	u16                               searchWordIndex;

	u32 					identify;
	IPTV_QUERY_MODE_T	query_mode;
	ignore_callback 		cb;


	// not use
	int 		years;
	char 		names[IPTV_STRING_LONG];
	char 		title[IPTV_STRING_LONG];
	char 		area[IPTV_STRING_SHORT];

	/*
	*only one of "最新", "热播","推荐"
	*/
	char 		attr[IPTV_STRING_SHORT];

	char		 actor[IPTV_STRING_SHORT];
	char 		director[IPTV_STRING_SHORT];

	IPTV_PAGE_SORT_TYPE sort;

	int 		page_index;
	int 		page_size;
}IPTV_UPPAGE_REQ_T;


/*
* the parameter of IIPTV_getRecommendInfo()
*/
  typedef struct{
      VDO_ID_t id;
      int cat_id;
      int page_index;
      int page_size;
      
      char area[IPTV_STRING_SHORT];
      char types[IPTV_STRING_SHORT];  

      ignore_callback cb;
      u32 identify;
  }IPTV_RECOMMEND_REQ_T;


/*
* the parameter of IIPTV_getVideoInfo()
*/
typedef struct{

	int  cat_id;
	VDO_ID_t id; 

	/*
	* such as "搜狐", "优酷"
	*/
	char origin[IPTV_STRING_SHORT];

	int page_index;
	int page_size;

	/*
	* is_description
	*   1: return program description without url
	*   0: return description and urls if they exist
	*/
	int is_description;
	u32 identify;
	ignore_callback cb;
    u32 query_mode;
}IPTV_VIDEO_INFO_REQ_T;


/*
* the parameter of IIPTV_getPlayUrl()
*/
typedef struct{
	VDO_ID_t id; 

	/*
	* a program's website url, some data provider use 
	* this url to get video play url
	*/
	char    tvQid[IPTV_STRING_ID_LEN];                //剧集奇谱ID
	char    vid[IPTV_STRING_ID_LEN];                   //播放码流id

	int  	episode_num;

	/*
	* definition: normal, high, super
	*/
	int 	format;

	ignore_callback cb;

	u32 identify;
}IPTV_VIDEO_URL_REQ_T;

typedef s32  (*iptv_event_callback)(IPTV_EVENT_TYPE event, u32 param);

typedef void  	(*FunInit)(DO_CMD_TASK_CONFIG_T * p_config);
typedef int  	(*FunDeinit)( void);
typedef int  	(*FunDelCmdTask)(void);
typedef int   	(*FunGetCategoryList)(u32 identify);
/*cat_id: 0 means all types in all categary*/
typedef int   	(*FunGetCategoryTypeList)(int cat_id, u32 identify);
typedef int  	(*FunUpdatePage)(IPTV_UPPAGE_REQ_T  *p_req);
typedef int  	(*FunGetRecommendInfo)(IPTV_RECOMMEND_REQ_T *p_req);
typedef int  	(*FunGetVideoInfo)(IPTV_VIDEO_INFO_REQ_T *p_req);
typedef int  	(*FunGetPlayUrl)(IPTV_VIDEO_URL_REQ_T *p_req);
typedef int  	(*FunGetPlayUrlFormat)(IPTV_VIDEO_URL_REQ_T *p_req);
typedef int  	(*FunSetPageSize)(int pagesize);
typedef int 	(*FunGetArea)(u32 identify);
typedef BOOL (*FunCmpId)(VDO_ID_t *id,VDO_ID_t *id2);
typedef int   	(*FunGetSearchWord)(u8 *searchWord);
typedef void 	(*FunRegisterCallback)(void * p_cb_func);


typedef  struct vod_interface_s{
	FunInit 			init; 
	FunDeinit			deinit;
	FunDelCmdTask	delCmdTask;
	FunGetCategoryList getCategoryList;
	FunGetCategoryTypeList	getCategoryTypeList;
	FunUpdatePage	updatePage;
	FunGetRecommendInfo getRecommendInfo;
	FunGetVideoInfo	getVideoInfo;
	FunGetPlayUrl		getPlayUrl;
	FunGetPlayUrlFormat getPlayUrlFormat;
	FunSetPageSize	setPageSize;
	FunGetArea		getArea;
	FunGetSearchWord	getSearchWord;
	FunCmpId		cmpId;
	FunRegisterCallback registerCb;
}VodDpInterface_t;





typedef struct video_url_info
{
    char  tvQid[IPTV_STRING_ID_LEN];                //剧集奇谱ID
    char  vid[IPTV_STRING_ID_LEN];                   //播放码流id
}video_url_info_t;



typedef void  	(*FunVoidVoid)( void);
typedef int  	(*FunIntVoid)( void);
typedef u32  	(*FunUintVoid)( void);
typedef BOOL (*FunBoolVoid)( void);
typedef u8  	(*FunU8Void)( void);
typedef int  	(*FunIntU8)(u8);
typedef int  	(*FunIntInt)(int);
typedef int (*FunPlayUrlEx)(video_url_info_t *pp_url_list, u32 url_cnt, u32 seek_play_time, int vd);
typedef int (*FunPlayUrl)(video_url_info_t *p_net_url, u32 seek_play_time,int vd);
typedef void (*FunGetResolution)(rect_size_t *p_rect);
typedef int (*FunSeek)(s32 time_sec);

typedef  struct vod_player_interface_s{
	FunIntU8  		create;
	FunIntVoid		destory;
	FunPlayUrlEx		playUrlEx;
	FunPlayUrl		playUrl;
	FunIntVoid		videoStop;
	FunIntVoid		pause_resume;
	FunGetResolution	getResolution;
	FunUintVoid		getCurrentPlayTime;
	FunIntVoid		getPlayState;
	FunIntVoid		getCurrentFormat;
	FunUintVoid		getTotalTimeBySec;
	FunSeek			seek;
	FunBoolVoid		ifPlayingAd;
	FunUintVoid		getTotalAdTime;
}VodPlayerInterface_t;



//common functions 
/*u must malloc output buffer before call the function  */
int iptv_common_porting_http_get(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char* buffer,              //response
                     unsigned int bufferSize,
                     BOOL if_http_gzip);


int iptv_common_http_get_with_malloc_internal(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char** out_buffer,              //response
                     unsigned int *out_bufferSize,
                     BOOL if_http_gzip);


int iptv_common_http_get_with_malloc_internal_with_no_loop(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char** out_buffer,              //response
                     unsigned int *out_bufferSize,
                     BOOL if_http_gzip);

///
int iptv_common_http_post_with_malloc_internal(const char* url,
                                           const char* postBody,
                                           unsigned int postBodyLen,
                                           const char* extraHeaders,
                                           unsigned int timeout,
                                           char** out_buffer,              //response
                                           unsigned int *out_bufferSize,
                                           BOOL if_http_gzip);

#ifdef __cplusplus
}
#endif

#endif
