// 2011 0801 LHCAS VER0.2
// LHCASʹ�õĻ����������Ͷ��壬�������

#ifndef _CAs_2_1_PUBLIC
#define _CAs_2_1_PUBLIC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef BYTE /*  8bit  */
#define BYTE unsigned char
#endif
#ifndef WORD /* 16bit */
#define WORD unsigned short
#endif
#ifndef HRESULT
#define	HRESULT long
#endif
#ifndef DWORD
#define DWORD unsigned long
#endif
#ifndef bool /* 8bit */
#define bool unsigned char
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

#define BNULL 0
//BNULL
#define BU8							unsigned char
#define BU16						unsigned short
#define BU32						unsigned long
#define BS8						signed char
#define	BS16						short
#define BS32						long
#define BVOID						void
#define BUS16						unsigned short	
/* �ź������� */
typedef long  CA_Semaphore ;
/*  �̺߳�����ַ����  */
typedef void (*pThreadFunc) (void);
// ��Ϣ����
typedef  struct _CA_MSG
{
	BU8 msgtype;
	BU16 msgparm1;
	BU16 msgparm2;
}CA_MSG;

#ifndef stb_printf
//#undef stb_printf
//extern 	GXUART_Handle_t 		  UART1;
#define stb_printf	//GXUART_Printf(UART1, __VA_ARGS__)
#endif

#define SDT_TABLE_ID					0x42
#define EMM_TABLE_ID					0x80
#define ECM_TABLE_ID1					0x82 
#define ECM_TABLE_ID					0x83 
#define	PubEmail_Table_ID				0xa1
#define	PriEmail_Table_ID				0xa2
#define   PubAdress_Table_ID				0xa3
#define   AD_SCROLL_Table_ID			0xa4
#define CA_MENU_Table_ID				0xa5

//�����ǳ���ֵ����     
#define AD_PROMPTOSD_MAXLEN			256
#define EMail_TIME						5
#define	EMail_Title_MAXLEN		50
#define	Email_Content_MAXLEN	300
#define	EMail_MAXNUM					40
#define CAMENU_PRODUCT_MAX					20
#define CAMENU_PRODUCTNAME_MAXLEN		40
/*----------------------------------------------------------------------*/
/*                       ������OSD ��ʾ����Ϣ����ֵ����                               */
/*----------------------------------------------------------------------*/
#define STBCA_NO_ERROR												0	
// ���ܿ��ѻ�ȡ��Ȩ��  �������ܿ��������ȷʶ��   ,CA OSD ����ʾ
#define STBCA_ERROR_InitCardCommand								1   //���ܿ���ʼ������
#define STBCA_ERROR_ReadCardCommand								2	// ���ܿ��޷�ʶ��
#define STBCA_ERROR_NO_CARD												3	//�������ܿ�
#define STBCA_ERROR_EMM_RET												4  // ��Ч��EMM ����
#define STBCA_ERROR_ECM_RET												5	//��Ч��ECM����
#define STBCA_ECM_RET_READCOMM_ERROR								6
#define STBCA_EMMECM_RET_P1P2_ERROR								7    //��Чָ��
#define STBCA_EMMECM_RET_P3_ERROR									8	// ���ݸ�ʽ��Ч
#define STBCA_EMMECM_RET_DES_ERROR									9
#define STBCA_ECM_RET_NOENTITLE_ERROR							10			//��ǰ����û����Ȩ
#define STBCA_EMMECM_RET_STORE_ERROR								11
#define STBCA_EMMECM_RET_CHECK_ERROR								12			// CRC   ����,��ǰ��Ŀû����Ȩ
#define STBCA_EMM_NOKEY_OR_NOFILE_ERROR						13
#define STBCA_ECM_OUTDATE_ERROR										14
#define STBCA_EMMECM_RET_UNKNOW_ERROR				  		15
#define STBCA_NO_ECM_INFO												16
#define STBCA_ZONE_FORBID_ERROR										17  // �����޲���Ч
#define STBCA_UserCdn_Limit_Ctrl_Enable									18			// ������ֹ����
#define STBCA_UserCdn_Limit_Ctrl_Disable									19			// ����������Ч
#define STBCA_ZoneCdn_Limit_Ctrl_Enable									20			// ������ֹ����
#define STBCA_ZoneCdn_Limit_Ctrl_Disable								21			// ����������Ч


//�������
#define STBCA_CARD_NEED_PAIRING										22   // û�л������
#define STATUE_CA_SET_PAIR												23			//  is pairing            �������������Ϣ����ȴ�
#define STBCA_CARD_PAIRING_OK											24	// ������Գɹ������31��16  24 25����Ϣ
#define STBCA_PAIRED_OtherCARD_ERROR									25 /*�û��������������*/
#define STBCA_PAIRED_OtherMachine_ERROR								26 /*�ÿ���˻��������*/

//��ĸ�����STATUE_CA_TIME_VAILD
#define	STATUE_CA_CHILD_SC									27 //��ĸ���ȴ����
#define STATUE_CA_TIME_VAILD                                         			28// ��ĸ�����ʱ���Ѹ���
#define STATUE_CA_NEED_PAIR									29  //"�ѹ���Ч��,��ѿ������������"
//����
#define STATUE_CA_CARD_ZONE_INVALID							30		//�������ܿ����ٷ�����
#define  STATUE_CA_CARD_ZONE_CHECK_ENTER                 			31                    /* ����Ѱ�����磬��ȴ�*/
#define  STATUE_CA_CARD_ZONE_CHEKC_OK                       			32         // ��stb�õ�����Ϣ����������ϣ�
				//			ǿ�Ƹ�Ƶͷ������ǰ��ĿƵ��,���ŵ�ǰ��Ŀ��

#define  STATUE_CA_OSD_CLS                     			0XFF  //��caosd ��ʾ

/*----------------------------------------------------------------------*/
/*                       ������һЩ���ݽṹ����                         */
/*----------------------------------------------------------------------*/

typedef struct tag_mail_Data_struct{
	BU8		 	MessageFlag; 	/*	0 -- ����Ҫ�洢 1 -- ��Ҫ�洢 2 -- δ�Ķ� 3-- �Ѿ��Ķ� 4 -- ɾ�� 0xff û�г�ʼ��*/
	BU32		MessageId;		
	BU8 			MessageType; 
	BU8			TitleLen;			
	BU16		ContentLen;  	
	BU8			DisplayType;		
	BU16 		reserved1;  	
  	BU8			SendTime[EMail_TIME]; 
  	BU8			VaildTime[EMail_TIME];
	BU8 		Title[EMail_Title_MAXLEN];
	BU8   	Content[Email_Content_MAXLEN];
}Email_Data;

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

#ifdef  __cplusplus
}
#endif

#endif	
