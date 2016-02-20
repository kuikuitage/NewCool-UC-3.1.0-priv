//******************************************************************//
//	����������ͨ�Ƽ��ɷ����޹�˾ CopyRight 1998-2010
//	��������ϵͳ V2.0
//	��������ֲ��ͷ�ļ�
//	�汾��2.1.0.0
//	���ڣ�2007-9-3
//  ���£�2009-12-29
//******************************************************************//
#ifndef __Pub_DiviGuard
#define __Pub_DiviGuard

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char INT8;
typedef signed short INT16;
typedef signed long  INT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long  UINT32;

//------------------------------------------------------------------------------------
//#define debug

//------------------------------------------------------------------------------------
#ifndef BOOL
#define BOOL unsigned char
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAILTURE
#define FAILTURE -1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif


//------------------------------------------------------------------------------------
#define CA_MAX_SERVICE_PER_ECM 10


//------------------------------------------------------------------------------------
#define CA_STB_FILTER_1 1
#define CA_STB_FILTER_2 2
#define CA_STB_FILTER_3 3
#define CA_STB_FILTER_4 4

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#define DIVI_WAIT_INFO 0
#define DIVI_WAIT_PRIO 1
#define DIVI_TASK_NOWAIT 2
#define DIVI_TASK_WAIT 3

//--------------------�ʼ�ϵͳ�õ��ĺ�
#define   MAX_EMAIL_NUM            10
#define   EMAIL_TITLE_LEN          36
#define   EMAIL_CONTENT_LEN      128

//-----------------------------------------------------------------------------------
typedef struct _ProductInfo{
     UINT8 PackageId;
     UINT16 StartYear;
     UINT16 StartMonth;
     UINT8   StartDate;
     UINT16 EndYear;
     UINT16 EndMonth;
     UINT8   EndDate;	 
     UINT16 ExpireDate;
     UINT16 PackagePrice;
     UINT8 PackageScript[20];	 
}ProductInfo;

typedef struct _TotalProductInfo{
     UINT8 Package_Count;
     ProductInfo PackageInfo[32];
}PackageInfo;

//harvey, 2009.06.17
typedef struct _TotalChildInfo{

     UINT16  ExpireYear;        //��
     UINT16  ExpireMonth;       //��
     UINT8   ExpireDate;        //��
     UINT8   ExpireHour;        //Сʱ
     UINT8   ExpireMinute;	    //��
     UINT8   ExpireSecond;      //��
	 
     UINT8 Main_Card_NO[8];     //ĸ������
     UINT8 Active_Duration[6];  //�������� 
     UINT8 Effective_Days;      //��Ч����
     UINT8 Lead_Time;           //��������ʱ��
     UINT8 Temp_Active_Duration;//��ʱ��������
	 
}ChildSMCInfo;


typedef struct _PayRecordInfo{
	UINT8 Record_Num;
	UINT8 Content[18][8];
}PayRecordInfo;

//-----------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
typedef struct {
        UINT32 q1stWordOfMsg;
	 UINT32 q2ndWordOfMsg;
	 UINT32 q3rdWordOfMsg;
	 UINT32 q4thWordOfMsg;
}DIVI_QUEUE_MESSAGE;

//------------------------------------------------------------------------------------
typedef struct _CAServiceInfo {
	UINT16 m_wEcmPid;
	UINT16 m_wOriNetworkID;
	UINT16 m_wTSID;
	UINT8 m_bServiceCount;
	UINT8 m_reserved;
	UINT16 m_wServiceId[CA_MAX_SERVICE_PER_ECM];
}DIVI_CAServiceInfo;

//---------------------�ʼ�ϵͳ��Ҫʹ�õĽṹ----------------------------
typedef struct _TDIVI_DATETIME
{
	UINT16 Y;   //��
	UINT16 M;  //��
	UINT8   D;  //��
	UINT8   H;  //ʱ
	UINT8   MI; //��
	UINT8   S;  //��
}TDIVI_DATETIME;

typedef struct _TDIVI_EmailHead{
	TDIVI_DATETIME	m_tCreateTime;      /*EMAIL������ʱ��*/
	UINT8    		m_bEmail_Level;     /*�ʼ���Ҫ�̶�*/
	UINT8		m_bNewEmail;         /*0 �������ʼ� 1�����ʼ�*/
	UINT16		m_bEmailID;            /*Email ID  */
	char   		m_szEmailTitle[EMAIL_TITLE_LEN];/*�ʼ����⣬�Ϊ36*/
}TDIVI_EmailHead;

typedef struct _TDIVI_EmailContent{
	char		m_szEmail[EMAIL_CONTENT_LEN];	/*Email������*/
}TDIVI_EmailContent;

//------------------------------------------------------------------------------------
typedef enum{
	DIVI_ERR = -100,
	DIVI_PIN_ERR,
	DIVI_PARENTLEVEL_ERR,
	DIVI_CARD_NOPAIR,
	DIVI_CARD_MISPAIR,
	DIVI_INPUT_PARA_ERR,
	DIVI_OK = 0
}DIVI_RESULT;
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
/*
INT32 DIVI_Initialize(UINT8 mailManagerType); 
�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
��ʱ���ã� �ڻ����г�ʼ����ʱ����øú����� 
���������� ��ʼ��CAģ�顣 
��������� �������� 
��������� �ޡ� 
�� �� ֵ�� 
SUCCESS���ɹ��� 
FAILURE�� ʧ�ܡ�
*/
INT32 DIVI_Initialize(UINT8 mailManagerType);

