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


#define     XM_LEN_URL                  256    //url����
#define     XM_TAG_MAX                  32     //�����ȡ�õ�tag����        max = 32 ?
#define 	 XM_ITEM_MAX_NUM		    50
#define     XM_DESC_LEN                  256    //url����
#define 	 XM_CATEGORY_MAX_NUM		    50







//2.2
//��Ŀ(����)
typedef struct st_XM_category_C
{
	int id;//l��Ŀid
	char type[XM_TAG_MAX];//��Ŀ����vod ,live
	char name[XM_TAG_MAX];//��Ŀ����
	char subCategoryUrl[XM_LEN_URL];//����Ŀ�ӿ�
	char programListUrl[XM_LEN_URL];//��Ŀ�б�ӿ�
	char hpbcUrl[XM_LEN_URL];//��Ŀ�µ����Ž�ĿӰƬ��Ŀ��Ƶ����Ŀ
} XM_tag;

//-------------------------------------------------------------


//2.1��ҳ��Ϣ�ӿ�
//һ����Ŀ
typedef struct st_XM_Channel_C
{
	int  id;			//��Ŀid
	char  type[XM_TAG_MAX]; 		//��Ŀ����vod ,live
	char name[XM_TAG_MAX];//��Ŀ����
	char subCategoryUrl[XM_LEN_URL];//����Ŀ�ӿ�

	int    tagSize; //tag size ����Ŀ����
	XM_tag tags[XM_TAG_MAX];
} XM_Channel;


//����
typedef struct st_XM_hotest_C
{
	int id;//l��Ŀid
	char name[XM_TAG_MAX];//��Ŀ����
	char type[XM_TAG_MAX];//��Ŀ����vod ,live
	char image[XM_LEN_URL];//
	char detailUrl[XM_LEN_URL];//����Ŀ�ӿ�
} XM_hotest;

typedef struct st_XM_firstrecommend_C
{
	int id;//l��Ŀid
	char name[XM_TAG_MAX];//��Ŀ����
	char type[XM_TAG_MAX];//��Ŀ����vod ,live
	char image[XM_LEN_URL];//
	char actor[XM_TAG_MAX];//��Ա
	char director[XM_TAG_MAX];//����
	char detailUrl[XM_LEN_URL];//����Ŀ�ӿ�
} XM_firstrecommend;

