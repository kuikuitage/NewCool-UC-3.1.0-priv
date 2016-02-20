#ifndef IQIYITVCWRAPPER_H_INCLUDED
#define IQIYITVCWRAPPER_H_INCLUDED



/// /////////////////////////////////////////////////////////////
///                         error code
/// /////////////////////////////////////////////////////////////
#define     IQY_NORMAL                   0      //
#define     IQY_ERROR_PARAM              1      //
#define     IQY_ERROR_SYSTEM             2      //system error
#define     IQY_ERROR_BUSY               3      //system busy
#define     IQY_ERROR_DISABLE_CHG_STATUS 4      //status change disable
#define     IQY_ERROR_DEVICE_ERROR       5
#define     IQY_ERROR_BUFFERING          6
#define     IQY_ERROR_OPERATE_DISABLE    7
#define     IQY_ERROR_PLAY_CHECK_ERROR   8



/// /////////////////////////////////////////////////////////////
///                         define part
/// /////////////////////////////////////////////////////////////
#define     IQY_TAG_MAX                  16     //�����ȡ�õ�tag����        max = 32 ?
#define     IQY_SUBTAG_MAX               40     //�����ȡ�õ�subtag����     max = 32 ?
#define     IQY_SEARCHWORD_MAX           32     //search word������       max = 128
#define     IQY_ALBUM_LIST_MAX           50     //�����ȡ�õ�ר������         max = 60
#define     IQY_CHANNEL_MAX              20     //�����ȡ�õ�channel����     max = 60
#define     IQY_EPISODE_MAX              20     //�����ȡ�õ�episode����     max = 120
#define     IQY_CHANNEL_LABEL_MAX        20     //label of channel's max
#define     IQY_PLAY_LIST_MAX            200    //play list's max
#define     IQY_VD_MAX                   30     //vd max
#define     IQY_AD_DURATION_MAX          30     //ad duration list max

#define     IQY_LEN_MAX                  512    //�ַ�����󳤶�
#define     IQY_LEN_URL                  256    //url����
#define     IQY_LEN_MSG                  128    //����msg����
#define     IQY_LEN_QPID                 32     //qipuId����
#define     IQY_LEN_VID                  36     //VID���� = 32
#define     IQY_LEN_CHANNEL_ITEM0        17     //channel�ṹ���У�������λ�У�����С��item
#define     IQY_LEN_CHANNEL_ITEM1        33     //channel�ṹ���У�������λ�У����Ƚ�С��item
#define     IQY_LEN_CHANNEL_ITEM2        65     //channel�ṹ���У�������λ�У�������ͨ��item
#define     IQY_LEN_CAST_ITEM1           33     //cast�ṹ���У�������λ�У����Ƚ�С��item
#define     IQY_LEN_CAST_ITEM2           65     //cast�ṹ���У�������λ�У����Ƚϴ��item
#define     IQY_LEN_ALBUM_INFO_ITEM1     40     //albumInfo�ṹ���У�������λ�У����Ƚ�С��item
#define     IQY_LEN_ALBUM_INFO_ITEM2     65     //albumInfo�ṹ���У�������λ�У�������ͨ��item
#define     IQY_LEN_ALBUM_INFO_ITEM3     129    //albumInfo�ṹ���У�������λ�У����Ƚϴ��item
#define     IQY_LEN_EPISODE_ITEM1        33     //episode�ṹ���У�������λ�У����Ƚ�С��item
#define     IQY_LEN_EPISODE_ITEM2        65     //episode�ṹ���У�������λ�У�������ͨ��item
#define     IQY_LEN_EPISODE_ITEM3        129    //episode�ṹ���У�������λ�У����Ƚϴ��item
#define     IQY_LEN_SEARCHWORD_ITEM1     33     //SearchWord�ṹ���У�������λ�У����Ƚ�С��item
#define     IQY_LEN_SEARCHWORD_ITEM2     65     //SearchWord�ṹ���У�������λ�У�������ͨ��item
#define     IQY_LEN_CHANNEL_LABEL_ITEM1  33     //ChannelLabelInfo�ṹ���У�������λ�У����Ƚ�С��item
#define     IQY_LEN_CHANNEL_LABEL_ITEM2  65     //channelLabelInfo�ṹ���У�������λ�У�������ͨ��item
#define     IQY_LEN_RESULOTION_ITEM      33     //resulotion len
#define     IQY_LEN_TIME                 17     //system time len
#define     IQY_LEN_COMPILE_TIME         33     //SDK compile time len