/*
BOOL DIVI_IsMatchCAID(UINT16 wCASystemID) 
�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
��ʱʹ�ã� �յ�CA�����������ݺ� 
���������� �ж�CAϵͳID�Ƿ���DIVI�� 
��������� 
		wCASystemID��CAϵͳID�� 
��������� �ޡ� 
�� �� ֵ�� 
		TRUE���Ǳ�CA��
		FALSE�����Ǳ�CA��
*/
BOOL  DIVI_IsMatchCAID(UINT16 wCASystemID);

/*
void DIVI_SetEcmPID (); 
�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
��ʱʹ�ã� ������ECM PIDʱ(���л�Ƶ��)���øýӿڰ������Ϣ���ݸ�CAģ�顣 
���������� ��ECM��PID����Ϣ���ݸ�CAģ�顣 
��������� 
		bType�� �������ã�
		pEcmInfo:T_CAServiceInfo�ṹ��������Ŀ����ϸ��Ϣ; ��pEcmInfo->m_wEcmPidΪ0xffff,���ͷ�channel��filter��
 
��������� �ޡ� 
�� �� ֵ�� �ޡ� 

*/
void  DIVI_SetEcmPID(UINT8 bType,DIVI_CAServiceInfo * pEcmInfo);

/*
void DIVI_SetEmmPID (); 
�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
��ʱʹ�ã� �����µ�EMM PIDʱ���øú��������ݴ��ݸ�CAģ�顣 
���������� ������CAT������ݴ��ݸ�CAģ�顣 
��������� 
		bType���������ã� 
		wEmmPid����Ӧ��DIVI ��CAS_System_ID��EMM��PIDֵ����wEmmPidΪ0xffff,���ͷ�channel��filter�� 

��������� �ޡ� 
�� �� ֵ�� �ޡ�
*/
void  DIVI_SetEmmPID(UINT8 bType,UINT16 wEmmPid);

/*
void DIVI_TableReceived (); 
�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
��ʱʹ�ã� ����CA��˽�����ݵ�ʱ�򣬽�˽�����ݽ�����������ʱ��������ӿڡ� 
���������� ����CA��˽�����ݵ�ʱ�򣬽�˽�����ݽ������������������ݴ��ݸ� CAģ�飬ʹ��CAģ����Դ�����Щ���ݡ� 
��������� 
		bRequestID��CA_STB_FILTER_4ΪEcm Filter���յ����ݣ�
					CA_STB_Filter_1ΪEmm Filter1���յ����ݣ�
					CA_STB_Filter_2ΪEmm Filter2���յ����ݣ�
					CA_STB_Filter_3ΪEmm Filter3���յ����ݡ�
		bOK�� ��ȡ������û�гɹ���TRUE���ɹ���FALSE��ʧ�ܡ� 
		wPid�� ���յ�����PID�� 
		pbReceiveData�� ��ȡ˽�����ݵ�ָ�룬CAģ�鲻������ռ���ͷš� 
		wLen�� ��ȡ����˽�����ݵĳ��ȡ� 
��������� �ޡ� 
�� �� ֵ�� �ޡ�
*/
void  DIVI_TableReceived(UINT8 bRequestID,UINT8 bOK,UINT16 wPid,const UINT8 * pbReceiveData,UINT16 wLen);

/*
INT32 DIVI_GetSMCNO(); 
�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
��ʱ���ã� ��Ҫ��ȡ�û��Ŀ�NOʱ�� 
���������� ȡ���û����ܿ���NO�� 
��������� 
		pbCardNO�� ��ſ�NO�ĵ�ַָ�� 
��������� 
		pbCardNO�� ��NO�� 
�� �� ֵ�� 
		DIVI_OK���ɹ��� 
		DIVI_ERR, ��������,����issueDate��expireDate�����ָ��Ϊ��ָ�룬�������ܿ���δ���롣 
����˵���� ��NOΪBCD�루8���ֽڣ���
*/
INT32 DIVI_GetSMCNO(UINT8 * pbCardNO);



/*
INT32 DIVI_GetSMCUserType() 
�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
���������� ��ȡ���ܿ��û������� 
��������� �� 
��������� �� 
�� �� ֵ�� 
		-1,����ʧ��
		����,�����͡�08:ĸ����09:�ӿ�
����˵����
		�û�������SMSϵͳ����
*/
INT32 DIVI_GetSMCUserType(void);

/*
INT32 DIVI_GetSMCUserLevel() 
�� �� �ߣ� CAģ�顣 
ʹ �� �ߣ� �����С� 
���������� ��ȡ���ܿ��û��ĵȼ� 
��������� �� 
��������� �� 
�� �� ֵ�� 
		-1,����ʧ��
		����,������
����˵���� 
		���ܿ��û��ȼ����û�����ϵͳ����,�������û�����ͨ���˺�����ѯ��
*/
INT32 DIVI_GetSMCUserLevel(void);

