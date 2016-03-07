#ifndef  __I11IFUNNYVIDEO_DATA_PROVIDER__
#define  __I11IFUNNYVIDEO_DATA_PROVIDER__

#ifdef __cplusplus
extern "C" {
#endif 
#include "iptv_interface.h"
#include "xmTV.h"



typedef struct{
    /*
    * the number of category in array
    */
    int total;

    /*
    * array of category
    */
	XM_Channel *array;
    /*
    * status of this request
    */
    IPTV_DATA_STATUS status;

    u32 identify;
}XM_CATEGORY_LIST_T;

/*
* XM_CATEGORY_TYPE_T is used to describe one category type(such as 爱情，动作)
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

}XM_CATEGORY_TYPE_T;

/*
* XM_CATEGORY_TYPE_LIST_T describe all types in a category
*/
typedef struct{

    /*
    * describe the size of array below
    */

    int 	cat_id;

    char 	*name;

    int	item_count;


    XM_CATEGORY_TYPE_T *array;


}XM_CATEGORY_TYPE_LIST_T;


/*
*XM_CATEGORY_TYPE_ALL_T describe all category's types for a data provider
*/
typedef struct{

    /*
    * describe the size of p_list below
    */
    XM_CATEGORY_LIST_T 	*category;
      
    IPTV_DATA_STATUS 		status;

    u32 					identify;
}XM_CATEGORY_TYPE_ALL_T;


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
  * describe the size of array of the vdo
  */
  int number;
  XM_mov *array;

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

}XM_CHANNEL_LIST_T;


typedef struct{

	//播放地址名
	char name[20];

	//播放码率
	int hdtv;

	//播放地址
	char playUrl[256];

	//频道时移地址
	char timeshiftUrl[256];

	//used for uplayers
	u32 identify;
}XM_CHANNEL_URL_T;

typedef struct{
	int total;
	int format[IPTV_MAX_FORMAT_COUNT];
}XM_VIDEO_FORMAT_T;

typedef struct{

    int id;
    int cat_id;
    int episode_total;
    int this_page_index;
    int this_page_size;
   
    XM_VIDEO_FORMAT_T   format; 

    XM_detail * mov_detail;	  
    /*
    * 
    */
    int is_description;

    IPTV_DATA_STATUS status;

    u32 identify;
    u32 query_mode;

}XM_CHANNEL_INFO_T;


typedef struct{

    int number;
    
	//XM_CHANNEL_URL_T *playurls;
    XM_urlobj * playurls;
    int terminalState;	

    u32 format;

    IPTV_DATA_STATUS status;

    u32 identify;
}XM_CHANNEL_PLAY_URL_T;


/*
*XM_RECMD_CHANNEL_T describes recommend information
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

}XM_RECMD_CHANNEL_T;


/*
* XM_RECMD_INFO_T describes a program's all recommands information
*/
#define  RECMD_MAX_NUMBER		50
typedef struct{
    VDO_ID_t id;

	int channel_num;

	int pagecount;

	int totalcount;

	int have_geted;

	XM_recommand  *recmd;

	IPTV_DATA_STATUS status;

	u32 identify;
  
}XM_RECMD_INFO_T;


typedef  struct{
   int id;
   char  * name;
   int status;
   int sort;
}XM_AREA_INFO_T;


/*
*XM_AREA_INFO_LIST_T describes all area infomation of a data provider
*/
typedef struct{

    /*
    * the number of members in array below
    */
    int number;

    /*
    * area arrayes
    */
    XM_AREA_INFO_T *array;

    /*
    * the status of this command
    */
    IPTV_DATA_STATUS status;

    u32 identify;
}XM_AREA_INFO_LIST_T;

typedef struct{
    /*
    * the status of this command
    */
    IPTV_DATA_STATUS status;

}XM_SEARCH_WORD_T;



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

    XM_VIDEO_FORMAT_T format;
    IPTV_PAGE_SORT_TYPE sort;
	int page_index;
	int page_size;

    int is_description;


    ignore_callback cb;
    u32 identify;
    u32 query_mode;
} XM_REQ_PARAM_T;


typedef XM_CATEGORY_LIST_T *   (*get_category_list_func)(void * hdl);
typedef XM_CATEGORY_TYPE_ALL_T *   (*get_category_type_list_func)(void * hdl, int cat_id);
typedef XM_CHANNEL_LIST_T *   (*update_page_func)(void * hdl, IPTV_UPPAGE_REQ_T * p_req);
typedef XM_RECMD_INFO_T *   (*get_recommend_info_func)(void * hdl, IPTV_RECOMMEND_REQ_T * p_req);
typedef XM_CHANNEL_INFO_T *   (*get_video_info_func)(void * hdl, IPTV_VIDEO_INFO_REQ_T * p_req);
typedef XM_CHANNEL_PLAY_URL_T *   (*get_playurl_func)(void * hdl, IPTV_VIDEO_URL_REQ_T *p_req);
typedef XM_VIDEO_FORMAT_T * 	   (*get_playurl_format_func)(void * hdl, IPTV_VIDEO_URL_REQ_T *p_req);
typedef XM_AREA_INFO_LIST_T *   (*get_area_func)(void * hdl);
typedef XM_SEARCH_WORD_T *   (*get_search_word_func)(void * hdl, void * p_req);


typedef struct {

	XM_CATEGORY_LIST_T  			* category_list;
   	XM_CATEGORY_TYPE_ALL_T 			* category_type_all;	
	XM_CHANNEL_LIST_T    			* channel_list;
	XM_CHANNEL_INFO_T   			* channel_info;
	XM_CHANNEL_PLAY_URL_T    		* channel_play_url;
	XM_RECMD_INFO_T 				* recommend_info;
	XM_AREA_INFO_LIST_T 			* area_info_list;

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
	get_category_list_func  				get_category_list;
	
    get_category_type_list_func 			get_category_type_list;

	update_page_func  					update_page;

	get_recommend_info_func 			get_recommend_info;

	get_video_info_func 				get_video_info;

	get_playurl_func 					get_playurl;

	get_playurl_format_func 				get_playurl_format;

	get_area_func 						get_area;

	get_search_word_func					get_search_word;
}XM_DP_HDL_T;

const VodDpInterface_t* GetXingMeiDpInterface(void);
#ifdef __cplusplus
}
#endif 





#endif