/// /////////////////////////////////////////////////////////////
///                         struct
/// /////////////////////////////////////////////////////////////
typedef struct st_IQY_SubTag_C
{
    char    name[IQY_LEN_CHANNEL_ITEM1];        //tag name
    char    value[IQY_LEN_CHANNEL_ITEM1];       //tag value
} IQY_SubTag;

typedef struct st_IQY_Tag_C
{
    char    name[IQY_LEN_CHANNEL_ITEM0];        //tag name
    int     subTagSize;                         //size
    IQY_SubTag   subtag[IQY_SUBTAG_MAX];        //tag value
} IQY_Tag;

typedef struct st_IQY_Channel_C
{
    char    backImg[IQY_LEN_URL];               //����ͼbackImage
    int     channelId;                          //Ƶ��id
    char    name[IQY_LEN_CHANNEL_ITEM0];        //Ƶ������
    int     spec;                               //ͼƬ���,picSpec1�Ǻ�ͼ��2����ͼ��3�Ǻ������ţ�4������
    char    recTag[IQY_LEN_CHANNEL_ITEM1];      //rec tag id
    char    focus[IQY_LEN_CHANNEL_ITEM1];       //recommand id
    IQY_Tag tags[IQY_TAG_MAX];                  //tags
    int     tagSize;                            //tag size
} IQY_Channel;

typedef struct st_IQY_ChannelList_C
{
    int     total;                              //allƵ������
    int     size;                               //ȡ�õ�Ƶ������
    IQY_Channel data[IQY_CHANNEL_MAX];          //Ƶ��������Ϣ����
    int     resultCode;                         //ִ�н������ 0��������>0:�쳣������code���ж���
} IQY_ChannelList;

typedef struct st_IQY_Cast_C
{
    char    composer[IQY_LEN_CAST_ITEM1];       //����
    char    host[IQY_LEN_CAST_ITEM2];           //������ ����"����,���,��ά��,�ź���,л��"
    char    dubber[IQY_LEN_CAST_ITEM1];         //����
    char    maker[IQY_LEN_CAST_ITEM1];          //��Ƭ��
    char    star[IQY_LEN_CAST_ITEM2];           //���� ����"�����ֵ�,��־��,ë����"
    char    producer[IQY_LEN_CAST_ITEM1];       //��Ʒ��
    char    songWriter[IQY_LEN_CAST_ITEM1];     //����
    char    guest[IQY_LEN_CAST_ITEM2];          //�α� ����"�����ֵ�,��־��,ë����"
    char    mainActor[IQY_LEN_CAST_ITEM2];      //����/�ݳ���/������/���� ����"����,���,��ά��,�ź���,л��"
    char    actor[IQY_LEN_CAST_ITEM2];          //��Ա/MV��Ա/�α�/������ɫ ����"�����ֵ�,��־��,ë����"
    char    writer[IQY_LEN_CAST_ITEM1];         //���
    char    director[IQY_LEN_CAST_ITEM1];       //����
} IQY_Cast;

