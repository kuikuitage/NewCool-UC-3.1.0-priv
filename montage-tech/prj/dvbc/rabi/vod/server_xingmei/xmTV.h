#ifndef XMTVCWRAPPER_H_INCLUDED
#define XMTVCWRAPPER_H_INCLUDED
#include "gx_jansson.h"
#include "sys_types.h"
#include "lib_util.h"



/// /////////////////////////////////////////////////////////////
///                         error code
/// /////////////////////////////////////////////////////////////
#define     XM_NORMAL                   0      //
#define     XM_ERROR_PARAM              1      //
#define     XM_ERROR_SYSTEM             2      //system error
#define     XM_ERROR_BUSY               3      //system busy
#define     XM_ERROR_DISABLE_CHG_STATUS 4      //status change disable
#define     XM_ERROR_DEVICE_ERROR       5
#define     XM_ERROR_BUFFERING          6
#define     XM_ERROR_OPERATE_DISABLE    7
#define     XM_ERROR_PLAY_CHECK_ERROR   8


#define     XM_LEN_URL                  256    //url长度
#define     XM_TAG_MAX                  32     //最大能取得的tag个数        max = 32 ?
#define 	 XM_ITEM_MAX_NUM		    50
#define     XM_DESC_LEN                  256    //url长度
#define 	 XM_CATEGORY_MAX_NUM		    50







//2.2
//栏目(二级)
typedef struct st_XM_category_C
{
	int id;//l栏目id
	char type[XM_TAG_MAX];//栏目类型vod ,live
	char name[XM_TAG_MAX];//栏目名称
	char subCategoryUrl[XM_LEN_URL];//子栏目接口
	char programListUrl[XM_LEN_URL];//节目列表接口
	char hpbcUrl[XM_LEN_URL];//栏目下的热门节目影片节目或频道节目
} XM_tag;

//-------------------------------------------------------------


//2.1首页信息接口
//一级栏目
typedef struct st_XM_Channel_C
{
	int  id;			//栏目id
	char  type[XM_TAG_MAX]; 		//栏目类型vod ,live
	char name[XM_TAG_MAX];//栏目名称
	char subCategoryUrl[XM_LEN_URL];//子栏目接口

	int    tagSize; //tag size 子栏目个数
	XM_tag tags[XM_TAG_MAX];
} XM_Channel;


//海报
typedef struct st_XM_hotest_C
{
	int id;//l栏目id
	char name[XM_TAG_MAX];//栏目名称
	char type[XM_TAG_MAX];//栏目类型vod ,live
	char image[XM_LEN_URL];//
	char detailUrl[XM_LEN_URL];//子栏目接口
} XM_hotest;

typedef struct st_XM_firstrecommend_C
{
	int id;//l栏目id
	char name[XM_TAG_MAX];//栏目名称
	char type[XM_TAG_MAX];//栏目类型vod ,live
	char image[XM_LEN_URL];//
	char actor[XM_TAG_MAX];//演员
	char director[XM_TAG_MAX];//导演
	char detailUrl[XM_LEN_URL];//子栏目接口
} XM_firstrecommend;

//首页
typedef struct st_XM_homepage_C
{
	char entry_url[XM_LEN_URL];

	int category_size;
	XM_Channel firstcategory[XM_CATEGORY_MAX_NUM];


	
	int hotest_size;
	XM_hotest hotest[XM_ITEM_MAX_NUM];
	int firstrecommend_size;
	XM_firstrecommend firstrecommend[XM_ITEM_MAX_NUM];
	char searchUrl[XM_LEN_URL];//子栏目接口
	char textUrl[XM_LEN_URL];
	char favouritesUrl[XM_LEN_URL];
	char bookmarkUrl[XM_LEN_URL];
	char orderProductUrl[XM_LEN_URL];
	char hotprogramsUrl[XM_LEN_URL];
	char orderHistoryUrl[XM_LEN_URL];
	char hotChannelUrl[XM_LEN_URL];
} XM_homepage;

//-------------------------------------------------------------

//2.3
//影片列表
typedef struct st_XM_mov_C
{
	int id;//节目id
	char name[XM_TAG_MAX];//节目名称
	char type[XM_TAG_MAX];//节目类型
	char image[XM_LEN_URL];//节目图片
	char detailUrl[XM_LEN_URL];//节目详情接口
} XM_mov;

typedef struct st_XM_movlist_C
{
	int totalNum;//栏目下节目总数
	int currStartIdx;//查询节目时开始位置
	int validCount;// 数组中有效的
	XM_mov data[XM_ITEM_MAX_NUM];//
	
} XM_movlist;


typedef struct st_XM_movLabelInfo_C
{
    char name[XM_TAG_MAX];
    char plid[XM_TAG_MAX];
} XM_movLabelInfo;


typedef struct st_XM_ChannelLabelList_C
{
    int size;
    XM_movLabelInfo labelList[XM_CATEGORY_MAX_NUM];
} XM_ChannelLabelList;


//-------------------------------------------------------------
typedef struct st_XM_series_C
{
	int id;
	char name[XM_TAG_MAX]; //多少集第几集
	int bookmarktime;//书签播放
	int duration;//播放时长
	char bookmarkUrl[XM_LEN_URL];//书签取消添加url
	char terminalStateUrl[XM_LEN_URL]; //播放链接
} XM_series;

typedef struct st_XM_seriesList_C
{
	int 	total;								//all专辑总数
	int	  size; 							  //取得的专辑总数
	XM_series data[XM_ITEM_MAX_NUM];		  //专辑基本信息数组 
} XM_seriesList;


