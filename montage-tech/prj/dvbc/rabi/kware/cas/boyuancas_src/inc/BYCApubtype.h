#ifndef BYCAPUBTYPE_H_2013_02_20
#define BYCAPUBTYPE_H_2013_02_20


#define BYCA_FUNCPOSTFIX	//large reentrant
#define BYCA_FAR				//far

#define BU8							unsigned char
#define BU16						unsigned short
#define BU32						unsigned long
#define BS8							char
#define	BS16						short
#define BS32						long
#define BVOID						void
#define BUS16						unsigned short
/* 信号量定义 */
typedef long  BY_Semaphore;
/*  线程函数地址类型  */
typedef void (*pThreadFunc) (void);

#define true					1
#define false					0
#define BYCA_OK				1
#define BYCA_ERROR		0
#define BNULL					0

#define EMail_TIME						5
#define	EMail_Title_MAXLEN				60//40
#define	Email_Content_MAXLEN			200//100
#define	EMail_MAXNUM					50//30

/*智能卡状态定义 */
#define BYCAS_SC_OUT							0		/*智能卡不在读卡器中 */
#define BYCAS_SC_IN							1		/*智能卡在读卡器中，工作正常 */

#define	BYCAS_NEW_EAMIL						0/*新邮件通知。*/
#define BYCAS_NEW_EMAIL_NO_ROOM		1/*有新邮件，但Email空间不够。*/
#define	BYCAS_EMAIL_NONE					2/*没有新邮件。*/
							
#define BYCA_MAXLEN_SERVICEPERECM				10		/*每个ECM支持的最大Service数目 */

#define BYCA_MAX_FILTERMASK							8

#define CAMENU_PRODUCT_MAX					20
#define CAMENU_PRODUCTNAME_MAXLEN		40


/*-----  ECM_PID设置的操作类型----- */
#define		BYCAS_LIST_START				0x10
#define		BYCAS_LIST_END					0x11	
#define		BYCAS_LIST_ADD					0x12


//#pragma pack(1)
typedef struct _SDVTCAServiceInfo{
	BU16 	m_wEcmPid;												/*加扰的ECMPID*/
	BU16		m_byServiceCount;									/*频道编号的个数*/
	BU16 	m_wServiceID[BYCA_MAXLEN_SERVICEPERECM];			/*频道的编号*/
}SBYCAServiceInfo;

typedef struct tag_mail_Data_struct{
	BU8		 	MessageFlag; 	/*	0 -- 不需要存储 1 -- 需要存储 2 -- 未阅读 3-- 已经阅读 4 -- 删除 0xff 没有初始化*/
	BU32		MessageId;		/* 	4 byte*/
	BU8 		MessageType; 	/* 	1 byte*/
	BU8			TitleLen;			/*  1 byte*/
	BU16		ContentLen;  	/* 	2 byte*/
	BU8			DisplayType;		/*  1 byte*/
	BU16 		reserved1;  	/* 	2 byte*/
  BU8			SendTime[EMail_TIME]; /* 5 byte*/
  BU8			VaildTime[EMail_TIME];/* 5 byte*/
	BU8 		Title[EMail_Title_MAXLEN];/*100 byte*/
	BU8   	Content[Email_Content_MAXLEN];/*400 byte*/
}MAIL_DATA;
//#pragma pack()

#define BYCA_BLACK 					0/*黑色*/
#define BYCA_BLUE						1/*蓝*/
#define BYCA_GREEN					2/*绿*/
#define BYCA_CYAN						3/*青*/
#define BYCA_RED						4/*红*/
#define BYCA_MAGENTA				5/*洋红*/
#define BYCA_BROWN					6/*棕*/
#define BYCA_LIGHTGRAY			7/*淡灰*/
#define	BYCA_DARKGRAY				8/*深灰*/
#define BYCA_LIGHTBLUE			9/*淡蓝*/
#define BYCA_LIGHTGREEN			10/*淡绿*/
#define	BYCA_LIGHTCYAN			11/*淡青*/
#define BYCA_LIGHTRED				12/*淡红*/
#define BYCA_LIGHTMAGENTA		13/*淡洋红*/
#define BYCA_YELLOW					14/*黄*/
#define BYCA_WHITE					15/*白*/
#define BYCA_Transparency		16/*透明*/

#define AD_PROMPTOSD_MAXLEN			256//12
typedef struct tag_ADScrollInfo_struct
{
	BU16	ADScroll_id;
	BU8		Scroll_time;	
	BU8		Font_color;
	BU8		Background_color;
	BU8		Postion;
	BU8  	xoffset;
	BU8		AD_text[AD_PROMPTOSD_MAXLEN];
}ADScroll_Info;