typedef struct st_IQY_AlbumInfo_C
{
    char    qpId[IQY_LEN_QPID];                 //ר�����ף���Ƶ/ר��/��Դ��ID
    char    tvQid[IQY_LEN_QPID];                //�缯����ID
    char    name[IQY_LEN_ALBUM_INFO_ITEM2];     //ר������Ƶ/ר��/��Դ������ ����"ͬ������"
    int     type;                               //ר�����ͣ�0����Ƶ��1��ר�������ô˽ӿڴ��������Ǹ�qpId���������ͣ����Ƿ��ؽ���е�qpId����������
    char    pic[IQY_LEN_URL];                   //ר��ͼ ����"http://pic6.qiyipic.com/image/20140808/ce/b7/f1/v_106541155_m_600_m1.jpg"
    char    tvPic[IQY_LEN_URL];                 //����ͼ ����"http://pic6.qiyipic.com/image/20140808/ce/b7/f1/v_106541155_m_600_m1.jpg"
    float   score;
    char    desc[IQY_LEN_MAX];                  //ר������ ����"1993�������"
    char    key[IQY_LEN_ALBUM_INFO_ITEM1];      //ר���ؼ���
    int     isSeries;                           //�Ƿ�༯��0Ϊ���� 1Ϊ�༯
    char    chnName[IQY_LEN_ALBUM_INFO_ITEM1];  //Ƶ������ ����"��Ӱ"
    int     chnId;                              //Ƶ��ID ����1
    char    tag[IQY_LEN_ALBUM_INFO_ITEM2];      //��ǩ�б� ����"����,����,����,���
    IQY_Cast cast;                              //cast��Ϣ
    int     exclusive;                          //�Ƿ������1Ϊ��;0Ϊ��
    char    sourceCode[IQY_LEN_ALBUM_INFO_ITEM2];   //��Դcode
    int     is3D;                               //�Ƿ�3D,1Ϊ��;0Ϊ��
    int     tvsets;                             //�ܼ���
    int     tvCount;                            //��ר��/��Դ������Ƶ������Ƶ����ǰ����
    char    vid[IQY_LEN_VID];                   //��������id
    long    len;                                //����ʱ�� ����5905
    char    initIssueTime[IQY_LEN_ALBUM_INFO_ITEM1];    //�����״�����ʱ�� ����"2014-11-12 14:58:56"
    //pic url
    char    pic360x180[IQY_LEN_URL];            //360x180 pic url
    char    pic260x360[IQY_LEN_URL];            //260x360 pic url
    char    pic195x260[IQY_LEN_URL];            //195x360 pic url
} IQY_AlbumInfo;

typedef struct st_IQY_AlbumList_C
{
    int     total;                              //allר������
    int     size;                               //ȡ�õ�ר������
    IQY_AlbumInfo data[IQY_ALBUM_LIST_MAX];     //ר��������Ϣ����
    int     resultCode;                         //ִ�н������ 0��������>0:�쳣������code���ж���
} IQY_AlbumList;

typedef struct st_IQY_Episode_C
{
    char    tvQid[IQY_LEN_QPID];                //�缯ID ����1181747
    char    vid[IQY_LEN_VID];                   //��������id
    int     type;                               //���� 0,Ԥ��Ƭ��1����Ƭ
    char    name[IQY_LEN_EPISODE_ITEM2];        //�缯���� ����"���鹫Ԣ4��1��"
    char    albumName[IQY_LEN_EPISODE_ITEM1];   //����ר������
    char    year[IQY_LEN_EPISODE_ITEM1];        //������Ϊ�ڼ���ʹ�� ����20130101
    long    len;                                //ʱ��
    int     order;                              //���Ŵ��򣬵ڼ����������Դר���µľ缯order
    char    pic[IQY_LEN_URL];                   //�缯ͼƬ ����"http://pic4.qiyipic.com/image/20140116/v_104637386_m_601_m1.jpg"
    IQY_Cast cast;                              //cast��Ϣ
    char    desc[IQY_LEN_MAX];                  //����
} IQY_Episode;

typedef struct st_IQY_EpisodeList_C
{
    int     total;                              //allר������
    int     size;                               //ȡ�õ�ר������
    IQY_Episode data[IQY_EPISODE_MAX];          //ר��������Ϣ����
    int     resultCode;                         //ִ�н������ 0��������>0:�쳣������code���ж���
} IQY_EpisodeList;

typedef struct st_IQY_Word_C
{
    char word[IQY_LEN_SEARCHWORD_ITEM1];        //word
} IQY_Word;

typedef struct st_IQY_SearchWord_C
{
	char site[IQY_LEN_SEARCHWORD_ITEM1];        //
	char eventId[IQY_LEN_SEARCHWORD_ITEM2];     //
	IQY_Word wordList[IQY_SEARCHWORD_MAX];      //search word's list
	int wordSize;                               //wordlist's size
} IQY_SearchWord;