//产品对象
typedef struct st_XM_product_C
{
	int id;//节目id
	char billingMode[XM_TAG_MAX];//计费模式
	char billingtype[XM_TAG_MAX];//计费对象
	char name[XM_TAG_MAX];//名称
	int producttype;//产品类型
	int purchasetype;//计费方式
	int listprice;//费用
	int rentalterm;//产品租期
	int rentaltimes;//产品租用次数
	utc_time_t startTime;
	utc_time_t endTime;
	char description[XM_DESC_LEN];//产品描述
	
} XM_product;


//2.4
//影片详情
typedef struct st_XM_detail_C
{
	int id;//节目id
	char name[XM_TAG_MAX];//节目名称
	char type[XM_TAG_MAX];//节目类型program series
	char image[XM_LEN_URL];//节目缩略图片
	char actor[XM_TAG_MAX];//节目演员
	char director[XM_TAG_MAX];//导演
	int duration;//节目播放时长
	char originalcountry[XM_TAG_MAX];//拍摄地
	char description[XM_DESC_LEN];//节目介绍
	int releaseyear;//上映年份
	char terminalStateUrl[XM_LEN_URL];//播放地址
	char recommandUrl[XM_LEN_URL];//推荐接口url
	XM_seriesList seriesList;//剧集列表
	XM_product	product;//产品对象
	int orderProduct;//用户和产品间的状态
	int bookmarktime;//书签播放时间
	int favorites;//有没有收藏节目
	char favoriteUrl[XM_LEN_URL];//添加取消收藏地址
	char bookmarkUrl[XM_LEN_URL];//添加取消书签地址
	
} XM_detail;




//-------------------------------------------------------------

//2.5相关推荐
typedef struct st_XM_recommand_C
{
	int id;//节目id
	char name[XM_TAG_MAX];//节目名称
	char type[XM_TAG_MAX];//节目类型
	char image[XM_LEN_URL];//节目图片
	char detailUrl[XM_LEN_URL];//节目详情接口
} XM_recommand;

typedef struct st_XM_RecmdList_C
{							
	int	  num; 						
	XM_recommand data[XM_ITEM_MAX_NUM];	
} XM_RecmdList;


//-------------------------------------------------------------


//剧集详情连续剧与电影共用
#if 0
typedef struct st_XM_seriesDetail_C
{
	int id;
	char Name[20]; //电视剧名
	int type; //类型 series
	char actor[20]; //演员
	char director[20];//导演
	int duration; //时长
	char originalcountry[20]; //地区
	char description[1024]; //剧情介绍
	char image[XM_LEN_URL]; //海报链接
	char playUrl[XM_LEN_URL]; //码流链接
	char recommendUrl[XM_LEN_URL]; //推荐链接
	XM_seriesList seriesList[XM_ITEM_MAX_NUM]; //连续剧剧集列表

} XM_SeriesDetail;
#endif 

//-------------------------------------------------------------

//2.7播放地址
typedef struct st_XM_urlobj_C
{
	int id;
	char name[XM_TAG_MAX];//播放地址名
	int hdtv;//播放码率
	char playUrl[XM_LEN_URL];//播放地址
	char timeshiftUrl[XM_LEN_URL];
} XM_urlobj;

typedef struct st_XM_play_urlList_C
{
	int terminalState;//0空闲1在播
	int num;
	XM_urlobj urlobj[100];//播放地址集合
	//char timeshiftUrl[XM_LEN_URL];//频道时移播放地址
} XM_play_urlList;

//-------------------------------------------------------------

//2.9
//节目单
typedef struct st_XM_schedule_C
{
	char name[XM_TAG_MAX];//节目名称
	utc_time_t start_time;
	utc_time_t end_time;
} XM_schedule;

//2.8
//直播列表
typedef struct st_XM_channelList_C
{
	int id;//频道id
	char Name[XM_TAG_MAX]; //频道名称
	int number; //频道号
	char image[XM_LEN_URL];//缩略图url
	char playUrl[XM_LEN_URL];//	terminalStateurl播放地址查找
	char timeshiftUrl[XM_LEN_URL];//频道信息cbidUrl  ? 时移url
	int timeshiftLength;//频道时移时长
	int storageLength;//频道存储时长
	char scheduleListUrl[XM_LEN_URL];//频道节目单接口url
	XM_schedule currentSchedule;//当前节目单信息
	XM_schedule nextSchedule;//下个节目单信息
	
} XM_channelList;

//-------------------------------------------------------------


typedef enum
{
	NC_AUTH_UNKNOW				= 0,
	NC_AUTH_REGISTERED 			= 1,
	NC_AUTH_LOGIN				= 2,
	NC_AUTH_LOGOUT				= 3,
	NC_AUTH_BEATING_FAILURE		= 4,
}NC_STATUS_t;



void NC_AuthStart(void);
void NC_AuthClose(void);
int NC_AuthInit(const char *stbId,const unsigned char* mac,int mac_len);
NC_STATUS_t NC_AuthGetState(void);

/*for dataprocess */
GxJson_t XM_HomeRequest(void);
GxJson_t XM_CateGoryRequest(const char * domain);
GxJson_t XM_ProgramListRequest(const char * domain,unsigned int start, unsigned int total);
GxJson_t XM_ProgramDetailRequest(const char * domain);
GxJson_t XM_ProgramRecommendUrlRequest(const char * domain);
GxJson_t XM_ProgramPlayInfoRequest(const char * domain);


#endif