typedef struct tag_CAMENU_UNIT{
	BU8				Product_ID;
	BU8				Purchase_start_time[2];
	BU8				Purchase_end_time[2];
}CaMENU_UNIT;

typedef struct tag_CAMENU_NAME{
	BU8				Product_ID;
	BU8				ProductName_Len;
	BU8				ProductName[CAMENU_PRODUCTNAME_MAXLEN];
}CaMENU_NAME;

typedef struct tag_CAMenu_Product{
	BU8				CaMenu_Product_Init;
	BU8				CaMenu_number;
	CaMENU_NAME		CaMenu_Name[CAMENU_PRODUCT_MAX];
	CaMENU_UNIT		Camenu_Unit[CAMENU_PRODUCT_MAX];
}CAMenu_Product;



#define BYCA_NO_ERROR											0				/*没有错误，取消上次显示*/
#define BYCA_ERROR_InitCardCommand								1				/*智能卡初始化错误 0x01*/
#define BYCA_ERROR_ReadCardCommand								2				/*智能卡读卡号命令错误 0x02*/
#define BYCA_ERROR_NO_CARD										3				/*请放入智能卡*/
#define BYCA_ERROR_EMM_RET										4				/*EMM错误 0x04*/
#define BYCA_ERROR_ECM_RET										5				/*ECM错误 0x05*/
#define BYCA_ECM_RET_READCOMM_ERROR								6				/*智能卡返回ECM数据错误 0x06*/
#define BYCA_EMMECM_RET_P1P2_ERROR								7				/*智能卡返回ECM EMM P1P2数据错误 0x07*/
#define BYCA_EMMECM_RET_P3_ERROR								8				/*智能卡返回ECM EMM P3数据错误 0x08*/
#define BYCA_EMMECM_RET_DES_ERROR								9				/*智能卡用户无授权*/
#define BYCA_ECM_RET_NOENTITLE_ERROR							10			/*智能卡用户无授权*/
#define BYCA_EMMECM_RET_STORE_ERROR								11			/*智能卡返回错误 0x0a*/
#define BYCA_EMMECM_RET_CHECK_ERROR								12			/*智能卡用户无授权*/
#define BYCA_EMM_NOKEY_OR_NOFILE_ERROR						13			/*智能卡用户无授权*/
#define BYCA_ECM_OUTDATE_ERROR										14			/*智能卡用户无授权*/
#define BYCA_EMMECM_RET_UNKNOW_ERROR				  		15			/*智能卡ECM EMM返回未知错误 0x0f*/
#define	BYCA_CARD_NEED_PAIRING										16		  /*节目要求机卡配对*/
#define BYCA_CARD_PAIRING_OK											17			/*节目机卡配对成功*/
#define BYCA_NO_ECM_INFO													18			/*没有ECM流信息*/
#define BYCA_ZONE_FORBID_ERROR										19			/*该节目被区域限播*/
#define BYCA_UserCdn_Limit_Ctrl_Enable						20			/*该用户被条件限播*/
#define BYCA_UserCdn_Limit_Ctrl_Disable						21			/*该用户被取消条件限播*/
#define BYCA_ZoneCdn_Limit_Ctrl_Enable						22			/*该区域被条件限播*/
#define BYCA_ZoneCdn_Limit_Ctrl_Disable						23			/*该区域被取消条件限播*/
#define BYCA_PAIRED_OtherCARD_ERROR								24 /*该机器与其它卡配对*/
#define BYCA_PAIRED_OtherMachine_ERROR							25 /*该卡与此机器不配对*/
#define BYCA_MASTERSLAVE_PAIRED_OK								26	/*子母卡配对成功*/
#define BYCA_MASTERSLAVE_PAIRED_TIMEUPDATE						27	/*子母卡配对时间更新*/
#define BYCA_MASTERSLAVE_NEEDPAIRED								28	/*子母卡需要配对*/
#define BYCA_CARD_CANCEL_PAIRED_OK								29  /*机卡配对取消成功*/
//漫游
#define STATUE_CA_CARD_ZONE_INVALID							30		/*您的智能卡不再服务区*/
#define  STATUE_CA_CARD_ZONE_CHECK_ENTER                 	31 		/* 正在寻找网络，请等待*/
#define  STATUE_CA_CARD_ZONE_CHEKC_OK       				32		/* 漫游功能卡检测正常*/

#define BYCA_ERROR_CardSystemID_ERROR						40
#define BYCA_ERROR_STBFACTORY_ID_ERROR						41  /* 该机顶盒未经过认证，不能正常工作*/
#define BYCA_ERROR_SYSTEM_ID_ERROR                                               42     /* 系统ID错误**/
#endif

