#ifndef __DS_SC_H__
#define __DS_SC_H__

/****************************************************************/
/****************************************************************/
/*��ʾ��Ϣ*/
#define		MESSAGE_CANCEL_TYPE					0x00           
#define		MESSAGE_BADCARD_TYPE				0x01		/*�޷�ʶ�𿨣�����ʹ��*/
#define		MESSAGE_EXPICARD_TYPE				0x02		/*���ܿ��Ѿ����ڣ�������¿�*/
#define		MESSAGE_CARD_INIT_TYPE				0x03		/*���ܿ����ڳ�ʼ��*/
#define		MESSAGE_INSERTCARD_TYPE				0x04		/*���ܽ�Ŀ����������ܿ�*/
#define		MESSAGE_CARD_INIT_ERR_TYPE			0x05		/*���ܿ���ʼ��ʧ��*/
#define		MESSAGE_STB_NO_CARD_TYPE			0x06		/*�������в��������ܿ�*/
#define		MESSAGE_CARD_INIT_OK_TYPE			0x07		/*���ܿ���ʼ�����*/
#define		MESSAGE_NOOPER_TYPE					0x10		/*���в����ڽ�Ŀ��Ӫ��*/
#define		MESSAGE_CARDSTOP_TYPE				0x11		/*���ܿ���ͣ��*/
#define		MESSAGE_OUTWORKTIME_TYPE			0x12		/*���ڹ���ʱ����*/
#define		MESSAGE_WATCHLEVEL_TYPE				0x13		/*��Ŀ��������趨�ۿ�����*/
#define		MESSAGE_PAIRING_TYPE					0x14		/*����û�ж�Ӧ*/
#define		MESSAGE_NET_ERROR_TYPE					0x17		/*���粻��ȷ��������ȷ��������ʹ��*/
#define		MESSAGE_NOENTITLE_TYPE				0x20		/*��û�й���ý�Ŀ*/
#define		MESSAGE_DECRYPTFAIL_TYPE				0x21		/*��Ŀ����ʧ��*/
#define		MESSAGE_NOMONEY_TYPE				0x22		/*���ڽ���*/
#define		MESSAGE_ERRREGION_TYPE				0x23		/*������ȷ*/
#define		MESSAGE_FINGER_SHOW_TYPE			0x24		/*ָ����ʾ*/
#define		MESSAGE_FINGER_DISAPPEAR_TYPE		0x30		/*ָ������*/
#define		MESSAGE_CARDNOTACTIVE_TYPE			0x31		/*���ܿ�δ����*/
#define		MESSAGE_CARDLOGOUT_TYPE				0x32		/*���ܿ���ע��*/
#define		MESSAGE_CONDITION_CTL_TYPE			0x33		/*�ò�Ʒ���޲�*/
#define		MESSAGE_DESCRA_NOENTITLE_TYPE		0x34		/*��Ȩ�ѵ��ڣ�����ϵ��Ӫ��*/
#define		MESSAGE_ENTITLE_UPDATE_TYPE			0x35		/*��Ȩ�����У����Եȣ�*/
#define		MESSAGE_CARD_UNWONTED_TYPE			0x36		/*���ܿ��ڲ��쳣*/
#define		MESSAGE_CHANGEPRO_TOO_FAST_TYPE			0x37		/*�벻ҪƵ����̨*/
#define		MESSAGE_STB_NEED_UPDATE_TYPE		0x38		/*��������Ҫ���������Ե�*/

#define    	MESSAGE_MS_PAIR_OK_TYPE				0x40		/* ��ĸ����Գɹ� */
#define    	MESSAGE_MS_PAIR_FAILED_TYPE			0x41		/* ��ĸ�����ʧ��,�����²�����ȷ��ĸ�� */
#define    	MESSAGE_MS_REPAIR_TYPE				0x42		/* �ӿ������º�ĸ����� */
#define		MESSAGE_CARD_MATCH_OK_TYPE			0x43		/*������Գɹ�*/
#define		MESSAGE_CARD_REMATCH_TYPE			0x44		/*���»������*/
#define		MESSAGE_CANCEL_MATCH_TYPE			0x45		/*ȡ���������*/