/*
void  DIVI_SCStatusChange(UINT8 status)
�� �� �ߣ� CAģ�� 
ʹ �� �ߣ� �����С� 
��ʱʹ�ã���ϵͳ��⵽��ο��¼�ʱ���á� 

����������֪ͨCAģ���ο��¼������� 
��������� status,0x01��ʾ���ܿ������¼�������0x02��ʾ���ܿ��γ��¼�����
��������� �ޡ� 
�� �� ֵ�� ��
*/
void  DIVI_SCStatusChange(UINT8 status);

/*
UINT16 DIVI_GetEmailHeads(TDIVI_EmailHead* pEmailHead); 
�� �� �ߣ� CAģ��
ʹ �� �ߣ� ������
������������ȡ�ʼ������б���ͷ
��������� 
			pEmailHeads: ����ͷ����
			nTitleNum  : ����ͷ���е�Ԫ�ظ���
���������
                     pEmailHead: ������ ͷ����
�� �� ֵ�� 
                     �ʼ�����
*/
UINT16 DIVI_GetEmailHeads(TDIVI_EmailHead* pEmailHeads, UINT16 nTitleNum);


/*
UINT8 DIVI_GetNewMailCount();
�� �� �ߣ� CAģ��
ʹ �� �ߣ� ������
������������ȡδ���ʼ��ĸ���
�����������
�����������
�� �� ֵ: δ���ʼ��ĸ���                     
*/
INT32 DIVI_GetNewMailCount();

/*
BOOL DIVI_GetEmailHead(UINT8 bEmailID,TDIVI_EmailHead* pEmailHead); 
�� �� �ߣ� CAģ��
ʹ �� �ߣ� ������
������������ȡ�ض��ʼ��ı���ͷ
��������� 
			pEmailHead: ����ͷָ��
			bEmailID    : �ʼ����
���������
                     pEmailHead: ������ ͷ
�� �� ֵ�� 
                     TRUE: �ɹ�
                     FALSE: ʧ��
*/
BOOL DIVI_GetEmailHead(UINT16 bEmailID,TDIVI_EmailHead* pEmailHead); 
/*
BOOL DIVI_GetEmailContent(UINT8 bEmailID,TDIVI_EmailContent* pEmailContent);
�� �� �ߣ� CAģ��
ʹ �� �ߣ� ������
������������ȡ�ض��ʼ�������
��������� 
			pEmailContent: ���ݽṹָ��
			bEmailID    : �ʼ����
���������
                     pEmailContent: ������ݽṹ
�� �� ֵ�� 
                     TRUE: �ɹ�
                     FALSE: ʧ��
*/
BOOL DIVI_GetEmailContent(UINT16 bEmailID,TDIVI_EmailContent* pEmailContent) ;
/*
BOOL DIVI_DelEmail(UINT8 bEmailID);
�� �� �ߣ� CAģ��
ʹ �� �ߣ� ������
����������ɾ���ض��ʼ�
���������	bEmailID : �ʼ����
�����������
�� �� ֵ�� 
                     TRUE: �ɹ�
                     FALSE: ʧ��
*/
BOOL DIVI_DelEmail(UINT16 bEmailID); 

//INT32 DIVI_ClearPaired();
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------

/*
UINT8 DIVI_SC_Get_CardState(void);
�ṩ��:������
ʹ����:CAģ��
��ʱ����:CAģ����Ҫ��ÿ�״̬ʱ��
��������:������ܿ�״̬��
�������:��
�������:��
����ֵ:
0x00:δ�忨
0x01:�Ѳ忨
*/
extern UINT8 DIVI_SC_Get_CardState(void);


/*
INT32 DIVI_SC_DRV_Initialize(void); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
��ʱ���ã� CAģ���ʼ��ʱ���øú����� 
���������� ��ʼ�����ܿ������� 
��������� �ޡ� 
��������� �ޡ� 
�� �� ֵ�� 
SUCCESS���ɹ��� 
FAILURE�� ʧ�ܡ� 
*/
extern INT32 DIVI_SC_DRV_Initialize(void);

/*
INT32 DIVI_SC_ValidateATR( UINT8 *pbNewATR, UINT16 wLen,); 
�� �� �ߣ� CAģ�顣  
ʹ �� �ߣ� CAģ�顣 
��ʱ���ã� �����յ��µ�ATR��Ϣ��ʱ�� 
���������� ���ܿ��������øú��������ж��Ƿ���DIVI���ܿ�����ЧATR�� 
��������� 
	pbNewATR�� ATR���ݣ� 
	wLen�� ATR���ݳ��ȣ� 
��������� �ޡ� 
�� �� ֵ�� 
SUCCESS����CA�����ܿ��� 
FAILTURE�� ����CA�����ܿ��� 
*/
INT32  DIVI_SC_ValidateATR(UINT8 * pbNewATR,UINT16 wLen);

