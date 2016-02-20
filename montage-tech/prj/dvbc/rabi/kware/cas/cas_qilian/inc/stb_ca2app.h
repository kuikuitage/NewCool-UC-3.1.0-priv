//���ļ�����CAģ��ʵ���ṩ��STB���õĽӿ�
// 2011 0801 LHCAS VER0.2

#ifndef _STB_CAS2APP
#define _STB_CAS2APP

#include "stb_Public.h"

#ifdef  __cplusplus
extern "C" {
#endif 

/*-----------------------------------���½ӿ���CAS�ṩ��STB����----------------------------------*/

/*++
���ܣ�������������ʱ����ã���CASģ���ʼ����
������
	
����ֵ��
	true				��ʼ���ɹ���
	false				��ʼ��ʧ�ܡ�
--*/
BU8 STBCA_Init(void);
/*++
���ܣ������л��CAģ��İ汾��
������
	pdwVer:				���ذ汾�š�					
����ֵ��
	true				�ɹ���	
˵����
	����CAģ��İ汾�š�����: �����0x01020304��˼����1.2.3.4��
--*/
void	STBCA_GetVer(BU32 *pdwVer);
/*++
���ܣ������л��CAģ��ĳ�����Ϣ��
������
	pdwVer:				���س�����Ϣ				
����ֵ��
	true				�ɹ���	
--*/
void STBCA_GetServiceInfo(char *pdwVer);
/*++
���ܣ����û��忨ʱ��������������ã�֪ͨCASģ��
������VOID
����:	  cas��ʼ���ɹ�����true
--*/
bool STBCA_SCInsert( void);

/*++
���ܣ����û��γ����ܿ�ʱ��������������ã�֪ͨCASģ�顣
--*/
void STBCA_SCRemove(void);

/*++
���ܣ�����CASģ�飬������ܿ��ڲ�ID��
������pdwCardID ����
����ֵ��
	0				ʧ��,�޿�
	1    ��ȡ�ɹ���
--*/
bool STBCA_GetSmartCardID(BU32 *pdwCardID,BU16* Zone_ID);

//���ܣ��������ڽ���PMT CAT ʱ������ǰ������Ϣ�Ƿ���	CAS��Ӧ���ṩ
//
BU8    STBCA_IsLYCA( BU16 wCaSystemID );
/*++
���ܣ��û���̨ʱ���õ�CAS�ӿڡ��˹�����APPʵ�֣��ѵ�ECM pid�����STBCA_SetCurECMInfo
����:  ��ǰ��Ŀ��pmtpid����Ŀ���
--*/

/*++
���ܣ����������յ�CAT���,֪ͨ	CAS��ǰ��CAS ��EMM pid��
--*/
void STBCA_SetEmmPid(BU16 pid);
/*++
���ܣ����������յ�PMT���,֪ͨ	CAS��ǰ��CAS��ECM pid��������ȫΪ0x1fff ʱ��ca demux
		ֹͣ�����������
--*/
void STBCA_SetCurECMInfo(BU16 ecmpid,BU16 serverid);
/*
demo: cat:
    if( STBCA_IsLRCA() == 1 )
       STBCA_SetEmmPid();
       pmt:
        if( STBCA_IsLRCA() == 1 )
       STBCA_SetCurECMInfo();
*/
/*++
���ܣ����������������յ�CAS˽�����ݻ���Time_Out����������ӿڽ������ṩ��CASģ�鴦��
������
	ReqID��				��ǰ������ȡ˽�б������ţ���STBCA_SetStreamGuardFilter���ReqID��Ӧ
	bSuccess��				��ȡ������û�гɹ����ɹ�Ϊtrue��ʧ��Ϊfalse
	wPID:					���յ����ݵ�����PID��
	b8ReceiveData		��ȡ��˽������
	wLen��					��ȡ����˽�����ݵĳ���
--*/
BVOID  STBCA_StreamGuardDataGot(BU8 ReqID,BU8 bSuccess,BU16 wPID,BU8* b8ReceiveData,BU32 wLen);

/*++
���ܣ�����CASģ�飬caֹͣ����Dexmux section���ݣ������ָ��ɵ���STBCA_DemuxRun
������void	
*/
void STBCA_DemuxStop(void );
/*++
����:�෴
*/
void STBCA_DemuxRun(void );

/*++
���ܣ�����CASģ�飬   ɾ���ʼ�����
������0����ʾɾ�����е��ʼ���1��ʾɾ����һ�⡣����
*/
BU8 STBCA_DelEmail(BU16 savedNO);

/*
���ܣ�����CASģ�飬��ȡ�ʼ�������Ϣ,���40�⡣
��������һ�����������ʼ�����������2�������������ʼ�����
���أ�0 ��ʾ��ȡʧ�ܡ�
*/
BU8 STBCA_GetEmailCount(BU16 *pEmailCount,BU16 *pNewEmailCount);

/*
���ܣ�����CASģ�飬��ȡ�ʼ���
��������һ��������ʾҪ���ʼ���ţ���0��ʼ С��EMail_MAXNUM����2�����������ʼ�����
���أ�0 ��ʾ��ȡʧ�ܣ������ʼ������ڡ�
*/
BU8 STBCA_GetEmailContent(BU16 b16index,Email_Data *pEmail);
/*
���ܣ�����CASģ�飬��0��ʼ ���ô洢���ΪsaveNO ���ʼ�Ϊ�Ѷ�
���أ�0 ��ʾ��ȡʧ�ܣ������ʼ������ڡ�
*/

BU8 STBCA_EmailRead(BU16 savedNO);
/*
���ܣ�����CASģ�飬��ϵͳ��������������ã��������״̬��
		֪ͨ������OSD�˵���ʾ�Ƿ��������䡢���ʼ�������
*/
 void STBCA_CHECK_MailBox(void);
//NIT������������� 0x9c
//��Ϊ��������ӳ��Ȳ��ǹ̶��ģ����Դ���������������������1�����ݾ��Ǳ�׼9c
// �ѵ������������ca������Լ�����BAT,�Լ��������ⲿӦ��demux����й���bat���ܡ�
void STBCA_CurrentNitDes(BU8* databuf );

/*
���ܣ�����CASģ�飬��ȡ��ǰ���ܿ��û������Ĳ�Ʒ��Ϣ
    CAMenu_Product���ͱ�ʾ��Ʒ���͡�
���أ�0 ��ʾ��ȡʧ��    
*/

BU8 STBCA_GetCaMenuProduct(CAMenu_Product *pCaMenu_product);
/*
���ܣ�����CASģ�飬��ȡ��ǰ���ܿ��û������Ĳ�Ʒ����
    CAMenu_Product���ͱ�ʾ��Ʒ���͡�
���أ�0 ��ʾ��ȡʧ��    
*/
BU8 STBCA_GetProductCount(BU16 *pProductCount);
/*
���ܣ�����CASģ�飬�ر�cas��ӡ��Ϣ
*/
/*
���ܣ�����CASģ�飬��ȡ��ǰ���ܿ� ��Ϣ���ж��Ƿ�Ϊ�ӿ�
���أ�0 ��ʾ��ȡʧ�� ����ǰ��Ϊĸ�� 
                 1��ʾΪ�ӿ���pdwCardIDΪĸ������
*/

bool STBCA_isDaughterCard(BU32 *pdwCardID);
/*
���ܣ�����CASģ�飬��ȡ��ǰcaϵͳʹ�õ�ʱ�䣬��ʽΪMJD������ֽڡ�
����:����ֽڵ����顣
	mjd1 = time1[0];
	mjd1 = (mjd1 <<8)+time1[1];
	h = BCDTOHex(time1[2]);
	m = BCDTOHex(time1[3]);
*/
BU8* STBCA_GetCaSysTime(void);
#ifdef  __cplusplus
}
#endif
#endif  