#define		MESSAGE_NEWEMAIL_TYPE				0x50		/* ��ʾ���ʼ�ͼ�� */  
#define		MESSAGE_OVEREMAIL_TYPE				0x51		/* ��ʾ�ʼ����� */
#define		MESSAGE_HIDE_NEWEMAIL_TYPE			0x52		/* �������ʼ�ͼ��*/
#define		MESSAGE_EXIGENT_PROG_TYPE			0x53		/*�����㲥*/
#define		MESSAGE_EXIGENT_PROG_STOP_TYPE		0x54		/*ֹͣ�����㲥*/
#define		MESSAGE_CARD_UNLOCK_OK_TYPE		0x55		/*���ܿ���������ɹ�*/
#define		MESSAGE_PURSER_UPDATE_TYPE			0x56		/*����Ǯ������*/
#define		MESSAGE_NET_ERR_LOCK_TYPE			0x57		/*�����쳣����������*/
#define		MESSAGE_SYSTEM_ERR_LOCK_TYPE		0x58		/*ϵͳ�쳣����������*/
#define		MESSAGE_NET_ERR_UNLOCK_TYPE			0x59		/*�����쳣����*/
#define		MESSAGE_SYSTEM_ERR_UNLOCK_TYPE		0x60		/*ϵͳ�쳣����*/

/*OSD��Ϣ*/
#define		MESSAGE_SHOWOSD_TYPE				0x61		/*��ʾOSD��Ϣ*/
#define		MESSAGE_HIDEOSD_TYPE					0x62		/*����OSD��Ϣ*/

/*���ܿ�����*/
#define		MESSAGE_CARD_UPDATE_BEGIN_TYPE		0x65		/*���ܿ���ʼ���£���ʾ������*/
#define		MESSAGE_CARD_UPDATE_PROGRESS_TYPE	0x66		/*���½���*/
#define		MESSAGE_CARD_UPDATE_ERR_TYPE		0x67		/*���³����������*/
#define		MESSAGE_CARD_UPDATE_END_TYPE		0x68		/*���³ɹ�����������ʧ*/
#define		MESSAGE_CARD_NEED_UPDATE_TYPE		0x69		/*���ܿ���Ҫ���£����Ժ�*/

/*��ʾ�Ի���*/
#define		MESSAGE_CARDISDUE_TYPE					0x70		/*���ܿ�����Ƿ�Ѵ߽�*/
#define		MESSAGE_IPPVBUY_TYPE					0x71		/*IPPV������ʾ�� */
#define		MESSAGE_IPPV_OUTTIME_TYPE				0x72		/*IPPV��Ŀ����*/
#define		MESSAGE_HIDE_IPPVBUY_TYPE				0x73		/*���� IPPV������ʾ�� */
#define		MESSAGE_CONDITION_SEARCH_TYPE			0x74		/*����NIT��������Ŀ*/

