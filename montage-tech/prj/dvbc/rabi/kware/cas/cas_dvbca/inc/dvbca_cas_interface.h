#ifndef __dvbca_interface_h__
#define __dvbca_interface_h__


#define	OUT				/*�������������*/
#define 	IN				/*�������������*/
#define 	INOUT			/*������������,�������������*/

#include "dvbca_types.h"
#include "stdio.h"

#define DVBCA_NET_NAME_LEN						(8)			/*�������Ƴ���*/
#define DVBCA_MAX_MAIL							(50)		/*����ʼ�����*/

#define DVBCA_MAIL_TITLE_SIZE					(10)		/*�ʼ�������󳤶�*/
#define DVBCA_MAIL_TERM_SIZE					(20)		/*�ʼ���������󳤶�*/
#define DVBCA_MAIL_CONTEXT_SIZE					(512)		/*�ʼ�������󳤶�*/

#define DVBCA_OPERATOR_NAME_SIZE				(20)		/*��Ӫ��������󳤶�*/

#define DVBCA_NOTIFY_TITLE_SIZE					(20)		/*Ӧ��ͨ�������󳤶�*/
#define DVBCA_NOTIFY_CONTENT_SIZE				(512)		/*Ӧ��ͨ��������󳤶�*/


#define DVBCA_MAX_FILTER_SIZE					(12)		/*��������󳤶�*/
#define DVBCA_MAXLEN_ENTITL						(100)		/*����Ӫ�����ܿ����������Ȩ��Ʒ�ĸ���*/
#define DVBCA_MAXLEN_ACLIST      				(5)      	/*���ܿ��ڱ���ÿ��Ӫ�̵�AC����*/ 
#define DVBCA_MAX_ECM_NUM						(2)			/*���֧��ECM pid ����*/
#define DVBCA_MAXLEN_MSG_TEXT					(4096)

/*��Ŀ�޷����ŵ���ʾ*/
typedef enum 
{
	DVBCA_MESSAGE_INSERTCARD = 0x01,		/*���Ž�Ŀ����������ܿ�*/
	DVBCA_MESSAGE_BADCARD = 0x02,			/*�޷�ʶ�𿨣�����ʹ��*/
	DVBCA_MESSAGE_NOENTITLE = 0x03,		/*û����Ȩ*/
	DVBCA_MESSAGE_NOOPER_TYPE = 0x04,		/*���в����ڽ�Ŀ��Ӫ��*/
	DVBCA_MESSAGE_BLOCK_AREA = 0x05,		/*�������*/
	DVBCA_MESSAGE_EXPIRED = 0x06,			/*��Ȩ����*/
	DVBCA_MESSAGE_PASSWORD_ERROR = 0x07,	/*��Ӫ���������*/
	DVBCA_MESSAGE_ERRREGION = 0x08,		/*������ȷ*/
	DVBCA_MESSAGE_PAIRING_ERROR = 0x09,	/*������ƥ��*/
	DVBCA_MESSAGE_WATCHLEVEL = 0x10,		/*��Ŀ��������趨�ۿ�����*/
	DVBCA_MESSAGE_EXPIRED_CARD = 0x011,	/*���ܿ�����*/
	DVBCA_MESSAGE_DECRYPTFAIL = 0x12,		/*��Ŀ����ʧ��*/	
	DVBCA_MESSAGE_WORKTIME = 0x13,		/*���ڹ���ʱ����*/	
	DVBCA_MESSAGE_MAX_NUM = 0xFF,
}DVBCA_MESSAGE;

