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
/* �ź������� */
typedef long  BY_Semaphore;
/*  �̺߳�����ַ����  */
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

/*���ܿ�״̬���� */
#define BYCAS_SC_OUT							0		/*���ܿ����ڶ������� */
#define BYCAS_SC_IN							1		/*���ܿ��ڶ������У��������� */

#define	BYCAS_NEW_EAMIL						0/*���ʼ�֪ͨ��*/
#define BYCAS_NEW_EMAIL_NO_ROOM		1/*�����ʼ�����Email�ռ䲻����*/
#define	BYCAS_EMAIL_NONE					2/*û�����ʼ���*/
							
#define BYCA_MAXLEN_SERVICEPERECM				10		/*ÿ��ECM֧�ֵ����Service��Ŀ */

#define BYCA_MAX_FILTERMASK							8

#define CAMENU_PRODUCT_MAX					20
#define CAMENU_PRODUCTNAME_MAXLEN		40


/*-----  ECM_PID���õĲ�������----- */
#define		BYCAS_LIST_START				0x10
#define		BYCAS_LIST_END					0x11	
#define		BYCAS_LIST_ADD					0x12


//#pragma pack(1)
typedef struct _SDVTCAServiceInfo{
	BU16 	m_wEcmPid;												/*���ŵ�ECMPID*/
	BU16		m_byServiceCount;									/*Ƶ����ŵĸ���*/
	BU16 	m_wServiceID[BYCA_MAXLEN_SERVICEPERECM];			/*Ƶ���ı��*/
}SBYCAServiceInfo;

typedef struct tag_mail_Data_struct{
	BU8		 	MessageFlag; 	/*	0 -- ����Ҫ�洢 1 -- ��Ҫ�洢 2 -- δ�Ķ� 3-- �Ѿ��Ķ� 4 -- ɾ�� 0xff û�г�ʼ��*/
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

#define BYCA_BLACK 					0/*��ɫ*/
#define BYCA_BLUE						1/*��*/
#define BYCA_GREEN					2/*��*/
#define BYCA_CYAN						3/*��*/
#define BYCA_RED						4/*��*/
#define BYCA_MAGENTA				5/*���*/
#define BYCA_BROWN					6/*��*/
#define BYCA_LIGHTGRAY			7/*����*/
#define	BYCA_DARKGRAY				8/*���*/
#define BYCA_LIGHTBLUE			9/*����*/
#define BYCA_LIGHTGREEN			10/*����*/
#define	BYCA_LIGHTCYAN			11/*����*/
#define BYCA_LIGHTRED				12/*����*/
#define BYCA_LIGHTMAGENTA		13/*�����*/
#define BYCA_YELLOW					14/*��*/
#define BYCA_WHITE					15/*��*/
#define BYCA_Transparency		16/*͸��*/

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



#define BYCA_NO_ERROR											0				/*û�д���ȡ���ϴ���ʾ*/
#define BYCA_ERROR_InitCardCommand								1				/*���ܿ���ʼ������ 0x01*/
#define BYCA_ERROR_ReadCardCommand								2				/*���ܿ�������������� 0x02*/
#define BYCA_ERROR_NO_CARD										3				/*��������ܿ�*/
#define BYCA_ERROR_EMM_RET										4				/*EMM���� 0x04*/
#define BYCA_ERROR_ECM_RET										5				/*ECM���� 0x05*/
#define BYCA_ECM_RET_READCOMM_ERROR								6				/*���ܿ�����ECM���ݴ��� 0x06*/
#define BYCA_EMMECM_RET_P1P2_ERROR								7				/*���ܿ�����ECM EMM P1P2���ݴ��� 0x07*/
#define BYCA_EMMECM_RET_P3_ERROR								8				/*���ܿ�����ECM EMM P3���ݴ��� 0x08*/
#define BYCA_EMMECM_RET_DES_ERROR								9				/*���ܿ��û�����Ȩ*/
#define BYCA_ECM_RET_NOENTITLE_ERROR							10			/*���ܿ��û�����Ȩ*/
#define BYCA_EMMECM_RET_STORE_ERROR								11			/*���ܿ����ش��� 0x0a*/
#define BYCA_EMMECM_RET_CHECK_ERROR								12			/*���ܿ��û�����Ȩ*/
#define BYCA_EMM_NOKEY_OR_NOFILE_ERROR						13			/*���ܿ��û�����Ȩ*/
#define BYCA_ECM_OUTDATE_ERROR										14			/*���ܿ��û�����Ȩ*/
#define BYCA_EMMECM_RET_UNKNOW_ERROR				  		15			/*���ܿ�ECM EMM����δ֪���� 0x0f*/
#define	BYCA_CARD_NEED_PAIRING										16		  /*��ĿҪ��������*/
#define BYCA_CARD_PAIRING_OK											17			/*��Ŀ������Գɹ�*/
#define BYCA_NO_ECM_INFO													18			/*û��ECM����Ϣ*/
#define BYCA_ZONE_FORBID_ERROR										19			/*�ý�Ŀ�������޲�*/
#define BYCA_UserCdn_Limit_Ctrl_Enable						20			/*���û��������޲�*/
#define BYCA_UserCdn_Limit_Ctrl_Disable						21			/*���û���ȡ�������޲�*/
#define BYCA_ZoneCdn_Limit_Ctrl_Enable						22			/*�����������޲�*/
#define BYCA_ZoneCdn_Limit_Ctrl_Disable						23			/*������ȡ�������޲�*/
#define BYCA_PAIRED_OtherCARD_ERROR								24 /*�û��������������*/
#define BYCA_PAIRED_OtherMachine_ERROR							25 /*�ÿ���˻��������*/
#define BYCA_MASTERSLAVE_PAIRED_OK								26	/*��ĸ����Գɹ�*/
#define BYCA_MASTERSLAVE_PAIRED_TIMEUPDATE						27	/*��ĸ�����ʱ�����*/
#define BYCA_MASTERSLAVE_NEEDPAIRED								28	/*��ĸ����Ҫ���*/
#define BYCA_CARD_CANCEL_PAIRED_OK								29  /*�������ȡ���ɹ�*/
//����
#define STATUE_CA_CARD_ZONE_INVALID							30		/*�������ܿ����ٷ�����*/
#define  STATUE_CA_CARD_ZONE_CHECK_ENTER                 	31 		/* ����Ѱ�����磬��ȴ�*/
#define  STATUE_CA_CARD_ZONE_CHEKC_OK       				32		/* ���ι��ܿ��������*/

#define BYCA_ERROR_CardSystemID_ERROR						40
#define BYCA_ERROR_STBFACTORY_ID_ERROR						41  /* �û�����δ������֤��������������*/
#define BYCA_ERROR_SYSTEM_ID_ERROR                                               42     /* ϵͳID����**/
#endif