/*
INT32 DIVI_SC_DRV_ResetCard(UINT8 bCardNumber,UINT8 * ATRBuf,UINT8 BufferLen,
UINT8 * RLength); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ��λ���ܿ��� 
��������� 
        bCardNumber�� Ϊ���ܿ��Ŀ��ţ� 
	ATRBuf,���ص�ATR���ݴ�����ڴ��ַ��
	BufferLen,���ص�ATR���ݴ�����ڴ泤�ȣ�
	RLength,ʵ�ʷ��ص�ATR���ȡ�
��������� �ޡ� 
�� �� ֵ�� SUCCESS---��λ�ɹ�;FAILTURE---��λʧ��
*/
extern INT32  DIVI_SC_DRV_ResetCard(UINT8 bCardNumber,UINT8 * ATRBuf,UINT8 BufferLen,UINT8 * RLength);

/*
INT32 DIVI_SC_DRV_SendDataEx() 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
��ʱʹ�ã� �����ܿ���������ʱ�� 
���������� CAģ����ô˺��������ܿ��������ݡ� 
��������� 
		bCardNumber�� ��ʱ�������ã� 
		bLength�� pabMessage�ĳ��ȣ� 
		pabMessage�� �����������Ϣָ�룻 
		pabResponse�� ������Ӧ��������ݿ��ָ�룻 
		bRLength�� ��Ӧ��������ݿ鳤��ָ�롣 
��������� 
		pabResponse�� ��Ӧ��������ݿ飻 
		bRLength�� ��Ӧ��������ݿ鳤�ȡ� 
		pbSW1 ���ܿ�״̬�ֽ�1 
		pbSW2 ���ܿ�״̬�ֽ�2 
�� �� ֵ�� 
		TRUE���ɹ�
		FALSE��ʧ�� 
����˵���� 
*/
extern INT32 DIVI_SC_DRV_SendDataEx(UINT8 bCardNumber,UINT8 bLength,
	                                                      UINT8 * pabMessage,
	                                                      UINT8 * pabResponse,
	                                                      UINT8 RBufferLen,
	                                                      UINT8 * bRLength,
	                                                      UINT8 * pbSW1,
	                                                      UINT8 * pbSW2);

/*
INT32 DIVI_TableStart(UINT8 bRequestID,const UINT8 * pbFilterMatch,const UINT8 * pbFilterMask,
	                                            UINT8 bLen,
	                                            UINT16 wPid,
	                                            UINT8 bWaitTime);
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����������ȥ����CA�����ݡ� 
��������� 
		bRequestID,Ϊ���µļ���ֵ֮һ��
		                  CA_STB_FILTER_1,
		                  CA_STB_FILTER_2,
		                  CA_STB_FILTER_3,
		                  CA_STB_FILTER_4;
		pbFilterMatch,filter�����ݡ�
		pbFilterMask,filter��mask��
		bLen�� filter�ĳ��ȡ� 
		wPid�� ͨ����PIDֵ�� 
		bWaitTime�� ͨ������ʱ�䣬���Ӷ೤ʱ����ͨ���޷����յ��� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ��� 
		FAILURE�� ʧ�ܡ� 
����˵����
		filter match��filter mask�Ĺ�ϵ��ÿ��filter match��λ��Ӧ��һ��filter mask��λ�������Ӧ��filter mask��λΪ1�����ʾ��filter matchλ��Ҫƥ�䡣�磺filter match�е�һ���ֽ�Ϊ��0x5a�� filter mask ����Ӧ��һ���ֽ�Ϊ��0x55����������и�filter match�ֽ������õĽ���Ϊ��0��2��4��6λ������ƥ��0��0��1��1�����⣬filter match��filter mask������table�еĵ�1����2�ֽڣ���0��ʼ����������Ϊ�������ֽ���DVB�еĶ���Ƚ����⡣��ϸ��13818��Table�ĸ�ʽ�� ?? 

*/
extern INT32 DIVI_TableStart(UINT8 bRequestID,
                                                   const UINT8 * pbFilterMatch,
                                                   const UINT8 * pbFilterMask,
	                                            UINT8 bLen,
	                                            UINT16 wPid,
	                                            UINT8 bWaitTime);


/*
void DIVI_SetCW (); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ���ý���ͨ���Ŀ����֡� 
��������� 
		wEcmPid�� �����ֵ����ڵ�ecm����PIDֵ�� 
		szOddKey�� ����������ݵ�ָ�롣 
		szEvenKey�� ż���������ݵ�ָ�롣 
		bKeyLen�� �����ֳ��ȡ� 
		bReservedFlag, ������ 
��������� �ޡ� 
�� �� ֵ�� �ޡ� 

*/
extern void DIVI_SetCW(UINT16 wEcmPid,const UINT8 * szOddKey,const UINT8 * szEvenKey,
	                              UINT8 bKeyLen,UINT8 bReservedFlag);

/*
BOOL DIVI_GetBuffer (); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ��û������ṩ��CAģ�鱣����Ϣ����ʼ��ַ�Ϳռ��С 
��������� 
��������� 
		lStartAddr �����з���ռ����ʼ��ַ 
		lSize �����з���ռ�Ĵ�С 
�� �� ֵ�� 
		TRUE �ɹ� 
		FALSE ʧ�� 
*/
extern BOOL DIVI_GetBuffer(UINT32 * lStartAddr,UINT32 * lSize);