/*�ڲ����Կ���ʾ��Ϣ*/
#define		MESSAGE_URGE_OPEN_TYPE					0x80
#define		MESSAGE_URGE_CLOSE_TYPE					0x81
#define		MESSAGE_URGE_DAYS_TYPE					0x82
#define		MESSAGE_URGE_OSD_SHOWTIME_TYPE		0x83
#define		MESSAGE_ALLCARD_ADDR_CTL_OPEN_TYPE	0x84
#define		MESSAGE_ALLCARD_ADDR_CTL_CLOSE_TYPE	0x85
#define		MESSAGE_CARD_PAIR_DAYS_TYPE				0x86
#define		MESSAGE_WT_OPEN_TYPE					0x87
#define		MESSAGE_WT_CLOSE_TYPE					0x88
#define		MESSAGE_WL_OPEN_TYPE					0x89
#define		MESSAGE_WL_CLOSE_TYPE					0x8A
#define		MESSAGE_ALLCARD_FINGER_OPEN_TYPE		0x8B
#define		MESSAGE_ALLCARD_FINGER_CLOSE_TYPE		0x8C
#define		MESSAGE_FINGER_SHOW_TIME_TYPE			0x8D
#define		MESSAGE_FINGER_STOP_TIME_TYPE			0x8E
#define		MESSAGE_DUE_OPEN_TYPE					0x8F
#define		MESSAGE_DUE_CLOSE_TYPE					0x90
#define		MESSAGE_DUE_DAYS_TYPE					0x91
#define		MESSAGE_CARD_OVERDUE_TIME_TYPE			0x92
#define		MESSAGE_URGE_OSD_UPDATE_TYPE			0x93
#define		MESSAGE_EXIGENT_OSD_UPDATE_TYPE		0x94
#define		MESSAGE_CARD_TYPE_TYPE					0x95
#define		MESSAGE_MOTHER_CARD_UPDATE_TYPE		0x96
#define		MESSAGE_CARD_FINGER_OPEN_TYPE			0x97
#define		MESSAGE_CARD_FINGER_CLOSE_TYPE			0x98
#define		MESSAGE_CARD_ADDR_CTL_OPEN_TYPE		0x99
#define		MESSAGE_CARD_ADDR_CTL_CLOSE_TYPE		0x9A
#define		MESSAGE_CARD_ADDR_UPDATE_TYPE			0x9B
#define		MESSAGE_ENTITLE_TYPE_UPDATE_TYPE		0x9C
#define		MESSAGE_ALL_CARD_ENTITLE_OPEN_TYPE		0x9D
#define		MESSAGE_ALL_CARD_ENTITLE_CLOSE_TYPE	0x9E
#define		MESSAGE_ALL_CARD_ENTITLE_UPDATE_TYPE	0x9F
#define		MESSAGE_EXIGENT_PROG_OPEN_TYPE			0xA0
#define		MESSAGE_EXIGENT_PROG_CLOSE_TYPE		0xA1
#define		MESSAGE_CARD_GROUP_ID_UPDATE_TYPE		0xA2
#define		MESSAGE_CARD_LCO_UPDATE_TYPE			0xA3
#define		MESSAGE_SYS_MONITOR_OPEN_TYPE			0xA4
#define		MESSAGE_SYS_MONITOR_CLOSE_TYPE			0xA5
#define		MESSAGE_NET_MONITOR_OPEN_TYPE			0xA6
#define		MESSAGE_NET_MONITOR_CLOSE_TYPE			0xA7
#define		MESSAGE_NET_ERR_TIME_UPDATE_TYPE		0xA8
#define		MESSAGE_CARD_OUTTIME_OPEN_TYPE			0xA9
#define		MESSAGE_CARD_OUTTIME_CLOSE_TYPE		0xAA
#define		MESSAGE_DESRCAM_PRO_OPEN_TYPE			0xAB
#define		MESSAGE_DESRCAM_PRO_CLOSE_TYPE			0xAC
#define		MESSAGE_MULTI_PRO_OPEN_TYPE				0xAD
#define		MESSAGE_MULTI_PRO_CLOSE_TYPE			0xAE

#define		MESSAGE_CLEAN_SCREEN_TYPE			0xF0
/****************************************************************/
/****************************************************************/

#define		DSCA_OPERATOR_NAME_LEN				(40)
#define		DSCA_OPERATOR_PHONE_LEN				(40)
#define		DSCA_OPERATOR_ADDRESS_LEN			(80)

#define    IN                         /*��������(��������)*/
#define	 OUT                      /*��������(�ش�����)*/
#define    DSCA_CARD_REGION_DESC   0XA5/*NIT�������ܿ�����ID������*/

#ifndef		NULL
#define		NULL		0
#endif

#ifndef TRUE
#define TRUE				(1)
#endif

