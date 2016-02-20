//���ļ�����Ҫ�������ʵ�ֲ��ṩ��CAģ��ʹ�õĽӿڡ�
// 2011 0801 LHCAS VER0.2

#ifndef STB_APP_CAS
#define STB_APP_CAS

#include "stb_Public.h"

#ifdef  __cplusplus
extern "C" {
#endif 
/* c lib used */
//memset(buffer,0,sizeof(buffer));
// memcpy(dest,source,size);
// memcmp(dest,source,length)
/*----------------------------------���½ӿ���STB�ṩ��CAS����--------------------------------------------*/
/*++
���ܣ���������д���num����Ϣջ��ÿ����Ϣջ�����20����Ϣ�Ŷӡ�һ�����ȳ�ԭ��
������
	BU8  MSGQUE1;
	STBCA_MSGQUENE_Init(2);
����:
    1:  ִ�гɹ���
    0:  ʧ��
--*/
BU8 STBCA_MSGQUENE_Init(BU8 num);
/*++
���ܣ��ӵ�NO����Ϣ�����л�ȡͷ����Ϣ���������͡�
�������������Ϊ�գ���  ����msg Ϊ��
����:    �ɹ���ȡ����Ϣ�����е�ǰ��ϢΪ������Ϣ���ݡ�
	CA_MSG *msg1= NULL;
	STBCA_GETCAMSG(0,msg1);
	if(msg1 != NULL )
	{
		//�ӵ�һ����Ϣ�����л�ȡ��  ������Ϣ
	}
	CA_MSG *msg2= NULL;
	STBCA_GETCAMSG(1,msg2);
	if(msg2 != NULL )
	{
		//�ӵڶ�����Ϣ�����л�ȡ��  ������Ϣ
	}
--*/
//void STBCA_GETCAMSG(BU8 NO,CA_MSG* msg);
CA_MSG* STBCA_GETCAMSG(BU8 NO);
/*++
���ܣ��ڵ�NO����Ϣ����������һ����Ϣ�������������ɾ�����е���Ϣ��������Ϣ��
������
	static CA_MSG msg1;
	msg1.msgtype = 1;
	msg1.msgparm1 = 0x33;
	msg1.msgparm2 = 0x44;
	
	STBCA_SENDCAMSG(0,&msg1);
	STBCA_SENDCAMSG(1,&msg1);
--*/
void STBCA_SENDCAMSG(BU8 NO,CA_MSG* msg);
//����ӿ���ʱ���á�
void STBCA_ReleaseCAMSG(BU8 NO,CA_MSG* msg);

/*++
���ܣ�����ǰ�̡߳�
������
	dwMicroseconds:			���롣
--*/
void STBCA_Sleep( int dwMicroseconds );

/*++
���ܣ��õ�һ�����8bit���ݣ�0--255֮�䡣
������
	dwMicroseconds:			���롣
--*/
BU8 STBCA_RAND_GET( void );

/*++
���ܣ���ʼ���ź�����
������
	pSemaphore:				ָ���ź�����ָ�롣
	flag Ϊ1��ʾ��ʼ����ź��и��źŵȴ���0���ʾû��
����:
 1: �ɹ�ִ��
 0: ��ʾִ�г���
CA_SemaphoreInit
--CA_SemaphoreInit(&gBycaParseDatasem,1);*/
BU8 STBCA_SemaphoreInit( CA_Semaphore *pSemaphore ,BU8 flag);
/*
demo:
	static CA_Semaphore ParseDatasem;
	STBCA_SemaphoreInit(&ParseDatasem,1)
	
	STBCA_SemaphoreWait(&ParseDatasem);
	......
	STBCA_SemaphoreSignal(&ParseDatasem);

*/

/*++
���ܣ����ź������źš�
������
	pSemaphore: 			ָ���ź�����ָ�롣
����:
   	
--*/
void STBCA_SemaphoreSignal( CA_Semaphore *pSemaphore );

/*++
���ܣ��ȴ��ź���,�ȴ��ɹ�֮��,�ź���Ϊ���źš�
������
	pSemaphore: 			ָ���ź�����ָ�롣
����:
   ͬ��		
--*/
void STBCA_SemaphoreWait( CA_Semaphore *pSemaphore );

/*++
���ܣ�CASģ���������ע���̡߳�
������
	szName:					ע����߳����ơ�
	pTaskFun:				Ҫע����̺߳�����
����:
   1: �ɹ�ִ��
 0: ��ʾִ�г���
--*/
BU8	STBCA_RegisterTask(const char* szName, pThreadFunc pTaskFun);

/*++
���ܣ���û����з����CAS�Ĵ洢�ռ������ַ�ʹ�С�����ֽ�Ϊ��λ����
������
	ppStartAddr:			�����з����CAS�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	lSize:					��������з����CAS�Ĵ���ռ�Ĵ�С��
--*/
void STBCA_GetDataBufferAddr(BU32* lSize, BU32* ppStartAddr);

/*++
���ܣ���ȡ�����ڻ������е���Ϣ��
������
	pStartAddr:				Ҫ��ȡ�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	plDataLen:				����ΪҪ��ȡ�������ֵ�����Ϊʵ�ʶ�ȡ�Ĵ�С��
	pData:					���������ݡ�
����:
   1: �ɹ�ִ��
   0: ��ʾִ�г���
--*/
BU8 STBCA_ReadDataBuffer(BU32* pStartAddr, BU32* plDataLen, BYTE* pData);

/*++
���ܣ�������еĴ洢�ռ�д��Ϣ��
������
	lStartAddr:				Ҫд�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	plDataLen:				����ΪҪд�����ݵĳ��ȣ����Ϊд���ʵ�ʳ��ȡ�
	pData:					Ҫд�����ݡ�
����:
   1: �ɹ�ִ��
   0: ��ʾִ�г���
--*/
BU8 STBCA_WriteDataBuffer(BU32 pStartAddr, BU32 plDataLen, const BYTE* pData);

/*++
���ܣ����ù���������CAS˽�����ݡ�
������
	ReqID:				�����ձ��table id��������Ӧ�ñ�������ݣ����յ�CAS˽�����ݺ����STBCA_StreamGuardDataGot()����ecm��tableid������0x82��0x83
	wPID:				��Ҫ���˵�����PID��
	szFilter:				��������ֵ��Ϊһ��8���ֽڵ����顣
	szMask:					�����������룬Ϊһ��8���ֽڵ����顣
	u8Len:					�������ĳ��ȣ�Ϊ8��
	nWaitSeconds:			�մ˱���ĵȴ�ʱ�䣬���Ϊ0�����޵ȴ�����λ�롣
����:
   ͬ��		
	
--*/
BU8 STBCA_SetStreamGuardFilter(BYTE ReqID, WORD wPID, const BYTE* szFilter, const BYTE* szMask,BYTE u8Len, int nWaitSeconds);


/*++
���ܣ���ӡ������Ϣ��
������
	pszMsg:					������Ϣ���ݡ�
--*/
void STBCA_AddDebugMsg(const char *pszMsg);

/*++
	����: ֪ͨ�˵�����ʾ�ʼ�ͼ��
	 ����
	 0,��ʾ��ʾ���ʼ�ͼ��
	 1����ʾ��ʾ��������
	 2, ��ʾû�����ʼ�ͼ��
*/
void STBCA_EmailNotify(BU8 type);
/*++
���ܣ��������ܿ���ǰ״̬��
����ֵ��0��ʾû�����룻1��ʾ�п�	
--*/
BU8 STBCA_GetSCStatus(void );

/*++
���ܣ�ca�����븴λ���ܿ���
--*/
void STBCA_SMARTCARD_ReSet(void);
/*++
���ܣ�ʵ�������ܿ�֮���ͨѶ������7816��׼��
������
	num:			ͨѶ��ţ���ʵ�����塣
	bSendData:			�����͵�һ�������ֽڡ�
	bReplyLen:			����ʱ����ָ�����ݳ��ȣ�������ʱ��ȡ�����ݳ��ȡ�
	bReplyData:         		���ص����ݴ�.
����ֵ   :
	0   : ����ͨѶʧ��
	1   : �ɹ�
--*/
BU8 STBCA_SCAPDU(BU8 num, BYTE* bSendData,BU32* bReplyLen, BYTE* bReplyData);



/*
���ܣ���ʾ�û����ܹۿ���Ŀ����ʾ��Ϣ����������������Ӧʹ�á�
������	
	b8MesageNo:				OSD Ҫ��ʾ����Ϣ����
--*/
void   STBCA_ShowOSDPromptMsg(BYTE b8MesageNo);


/*--------------------------- ����2�Ľ��棺�û�����ȡ����ʾ�Ľ��� -----------------------*/
/*
���ܣ���ʾOSD��Ϣ��������ͬ����ʾ��Ҳ�������첽��ʾ��ע�⵽����ʾҪ������ʾ
����:	
	szOSD:					������ʾ��OSD�ַ���
	wDuration:			Ϊ0ʱΪ������ʾ��
typedef enum {
	MSG_DATA_REMINDED=0,
		// �����ϵİ汾��0Ϊ������ʾ����
	MSG_DISPLAY_ICON=1,	
		//�����ʼ���ͼ����ʾ
	MSG_POP_TEXT=2,	
		//������ʾ����Ҫ�ֶ����
	MSG_DISPLAY_MAILBOX =4,
		//��������˵�
	MSG_SCROLL_TOP = 7,
		//����Ļ���������ַ�
	MSG_SCROLL_BOTTOM=8,
		//����Ļ�ײ������ַ�
	MSG_POP_TOP=14,
		//����Ļ������ʾ�ַ�
	MSG_POP_BOTTOM=15
	   //����Ļ�ײ���ʾ�ַ�
}CAMSG_Display_Type;
˵����
	�û�����ͨ��ң����ȡ����ǰOSD��ʾ��
	gx��������Ϣ:
		���������ʾ����ʾ������osd��Ϣ�����Ը��ǡ�
		����û��˳�������Ϣ��������ʾ����OSD��Ϣ��
		���������Ϣ����ʾ���յ�������ʾ��Ϣ��Ҫ��ʱ��ʾ������ʾ���ݡ�
--*/
void   STBCA_ShowOSDTXTMsg(const char* szOSD, CAMSG_Display_Type wDuration);

/*++
���ܣ���ʾָ����Ϣ����ͼ�귽ʽ��ʾָ��,��ʾλ��һ����ʼ�����ͼ�겻�ص�
������szOSD == NULL ʱ�����Ļ����ʾ����Ϣ��
--*/
void STBCA_FingerSHOW(const char* szOSD);
/*++
����:     ���õ�ǰ��Ŀ�Ľ�����Կ
��������ĿserverID
--*/

void STBCA_ScrSetCW( BU16  serverID, const BU8* pb8OddKey, const BU8* pb8EvenKey,BU8 KeyLen);

/*
���ܣ����ض���Ƶ��Ѱ��������Ϣ��
����:	�����������Ƶ��KHZ;
����:  1����ʾƵ��������������δ������Ƶ�㡣
ע��:  �ź������󣬴�ʱca���������BAT���ݡ�
--*/
BU8 STBCA_SetZoneFreq( BU32 Freq );

#ifdef  __cplusplus
}
#endif
#endif  
/***********************


******************************/