typedef enum
{
	DVBCA_SUCESS = 0,			/*ִ�гɹ�*/
	DVBCA_FAILED = -1,			/*ִ��ʧ��*/
	DVBCA_INSERTCARD = -2,		/*���ܿ�δ����*/
	DVBCA_CARD_INVALID = -3, 	/*���ܿ���Ч*/
	DVBCA_ECM_PID_ERROR = -4, 	/*ECM Pid ����*/
	DVBCA_EMM_PID_ERROR = -5,	/*EMM Pid ����*/
	DVBCA_NOENTITLE = -6, 		/*û����Ȩ*/
	DVBCA_PIN_ERROR = -7, 		/*Pin�����*/
	DVBCA_STB_PAIR_FAILED = -8,	/*�������ʧ��*/
	DVBCA_INIT_ERROR = -9,		/*CA��ʼ��ʧ��*/
	DVBCA_CARD_VER_ERROR = -10,		/*�ð汾����֧�ָ�����*/
}DVBCA_ERROR_CODE;

typedef enum
{
	MSG_BOX_INSERTCARD = 0x01,		/*���Ž�Ŀ����������ܿ�*/
	MSG_BOX_BADCARD = 0x02,			/*�޷�ʶ�𿨣�����ʹ��*/
	MSG_BOX_NOENTITLE = 0x03,		/*û����Ȩ*/
	MSG_BOX_NOOPER_TYPE = 0x04,		/*���в����ڽ�Ŀ��Ӫ��*/
	MSG_BOX_BLOCK_AREA = 0x05,		/*�������*/
	MSG_BOX_EXPIRED = 0x06,			/*��Ȩ����*/
	MSG_BOX_PASSWORD_ERROR = 0x07,		/*��Ӫ���������*/
	MSG_BOX_ERRREGION = 0x08,		/*������ȷ*/
	MSG_BOX_PAIRING_ERROR = 0x09,		/*������ƥ��*/
	MSG_BOX_WATCHLEVEL = 0x10,		/*��Ŀ��������趨�ۿ�����*/
	MSG_BOX_EXPIRED_CARD = 0x011,		/*���ܿ�����*/
	MSG_BOX_DECRYPTFAIL = 0x12,		/*��Ŀ����ʧ��*/	
	MSG_BOX_WORKTIME = 0x14,			/*���ڹ���ʱ����*/
	
	MSG_BOX_PPC_MSG = 0x20,			/*��Ŀ%d�����*/
	MSG_BOX_FREQ_UPDATE = 0x21,		/*�����½�Ŀ,���������Զ�ɨ��*/
	MSG_BOX_SOFTWARE_UPGRADE = 0x22,		/*���������, ���Ժ�...*/ 
	MSG_BOX_BAD_SIGNAL = 0x24,		/*�ź��ж�,�벦�����绰*/ 
}EN_MSG_BOX_TYPE;

typedef enum
{
	UNDEF,
	QAM16,
	QAM32,
	QAM64,
	QAM128,
	QAM256
}DVBCA_MODUNATION_TYPE;/*�������ͱ���*/

typedef enum _EN_CA_DB_MAIL_CMD
{
	CA_DB_MAIL_TOTAL_NUM,
	CA_DB_MAIL_READ_NUM,	
	CA_DB_MAIL_NOT_READ_NUM,	
}EN_CA_DB_MAIL_CMD;

typedef struct _ST_DVBCAWORKTIME
{
	 DVBCA_UINT8 ucHour;			/*Сʱ*/
	 DVBCA_UINT8 ucMinute;		/*����*/
}ST_DVBCAWORKTIME;/*ʱ��ṹ*/

typedef struct _ST_DVBCATIME
{
	 DVBCA_UINT8 ucYear;			/*��*/
	 DVBCA_UINT8 ucMonth;		/*��*/
	 DVBCA_UINT8 ucDay;			/*��*/
	 DVBCA_UINT8 ucHour;			/*Сʱ*/
	 DVBCA_UINT8 ucMinute;		/*����*/
	 DVBCA_UINT8 ucSecond;		/*��*/
}ST_DVBCATIME;/*ʱ��ṹ*/


typedef struct _ST_DVBCAFilter{
	DVBCA_UINT8 aucFilter[DVBCA_MAX_FILTER_SIZE];		/*��ǰ��������ֵ*/
	DVBCA_UINT8 aucMask[DVBCA_MAX_FILTER_SIZE];		/*��ǰ������������*/
	DVBCA_UINT8 ucLen;								/*��ǰ�������ĳ���*/
}ST_DVBCAFilter;