#ifndef FALSE
#define FALSE			(0)
#endif

#ifndef BOOL
#define BOOL		unsigned char
#endif

typedef char				INT8;
typedef unsigned char		UINT8;
typedef short				INT16;
typedef unsigned short		UINT16;
typedef int				INT32;
typedef unsigned int		UINT32;
typedef UINT32			DSCA_Semaphore;
typedef UINT32			DSCA_MsgQueue; 

//ʱ���ʽ
typedef struct _ST_TIME_INFO
{
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_TIME_INFO;

//����ʱ���ʽ
typedef struct _ST_DATE_TIME_INFO
{
	UINT16	usYear;
	UINT8	ucMonth;
	UINT8	ucDay;
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_DATE_TIME_INFO;

//�ʼ���Ϣ
typedef struct _ST_MAIL_INFO
{
	UINT8				ucMailID;				// Mail ID 
	UINT8				ucNewFlag;				// 0 �������ʼ� 1�����ʼ� 
	UINT8				aucMailTitle[32 + 1];		// �ʼ����⣬�Ϊ30
	UINT8				aucMailSender[32 + 1];	//�ʼ�������
	ST_DATE_TIME_INFO	stSendTime;				//�ʼ�����ʱ��(����ʱ��)
}ST_MAIL_INFO;

//��Ʒ��Ȩ��Ϣ
typedef struct  _ST_PRODUCT_ENTITLE
{
	UINT8			ucEnableType;		/*��Ȩģʽ*/
	UINT8          		ucRecoadFlag;		/* ��û���� ��0��������¼�� 1������¼��*/
	UINT16        		usProductID;		/* ��ƷID */
	ST_DATE_TIME_INFO 	stBeginTime;		/* ��Ʒ��ʼʱ��*/
	ST_DATE_TIME_INFO 	stEndTime;		/* ��Ʒ����ʱ��*/
}ST_PRODUCT_ENTITLE;

//�ʼ��б���Ϣ
typedef struct _ST_MAILLIST_INFO
{
	ST_MAIL_INFO	stMailInfo[50];
	UINT8 		ucMailNum;
	
}ST_MAILLIST_INFO;

//��Ӫ����Ϣ
typedef struct _ST_OPERATOR_INFO
{
	UINT8 aucOperatorName[DSCA_OPERATOR_NAME_LEN];
	UINT8 aucOperatorPhone[DSCA_OPERATOR_PHONE_LEN];
	UINT8 aucOperatorAddress[DSCA_OPERATOR_ADDRESS_LEN];
	
}ST_OPERATOR_INFO;

//IPPV��Ϣ
typedef struct _ST_IPPV_INFO
{
	UINT8			aucIppvName[33];
	UINT8			aucEventID[4];
	UINT8			ucProviderID;     
	UINT16			usPrice;  
	ST_DATE_TIME_INFO	stBeginTime;
	ST_DATE_TIME_INFO	stEndTime;

}ST_IPPV_INFO;

//ָ����ʾλ��
typedef enum _EN_FINGER_SHOW_POS
{
	SHOW_LEFT_TOP = 0,
	SHOW_RIGHT_TOP,
	SHOW_LEFT_BOTTOM,
	SHOW_RIGHT_BOTTOM,
	SHOW_BY_COORDINATE,
	
}EN_FINGER_SHOW_POS;

//ָ����ʾ��Ϣ
typedef struct _ST_FINGER_SHOW_INFO
{
	EN_FINGER_SHOW_POS enShowPostion;		/*��ʾλ��*/
	UINT8 aucFontShowColor[4];				/*������ʾ��ɫ*/
	UINT8 aucBackShowColor[4];				/*������ʾ��ɫ*/
	UINT16 usShowXPos;						/*��ʾX����*/
	UINT16 usShowYPos;						/*��ʾY����*/
	UINT8 ucShowFont;						/*��ʾ����*/
	UINT8 ucShowFontSize;					/*��ʾ�����С*/
	UINT8 pucShowContent[20];				/*��ʾ����*/
	
}ST_FINGER_SHOW_INFO;

//OSD��ʾλ��
typedef enum _EN_OSD_SHOW_POS
{
	SHOW_TOP = 0,					//�ڶ�����ʾ
	SHOW_BOTTOM,					//�ڵײ���ʾ
	SHOW_TOP_BOTTOM,				//�ڶ����͵ײ�ͬʱ��ʾ
	SHOW_LEFT,						//�������ʾ
	SHOW_RIGHT,					//���ұ���ʾ	
	SHOW_LEFT_RIGHT,				//����ߺ��ұ�ͬʱ��ʾ
}EN_OSD_SHOW_POS;

//OSD��ʾ��Ϣ
typedef struct _ST_OSD_SHOW_INFO
{
	EN_OSD_SHOW_POS	ucDisplayPos;
	UINT8	ucFontType;
	UINT8	ucFontSize;
	UINT8	aucBackgroundColor[4];
	UINT8	aucDisplayColor[4];
	UINT16	usContentSize;
	UINT16	usGetBufferSize;
	UINT8* 	pucDisplayBuffer;
	
}ST_OSD_SHOW_INFO;

//����ʱ����Ϣ
typedef struct _ST_WORKTIME_INFO
{
	ST_TIME_INFO stBeginTime;
	ST_TIME_INFO stEndTime;
}ST_WORKTIME_INFO;

typedef enum _EN_CA_LANGUAGE
{
	CA_LANGUAGE_CHINESE,
	CA_LANGUAGE_ENGLISH,
	CA_LANGUAGE_TOTAL,
}EN_CA_LANGUAGE;

typedef struct _ST_CA_EXIGENT_PROGRAM
{
	UINT16 usNetWorkID;
	UINT16 usTransportStreamID;
	UINT16 usServiceID;
}ST_CA_EXIGENT_PROGRAM;

typedef struct _ST_CA_MSG_QUEUE
{
	UINT32 uiFirstPara;
	UINT32 uiSecondPara;
	UINT32 uiThirdPara;
	UINT32 uiFourthPara;
}ST_CA_MSG_QUEUE;

typedef enum _EN_CAS_SHOW_MSG_TYPE
{
	CAS_MSG_TYPE_PICTURE,//��ʾͼƬ
	CAS_MSG_TYPE_FINGER_SHOW,//��ʾָ��
	CAS_MSG_TYPE_OSD,//��ʾosd
	CAS_MSG_TYPE_NOTE_LONG_TIME,//������ʾ����Ϣ
	CAS_MSG_TYPE_NOTE_SHORT_TIME,//��ʾʱ��̵ܶ���Ϣ
	CAS_MSG_TYPE_DIALOG,//�Ի���
	CAS_MSG_TYPE_PROGRESS,//������
	CAS_MSG_TYPE_NOT_SHOW,//ֻ�ǽ���ĳ�ֲ��������漰����ʾ
	CAS_MSG_TYPE_TOTAL,
}EN_CAS_SHOW_MSG_TYPE;

//��������ֵ����
typedef enum _EN_ERR_CAS
{
	/*�������Ͷ���*/
	CA_ERROR_OK = 0x00,
	CA_CARD_FAILURE,                           /*��ͨѶʧ��*/
	CA_CARD_SUCCESS,                             /*��ͨѶ�ɹ�*/
	CA_CARD_NOMONEY,                            /*���ڽ���*/
	CA_CARD_WRITEPURSER_ERROR,          /*дǮ��ʧ��*/
	CA_CARD_ERROR_REGION,		     /*������ȷ*/
	CA_CARD_CHECK_PIN_OK,                   /*��֤PIN��ɹ�*/
	CA_CARD_CHECK_PIN_ERROR,		     /*��֤PIN��ʧ��*/
	CA_CARD_CHANGE_PIN_FAILED,        /*�޸�PINʧ��*/
	CA_CARD_CHECK_PIN_LOCK,               /*��֤PIN������Ѵ�����ļ�����*/
	CA_CARD_UNLOCK_PIN_FAILED,	     /*����PINʧ��*/
	CA_CARD_INITING,				    /*���ܿ����ڳ�ʼ��*/
	CA_ERROR_IC_INVALID ,                     /* ���������޿�����Ч��         */ 
	CA_ERROR_CARD_PAIROTHER,            /* ���ܿ��Ѿ���Ӧ��Ļ�����     */
	CA_ERROR_CARD_STOP,        			/* ����ͣ��    */
	CA_ERROR_CARD_CANCELED,        		/* ����ע��  */
	CA_ERROR_CARD_OUTTIME,			/* ���ѹ��� */
	CA_ERROR_CARD_PENDING,        			/* ��δ����    */
	CA_ERROR_CARD_NOPAIR,                  /* ��Ҫ��������*/
	CA_ERROR_PARAMETER,                      /*��������*/   
	CA_ERROR_FLASHADDR,                      /*FLASH��ַ����ΪNULL��*/
	CA_ERROR_UNKOWNDATA,                  /*δ֪���ݣ�����CA����Ҫ������*/
	CA_ERROR_REPEATINGDATA,             /*�ظ�������*/
	CA_ERROR_PAIRED_OK,                      /*��ĸ����Գɹ�*/
	CA_ERROR_WORKTIME,                         /*���ڹ���ʱ��*/
	CA_ERROR_WATCH_LEVLE,                 /*��ǰ��Ŀ�ۿ�����������ü���*/
	CA_ERROR_PAIRED_FAILED,              /*��ĸ�����ʧ��*/
	CA_ERROR_PAIRED_REPAIRED,          /*��ĸ����Ҫ�������*/
	CA_ERROR_NOENTITLE,                      /*û����Ȩ*/
	CA_ERROR_IPPV_NOBUY,                    /*IPPVδ����*/
	CA_ERROR_NETTEST,                         /*��������*/
	CA_ERROR_CRC,                                 /*CRC����*/
	CA_ERROR_WORKTIME_FORMAT,		/*����ʱ�θ�ʽ����*/
	CA_ERROR_MALLOC_ERR,			/*�ڴ����ʧ��*/
	CA_ERROR_CONDITION_CTL,			/*�ò�Ʒ�޲�*/
	CA_ERROR_NET_ERR,			/*�����쳣��STB������*/
	CA_ERROR_SYS_ERR,			/*ϵͳ�쳣��STB������*/
	CA_ERROR_STATE_ERR,			/*״̬������*/
	CA_ERROR_CARD_UNWONTED,			/*���������쳣*/
	CA_ERROR_OPT_TOO_FAST,		/*����̫Ƶ��*/
	CA_ERROR_CARD_NEED_UPDATE,		/*���ܿ���Ҫ����*/
	CA_ERROR_STB_NEED_UPDATE,		/*��������Ҫ����*/
	CA_ERROR_UNKNOWN,                       /* δ֪����*/
}EN_ERR_CAS;

typedef enum _EN_CA_MSG_QUEUE_MODE
{
	CA_MSG_QUEUE_WAIT,
	CA_MSG_QUEUE_NOWAIT,
	CA_MSG_QUEUE_UNKNOW,
}EN_CA_MSG_QUEUE_MODE;

typedef enum _EN_CA_DATA_ID
{
	ECM_DATA_ID = 1,
	EMM_DATA_ID = 2,
	OSD_DATA_ID = 3,
	MAIL_DATA_ID = 4,
	DELE_OSD_DATA_ID = 5,
	CONFIG_DATA_ID = 6,
	CADRESS_DATA_ID = 7,
	DELE_CADRESS_DATA_ID = 8,
	SK_DATA_ID = 9,
	TOTAL_DATA_TYPE = 10,
}EN_CA_DATA_ID;

/***********************************�������ṩ��CA�ĺ���**************************************/

extern void DSCA_printf(IN const INT8 *string,...);
extern void DSCA_GetBuffer(OUT UINT32 * puiStartAddr,  OUT UINT32 * puiSize);
extern void DSCA_FlashRead(UINT32 uiStartAddr,  OUT UINT8 * pucData,  IN UINT32 * puiLen);
extern void DSCA_FlashWrite(UINT32 uiStartAddr, IN UINT8* pucData, IN UINT32 uiLen);
extern void DSCA_FlashErase(void);
extern void* DSCA_Malloc(UINT16 usBufferSize);
extern void DSCA_Memset(IN void* pucBuffer, UINT8 ucValue, UINT32 uiSize);
extern void DSCA_Memcpy(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSCA_Memcmp(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSCA_Sprintf(IN UINT8* pucDestBuffer, IN const UINT8* pucFormatBuffer, ...);
extern INT32 DSCA_StrLen(IN const UINT8* pucFormatBuffer);
extern void DSCA_Free(IN void* pucBuffer);
extern void DSCA_Sleep(UINT16 usMilliSeconds);				       
extern void DSCA_SemaphoreInit(IN DSCA_Semaphore* puiSemaphore , UINT8 ucInitVal);
extern void DSCA_ReleaseSemaphore(IN DSCA_Semaphore* puiSemaphore);
extern void DSCA_WaitSemaphore(IN DSCA_Semaphore* puiSemaphore);
extern void DSCA_ShowMessage(UINT8 ucMessageType, EN_CAS_SHOW_MSG_TYPE  enShowType, IN UINT8 * pucMessage);
extern void DSCA_SetCW(UINT16 usEcmPID, IN UINT8* pucOddKey, IN UINT8* pucEvenKey, IN UINT8* pucKeyLen);
extern void DSCA_StopCurrentProgram(void);
extern void DSCA_PlayCurrentProgram(void);
extern void DSCA_SendDataToSmartCard(IN UINT8* pucSend, IN UINT8 *pucSendLen, OUT UINT8* pucReceive,
											OUT UINT8 *pucReceiveLen, UINT32 uiTimeout, OUT UINT8* pucStatusWord );
extern BOOL DSCA_RegisterTask (IN INT8* pucName, UINT8 ucPriority, IN void* pTaskFun );
extern BOOL DSCA_SCReset(UINT8 *pucAtr, UINT8 *pucLen);
extern BOOL DSCA_SetFilter(EN_CA_DATA_ID enDataID, IN UINT8* pucFilter, IN UINT8* pucMask,
							UINT8 ucFilterLen, UINT16 usPID, UINT8 ucWaitSeconds);
extern BOOL DSCA_StopFilter(EN_CA_DATA_ID enDataID, UINT16 usPID);
extern BOOL DSCA_QueryProgram(UINT16 usNetWorkID, UINT16 usTransportStreamID, UINT16 usServiceID);
extern BOOL DSCA_MsgQueueInit(IN INT8* pucName, UINT32* uiMsgQueueHandle, UINT32 uiMaxMsgLen, EN_CA_MSG_QUEUE_MODE enMode);
extern BOOL DSCA_MsgQueueGetMsg(UINT32 uiMsgHandle, ST_CA_MSG_QUEUE* pstMsg, EN_CA_MSG_QUEUE_MODE enMode, UINT32 uiWaitMilliSecond);
extern BOOL DSCA_MsgQueueSendMsg(UINT32 uiMsgHandle, ST_CA_MSG_QUEUE* pstMsg);
/*****************************************************************************************************/

/*******************************CA �ṩ�������е��õĺ���********************************/
void DSCA_SetEcmPid(UINT16 usPid);
void DSCA_SetEmmPid(UINT16 usPid);
void DSCA_PowerOff(void);
void DSCA_PowerOn(void);
void DSCA_SetCardState(UINT8  ucInOrOut);
void DSCA_SetSysLanguage(EN_CA_LANGUAGE enLanguage);
void DSCA_SetCardAddressCode(IN UINT8* pucCardCode);
void DSCA_OsdOneTimeOk(void);
void DSCA_GetPrivateData(UINT16 usPid, EN_CA_DATA_ID enDataID, IN UINT8* pucReceiveData, UINT16 usDataLen);
BOOL DSCA_IsDSCa(UINT16 usCaSystemID);
BOOL DSCA_QueryBroadcastEntitle(void);
EN_ERR_CAS DSCA_SC_UnlockPin(void);
EN_ERR_CAS DSCA_DelEmail(UINT8 ucMailID);
EN_ERR_CAS DSCA_DelAllEmail(void);
EN_ERR_CAS DSCA_GetCardRegion(OUT UINT8 * pucCardRegion);
EN_ERR_CAS DSCA_GetOperatorID(OUT UINT8 * pucOperator);
EN_ERR_CAS DSCA_GetOperatorInfo(ST_OPERATOR_INFO* pstOperatorInfo );
EN_ERR_CAS DSCA_SetWatchLevelState(UINT8 ucOpenOrClose);
EN_ERR_CAS DSCA_SetWorkTimeState(UINT8 ucOpenOrClose);
EN_ERR_CAS DSCA_IppvBuy(UINT8 ucBuy,UINT8 ucProvideID) ;
EN_ERR_CAS DSCA_Init(UINT8 ucPriority, EN_CA_LANGUAGE enLanguage);
EN_ERR_CAS DSCA_GetProductNumber(UINT32 uiOperatorID, OUT UINT32 *puiNumber);
EN_ERR_CAS DSCA_GetProductInfo (UINT32 uiOperatorID,  UINT32 uiStartNo,  UINT32 uiReadNum, OUT ST_PRODUCT_ENTITLE *pstProductEntitles);
EN_ERR_CAS DSCA_GetCardNo(OUT UINT8 * pucCardNO, OUT UINT8* pucCardType);
EN_ERR_CAS DSCA_GetMasterCard(UINT8 * pucMasterCardNO);
EN_ERR_CAS DSCA_GetCAVer(OUT UINT8 * pucCAVersion);
EN_ERR_CAS DSCA_GetLCO(OUT UINT8 aucLco[8]);
EN_ERR_CAS DSCA_GetGroupId(OUT UINT8 aucGroupId[16]);
EN_ERR_CAS DSCA_GetCardVersion(OUT UINT8 aucCardVer[32]);
EN_ERR_CAS DSCA_GetPurserInfo(UINT8 ucNo, OUT UINT8 * pucPurser);
EN_ERR_CAS DSCA_GetEmailNumber(OUT UINT8 *pstEmailNo);
EN_ERR_CAS DSCA_GetEmailInfoList(OUT ST_MAILLIST_INFO *pstEmailInfoList);
EN_ERR_CAS DSCA_GetEmailContent(UINT32 uiEmailID, OUT UINT8* pucContentBuf, OUT UINT16 *pusContentLen);
EN_ERR_CAS DSCA_GetCardState(void);
EN_ERR_CAS DSCA_ChangePin(IN UINT8* pucOldPin, IN UINT8* pucNewPin, OUT UINT8* ucOddTimes);
EN_ERR_CAS DSCA_PurseCheckPin(UINT8* pucPin, UINT8* pucOddTimes);
EN_ERR_CAS DSCA_SetWorkTime(IN UINT8* pucPin, OUT UINT8* pucOddTimes, IN ST_WORKTIME_INFO* pstWorkTime);
EN_ERR_CAS DSCA_SetWatchLevel(IN UINT8* pucPin, OUT UINT8* pucOddTimes, IN UINT8* pucWorkLevel);
EN_ERR_CAS DSCA_GetWorkTime(OUT ST_WORKTIME_INFO* pstWorkTime);
EN_ERR_CAS DSCA_GetWorkLevel(OUT UINT8* pucWorkLevel);
/****************************************************************************************************/
#endif

