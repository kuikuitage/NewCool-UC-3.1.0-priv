#ifndef BYCACA_STB_H_2013_02_20
#define BYCACA_STB_H_2013_02_20

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "BYCApubtype.h"
/*++
���ܣ�������������ʱ����ã���CASģ���ʼ����
������byReaderNo��		StreamGuard���������ܿ��������š�
����ֵ��
	true				��ʼ���ɹ���
	false				��ʼ��ʧ�ܡ�
--*/
BU8  BYCASTB_Init(BU8 byReaderNo);

/*++
���ܣ������л��CAģ��İ汾��
������pdwVer:				���ذ汾�š�					
		   pCaSysSp:                       CA��������
		   pCaSysCt:				CA����Ȩ��
����ֵ��
	true				�ɹ���
	false				ʧ�ܡ�
˵��������CAģ��İ汾�š�����: �����0x01020304��˼����1.2.3.4��
--*/
BU8 BYCASTB_GetVer(BU32 *pdwVer,BU8 *pCaSysSp,BU8 *pCaSysCt);

/*++
���ܣ�����CASģ�飬������ܿ��ڲ�ID��
������pdwCardID:			�������ܿ��ڲ�ID��						
����ֵ��
	true				�ɹ�
	false				ʧ��
--*/
BU8 BYCASTB_GetCardID(BU32 *pdwCardID);

/*++
���ܣ������л��CASID����øýӿ��ж�CASID�Ƿ���Ч�������Ч�򷵻�true�����򷵻�false��
������wCaSystemID:		�����з���SI/PSI��Ϣ��õ�CASID��
����ֵ��
	true				CASID��Ч
	false				CASID��Ч
--*/
BU8 BYCASTB_CASIDVerify(BU16 wCaSystemID);

/*++
���ܣ����û��忨ʱ��������������ã�֪ͨCASģ��
������byReaderNo:			�������ı��
--*/
BVOID BYCASTB_SCInsert(BVOID);

/*++
���ܣ����û��γ����ܿ�ʱ��������������ã�֪ͨCASģ�顣
������byReaderNo:			�������ı�š�
--*/
BVOID BYCASTB_SCRemove(BVOID);

/*++
���ܣ����õ�ǰ�����տ��Ľ�Ŀ����ϸ��Ϣ��
������	pServiceInfos:		SBYCAServiceInfo�ṹ��������Ŀ����ϸ��Ϣ��
		byCount:			EcmPid�ĸ�����Ŀǰֻ֧��byCount=1 �����Ϊ0 ��ʾֹͣECM
˵����
	���û���Ƶ��ʱ����������Ҫ����CAS�ṩ�Ĵ˺���������ǰ��Ŀ��ԭʼ��Ϣ��ECM PID���ݸ�CASģ�飬CASģ���
	���ݵ�ǰƵ������Ϣ�����뵱ǰƵ����صĲ����������ǰƵ��Ϊ�����ŵ�Ƶ��������Ҫ��ECM��PID��Ϊ0��
--*/
BVOID BYCASTB_SetCurEcmInfos(BU8 bType,const SBYCAServiceInfo* pServiceInfos);

/*++
���ܣ�����EMMPID��CASģ�齫�������õ�EMMPID��Ϣ����EMM���ݽ��մ���
������
	pwEmmPids:			CAT���а�����descriptor�н���CA_system_ID��CA_PID������BYCASTB_CASIDVerify()У����Ч��CA_PID��ΪEMM PID��
	nEmmCount:			EmmPid�ĸ�����Ŀǰֻ֧��nEmmCount��1�� ���Ϊ0 ��ʾֹͣEMM
˵��:
	���������յ�CAT��󣬻�������Ҫ����CA�ṩ�Ĵ˺�������EMM PID������������CA����Ϳ��Ը��ݴ�PID��ȡ
	˽�б������û��������Ȩ��Ϊ�û��ṩ��ֵ���񣬵�����nEmmCount = 0�� pwEmmPids = NULL ʱ��CAģ���ֹͣ��ȡEMM��
--*/
BVOID BYCASTB_SetEmmPids(BU8 nEmmCount,BU16* pwEmmPids);