typedef struct _ST_DVBCAEmail{
	DVBCA_UINT16 usEmailID;								/*�ʼ�ID*/
	DVBCA_UINT8 ucNewEmail;								/*�Ƿ����ʼ�*/
	DVBCA_UINT8 aucTitle[DVBCA_MAIL_TITLE_SIZE + 1];		/*�ʼ�����*/
	DVBCA_UINT8 aucTerm[DVBCA_MAIL_TERM_SIZE + 1];		/*�ʼ�������*/
	DVBCA_UINT8 aucContext[DVBCA_MAIL_CONTEXT_SIZE + 1];	/*�ʼ�����*/
}ST_DVBCAEmail;


typedef struct _ST_DVBCA_OPERATORINFO
{
	DVBCA_UINT8 aucOperatorName[DVBCA_OPERATOR_NAME_SIZE];	/*��Ӫ������*/
	DVBCA_UINT16 usNetworkID;									/*������*/
	DVBCA_UINT32 uiOperatorID;										/*��Ӫ�̱��*/
	ST_DVBCATIME	stExpiredTime;									/*���ܿ�����ʱ��*/
}ST_DVBCA_OPERATORINFO;

typedef struct _ST_DVBCA_ENTITLE{
	DVBCA_UINT8  ucStartYear;									/*��Ȩ��ʼʱ��*/
	DVBCA_UINT8  ucStartMonth;
	DVBCA_UINT8  ucStartDay;
	DVBCA_UINT8  ucExpiredYear;									/*��Ȩ����ʱ��*/
	DVBCA_UINT8  ucExpiredMonth;
	DVBCA_UINT8  ucExpiredDay;
	DVBCA_UINT16 usProductID;									/*��Ʒ���*/
}ST_DVBCA_ENTITLE;

typedef struct _ST_DVBCA_ENTITLES
{
	DVBCA_UINT16 	usProductCount;								/*��Ʒ����*/
	DVBCA_UINT16	usEventCount;
	DVBCA_UINT8 	ucProductVersion;
	DVBCA_UINT8 	ucEventVersion;
	ST_DVBCA_ENTITLE	astEntitles[DVBCA_MAXLEN_ENTITL];
}ST_DVBCA_ENTITLES;

typedef struct _ST_DVBCA_RGB
{
	DVBCA_UINT8 	ucRed;
	DVBCA_UINT8 	ucGreen;
	DVBCA_UINT8 	ucBlue;
	DVBCA_UINT8 	ucAlpha;
}ST_DVBCA_RGB;

typedef struct _ST_DVBCA_MSG
{
	DVBCA_UINT8 ucDispPos;
	DVBCA_UINT8 ucDispWay;
	DVBCA_UINT8 ucDispCount;
	DVBCA_UINT8 ucDispDirection;
	DVBCA_UINT8 ucIntervalTime;
	DVBCA_UINT8 ucBgColor;
	DVBCA_UINT8 ucFontColor;
	DVBCA_UINT8 aucText[DVBCA_MAXLEN_MSG_TEXT];
	ST_DVBCA_RGB stBgColor;
	ST_DVBCA_RGB stFontColor;
}ST_DVBCA_MSG;

typedef struct _ST_DVBCA_FREQ
{
	DVBCA_MODUNATION_TYPE ucModunation;
	unsigned int uiFrequency;
	unsigned int uiSymbolrate;
}ST_DVBCA_FREQ;

typedef struct _ST_DVBCA_UPGRADEPROG
{
	unsigned char ucFreqCount;  /*Ƶ��仯����*/
	unsigned char ucVer;/*Ƶ��仯�汾��*/
	ST_DVBCA_FREQ *pstFreq;
}ST_DVBCA_UPGRADEPROG;	