typedef struct st_IQY_ChannelLabelInfo_C
{
    char name[IQY_LEN_CHANNEL_LABEL_ITEM1];
    char plid[IQY_LEN_CHANNEL_LABEL_ITEM1];
} IQY_ChannelLabelInfo;

typedef struct st_IQY_ChannelLabelList_C
{
    int size;
    IQY_ChannelLabelInfo labelList[IQY_CHANNEL_LABEL_MAX];
} IQY_ChannelLabelList;

typedef struct st_IQY_PlayList_C
{
    int     total;                              //allר������
    int     size;                               //ȡ�õ�ר������
    IQY_AlbumInfo data[IQY_PLAY_LIST_MAX];      //ר��������Ϣ����
    int     resultCode;                         //ִ�н������ 0��������>0:�쳣������code���ж���
} IQY_PlayList;

///callback
typedef void (*error_callback) (int errorCode);
typedef void (*playstate_callback) (int nCase, int param);

typedef struct st_IQY_CollectingInfo_C
{
    char        qpid[IQY_LEN_QPID];                     //qpid
    char        plid[IQY_LEN_CHANNEL_LABEL_ITEM1];      //�Ӳ�������ʱ�򣬴�����
    int         chnId;
    char        resulotion[IQY_LEN_RESULOTION_ITEM];    //�豸�ֱ��� 1920x1080
    int         tvepin;                                 //�缯�������ھ缯�б����ļ���
    int         vvfrom;                                 //0:������   2:ר��������   3:ר��������  (ע��)
} IQY_CollectingInfo;

typedef struct st_IQY_PlaySetup_C
{
    char                tvQid[IQY_LEN_QPID];    //�缯ID ����1181747
    char                vid[IQY_LEN_VID];       //��������id
    int                 vd;
    int                 startTime;              //start play time
    IQY_CollectingInfo  info;
    error_callback      error;
    playstate_callback  playstate;
} IQY_PlaySetup;

typedef struct st_IQY_PlayerInit_C
{
    void*               cfg;
    long                size;
} IQY_PlayerInit;

typedef struct st_IQY_Init_C
{
    int                 mode;                   //mode
    int                 firstPwOn;              //1:��һ�ο���   0:���ǵ�һ�ο���
    IQY_PlayerInit      playerInit;
} IQY_Init;

typedef struct st_IQY_CurrentPlayingVideoInfo_C
{
    char    vid[IQY_LEN_VID];           //��������id
    int     vd;                         //��ǰ���������汾��1:������2:����, 3:���壬4:720P, 5:1080P
    int     duration;                   //��Ƶʱ��
    int     cid;                        //channel id
    int     currentPos;
    int     videoWidth;
    int     videoHeight;
    int     leftVolume;
    int     rightVolume;
    int     vdlist[IQY_VD_MAX];         //vd list
    int     size;
    long    adDurationList[IQY_AD_DURATION_MAX]; //���ص���ÿ��ad��ʱ��
    int     adDurationListSize;
} IQY_CurrentPlayingVideoInfo;

typedef struct st_IQY_SystemTime_C
{
    char    time[IQY_LEN_TIME];
} IQY_SystemTime;


typedef struct st_IQY_VdList_C
{
    int     vdlist[IQY_VD_MAX];         //vd list
    int     size;
} IQY_VdList;

typedef struct st_IQY_SDK_Version_C
{
    char    compileTime[IQY_LEN_COMPILE_TIME];
} IQY_SDK_Version;


/// /////////////////////////////////////////////////////////////
///                         enum
/// /////////////////////////////////////////////////////////////
enum en_IdType_C
{
    TYPE_PLAYLIST            = 0,
    TYPE_RECOMMAND,
    TYPE_MAX
};

enum en_vvfrom_C
{
    TYPE_VVFROM_CLICK        = 0,          //������
//    TYPE_VVFROM_CONT_REC,                //
    TYPE_VVFROM_CONT_EPISODE = 2,          //����
    TYPE_VVFROM_CONT_ALBUM,                //ר��������
    TYPE_VVFROM_MAX
};

