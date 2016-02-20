#ifndef BYCASTB_CA_H_2013_02_20
#define BYCASTB_CA_H_2013_02_20

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "BYCApubtype.h"

/*------------------------���½ӿ���STB�ṩ��BYCA����-----------------------------*/

/*++
���ܣ��������ܿ���ǰ״̬��
������pbyStatus��	�������ܿ�״̬��ΪBYCAS_SC_OUT��BYCAS_SC_IN����֮һ��
--*/
BVOID BYSTBCA_GetSCStatus(BU8 * pbyStatus);

/*++
���ܣ���û����з����CAS�Ĵ洢�ռ������ַ�ʹ�С�����ֽ�Ϊ��λ����
������
	ppStartAddr:			�����з����CAS�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	lSize:					��������з����CAS�Ĵ���ռ�Ĵ�С��
--*/
BU8 BYSTBCA_GetDataBufferAddr(BU32* lSize,BU32 * ppStartAddr);

/*++
���ܣ���ȡ�����ڻ������е���Ϣ��
������
	pStartAddr:				Ҫ��ȡ�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	plDataLen:				����ΪҪ��ȡ�������ֵ�����Ϊʵ�ʶ�ȡ�Ĵ�С��
	pData:					���������ݡ�
--*/
BU8 BYSTBCA_ReadDataBuffer(BU32 pStartAddr,BU32* plDataLen,BU8* pData);

/*++
���ܣ�������еĴ洢�ռ�д��Ϣ��
������
	lStartAddr:				Ҫд�Ĵ洢�ռ�Ŀ�ʼ��ַ��
	plDataLen:				����ΪҪд�����ݵĳ��ȣ����Ϊд���ʵ�ʳ��ȡ�
	pData:					Ҫд�����ݡ�
--*/
BU8 BYSTBCA_WriteDataBuffer(BU32 pStartAddr,BU32 DataLen,BU8* pData);

/*++
���ܣ�����ǰ�̡߳�
������
	dwmsec:			���롣
--*/
BU8 BYSTBCA_Sleep(BU32 dwmsec);

/*++
���ܣ���ʼ���ź���,��ʼ�����ź���û���źš�
������pSemaphore:				ָ���ź�����ָ�롣
	    flag:						  ��ʼ���ź���ֵ
--*/
BU8 BYSTBCA_SemaphoreInit(BY_Semaphore *pSemaphore,BU8 flag);

/*++
���ܣ��ͷ�һ���ź�����
������pSemaphore: 			ָ���ź�����ָ�롣
--*/
BU8 BYSTBCA_SemaphoreDestory(BY_Semaphore *pSemaphore );

/*++
���ܣ����ź������źš�
������pSemaphore: 			ָ���ź�����ָ�롣
--*/
BU8 BYSTBCA_SemaphoreSignal(BY_Semaphore *pSemaphore );

/*++
���ܣ��ȴ��ź���,�ȴ��ɹ�֮��,�ź���Ϊ���źš�
������pSemaphore: 			ָ���ź�����ָ�롣
--*/
BU8 BYSTBCA_SemaphoreWait(BY_Semaphore *pSemaphore );

/*++
���ܣ�CASģ���������ע���̡߳�
������
	szName:					ע����߳����ơ�
	pTaskFun:				Ҫע����̺߳�����
--*/
BU8 BYSTBCA_RegisterTask(const BS8* szName,pThreadFunc pTaskFun);

/*++
���ܣ����ù���������CAS˽�����ݡ�
������
	byReqID:				�����ձ������š�������Ӧ�ñ�������ݣ����յ�CAS˽�����ݺ����BYCASTB_StreamGuardDataGot()ʱӦ�ý������ݷ��ء�
	wEcmPID:				��Ҫ���˵�����PID��
	szFilter:				��������ֵ��Ϊһ��8���ֽڵ����飬�൱��8*8�Ĺ�������
	szMask:					�����������룬Ϊһ��8���ֽڵ����飬��8*8�Ĺ�������Ӧ����szMask��ĳһbitλΪ0ʱ��ʾҪ���˵ı��Ӧ��bitλ����Ҫ��szFilter���Ӧ��bitλ��ͬ����szMaskĳһλΪ1ʱ��ʾҪ���˵ı��Ӧ��bitλһ��Ҫ��szFilter���Ӧ��bitλ��ͬ��
	byLen:					�������ĳ��ȣ�Ϊ8��
	nWaitSecs:			�մ˱���ĵȴ�ʱ�䣬���Ϊ0�����޵ȴ�����λ�롣
--*/
BU8 BYSTBCA_SetStreamGuardFilter(BU8 byReqID,BU16 wPID,BU8* szFilter,BU8* szMask,BU8 byLen,BU16 nWaitSecs);

/*++
���ܣ�CA�����ô˺������ý�����������ǰ���ڼ���һ���ڵ�CW�͸���������
������
	wEcmPID:				CW������ECMPID��
	szOddKey:				��CW�����ݡ�
	szEvenKey:				żCW�����ݡ�
	byKeyLen:				CW�ĳ��ȡ�
	bTaingControl:			true������¼��,false��������¼��
--*/
BU8 BYSTBCA_SetDescrCW(BU16 wEcmPid,BU8 byKeyLen,const BU8* szOddKey,const BU8* szEvenKey,BU8 bTaingControl);