//��ҳ
typedef struct st_XM_homepage_C
{
	char entry_url[XM_LEN_URL];

	int category_size;
	XM_Channel firstcategory[XM_CATEGORY_MAX_NUM];


	
	int hotest_size;
	XM_hotest hotest[XM_ITEM_MAX_NUM];
	int firstrecommend_size;
	XM_firstrecommend firstrecommend[XM_ITEM_MAX_NUM];
	char searchUrl[XM_LEN_URL];//����Ŀ�ӿ�
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
//ӰƬ�б�
typedef struct st_XM_mov_C
{
	int id;//��Ŀid
	char name[XM_TAG_MAX];//��Ŀ����
	char type[XM_TAG_MAX];//��Ŀ����
	char image[XM_LEN_URL];//��ĿͼƬ
	char detailUrl[XM_LEN_URL];//��Ŀ����ӿ�
} XM_mov;

typedef struct st_XM_movlist_C
{
	int totalNum;//��Ŀ�½�Ŀ����
	int currStartIdx;//��ѯ��Ŀʱ��ʼλ��
	int validCount;// ��������Ч��
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
	char name[XM_TAG_MAX]; //���ټ��ڼ���
	int bookmarktime;//��ǩ����
	int duration;//����ʱ��
	char bookmarkUrl[XM_LEN_URL];//��ǩȡ�����url
	char terminalStateUrl[XM_LEN_URL]; //��������
} XM_series;

typedef struct st_XM_seriesList_C
{
	int 	total;								//allר������
	int	  size; 							  //ȡ�õ�ר������
	XM_series data[XM_ITEM_MAX_NUM];		  //ר��������Ϣ���� 
} XM_seriesList;


//��Ʒ����
typedef struct st_XM_product_C
{
	int id;//��Ŀid
	char billingMode[XM_TAG_MAX];//�Ʒ�ģʽ
	char billingtype[XM_TAG_MAX];//�ƷѶ���
	char name[XM_TAG_MAX];//����
	int producttype;//��Ʒ����
	int purchasetype;//�Ʒѷ�ʽ
	int listprice;//����
	int rentalterm;//��Ʒ����
	int rentaltimes;//��Ʒ���ô���
	utc_time_t startTime;
	utc_time_t endTime;
	char description[XM_DESC_LEN];//��Ʒ����
	
} XM_product;


//2.4
//ӰƬ����
typedef struct st_XM_detail_C
{
	int id;//��Ŀid
	char name[XM_TAG_MAX];//��Ŀ����
	char type[XM_TAG_MAX];//��Ŀ����program series
	char image[XM_LEN_URL];//��Ŀ����ͼƬ
	char actor[XM_TAG_MAX];//��Ŀ��Ա
	char director[XM_TAG_MAX];//����
	int duration;//��Ŀ����ʱ��
	char originalcountry[XM_TAG_MAX];//�����
	char description[XM_DESC_LEN];//��Ŀ����
	int releaseyear;//��ӳ���
	char terminalStateUrl[XM_LEN_URL];//���ŵ�ַ
	char recommandUrl[XM_LEN_URL];//�Ƽ��ӿ�url
	XM_seriesList seriesList;//�缯�б�
	XM_product	product;//��Ʒ����
	int orderProduct;//�û��Ͳ�Ʒ���״̬
	int bookmarktime;//��ǩ����ʱ��
	int favorites;//��û���ղؽ�Ŀ
	char favoriteUrl[XM_LEN_URL];//���ȡ���ղص�ַ
	char bookmarkUrl[XM_LEN_URL];//���ȡ����ǩ��ַ
	
} XM_detail;




//-------------------------------------------------------------

//2.5����Ƽ�
typedef struct st_XM_recommand_C
{
	int id;//��Ŀid
	char name[XM_TAG_MAX];//��Ŀ����
	char type[XM_TAG_MAX];//��Ŀ����
	char image[XM_LEN_URL];//��ĿͼƬ
	char detailUrl[XM_LEN_URL];//��Ŀ����ӿ�
} XM_recommand;

typedef struct st_XM_RecmdList_C
{							
	int	  num; 						
	XM_recommand data[XM_ITEM_MAX_NUM];	
} XM_RecmdList;


//-------------------------------------------------------------


//�缯�������������Ӱ����
#if 0
typedef struct st_XM_seriesDetail_C
{
	int id;
	char Name[20]; //���Ӿ���
	int type; //���� series
	char actor[20]; //��Ա
	char director[20];//����
	int duration; //ʱ��
	char originalcountry[20]; //����
	char description[1024]; //�������
	char image[XM_LEN_URL]; //��������
	char playUrl[XM_LEN_URL]; //��������
	char recommendUrl[XM_LEN_URL]; //�Ƽ�����
	XM_seriesList seriesList[XM_ITEM_MAX_NUM]; //������缯�б�

} XM_SeriesDetail;
#endif 

//-------------------------------------------------------------

//2.7���ŵ�ַ
typedef struct st_XM_urlobj_C
{
	int id;
	char name[XM_TAG_MAX];//���ŵ�ַ��
	int hdtv;//��������
	char playUrl[XM_LEN_URL];//���ŵ�ַ
	char timeshiftUrl[XM_LEN_URL];
} XM_urlobj;

typedef struct st_XM_play_urlList_C
{
	int terminalState;//0����1�ڲ�
	int num;
	XM_urlobj urlobj[100];//���ŵ�ַ����
	//char timeshiftUrl[XM_LEN_URL];//Ƶ��ʱ�Ʋ��ŵ�ַ
} XM_play_urlList;

//-------------------------------------------------------------

//2.9
//��Ŀ��
typedef struct st_XM_schedule_C
{
	char name[XM_TAG_MAX];//��Ŀ����
	utc_time_t start_time;
	utc_time_t end_time;
} XM_schedule;

//2.8
//ֱ���б�
typedef struct st_XM_channelList_C
{
	int id;//Ƶ��id
	char Name[XM_TAG_MAX]; //Ƶ������
	int number; //Ƶ����
	char image[XM_LEN_URL];//����ͼurl
	char playUrl[XM_LEN_URL];//	terminalStateurl���ŵ�ַ����
	char timeshiftUrl[XM_LEN_URL];//Ƶ����ϢcbidUrl  ? ʱ��url
	int timeshiftLength;//Ƶ��ʱ��ʱ��
	int storageLength;//Ƶ���洢ʱ��
	char scheduleListUrl[XM_LEN_URL];//Ƶ����Ŀ���ӿ�url
	XM_schedule currentSchedule;//��ǰ��Ŀ����Ϣ
	XM_schedule nextSchedule;//�¸���Ŀ����Ϣ
	
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