/*
�ṩ�ߣ�  ������
ʹ���ߣ�CAģ��
������������û������ṩ��CAģ�����ڱ�����������Ϣ����ʼ��ַ�Ϳռ��С���ռ��С����Ϊ16���ֽ�
���������
���������
          lStartAddr ���������Ϣ�洢����ʼ��ַ
          lSize ���������Ϣ�洢����С������ڵ���16���ֽ�
����ֵ��  TRUE �ɹ� FALSEʧ��
*/
extern BOOL DIVI_Get_STBToCardSuit_Buffer(UINT32 *lStartAddr,UINT32 * lSize);

/*
BOOL DIVI_ReadBuffer(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� �ӻ����з���ռ��ָ����ʼ��ַ��ָ�����ȵ����ݡ� 
��������� 
		lStartAddr�� Ҫ��ȡ���ݵĴ洢�ռ�ĵ�ַ�� 
		pbData�� ����ȡ���ݵĴ�ŵ�ַָ�롣 
		nLen�� Ҫ�������ݵĳ��� 
��������� 
		pbData�� �����������ݡ� 
�� �� ֵ�� 
		TRUE���ɹ�
		FALSE ʧ�ܡ�
*/
extern BOOL  DIVI_ReadBuffer(UINT32 lStartAddr,UINT8 *pData,INT32 nLen);

/*
BOOL DIVI_WriteBuffer(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ������еĴ洢�ռ�д��Ϣ 
��������� 
		lStartAddr�� Ҫд�Ĵ洢�ռ��Ŀ���ַ�� 
		pData�� Ҫд������ 
		nLen�� Ҫд�����ݵĳ��� 
��������� �ޡ� 
�� �� ֵ�� 
		TRUE���ɹ�
		FALSE�� ʧ�ܡ�
*/
extern BOOL DIVI_WriteBuffer(const UINT32 lStartAddr,const UINT8 *pData,INT32 nLen);


/*
extern INT32 DIVI_OSPSCTaskCreate(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����SC���� 
��������� 
		name[]�� 4���ֽڵ��������ơ� 
		stackSize�� ������ʹ�õĶ�ջ�Ĵ�С�� 
		entryPoint�� �������ڵ�ַ�� 
		priority�� ��������ȼ��� 
		arg1�� ���ݸ�����ĵ�һ�������� 
		arg2�� ���ݸ�����ĵڶ��������� taskId�� �����ID�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������
*/
extern INT32 DIVI_OSPSCTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void *),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);
/*
extern INT32 DIVI_OSPCaCoreTaskCreate(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����CaCore���� 
��������� 
		name[]�� 4���ֽڵ��������ơ� 
		stackSize�� ������ʹ�õĶ�ջ�Ĵ�С�� 
		entryPoint�� �������ڵ�ַ�� 
		priority�� ��������ȼ��� 
		arg1�� ���ݸ�����ĵ�һ�������� 
		arg2�� ���ݸ�����ĵڶ��������� taskId�� �����ID�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������
*/
extern INT32 DIVI_OSPCaCoreTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void *),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);

/*
extern INT32 DIVI_OSPOSDTaskCreate(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����OSD���� 
��������� 
		name[]�� 4���ֽڵ��������ơ� 
		stackSize�� ������ʹ�õĶ�ջ�Ĵ�С�� 
		entryPoint�� �������ڵ�ַ�� 
		priority�� ��������ȼ��� 
		arg1�� ���ݸ�����ĵ�һ�������� 
		arg2�� ���ݸ�����ĵڶ��������� taskId�� �����ID�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������
*/
extern INT32 DIVI_OSPOSDTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void *),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);
/*
extern INT32 DIVI_OSPUserTaskCreate(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����User���� 
��������� 
		name[]�� 4���ֽڵ��������ơ� 
		stackSize�� ������ʹ�õĶ�ջ�Ĵ�С�� 
		entryPoint�� �������ڵ�ַ�� 
		priority�� ��������ȼ��� 
		arg1�� ���ݸ�����ĵ�һ�������� 
		arg2�� ���ݸ�����ĵڶ��������� taskId�� �����ID�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������
*/
extern INT32 DIVI_OSPUserTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void *),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);

/*
extern INT32 DIVI_OSPTaskTemporarySleep(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ��ͣ����һ��ָ����ʱ�䡣 
��������� 
		milliSecsToWait�� ��ֹ�����ʱ�䡣���Ժ���Ϊ��λ���� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� �������� 
*/
extern INT32 DIVI_OSPTaskTemporarySleep(UINT32 milliSecsToWait);

/*
extern INT32 DIVI_OSPSemaphoreCreate(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� 
��������� 
		name[]�� 4���ֽڵ��ź������ơ� 
		initialTokenCount�� ��ʼ��ʱ�ź����������� 
		taskWaitMode�� ȡ�ø��ź����ķ�ʽ��Ϊ��������֮һ�� 
		DIVI_WAIT_FIFO�� ���ȵȴ����ȵõķ�ʽȡ�á� 
		DIVI_WAIT_PRIO�� �����ȼ������ȵõķ�ʽȡ�á� 
		semaphoreId�� ���ź�����ID�� 
��������� �� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� �������� 
����˵���� 
		ĿǰCAģ��ֻ�õ���DIVI_WAIT_FIFOģʽ������޷�ʵ��DIVI_WAIT_PRIO�������ʱ��ʵ�֡�
*/
extern INT32 DIVI_OSPSemaphoreCreate(char name[4],UINT32 initialTokenCount,UINT32 taskWaitMode,UINT32 * semaphoreId);