/*������ʵ�ֺ���*/
int DVBSTBCA_SCReset(void);
int DVBSTBCA_SetEcmFilter(IN ST_DVBCAFilter *pucFilter,					   
						   DVBCA_UINT8 ucFilterNum,					   
						   DVBCA_UINT16 usPid);
int DVBSTBCA_SetEmmFilter(IN ST_DVBCAFilter *pucFilter,							   
						   DVBCA_UINT8 ucFilterNum,					   
						   DVBCA_UINT16 usPid);
void DVBSTBCA_ScrSetCW(DVBCA_UINT16 usEcmPid,
					  IN DVBCA_UINT8 *pucOddKey,
					  IN DVBCA_UINT8 *pucEvenKey,
					  DVBCA_UINT8 ucKeyLen);
int DVBSTBCA_SCPBRun(IN DVBCA_UINT8 *pucCommand,
					DVBCA_UINT16 usCommandLen,
					OUT DVBCA_UINT8 *pucReply,
					OUT DVBCA_UINT16 *pusReplyLen,
					OUT DVBCA_UINT8 *pucStatusWord);

void DVBSTBCA_ReadBuffer(DVBCA_UINT32 uiAddr, 
						OUT DVBCA_UINT8 *pucData,
						DVBCA_UINT32 uiLen);
void DVBSTBCA_WriteBuffer(DVBCA_UINT32 uiAddr, 
						IN DVBCA_UINT8 *pucData,
						DVBCA_UINT32 uiLen);
void DVBSTBCA_FormatBuffer(DVBCA_UINT32 uiAddr, 
						DVBCA_UINT32 uiLen);

void DVBSTBCA_ShowPPCMessage(DVBCA_UINT16 usEcmPid, 
						 DVBCA_UINT8 ucDay);
void DVBSTBCA_ShowMessage(DVBCA_UINT16 usEcmPid, 
						 DVBCA_UINT8 ucMessageType);
void DVBSTBCA_HideMessage(DVBCA_UINT16 usEcmPid, 
						 DVBCA_UINT8 ucMessageType);
void DVBSTBCA_ShowFingerMessage(DVBCA_UINT16 usEcmPid, 
						 DVBCA_UINT32 uiCardID);
void DVBSTBCA_HideFingerMessage(void);
void DVBSTBCA_NewEmailNotify(DVBCA_UINT16 usEmailID);

void DVBSTBCA_HideUrgent(void);
void DVBSTBCA_ShowUrgent(IN DVBCA_UINT8 *pucTitle,
						IN DVBCA_UINT8 *pucContent);

void DVBSTBCA_ShowOSDMessage(IN ST_DVBCA_MSG *pstAdvertize);

void DVBSTBCA_HideOSDMessage(void);
void DVBSTBCA_ResumeOSDMessage(void);

void DVBSTBCA_UpgradeFreq(ST_DVBCA_FREQ *pstFreq, unsigned char ucFreqSum);

int DVBSTBCA_RegisterTask(IN const char *szName,
						 DVBCA_UINT8 ucPriority,
						 IN void *pTaskFun,
						 IN void *pParam,
						 DVBCA_UINT16 usStackSize);
void DVBSTBCA_Sleep(DVBCA_UINT16 usMilliSeconds);
void DVBSTBCA_SemaphoreInit(OUT DVBCA_SEMAPHORE *pSemaphore,
						   DVBCA_UINT32 uiInitValue);
void DVBSTBCA_SemaphoreSignal(INOUT DVBCA_SEMAPHORE *pSemaphore);
void DVBSTBCA_SemaphoreWait(INOUT DVBCA_SEMAPHORE *pSemaphore);
void DVBSTBCA_SemaphoreDestory(INOUT DVBCA_SEMAPHORE *pSemaphore);
void *DVBSTBCA_Malloc(DVBCA_UINT32 uiBufSize);
void DVBSTBCA_Free(void *pBuf);
void DVBSTBCA_Memset(void *pDestBuf, DVBCA_UINT8 ucValue, DVBCA_UINT32 uiSize);
void DVBSTBCA_Memcpy(void *pDestBuf, void *pSrcBuf, DVBCA_UINT32 uiSize);
DVBCA_UINT32 DVBSTBCA_Strlen(char *pString);