/*++
���ܣ���ӡ������Ϣ��
������Param1,Param2,pszMsg:					������Ϣ���ݡ�
--*/
void BYSTBCA_AddDebugMsg(BU32 Param1,BU32 Param2,const BS8 *pszMsg);
/*++
���ܣ����û��忨ʱ,�ȵ��øú�����λ��
����:     true  �ɹ���falseʧ��
������pATR ���64 �ֽ�
--*/
BU8 BYCASTB_SCReset(BU8 *pATR);

/*++
���ܣ�ʵ�������ܿ�֮���ͨѶ������7816��׼��
������
	byReaderNo:				��Ӧ�Ķ�������š�
	pbyLen:					����Ϊ�����������ֽڵĳ���;���Ϊ�����ֽڳ��ȡ�
	byszCommand:			�����͵�һ�������ֽڡ�
	szReply:				���ص����ݣ��ռ䳤�Ⱥ�Ϊ256�ֽڡ�
--*/
BU8 BYSTBCA_SCAPDU(BU8 byReaderNo,const BU8* byszCommand,BU16* pbyLen,BU8* byszReply);

/*------------ ����1�Ľ��棺�������û�ͨ��ң����ȡ����ʾ�Ľ��� -----------------------*/
/*
���ܣ���ʾ�û����ܹۿ���Ŀ����ʾ��Ϣ����������������Ӧʹ�á�
������	
	byMesageNo:				Ҫ��ʾ����Ϣ��š�
--*/
BVOID   BYSTBCA_ShowPromptMessage(BU8 byMesageNo);
BVOID   BYSTBCA_HidePromptMessage(BVOID);

/*
���ܣ���ʾָ����Ϣ��
������
	wEcmPid					ECM PID
	dwCardID:				���š�
	wDuration:				����������
--*/
BVOID	BYSTBCA_ShowFingerPrinting(BU16 wEcmPid,BU32 dwCardID,BU32 wDuration);

/*--------------------------- ����2�Ľ��棺�û�����ȡ����ʾ�Ľ��� -----------------------*/
/*
���ܣ��������Ļ�ķ�ʽ��ʾOSD��Ϣ��������ͬ����ʾ��Ҳ�������첽��ʾ��
����:	
	szOSD:					������ʾ��OSD��Ϣ��
	wDuration:				0:��ʾһ�κ���������;����ֵΪ��ʾ�ľ������������
˵����
	�û�����ͨ��ң����ȡ����ǰOSD��ʾ��
--*/
BVOID	BYSTBCA_ShowOSDMsg(const BS8* szTitle,const BS8* szOSD,BU16 wDuration);
/*--------------------------- ����3�Ľ��棺�û�����ȡ����ʾ�Ľ��� -----------------------*/
/*
���ܣ�������ʾOSD��Ϣ��������ͬ����ʾ��Ҳ�������첽��ʾ��
����:	
	szOSD:					������ʾ��OSD��Ϣ��
˵����
	�û�����ͨ��ң����ȡ����ǰOSD��ʾ��
--*/
BVOID	BYSTBCA_ShowOSDFrameMsg(const BS8* szTitle,const BS8* szOSD);
/*++
���ܣ���ʾ���ʼ��ı�־��
������
	byShow:					��־���͡��������£�
							BYCAS_NEW_EAMIL						���ʼ�֪ͨ��
							BYCAS_NEW_EMAIL_NO_ROOM		�����ʼ�����Email�ռ䲻����
							BYCAS_EMAIL_NONE					û�����ʼ���
--*/
BVOID BYSTBCA_EmailNotify(BU8 byShow);
/*
����: �ṩ�������
����: blen ����pbBuf ����blen�������
*/
BU8	BYSTBCA_Random(BU8 blen,BU8 *pbBuf);
/*
���ܣ�֪ͨAPP����ĳ��Ƶ���źţ��ڸ��ض���Ƶ��Ѱ��������Ϣ��
����:	KHZ;
����:  1����ʾƵ��������������δ������Ƶ�㡣
--*/
BU8 BYCA_SetZoneFreq( BU32 Freq );

/*
���ܣ��Թ���Ļ�ķ�ʽ��ʾOSD��Ϣ��������ͬ����ʾ��Ҳ�������첽��ʾ��
����:	
	ADScroll_Info:
˵����
	�û�����ͨ��ң������
--*/
BVOID	BYSTBCA_ShowPromptOSDMsg(const ADScroll_Info* padInfo);/* �����ʱ���Բ���ʵ��*/

/*--------------------------- Ӧ���㲥 -----------------------*/
/*++
���ܣ�Ӧ���㲥, �л���ָ��Ƶ����
������
	wOriNetID:				ԭʼ����ID��
	wTSID:					������ID��
	wServiceID:				ҵ��ID��
	pUTCTime--5 bytes:				UTCʱ�䡣
--*/
BVOID BYCASTBCA_UrgencyBroad(BU16 wOriNetID,BU16 wTSID,BU16 wServiceID,BU8 *pUTCTime);

/*++
���ܣ�ȡ��Ӧ���㲥���л���Ӧ���㲥ǰ�û��ۿ���Ƶ����
�������ޡ�
--*/
BVOID BYCASTBCA_CancelUrgencyBroad(BVOID);

/*-------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