/*
extern INT32 DIVI_OSPSemaphoreGetToken(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ȡ���ź��� 
��������� 
		semaphoreId�� ��Ҫ�ȴ����ź�����ID�� 
		waitMode�� �ȴ��ź����ķ�ʽ��Ϊ��������֮һ�� 
		DIVI_TASK_WAIT���ȴ��ź���ֱ����ʱ�� 
		DIVI_TASK_NOWAIT�������ź�����������������ء�
		milliSecsToWait�� �ȴ���ʱ�䡣 ���waitModeΪ
		DIVI_TASK_NOWAIT����ò�����Ч�� 
		DIVI_TASK_WAIT�������ʱ�䡣 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ʧ�ܡ� 
����˵���� 
		ע����������ĵȴ�ģʽ�ʹ����ź����ĵȴ�ģʽ֮�������
		�����ź�������ָͬʱ�ж�������ڵȴ�ͬһ���ź���ʱ˭����ȡ���ź�����
		��OSPSemaphoreGetToken���ź����ĵȴ���ָ������ȴ��ź���,�ǵȴ�һ��ʱ��ֱ��ȡ���ź���
		Ϊֹ���ǲ����κ�����ĵȴ��������Ƿ����ȡ���ź������������ء�

*/
extern INT32 DIVI_OSPSemaphoreGetToken(UINT32 semaphoreId,UINT32 waitMode,UINT32 milliSecsToWait);

/*
extern void * DIVI_OSPMalloc(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� �����ڴ档 
��������� 
		numberOfBytes�� ������ڴ�ռ�Ĵ�С�������ֽ�Ϊ��λ���� 
��������� �ޡ� 
�� �� ֵ�� 
		NULL��ʧ��
		>0: �����ڴ��ַָ�롣
����˵����
*/
extern void * DIVI_OSPMalloc(UINT32 numberOfBytes);

/*
extern INT32 DIVI_OSPFree(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� �ͷŵ������˵��ڴ�ռ䡣 
��������� 
		ptrToSegment�� Ҫ�ͷŵ��ڴ�ռ�ָ�롣 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS�� �ɹ��ͷš� 
		FAILURE�� �ͷŲ��ɹ�����������Ϊ�����ݽ�����ָ����Ч�� 
*/
extern INT32 DIVI_OSPFree(void * ptrToSegment);

/*
extern INT32 DIVI_OSPSemaphoreReturnToken(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����һ���ź����� 
��������� 
		semaphoreId�� �ź�����ID�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� �������� 
*/
extern INT32 DIVI_OSPSemaphoreReturnToken(UINT32 semaphoreId);

/*
extern INT32 DIVI_OSPQueueCreate(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����һ����Ϣ���С� 
��������� 
		name[]�� 4���ֽڵĶ������ơ� 
		maxQueueLength����Ϣ�����п��Դ�ŵ���Ϣ������������Ϣ�����и������ﵽ ������ʱ����������Ϣ���з���Ϣ����ʧ�ܡ� 
		taskWaitMode�� ȡ����Ϣ��������Ϣ�ķ�ʽ��Ϊ��������֮һ��
		DIVI_WAIT_FIFO�� ���ȵȴ����ȵõķ�ʽȡ�á� 
		DIVI_WAIT_PRIO�� �����ȼ������ȵõķ�ʽȡ�á�
		queueId�� ��Ϣ���е�ID�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������
*/
extern INT32 DIVI_OSPQueueCreate(char name[],UINT32 maxQueueLength,UINT32 taskWaitMode,UINT32 * queueId);

/*
extern INT32 DIVI_OSPQueueGetMessage(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� 
��������� 
		queueId�� ��Ҫȡ�õ���Ϣ��ID�� 
		messsage�� Ϊ��Ϣ�ĸ�ʽ������DIVI_QUEUE_MESSAGE�� 
		waitMode�� �ȴ��ź����ķ�ʽ��Ϊ��������֮һ�� 
				DIVI_TASK_WAIT���ȴ���Ϣֱ����ʱ�� 
				DIVI_TASK_NOWAIT��������Ϣ������������� �ء� 
		milliSecsToWait�� �ȴ���ʱ�䡣���waitModeΪ 
				DIVI_TASK_NOWAIT����ò�����Ч
				DIVI_TASK_WAIT�������ʱ�䡣 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������
*/
extern INT32 DIVI_OSPQueueGetMessage(UINT32 queueId,DIVI_QUEUE_MESSAGE * message,UINT32 waitMode,UINT32 milliSecsToWait);

/*
extern INT32 DIVI_OSPQueueSendMessage(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ������Ϣ����Ϣ���С� 
��������� 
		queueId�� ��Ϣ����ID�� 
		message�� Ҫ���͵���Ϣ�����ʽ��DIVI_QUEUE_MESSAGE�ṹ�� 
��������� �ޡ� 
�� �� ֵ�� 
		SUCCESS���ɹ���
		FAILURE�� ��������
*/
extern INT32 DIVI_OSPQueueSendMessage(UINT32 queueId,DIVI_QUEUE_MESSAGE * message);