/*++
���ܣ����������������յ�CAS˽�����ݻ���Time_Out����������ӿڽ������ṩ��CASģ�鴦��
������
byReqID��				
��ǰ������ȡ˽�б������ţ���BYSTBCA_SetStreamGuardFilter���byReqID��Ӧ
	bSuccess��				��ȡ������û�гɹ����ɹ�Ϊtrue��ʧ��Ϊfalse
	wPID:					���յ����ݵ�����PID��
	byszReceiveData��		��ȡ��˽������
	wLen��					��ȡ����˽�����ݵĳ���
--*/
BVOID BYCASTB_StreamGuardDataGot(BU8 byReqID,BU8 bSuccess,BU16 wPID,BU8* byszReceiveData,BU32 wLen);

/*++
���ܣ��򿪻�رյ�����Ϣ��
������bDebugFlag:			�򿪵�����Ϣ��־��1:�򿪵�����Ϣ;0:�رյ�����Ϣ��
--*/
BVOID BYCASTB_AddDebugMsg(BU8 bDebugFlag);

/*
���ܣ���������ʾ��һ��OSD��Ϣ��������ӿ���֪ͨCAģ�顣
����:	
	wDuration:				��Ϣ��ʾ�ľ���ʱ�䣬��λΪ�롣
--*/
BVOID BYCASTB_ShowOSDMsgOver(BU16 wDuration);

/*++
����:�����л�ȡE-mail�����ӿڡ�
����:
	*pEmailCount:			��������E-mail�ܸ�����
	*pNewEmailCount:		��������δ������E-mail������
����ֵ��
	1				�ɹ�
	0				ʧ��
--*/
BU8 BYCASTB_GetEmailCount(BU16 *pEmailCount,BU16 *pNewEmailCount);

/*++
����:�����л�ȡһ��E-mail�����ݡ�
����:
	icount:				��ǰ�����ڼ����ʼ��ţ�0-��ʾ��һ�⡣
	*pEmail:				��ȡ��E-mail�ṹ��
����ֵ:
  
--*/
BU8 BYCASTB_GetEmailContent(BU16 icount,MAIL_DATA *pEmail);

/*++
����:�����п���E-mail��֪ͨCAS�Ľӿڡ�
����:
	icount:				��ǰ�����ڼ����ʼ��ţ�0-��ʾ��һ�⡣
����ֵ:
	1:				����E-mailΪ�Ѷ��ɹ���
	0:					ʧ��
--*/
BU8 BYCASTB_EmailRead(BU16 icount);

/*++
����:������ɾ��E-mail�ӿڡ�
����:
	icount:			���Ϊ0����ʾɾ������������email��1 ��ʾ��һ��
����ֵ:
	1:				ɾ��E-mail�ɹ���
	0:				Ҫɾ����E-mail�����ڡ�ʧ��
--*/
BU8 BYCASTB_DelEmail(BU16 icount);

/*
	����в�Ʒ������true;
	
	���û�У�����false
	*pProductCount������ʵ�ʲ�Ʒ����
*/
BU8 BYCASTB_GetProductCount(BU16 *pProductCount);
/*
	���ز�Ʒ���ݣ�����ɹ�����true,����false
	pCaMenu_product->CaMenu_Product_Init �����������û�����壬���Բ��ÿ���
*/
BU8 BYCASTB_GetCaMenuProduct(CAMenu_Product *pCaMenu_product);
/*
  Ϊ��ʵ���������ι��ܣ���Ҫ�û����õĹ��ܣ����û��յ�BAT���ݺ�
  ���øú���
*/
BVOID BYCA_ParseBAT(BU8* psibuffer);

/*
���ܣ�����CASģ�飬���õ�ǰ������
NIT����������ӽṹΪ:
	tag                    							8bit(0x9c)
	length									8bit
	zoneID									16BIT 
���أ������ӳ��ȡ�
*/

BVOID BYCAS_NitZONEDes(BU8* databuf);


/*
BVOID TestCardBufferTest(BU16 u16Count);
*/
/*++
���ܣ�����CASģ�飬���ĸ�������ڲ�ID
������pdwCardID:			����ĸ�������ڲ�ID					
����ֵ��
	true				�ɹ�
	false				ʧ��
--*/

BU8 BYCAS_GetMasterCardID(BU32 *pdwCardID);
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

