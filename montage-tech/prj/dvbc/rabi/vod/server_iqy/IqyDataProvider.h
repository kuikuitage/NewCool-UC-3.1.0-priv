#ifndef  __IIFUNNYVIDEO_DATA_PROVIDER__
#define  __IIFUNNYVIDEO_DATA_PROVIDER__

#ifdef __cplusplus
extern "C" {
#endif 
#include "iqiyiTV_C.h"
#include "iptv_interface.h"

#define IQY_TYPE_MOVE 			(0)
#define IQY_TYPE_VARIETY		(1)
#define IQY_TYPE_TV				(2)


typedef struct{
    /*
    * the number of category in array
    */
    int total;

    /*
    * array of category
    */
    IQY_Channel   *array;

    /*
    *  array of category's types
    */
    IQY_ChannelLabelList *labels;
    /*
    * status of this request
    */
    IPTV_DATA_STATUS status;

    u32 identify;
}IQY_CATEGORY_LIST_T;

/*
* IQY_CATEGORY_TYPE_T is used to describe one category type(such as 爱情，动作)
*/
typedef struct{
    int id;

    /*
    * which category it belongs to
    */
    int catoryid;

    /*
    * charaters being used for displaying in UI
    */
    char *title;

    /*
    * keys this type belongs to, mainly used to search
    */
    char *keys;

    int sort;

    /*
    * total programs in this category type
    */
    int total;

}IQY_CATEGORY_TYPE_T;

/*
* IQY_CATEGORY_TYPE_LIST_T describe all types in a category
*/
typedef struct{

    /*
    * describe the size of array below
    */

    int 	cat_id;

    char 	*name;

    int	item_count;


    IQY_CATEGORY_TYPE_T *array;


}IQY_CATEGORY_TYPE_LIST_T;


/*
*IQY_CATEGORY_TYPE_ALL_T describe all category's types for a data provider
*/
typedef struct{

    /*
    * describe the size of p_list below
    */
    IQY_CATEGORY_LIST_T 	*category;
      
    IPTV_DATA_STATUS 		status;

    u32 					identify;
}IQY_CATEGORY_TYPE_ALL_T;


typedef struct{
  /*
  * page count of this category calculate by current page size
  */
  int pagecount;

  /*
  *all programs count in this category
  */
  int totalcount;
  
  /*
  * describe the size of array
  */
  int number;
  IQY_AlbumInfo *array;
  
  IPTV_DATA_STATUS status;
  
  /*
  * being used when retriving data to up layer 
  * return just as parameters in
  */
  int category_id;
  int page_num;
  int page_size;

  u32 identify;
  u32 query_mode;

}IQY_CHANNEL_LIST_T;


typedef struct{

    /*
    *  url
    */
    char    tvQid[IQY_LEN_QPID];                //剧集奇谱ID
    char    vid[IQY_LEN_VID];                   //播放码流id

    /*
    * title
    */
    char urltitle[IQY_LEN_EPISODE_ITEM1];

    //used for uplayers
    u32 identify;
}IQY_CHANNEL_URL_T;

typedef struct{
	int total;
	int format[IPTV_MAX_FORMAT_COUNT];
}IQY_VIDEO_FORMAT_T;

typedef struct{

    int id;
    int cat_id;
    IQY_AlbumInfo	* album;
    int episode_total;
    int this_page_index;
    int this_page_size;
    IQY_Episode 	* episodes; // this page;
    IQY_VIDEO_FORMAT_T   format; 
	  
    /*
    * 
    */
    int is_description;

    IPTV_DATA_STATUS status;

    u32 identify;
    u32 query_mode;

}IQY_CHANNEL_INFO_T;


typedef struct{

    int number;
    
    IQY_CHANNEL_URL_T *playurls;

    u32 format;

    IPTV_DATA_STATUS status;

    u32 identify;
}IQY_CHANNEL_PLAY_URL_T;


/*
*IQY_RECMD_CHANNEL_T describes recommend information
*/
typedef struct{
      VDO_ID_t id;

      char *title;

      int cat_id;

      char *img;

      char *types;

      char *score;

      char *currentcollect;

      char *clarity;      

}IQY_RECMD_CHANNEL_T;


/*
* IQY_RECMD_INFO_T describes a program's all recommands information
*/
#define  RECMD_MAX_NUMBER		50
typedef struct{
       VDO_ID_t id;

	int channel_num;

	int pagecount;

	int totalcount;

	int have_geted;

	IQY_AlbumInfo	* albums;

	IPTV_DATA_STATUS status;

	u32 identify;
  
}IQY_RECMD_INFO_T;


typedef  struct{
   int id;
   char  * name;
   int status;
   int sort;
}IQY_AREA_INFO_T;


/*
*IQY_AREA_INFO_LIST_T describes all area infomation of a data provider
*/
typedef struct{

    /*
    * the number of members in array below
    */
    int number;

    /*
    * area arrayes
    */
    IQY_AREA_INFO_T *array;

    /*
    * the status of this command
    */
    IPTV_DATA_STATUS status;

    u32 identify;
}IQY_AREA_INFO_LIST_T;

typedef struct{
    /*
    * the status of this command
    */
    IPTV_DATA_STATUS status;

    IQY_SearchWord *search;
}IQY_SEARCH_WORD_T;



typedef struct{

    int id;
	int cat_id;
    int years;
    int month;
    int date;
    char names[IPTV_STRING_LONG];
    char title[IPTV_STRING_SHORT];
    char area[IPTV_STRING_SHORT];

    char actor[IPTV_STRING_SHORT];
    char director[IPTV_STRING_SHORT];
    char types[IPTV_STRING_SHORT];
    char origin[IPTV_STRING_SHORT];
    char url[IPTV_STRING_LONG];
    char pinyin[IPTV_STRING_SHORT];
    char attr[IPTV_STRING_SHORT];
    char keys[IPTV_STRING_SHORT];

    IQY_VIDEO_FORMAT_T format;
    IPTV_PAGE_SORT_TYPE sort;
	int page_index;
	int page_size;

    int is_description;


    ignore_callback cb;
    u32 identify;
    u32 query_mode;
} IQY_REQ_PARAM_T;


typedef IQY_CATEGORY_LIST_T *   (*get_category_list_func)(void * hdl);
typedef IQY_CATEGORY_TYPE_ALL_T *   (*get_category_type_list_func)(void * hdl, int cat_id);
typedef IQY_CHANNEL_LIST_T *   (*update_page_func)(void * hdl, IPTV_UPPAGE_REQ_T * p_req);
typedef IQY_RECMD_INFO_T *   (*get_recommend_info_func)(void * hdl, IPTV_RECOMMEND_REQ_T * p_req);
typedef IQY_CHANNEL_INFO_T *   (*get_video_info_func)(void * hdl, IPTV_VIDEO_INFO_REQ_T * p_req);
typedef IQY_CHANNEL_PLAY_URL_T *   (*get_playurl_func)(void * hdl, IPTV_VIDEO_URL_REQ_T *p_req);
typedef IQY_VIDEO_FORMAT_T * 	   (*get_playurl_format_func)(void * hdl, IPTV_VIDEO_URL_REQ_T *p_req);
typedef IQY_AREA_INFO_LIST_T *   (*get_area_func)(void * hdl);
typedef IQY_SEARCH_WORD_T *   (*get_search_word_func)(void * hdl, void * p_req);


typedef struct {

	IQY_CATEGORY_LIST_T  			* category_list;
   	IQY_CATEGORY_TYPE_ALL_T 		* category_type_all;	
	IQY_CHANNEL_LIST_T    			* channel_list;
	IQY_CHANNEL_INFO_T   			* channel_info;
	IQY_CHANNEL_PLAY_URL_T    		* channel_play_url;
	IQY_RECMD_INFO_T 				* recommend_info;
	IQY_AREA_INFO_LIST_T 			* area_info_list;

	void  							* p_bg_task_hdl;
	void							* bg_task_stack_addr;

	register_event_cb_func_t    	register_event_callback;
	iptv_event_callback 			event_callback;

	int 							page_size;

	IPTV_DATA_PROVIDER  			dp;
	u32                         	sub_dp;

	/*
	* the private data of each data provider can be placed here
	*/
	void 							* priv;

	/*
	*
	*/
	char 							* mac;

	char 							* id;

	int (*read_flash)(void *start, int len);

	int (*write_flash)(void *start, int len);

	/*
	*
	*/
	get_category_list_func  			get_category_list;
	
    get_category_type_list_func 		get_category_type_list;

	update_page_func  					update_page;

	get_recommend_info_func 			get_recommend_info;

	get_video_info_func 				get_video_info;

	get_playurl_func 					get_playurl;

	get_playurl_format_func 			get_playurl_format;

	get_area_func 						get_area;

	get_search_word_func				get_search_word;
}IQY_DP_HDL_T;

const VodDpInterface_t* GetIqyDpInterface(void);
#ifdef __cplusplus
}
#endif 





#endif