/*
extern UINT32 DBGPrint (); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����ʱ���ڴ�ӡ��Ϣ�� 
��������� fmt, ��ʽ���ƣ���c�����е�printf()�����﷨��ͬ��
��������� �ޡ� 
�� �� ֵ�� ���ش�ӡ���ַ���Ŀ�� 
����˵���� 
*/
extern UINT32 DBGPrint(const char * fmt,...);

/*
extern UINT32 DIVI_GetRandomNumber(); 
�� �� �ߣ� �����С� 
ʹ �� �ߣ� CAģ�顣 
���������� ����һ��������ָ�����޵�������� 
��������� �� 
��������� �ޡ� 
�� �� ֵ�� ����� 
*/
extern UINT32 DIVI_GetRandomNumber();

/*
UINT8 GxShowPPDatas(UINT8 Value)
�� �� �ߣ� ������ 
ʹ �� �ߣ� CAģ�� 
�����������رջ�����Ƶ�� 
��������� Value,0x00�ر���Ƶ�㣬0x01������Ƶ��
��������� �ޡ� 
�� �� ֵ�� ������
*/
extern UINT8 GxShowPPDatas(UINT8 Value);

/*
UINT16 DIVI_GetCurr_ServiceID(void)
�� �� �ߣ� ������ 
ʹ �� �ߣ� CAģ�� 
������������õ�ǰ��Ŀ��ServiceID 
��������� ��
��������� �ޡ� 
�� �� ֵ�� ��ǰ��Ŀ��ServiceID*/
extern UINT16 DIVI_GetCurr_ServiceID(void);

/*
UINT16 DIVI_GetCurr_EmmID(void)
�� �� �ߣ� ������ 
ʹ �� �ߣ� CAģ�� 
������������õ�ǰ��Emm Pid 
��������� ��
��������� �ޡ� 
�� �� ֵ�� ��ǰ��Emm Pid*/
extern UINT16 DIVI_GetCurr_EmmID(void);

/*
UINT16 DIVI_GetCurr_EcmID(void)
�� �� �ߣ� ������ 
ʹ �� �ߣ� CAģ�� 
������������õ�ǰ��Ecm Pid 
��������� ��
��������� �ޡ� 
�� �� ֵ�� ��ǰ��Ecm Pid*/
extern UINT16 DIVI_GetCurr_EcmID(void);

/*
UINT16 DIVI_GetCurr_NetRegionID(void)
�� �� �ߣ� ������ 
ʹ �� �ߣ� CAģ�� 
���������� ��õ�ǰ��NetRegionID 
��������� ��
��������� �ޡ� 
�� �� ֵ�� 
           0xff: �������������
           ��������ǰ��NetRegionID
*/
extern UINT16 DIVI_GetCurr_NetRegionID(void);//danlly, 2009-11-26

/*
INT32 DIVI_Modify_Pin(UINT8 * OldPin,UINT8 * NewPin,UINT8 PinLength,UINT8 * Repeattime)
�ṩ��:CAģ��
ʹ����:������
��������:����Pin�� 
�������:  
        OldPin,��Pin��
                       NewPin,��Pin��
                       PinLength,Pin�볤��
�������:��
����ֵ:
		SUCCESS---�����ɹ�
                  FAILTURE---����ʧ��
����˵��:  Repeattime,PIN���������ʣ�����(0-7);��Ϊ17�����ʾ��PIN���ܱ�����
*/
INT32 DIVI_Modify_Pin(UINT8 * OldPin,UINT8 * NewPin,UINT8 PinLength,UINT8 * Repeattime);

/*
�ṩ�ߣ�  CAģ�顣
ʹ���ߣ������С�
��ʱ���ã���������Ҫ��ȡ��Ȩ��Ϣʱ��
������������ȡ��Ȩ��Ϣ��
�����������
�����������Ȩ��Ϣ������ݶ�Ϊ6����Ʒ��
����ֵ��
        0---��ѯʧ�ܣ�
        1---����Ȩ��Ϣ��
        2---��ѯ�ɹ�
����˵������
*/
INT32 DIVI_GetSMCEntitleInfo(PackageInfo * Package);

/*
�ṩ�ߣ�  CAģ�顣
ʹ���ߣ������С�
��ʱ���ã���������Ҫ��ȡ�ӿ������ĸ����Ϣʱ��
������������ȡ�ӿ���Ϣ��
�����������
����������ӿ���Ϣ
����ֵ��
		0-----�ɹ�;
		-1----δ�忨;
		-2----�����ӿ� 
����˵������
*/
INT32 DIVI_GetSMCChildInfo(ChildSMCInfo * ChildInfo);


/*
INT32 DIVI_GetSMCWorkTime(UINT8 * Enable,UINT8 * BeginTime,UINT8 * EndTime)
�� �� �ߣ� CAģ�� 
ʹ �� �ߣ� ������ 
������������ù���ʱ����Ϣ 
��������� ��
��������� 
		Enable,1�ֽڣ�0x00---����ʱ��δ���ã�0x01---����ʱ�������ã�
                            BeginTime, 3�ֽڣ���ʼʱ��,HMS��ʽ
                            EndTime,    3�ֽڣ�����ʱ��,HMS��ʽ 
�� �� ֵ��
		DIVI_ERR---��ѯʧ��;
                        DIVI_OK---��ѯ�ɹ�
*/ 
INT32 DIVI_GetSMCWorkTime(UINT8 * Enable,UINT8 * BeginTime,UINT8 * EndTime);