enum en_CURRENT_STATUS_C
{
    TYPE_STATUS_NORMAL       = 0,        //��ͨ״̬
    TYPE_STATUS_ADVERTISING,             //���ڲ��Ź��
    TYPE_STATUS_VIDEO,                   //���ڲ�����Ƭ
    TYPE_STATUS_MAX
};

enum en_vdType_C
{
    TYPE_VD_300              = 1,        //1: ����
    TYPE_VD_600 ,                        //2: ����
    TYPE_VD_1000,                        //3: ����
    TYPE_VD_720P,                        //4: 720p
    TYPE_VD_1080P,                       //5: 1080p
    TYPE_VD_FAST_DRM,                    //6: ����DRM
    TYPE_VD_AUTO_DRM,                    //7: ����DRM
    TYPE_VD_HD_DRM,                      //8: ����DRM
    TYPE_VD_SUPER_HD_DRM,                //9: ����DRM
    TYPE_VD_4K,                          //10: 4K
    TYPE_VD_5M,                          //11: 5M
    TYPE_VD_8M,                          //12: 8M
    TYPE_VD_1000_DOLBY,                  //13: 1000�ű�
    TYPE_VD_720P_DOLBY,                  //14: 720p�ű�
    TYPE_VD_1080P_DOLBY,                 //15: 1080p�ű�
    TYPE_VD_4K_DOLBY,                    //16: 4k�ű�
    TYPE_VD_720P_265,                    //17: 720P_265
    TYPE_VD_1080P_265,                   //18: 1080P_265
    TYPE_VD_4K_265,                      //19: 4K_265
    TYPE_VD_600_DOLBY,                   //20: 600_dolby
    TYPE_VD_FAST            = 96,        //96: ����
    TYPE_VD_MAX
};



/// /////////////////////////////////////////////////////////////
///                        interface api
/// /////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int iqy_libInit(IQY_Init* pInit);
int iqy_libUninit(void);

///epg
int iqy_getChannelList(const char* versionId, int pageNo, int pageSize, IQY_ChannelList* pChnlList);
int iqy_getAlbumInfo(char* qpId, IQY_AlbumInfo* pAlbumInfo);
int iqy_getAlbumList(int chnId, int fromChnId, const char* tagValues, int pageNo, int pageSize, IQY_AlbumList* pAlbumList);
int iqy_getAlbumVideo(char* qpId, const char* sourceCode, int videoType, int pageNo, int pageSize, IQY_EpisodeList* pEpisodeLst);
int iqy_getRealTimeSearchWord(const char* keyWord, IQY_SearchWord* pSearchWord);
int iqy_albumSearch(const char* keyword, int chnId, int pageNo, int pageSize, IQY_AlbumList* pAlbumList);
int iqy_getChannelLabelList(int isFree, const char* chnRecTagId, IQY_ChannelLabelList* pLabelList);
int iqy_getPlayListById(int isFree, const char* id, int idType, IQY_PlayList* pPlayList);
int iqy_getRelatedRecommendAlbumList(int isFree, int chnId, const char* qpid, const char* tvQid, int pageSize, IQY_AlbumList* pAlbumList);
int iqy_getVdList(char* tvQid, char* vid, IQY_VdList* pVdlist);

///player
int iqy_start(const IQY_PlaySetup* pPlaySetup);
int iqy_pause();
int iqy_resume();
int iqy_stop();
int iqy_seek(int jumpTime);
int iqy_getCurrentPlayingVideoInfo( IQY_CurrentPlayingVideoInfo* pCurrentPlayingVideoInfo );
int iqy_switchVd(int vd, int currentTime);
int iqy_getCurrentStatus();
int iqy_releasePlayer();

///time
int iqy_getSystemTime(IQY_SystemTime* pTime);

///compile time
int iqy_getSDKVersion(IQY_SDK_Version* pSdkVersion);

#ifdef __cplusplus
}
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif //IQIYITVCWRAPPER_H_INCLUDED