void DVBSTBCA_Printf(IN const char *fmt,...);


/*CAS�ӿں���*/
int DVBCASTB_Init(unsigned char ucThreadPriority, unsigned char *pucStbId);
int DVBCASTB_IsDVBCA(unsigned short usCaSystemID);
int DVBCASTB_SCInsert(void);
void DVBCASTB_SCRemove(void);
void DVBCASTB_SetEmmPid(unsigned short usEmmPid);
void DVBCASTB_SetEcmPid(IN unsigned short *pusEcmPid,
					   unsigned char ucEcmPidNum);
void DVBCASTB_PrivateDataGot(unsigned short usPid, 
							unsigned char *pucReceiveData,
							unsigned short usLen);
void DVBCASTB_NitDataGot(unsigned char *pucReceiveData,
							unsigned short usLen);
int DVBCASTB_GetCaVer(OUT unsigned char *pucCaVer);
int DVBCASTB_GetCardVer(OUT unsigned char *pucCardVer);
int DVBCASTB_GetCardID(OUT unsigned char *pucCardID);
int DVBCASTB_SetStbPair(IN unsigned char* pucPin);
int DVBCASTB_GetStbPair(OUT unsigned int *puiStbID, 
					OUT unsigned char *pucStbNum);
int DVBCASTB_GetOperatorID(OUT unsigned int *puiOperatorID, 
							OUT unsigned int *puiOperatorNum);
int DVBCASTB_GetOperatorInfo(unsigned int uiOperatorID, 
						OUT ST_DVBCA_OPERATORINFO *pstOperatorInfo);
int DVBCASTB_GetServiceEntitles(unsigned int uiOperatorID, 
						OUT ST_DVBCA_ENTITLES *pstEntitles);
int DVBCASTB_SetRating(IN unsigned char* pucPin, 
					unsigned char ucRating);
int DVBCASTB_GetRating(OUT unsigned char *pucRating);
int DVBCASTB_CheckPin(IN unsigned char* pucPin);
int DVBCASTB_ChangePin(IN unsigned char *pucOldPin, 
					unsigned char *pucNewPin);

unsigned short DVBCASTB_GetMailNum(EN_CA_DB_MAIL_CMD enCmd);
unsigned short DVBCASTB_GetMail(OUT ST_DVBCAEmail *pstMail, 
								unsigned char ucStartPos, 
								unsigned short usNum);
int DVBCASTB_SetMailRead(unsigned short usMailID);
int DVBCASTB_DelMail(unsigned short usMailID);

void DVBCASTB_ShowOsdMessageEnd(void);

void DVBCASTB_GetNetName(unsigned char *pucNetName);
int DVBCASTB_GetCurProgExpiredTime(unsigned short usEcmPid, 
						OUT ST_DVBCATIME *pstTime);
void DVBCASTB_RestoreDB(void);
int DVBCASTB_GetWorkTime(OUT ST_DVBCAWORKTIME *pstStartTime,
					OUT ST_DVBCAWORKTIME *pstEndTime);
int DVBCASTB_SetWorkTime(IN unsigned char* pucPin, 
					IN ST_DVBCAWORKTIME *pstStartTime,
					IN ST_DVBCAWORKTIME *pstEndTime);
int DVBCASTB_GetACList(unsigned int uiOperatorID, 
							OUT unsigned short *pusACArray,
							OUT unsigned int *puiACNum);
int DVBCASTB_GetPromptMsg(unsigned char ucPromptID, 
							unsigned char ucLanguage, 
							OUT unsigned char *pucText);

void DVBCASTB_UnInit(void);

int DVBCASTB_GetCurTime(OUT ST_DVBCATIME *pstTime);

#endif