/*
INT32 DIVI_GetRegion(UINT8 * Region)
�� �� �ߣ� CAģ�� 
ʹ �� �ߣ� ������ 
�������������������Ϣ 
��������� ��
��������� Region---- ��4���ֽڣ���ʾ��ʽ������001.002.003.004
�� �� ֵ��
		DIVI_ERR---��ѯʧ��;
                        DIVI_OK---��ѯ�ɹ�
*/ 
INT32 DIVI_GetRegion(UINT8 * Region);

/*
INT32 DIVI_SetCardLevel(UINT8 * Pin, UINT8 Level,UINT8 * Repeattime)
�� �� �ߣ� CAģ�� 
ʹ �� �ߣ� ������ 
�������������üҳ����Ƽ��� 
��������� 
        Pin,6λPin��
                             Level,�ҳ����Ƽ���
��������� �� 
�� �� ֵ�� 
		SUCCESS---�ɹ�;
		FAILTURE --- ʧ��
����˵��:  Repeattime,PIN���������ʣ�����(0-7);��Ϊ17�����ʾ��PIN���ܱ�����
*/
INT32 DIVI_SetCardLevel(UINT8 * Pin, UINT8 Level,UINT8 * Repeattime);

/*
INT32 DIVI_SetWorkPeriod(UINT8 * Pin, UINT8 * BeginTime,UINT8 * EndTime,UINT8 Enable,UINT8 * Repeattime)�� �� �ߣ� CAģ�� 
ʹ �� �ߣ� ������ 
�������������ù���ʱ��
��������� 
        Pin,6λPin��
                             BeginTime,��ʼʱ�䣬3�ֽڣ�4bitBCD��ʽ
                             EndTime,����ʱ�䣬3�ֽڣ�4bitBCD��ʽ
                             Enable,0x01---ʹ�ܹ���ʱ�Σ�0x00---�رչ���ʱ��
��������� �� 
�� �� ֵ�� 
        DIVI_OK---�ɹ�;
        DIVI_ERR --- ʧ��
����˵��:  Repeattime,PIN���������ʣ�����(0-7);��Ϊ17�����ʾ��PIN���ܱ�����
*/
INT32 DIVI_SetWorkPeriod(UINT8 * Pin, UINT8 * BeginTime,UINT8 * EndTime,UINT8 Enable,UINT8 * Repeattime);

/*
UINT32 DIVI_GetPaired()
�� �� �ߣ� CAģ�� 
ʹ �� �ߣ� ������ 
������������û������״̬ 
��������� ��
��������� �� 
�� �� ֵ�� 
        0x01---�������;
        0x02---δ�������;
        0x00---����ʧ��
*/
UINT32 DIVI_GetPaired(void);

/*�ṩ�ߣ�������
ʹ���ߣ�CAģ��
��ʱ���ã�CAģ����Ҫ��ʾ��Ϣʱ��
������������ʾOSD��Ϣ
���������������˵��
�����������
����ֵ����
����˵��:����ֲ�ĵ�
*/
extern void  DIVI_EventHandle(UINT32 event,UINT32 param1,UINT32 param2,UINT32 param3);

/*�ṩ�ߣ�CAģ��
ʹ���ߣ�������
��ʱ���ã���ʼ������ĸ������ʱ
������������ʼ������ĸ������
��������� ��
��������� �� 
����ֵ��  0x01--�ɹ�
����˵��:��
*/
INT32 DIVI_Active_ChildCard();
/*�ṩ�ߣ�CAģ��
ʹ���ߣ�������
��ʱ���ã�ֹͣ������ĸ������ʱ
����������ֹͣ������ĸ������
�����������
�����������
����ֵ��  0x01--�ɹ�
����˵��:��
*/
INT32 DIVI_Finish_ChildCard();

//��ȡ��ǰ���ں�ʱ��, 2013-02-26
//�����ڻ������޷���ȡTDTʱ��ʾ��ǰʱ��
//�����ʽ: YYYY-MM-DD HH:MM
//CurrentTime[0]-CurrentTime[4]�ֱ�Ϊ������ʱ��
//���������Ҫ��2000
INT32 Divi_GetCurrentTime(UINT8 *CurrentTime);

//2015-06-17, �����ڿ���������Ŀ��OTT�������ض�ҵ����������Ȩ�� 
//����ֵΪ1: ��ʾDVB������Ʒ������Ȩ������ֵΪ0: ��ʾDVB������Ʒ������Ȩ 
BOOL DIVI_GetProductOneIs();

//����������������Ϊ��
extern BOOL DIVI_Show_Finger(UINT8 Position,UINT8 Duriaon,UINT8 * Finger_Content,UINT8 Content_Length);

extern INT8 DIVI_GetFingerNum();


#ifdef __cplusplus
}
#endif

#endif

