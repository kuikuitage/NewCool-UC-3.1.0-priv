/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef CDCAS_CALIB_APIEX_H
#define CDCAS_CALIB_APIEX_H

#ifdef  __cplusplus
extern "C" {
#endif


/*-------------------------------------������������---------------------------------------*/
typedef unsigned  long  CDCA_U32;
typedef unsigned  short CDCA_U16;
typedef unsigned  char  CDCA_U8;

typedef signed char  CDCA_S8;
typedef signed long  CDCA_S32;

typedef CDCA_U8 CDCA_BOOL;

#define CDCA_TRUE    ((CDCA_BOOL)1)
#define CDCA_FALSE   ((CDCA_BOOL)0)

/*----------------------------------------�궨��--------------------------------------*/

/*--------- FLASH���� -------*/
#define CDCA_FLASH_BLOCK_A        1     /* BLOCK A */
#define CDCA_FLASH_BLOCK_B        2     /* BLOCK B */

/*--------- ������� --------*/
#define CDCA_MAXLEN_STBSN         12U   /* �����кŵĳ��� */
#define CDCA_MAXLEN_PINCODE       6U    /* PIN��ĳ��� */
#define CDCA_MAXNUM_OPERATOR      4U    /* ������Ӫ�̸��� */
#define CDCA_MAXLEN_TVSPRIINFO    32U   /* ��Ӫ��˽����Ϣ�ĳ��� */
#define CDCA_MAXNUM_ACLIST        18U   /* ÿ����Ӫ�̵��û��������� */


#define CDCA_MAXLEN_SN            16U   /* ���ܿ����кŵĳ��� */
#define CDCA_MAXNUM_SLOT          20U   /* ���洢�����Ǯ���� */

#define CDCA_MAXNUM_CURTAIN_RECORD	80U /*�߼�Ԥ����Ŀ��¼��������*/


#define CDCA_MAXNUM_IPPVP         300U  /* ���ܿ��������IPPV��Ŀ�ĸ��� */
#define CDCA_MAXNUM_PRICE         2U    /* ����IPPV�۸���� */


#define CDCA_MAXNUM_ENTITLE       300U  /* ��Ȩ��Ʒ�������� */

/*---------- ������������� ----------*/
#define CDCA_MAXNUM_PROGRAMBYCW   4U    /* һ������������Ľ�Ŀ�� */

#define CDCA_MAXNUM_DETITLE       5U    /* ÿ����Ӫ���¿ɱ���ķ���Ȩ����� */

/*---------- CAS��ʾ��Ϣ ---------*/
#define CDCA_MESSAGE_CANCEL_TYPE      0x00  /* ȡ����ǰ����ʾ */
#define CDCA_MESSAGE_BADCARD_TYPE     0x01  /* �޷�ʶ�� */
#define CDCA_MESSAGE_EXPICARD_TYPE    0x02  /* ���ܿ����ڣ�������¿� */
#define CDCA_MESSAGE_INSERTCARD_TYPE  0x03  /* ���Ž�Ŀ����������ܿ� */
#define CDCA_MESSAGE_NOOPER_TYPE      0x04  /* ��֧�ֽ�Ŀ��Ӫ�� */
#define CDCA_MESSAGE_BLACKOUT_TYPE    0x05  /* �������� */
#define CDCA_MESSAGE_OUTWORKTIME_TYPE 0x06  /* ��ǰʱ�α��趨Ϊ���ܹۿ� */
#define CDCA_MESSAGE_WATCHLEVEL_TYPE  0x07  /* ��Ŀ��������趨�Ĺۿ����� */
#define CDCA_MESSAGE_PAIRING_TYPE     0x08  /* ���ܿ��뱾�����в���Ӧ */
#define CDCA_MESSAGE_NOENTITLE_TYPE   0x09  /* û����Ȩ */
#define CDCA_MESSAGE_DECRYPTFAIL_TYPE 0x0A  /* ��Ŀ����ʧ�� */
#define CDCA_MESSAGE_NOMONEY_TYPE     0x0B  /* ���ڽ��� */
#define CDCA_MESSAGE_ERRREGION_TYPE   0x0C  /* ������ȷ */
#define CDCA_MESSAGE_NEEDFEED_TYPE    0x0D  /* �ӿ���Ҫ��ĸ����Ӧ�������ĸ�� */
#define CDCA_MESSAGE_ERRCARD_TYPE     0x0E  /* ���ܿ�У��ʧ�ܣ�����ϵ��Ӫ�� */
#define CDCA_MESSAGE_UPDATE_TYPE      0x0F  /* ���ܿ������У��벻Ҫ�ο����߹ػ� */
#define CDCA_MESSAGE_LOWCARDVER_TYPE  0x10  /* ���������ܿ� */
#define CDCA_MESSAGE_VIEWLOCK_TYPE    0x11  /* ����Ƶ���л�Ƶ�� */
#define CDCA_MESSAGE_MAXRESTART_TYPE  0x12  /* ���ܿ���ʱ���ߣ���5���Ӻ����¿��� */
#define CDCA_MESSAGE_FREEZE_TYPE      0x13  /* ���ܿ��Ѷ��ᣬ����ϵ��Ӫ�� */
#define CDCA_MESSAGE_CALLBACK_TYPE    0x14  /* ���ܿ�����ͣ����ش����Ӽ�¼����Ӫ�� */
#define CDCA_MESSAGE_CURTAIN_TYPE	  0x15 /*�߼�Ԥ����Ŀ���ý׶β�����ѹۿ�*/
#define CDCA_MESSAGE_CARDTESTSTART_TYPE 0x16 /*�������Կ�������...*/
#define CDCA_MESSAGE_CARDTESTFAILD_TYPE 0x17 /*�������Կ�����ʧ�ܣ��������ͨѶģ��*/
#define CDCA_MESSAGE_CARDTESTSUCC_TYPE  0x18 /*�������Կ����Գɹ�*/
#define CDCA_MESSAGE_NOCALIBOPER_TYPE    0x19/*���в�������ֲ�ⶨ����Ӫ��*/

#define CDCA_MESSAGE_STBLOCKED_TYPE   0x20  /* ������������ */
#define CDCA_MESSAGE_STBFREEZE_TYPE   0x21  /* �����б����� */



#define CDCA_MESSAGE_UNSUPPORTDEVICE_TYPE    0x30 /*��֧�ֵ��ն����ͣ���ţ�0xXXXX��*/

/*---------- ���ܵ��÷���ֵ���� ----------*/
#define CDCA_RC_OK                    0x00  /* �ɹ� */
#define CDCA_RC_UNKNOWN               0x01  /* δ֪���� */
#define CDCA_RC_POINTER_INVALID       0x02  /* ָ����Ч */
#define CDCA_RC_CARD_INVALID          0x03  /* ���ܿ���Ч */
#define CDCA_RC_PIN_INVALID           0x04  /* PIN����Ч */
#define CDCA_RC_DATASPACE_SMALL       0x06  /* �����Ŀռ䲻�� */
#define CDCA_RC_CARD_PAIROTHER        0x07  /* ���ܿ��Ѿ���Ӧ��Ļ����� */
#define CDCA_RC_DATA_NOT_FIND         0x08  /* û���ҵ���Ҫ������ */
#define CDCA_RC_PROG_STATUS_INVALID   0x09  /* Ҫ����Ľ�Ŀ״̬��Ч */
#define CDCA_RC_CARD_NO_ROOM          0x0A  /* ���ܿ�û�пռ��Ź���Ľ�Ŀ */
#define CDCA_RC_WORKTIME_INVALID      0x0B  /* �趨�Ĺ���ʱ����Ч */
#define CDCA_RC_IPPV_CANNTDEL         0x0C  /* IPPV��Ŀ���ܱ�ɾ�� */
#define CDCA_RC_CARD_NOPAIR           0x0D  /* ���ܿ�û�ж�Ӧ�κεĻ����� */
#define CDCA_RC_WATCHRATING_INVALID   0x0E  /* �趨�Ĺۿ�������Ч */
#define CDCA_RC_CARD_NOTSUPPORT       0x0F  /* ��ǰ���ܿ���֧�ִ˹��� */
#define CDCA_RC_DATA_ERROR            0x10  /* ���ݴ������ܿ��ܾ� */
#define CDCA_RC_FEEDTIME_NOT_ARRIVE   0x11  /* ι��ʱ��δ�����ӿ����ܱ�ι�� */
#define CDCA_RC_CARD_TYPEERROR        0x12  /* ��ĸ��ι��ʧ�ܣ��������ܿ����ʹ��� */

#define CDCA_RC_CAS_FAILED                  0x20 //����casָ��ִ��ʧ��
#define CDCA_RC_OPER_FAILED                0x21 //������Ӫ��ָ��ִ��ʧ��

/*-- �������е����ܿ�״̬ --*/
#define CDCA_SC_OUT         0x00    /* ��������û�п�          */
#define CDCA_SC_REMOVING    0x01    /* ���ڰο�������״̬      */
#define CDCA_SC_INSERTING   0x02    /* ���ڲ忨����ʼ��        */
#define CDCA_SC_IN          0x03    /* ���������ǿ��õĿ�      */
#define CDCA_SC_ERROR       0x04    /* �������Ŀ�����ʶ��      */
#define CDCA_SC_UPDATE      0x05    /* �������Ŀ������� */
#define CDCA_SC_UPDATE_ERR  0x06    /* �������Ŀ�����ʧ��      */

/*---------- ECM_PID���õĲ������� ---------*/
#define CDCA_LIST_OK          0x00
#define CDCA_LIST_FIRST       0x01
#define CDCA_LIST_ADD         0x02


/*------------ �ʼ���С���������� ------------*/
#define CDCA_MAXNUM_EMAIL         100U  /* �����б��������ʼ����� */
#define CDCA_MAXLEN_EMAIL_TITLE   30U   /* �ʼ�����ĳ��� */
#define CDCA_MAXLEN_EMAIL_CONTENT 160U  /* �ʼ����ݵĳ��� */
/*------------ �ʼ�ͼ����ʾ��ʽ ------------*/
#define CDCA_Email_IconHide       0x00  /* �����ʼ�֪ͨͼ�� */
#define CDCA_Email_New            0x01  /* ���ʼ�֪ͨ����ʾ���ʼ�ͼ�� */
#define CDCA_Email_SpaceExhaust   0x02  /* ���̿ռ�������ͼ����˸�� */

/*------------ OSD�ĳ������� -----------*/
#define CDCA_MAXLEN_OSD           256U  /* OSD���ݵ���󳤶� */
/*------------ OSD��ʾ���� ------------*/
#define CDCA_OSD_TOP              0x01  /* OSD�����ʾ����Ļ�Ϸ� */
#define CDCA_OSD_BOTTOM           0x02  /* OSD�����ʾ����Ļ�·� */
#define CDCA_OSD_FULLSCREEN       0x03  /* OSD���������ʾ */
#define CDCA_OSD_HALFSCREEN       0x04  /* OSD��񣺰�����ʾ */


/*------------ IPPV/IPPT��ͬ����׶���ʾ -------------*/
#define CDCA_IPPV_FREEVIEWED_SEGMENT  0x00  /* IPPV���Ԥ���׶Σ��Ƿ��� */
#define CDCA_IPPV_PAYVIEWED_SEGMENT   0x01  /* IPPV�շѽ׶Σ��Ƿ��� */
#define CDCA_IPPT_PAYVIEWED_SEGMENT   0x02  /* IPPT�շѶΣ��Ƿ��� */

/*------------ IPPV�۸����� ------------*/
#define CDCA_IPPVPRICETYPE_TPPVVIEW       0x0  /* ���ش�����¼������ */
#define CDCA_IPPVPRICETYPE_TPPVVIEWTAPING 0x1  /* ���ش�����¼������ */

/*------------ IPPV��Ŀ��״̬ -----------*/
#define CDCA_IPPVSTATUS_BOOKING   0x01  /* Ԥ�� */
#define CDCA_IPPVSTATUS_VIEWED    0x03  /* �ѿ� */


/*---------- Ƶ������Ӧ����ض��� ---------*/
#define CDCA_MAXNUM_COMPONENT     5U    /* ��Ŀ��������� */
#define CDCA_MAXLEN_LOCKMESS      40U


/*---------- ����Ȩȷ����Ӧ����ض��� --------*/
#define CDCA_Detitle_All_Read     0x00  /* ���з���Ȩȷ�����Ѿ�����������ͼ�� */
#define CDCA_Detitle_Received     0x01  /* �յ��µķ���Ȩ�룬��ʾ����Ȩ��ͼ�� */
#define CDCA_Detitle_Space_Small  0x02  /* ����Ȩ��ռ䲻�㣬�ı�ͼ��״̬��ʾ�û� */
#define CDCA_Detitle_Ignore       0x03  /* �յ��ظ��ķ���Ȩ�룬�ɺ��ԣ��������� */


/*---------- ��������ʾ��Ϣ ---------*/
#define CDCA_SCALE_RECEIVEPATCH   1     /* �������ݽ����� */
#define CDCA_SCALE_PATCHING       2     /* ���ܿ������� */

/*---------- CDSTBCA_ActionRequest �ӿڲ��� ------------------------------*/
#define CDCA_ACTIONREQUEST_RESTARTSTB		   0	/* ���������� */
#define CDCA_ACTIONREQUEST_FREEZESTB		   1	/* ��������� */
#define CDCA_ACTIONREQUEST_SEARCHCHANNEL       2    /* ����������Ŀ */
#define CDCA_ACTIONREQUEST_STBUPGRADE          3    /* �����г������� */
#define CDCA_ACTIONREQUEST_UNFREEZESTB         4    /* ������������ */
#define CDCA_ACTIONREQUEST_INITIALIZESTB	   5    /* ��ʼ�������� */
#define CDCA_ACTIONREQUEST_SHOWSYSTEMINFO      6    /* ��ʾϵͳ��Ϣ */

#define CDCA_ACTIONREQUEST_CARDINSERTFINISH   255    /* �忨������� */
/*------------------------------------------------------------------------*/

/*---------- �߼�Ԥ��״̬�� ---------*/
#define CDCA_CURTAIN_BASE				   0x0000		/*�߼�Ԥ��״̬�����ֵ,���ֽڱ���0x00*/
#define CDCA_CURTAIN_CANCLE			   (CDCA_CURTAIN_BASE+0x00)  /*ȡ���߼�Ԥ����ʾ*/
#define CDCA_CURTAIN_OK    				   (CDCA_CURTAIN_BASE+0x01)  /*�߼�Ԥ����Ŀ��������*/
#define CDCA_CURTAIN_TOTTIME_ERROR	   (CDCA_CURTAIN_BASE+0x02)  /*�߼�Ԥ����Ŀ��ֹ���ܣ��Ѿ��ﵽ�ܹۿ�ʱ��*/
#define CDCA_CURTAIN_WATCHTIME_ERROR (CDCA_CURTAIN_BASE+0x03)  /*�߼�Ԥ����Ŀ��ֹ���ܣ��Ѿ��ﵽWatchTime����*/
#define CDCA_CURTAIN_TOTCNT_ERROR 	   (CDCA_CURTAIN_BASE+0x04)  /*�߼�Ԥ����Ŀ��ֹ���ܣ��Ѿ��ﵽ������ۿ�����*/
#define CDCA_CURTAIN_ROOM_ERROR 	   (CDCA_CURTAIN_BASE+0x05)  /*�߼�Ԥ����Ŀ��ֹ���ܣ��߼�Ԥ����Ŀ��¼�ռ䲻��*/
#define CDCA_CURTAIN_PARAM_ERROR 	   (CDCA_CURTAIN_BASE+0x06)  /*�߼�Ԥ����Ŀ��ֹ���ܣ���Ŀ��������*/
#define CDCA_CURTAIN_TIME_ERROR 		   (CDCA_CURTAIN_BASE+0x07)  /*�߼�Ԥ����Ŀ��ֹ���ܣ����ݴ���*/


/*---------- CDSTBCA_ContinuesWatchLimit �ӿڲ��� ���Ͷ���------------------------------*/
#define CDCA_ContinuesWatchLimit_Disable	   0	/* ������ */
#define CDCA_ContinuesWatchLimit_Enable		   1	/* ���� */
#define CDCA_ContinuesWatchLimit_Clear		   2	/* ȡ����ǰ����*/
/*------------------------------------------------------------------------*/

/*-------------------------------------end of �궨��--------------------------------------*/



/*----------------------------------------���ݽṹ----------------------------------------*/

/*-- ϵͳʱ�� --*/
typedef CDCA_U32  CDCA_TIME;
typedef CDCA_U16  CDCA_DATE;

/*-- �ź������壨��ͬ�Ĳ���ϵͳ���ܲ�һ����--*/
typedef CDCA_U32  CDCA_Semaphore;

/*-- ��Ӫ����Ϣ --*/
typedef struct {
    char     m_szTVSPriInfo[CDCA_MAXLEN_TVSPRIINFO+1];  /* ��Ӫ��˽����Ϣ */
    CDCA_U8  m_byReserved[3];    /* ���� */
}SCDCAOperatorInfo;

/*-- ��Ŀ��Ϣ --*/
/* Y10_update : ֻ��ҪECMPID��ServiceID���� */
typedef struct {
    CDCA_U16  m_wEcmPid;         /* ��Ŀ��Ӧ������Ϣ��PID */
    CDCA_U8   m_byServiceNum;    /* ��ǰPID�µĽ�Ŀ���� */
    CDCA_U8   m_byReserved;      /* ���� */
    CDCA_U16  m_wServiceID[CDCA_MAXNUM_PROGRAMBYCW]; /* ��ǰPID�µĽ�ĿID�б� */
}SCDCASServiceInfo;

/*-- ��Ȩ��Ϣ --*/
typedef struct {
    CDCA_U32  m_dwProductID;   /* ��ͨ��Ȩ�Ľ�ĿID */    
    CDCA_DATE m_tBeginDate;    /* ��Ȩ����ʼʱ�� */
    CDCA_DATE m_tExpireDate;   /* ��Ȩ�Ĺ���ʱ�� */
    CDCA_U8   m_bCanTape;      /* �û��Ƿ���¼��1������¼��0��������¼�� */
    CDCA_U8   m_byReserved[3]; /* ���� */
}SCDCAEntitle;

/*-- ��Ȩ��Ϣ���� --*/
typedef struct {
    CDCA_U16      m_wProductCount;
    CDCA_U8       m_m_byReserved[2];    /* ���� */
    SCDCAEntitle  m_Entitles[CDCA_MAXNUM_ENTITLE]; /* ��Ȩ�б� */
}SCDCAEntitles;

/*-- Ǯ����Ϣ --*/
typedef struct {
    CDCA_U32  m_wCreditLimit; /* ���öȣ�������*/
    CDCA_U32  m_wBalance;     /* �ѻ��ĵ��� */
}SCDCATVSSlotInfo;

/*-- IPPV/IPPT��Ŀ�ļ۸� --*/
typedef struct {
    CDCA_U16  m_wPrice;       /* ��Ŀ�۸񣨵�����*/
    CDCA_U8   m_byPriceCode;  /* ��Ŀ�۸����� */
    CDCA_U8   m_byReserved;   /* ���� */
}SCDCAIPPVPrice; 

/*-- IPPV/IPPT��Ŀ������ʾ��Ϣ --*/
typedef struct {
    CDCA_U32        m_dwProductID;          /* ��Ŀ��ID */
    CDCA_U16        m_wTvsID;               /* ��Ӫ��ID */
    CDCA_U8         m_bySlotID;             /* Ǯ��ID */
    CDCA_U8         m_byPriceNum;           /* ��Ŀ�۸���� */
    SCDCAIPPVPrice  m_Price[CDCA_MAXNUM_PRICE]; /* ��Ŀ�۸� */
    union {
        CDCA_DATE   m_wExpiredDate;         /* ��Ŀ����ʱ��,IPPV�� */
        CDCA_U16    m_wIntervalMin;         /* ʱ��������λ����,IPPT �� */
    }m_wIPPVTime;
    CDCA_U8         m_byReserved[2];        /* ���� */
}SCDCAIppvBuyInfo;

/*-- IPPV��Ŀ��Ϣ --*/
typedef struct {
    CDCA_U32   m_dwProductID;   /* ��Ŀ��ID */
    CDCA_U8    m_byBookEdFlag;  /* ��Ʒ״̬��BOOKING��VIEWED */ 
    CDCA_U8    m_bCanTape;      /* �Ƿ����¼��1������¼��0��������¼�� */
    CDCA_U16   m_wPrice;        /* ��Ŀ�۸� */
    CDCA_DATE  m_wExpiredDate;  /* ��Ŀ����ʱ��,IPPV�� */
    CDCA_U8    m_bySlotID;      /* Ǯ��ID */
    CDCA_U8    m_byReserved;    /* ���� */
}SCDCAIppvInfo;


/*-- �ʼ�ͷ --*/
typedef struct {
    CDCA_U32   m_dwActionID;                 /* Email ID */
    CDCA_U32   m_tCreateTime;                /* EMAIL������ʱ�� */
    CDCA_U16   m_wImportance;                /* ��Ҫ�ԣ� 0����ͨ��1����Ҫ */
    CDCA_U8    m_byReserved[2];              /* ���� */
    char       m_szEmailHead[CDCA_MAXLEN_EMAIL_TITLE+4]; /* �ʼ����� */    
    CDCA_U8    m_bNewEmail;                  /* ���ʼ���ǣ�0���Ѷ��ʼ���1�����ʼ� */
}SCDCAEmailHead;

/*-- �ʼ����� --*/
typedef struct {
    char     m_szEmail[CDCA_MAXLEN_EMAIL_CONTENT+4];      /* Email������ */
    CDCA_U8  m_byReserved[3];              /* ���� */
}SCDCAEmailContent;

/* ����OSD �ṹ*/
typedef	struct{
	CDCA_U8		byDisplayMode; //��ʾģʽ�� 0����ʾ��Ļ��������ʾ��1����ʾ��Ļ�Ϸ�������ʾ��2����ʾ��Ļ�·�������ʾ������ֵ����
	//����5������ֻ��byDisplayModeΪ0ʱ����Ч��
	CDCA_U8		byShowType; //�Ƿ�ǿ����ʾ 0:��ǿ����ʾ��1:ǿ����ʾ��
	CDCA_U8		byFontSize;	//����OSD�����С 0��Ĭ�ϣ�1����ţ�2��С�ţ�3~255��ʾʵ���ֺŴ�С����������в�֧��3~255����Ĭ�������С��ʾ
	CDCA_U8		byBackgroundArea; //ռ��Ļ�����벿�ֵ�����ٷֱȣ���Χ��20~100����
	CDCA_BOOL	bShowCardSN; //�Ƿ�Ҫ���ڳ���OSD�����½���ʾ���ţ� 0������ʾ 1����ʾ
	char		szCardSN[17]; //���ܿ����ţ���0��β���ַ�����bShowCardSNΪ1ʱ��ʾ��ע�����������СΪ������Ĭ�������С��
	//����3��������byDisplayModeΪ0,1,2����Ч��
	CDCA_U32	dwFontColor;		//������ɫ
	CDCA_U32	dwBackgroundColor;	//������ɫ
	char 		szContent[CDCA_MAXLEN_OSD+4]; //����OSD���ݡ�ע��������ʾʱ�����СΪ������Ĭ�������С��
}SCDCAOSDInfo;


/*-- Ƶ��������Ϣ --*/
/*-- ��Ŀ�����Ϣ --*/
typedef struct {    /* �������֪ͨ�����н�Ŀ���ͼ�PID����Ϣ��һ����Ŀ���ܰ��������� */
    CDCA_U16   m_wCompPID;     /* ���PID */
    CDCA_U16   m_wECMPID;      /* �����Ӧ��ECM����PID���������ǲ����ŵģ���Ӧȡ0�� */
    CDCA_U8    m_CompType;     /* ������� */
    CDCA_U8    m_byReserved[3];/* ���� */
}SCDCAComponent;

/*-- Ƶ��������Ϣ --*/
typedef struct {    
    CDCA_U32   m_dwFrequency;              /* Ƶ�ʣ�BCD�� */
    CDCA_U32   m_symbol_rate;              /* �����ʣ�BCD�� */
    CDCA_U16   m_wPcrPid;                  /* PCR PID */
    CDCA_U8    m_Modulation;               /* ���Ʒ�ʽ */
    CDCA_U8    m_ComponentNum;             /* ��Ŀ������� */
    SCDCAComponent m_CompArr[CDCA_MAXNUM_COMPONENT];       /* ��Ŀ����б� */
    CDCA_U8    m_fec_outer;                /* ǰ��������� */
    CDCA_U8    m_fec_inner;                /* ǰ��������� */
    char       m_szBeforeInfo[CDCA_MAXLEN_LOCKMESS+1]; /* ���� */
    char       m_szQuitInfo[CDCA_MAXLEN_LOCKMESS+1];   /* ���� */
    char       m_szEndInfo[CDCA_MAXLEN_LOCKMESS+1];    /* ���� */
}SCDCALockService;

/*-- ָ����ʾ��Ϣ--*/
typedef struct {
	CDCA_U8		byShowType; 	//��ʾ��ʽ0���ַ���; 1������2����������ֵ������
	CDCA_U8		byContentType; 	//��ʾ����0�����ţ�1�������кţ�����ֵ������
	CDCA_BOOL	bIsVisible; 	//�ɼ���0�����ԣ�1������
	CDCA_BOOL   bIsForcedShow;  //�Ƿ�ǿ����ʾ 0����ǿ����ʾ 1��ǿ����ʾ��
	CDCA_U8       byEncryption;  //�����㷨�� 0�������ܣ���0������
	CDCA_U16     wFlashing_EachShowTime; //ÿ����˸��ʾʱ����Ϊ0ʱ��ʾ����˸��
	CDCA_U16     wFlashing_EachHideTime; //ÿ����˸��ͣʱ����Ϊ0ʱ��ʾ����˸��
	CDCA_U8       byFontSize; 	//ָ�������С��ȡֵ��Χ[0,255]��0��ʾĬ�������С��1��ʾ�����壬2��ʾС���壬3~255��ʾʵ�������С����������в�֧��3~255����Ĭ�������С��ʾ��byShowTypeΪ1��2ʱ��ʾָ������Ĵ�С��
	CDCA_U8		byReserved;	//0x00	����
	CDCA_U32	dwFontColor;	//������ɫ����׼RGBֵ��byShowTypeΪ1ʱ��ֵ�����塣
	CDCA_U32	dwBackgroundColor;	//������ɫ����׼RGBֵ��byShowTypeΪ1��2ʱ����Ҫ��
	CDCA_U8		byX_position;		//ָ����ʾλ�õ�X�����꣬ȡֵ��Χ0~100
	CDCA_U8		byY_position;		//ָ����ʾλ�õ�Y�����꣬ȡֵ��Χ0~100
	CDCA_U8	    pbyMatrixInnerPos[16];	// byShowTypeΪ2ʱ��ʾ����ָ��ÿ���ַ���Ӧ���ڲ�����ֵ��byShowTypeΪ����ֵʱ���ֶ������塣
    char		szContent[17]; //��Ҫ��ʾ��ָ�����ݣ���0��β���ַ�����
}SCDCAFingerInfo;

/*-- �߼�Ԥ����Ŀ��Ϣ --*/
typedef struct {
    CDCA_U16   m_wProgramID;        /* ��Ŀ��ID */
    CDCA_TIME  m_dwStartWatchTime;   /* ��ʼ�ۿ�ʱ�� */
    CDCA_U8    m_byWatchTotalCount;  /* �ۼƹۿ����� */
    CDCA_U16   m_wWatchTotalTime;    /* �ۼƹۿ�ʱ��,(cp������) */
}SCDCACurtainInfo;


/*-----------------------------------------------------------------------------------
a. ��ϵͳ�У�����m_dwFrequency��m_symbol_rateʹ��BCD�룬����ǰȡMHzΪ��λ��
   ����ʱ��ǰ4��4-bit BCD���ʾС����ǰ��ֵ����4��4-bit BCD���ʾС������ֵ��
   ���磺
        ��Ƶ��Ϊ642000KHz����642.0000MHz�����Ӧ��m_dwFrequency��ֵӦΪ0x06420000��
        ��������Ϊ6875KHz����6.8750MHz�����Ӧ��m_symbol_rate��ֵӦΪ0x00068750��

b. ��ϵͳ�У�m_Modulation��ȡֵ���£�
    0       Reserved
    1       QAM16
    2       QAM32
    3       QAM64
    4       QAM128
    5       QAM256
    6��255  Reserved
------------------------------------------------------------------------------------*/ 


/*------------------------------------end of ���ݽṹ-------------------------------------*/



/*---------------------------���½ӿ���CA_LIB�ṩ��STB------------------------*/

/*------ CA_LIB���ȹ��� ------*/

/* CA_LIB��ʼ�� */
extern CDCA_BOOL CDCASTB_Init( CDCA_U8 byThreadPrior );

/* �ر�CA_LIB���ͷ���Դ */
extern void CDCASTB_Close( void );

/* CDCASͬ���ж� */
extern CDCA_BOOL  CDCASTB_IsCDCa(CDCA_U16 wCaSystemID);


/*------ Flash���� ------ */

/* �洢�ռ�ĸ�ʽ�� */
extern void CDCASTB_FormatBuffer( void );

/* ���ζԴ洢�ռ�Ķ�д���� */
extern void CDCASTB_RequestMaskBuffer(void);

/* �򿪶Դ洢�ռ�Ķ�д���� */
extern void CDCASTB_RequestUpdateBuffer(void);


/*------ TS������ ------*/

/* ����ECM�ͽ�Ŀ��Ϣ */
extern void CDCASTB_SetEcmPid( CDCA_U8 byType,
                               const SCDCASServiceInfo* pServiceInfo );

/* ����EMM��Ϣ */
extern void  CDCASTB_SetEmmPid(CDCA_U16 wEmmPid);

/* ˽�����ݽ��ջص� */
extern void CDCASTB_PrivateDataGot( CDCA_U8        byReqID,
								  	CDCA_BOOL      bTimeout,
									CDCA_U16       wPid,
									const CDCA_U8* pbyReceiveData,
									CDCA_U16       wLen            );

/*------- ���ܿ����� -------*/

/* �������ܿ� */
extern CDCA_BOOL CDCASTB_SCInsert( void );

/* �γ����ܿ�*/
extern void CDCASTB_SCRemove( void );

/* ��ȡ���ܿ��ⲿ���� */
extern CDCA_U16 CDCASTB_GetCardSN( char* pCardSN );

/* PIN����� */
extern CDCA_U16 CDCASTB_ChangePin( const CDCA_U8* pbyOldPin,
                                   const CDCA_U8* pbyNewPin);

/* �����û��ۿ����� */
extern CDCA_U16 CDCASTB_SetRating( const CDCA_U8* pbyPin,
                                   CDCA_U8 byRating );

/* ��ѯ�û��ۿ����� */
extern CDCA_U16 CDCASTB_GetRating( CDCA_U8* pbyRating );

/* �������ܿ�����ʱ�� */
extern CDCA_U16 CDCASTB_SetWorkTime( const CDCA_U8* pbyPin,
									 CDCA_U8        byStartHour,
									 CDCA_U8        byStartMin,
									 CDCA_U8        byStartSec,
									 CDCA_U8        byEndHour,
									 CDCA_U8        byEndMin,
									 CDCA_U8        byEndSec    );

/* ��ѯ���ܿ���ǰ����ʱ�� */
extern CDCA_U16 CDCASTB_GetWorkTime( CDCA_U8* pbyStartHour,
									 CDCA_U8* pbyStartMin,
									 CDCA_U8* pbyStartSec,
									 CDCA_U8* pbyEndHour,
									 CDCA_U8* pbyEndMin,
									 CDCA_U8* pbyEndSec   );

/*------- ������Ϣ��ѯ -------*/

/* ��ѯCA_LIB�汾�� */
extern CDCA_U32 CDCASTB_GetVer( void );

/* ��ѯ��Ӫ��ID�б� */
extern CDCA_U16 CDCASTB_GetOperatorIds( CDCA_U16* pwTVSID );

/* ��ѯ��Ӫ����Ϣ */
extern CDCA_U16 CDCASTB_GetOperatorInfo( CDCA_U16           wTVSID,
                                         SCDCAOperatorInfo* pOperatorInfo );

/* ��ѯ�û����� */
extern CDCA_U16 CDCASTB_GetACList( CDCA_U16 wTVSID, CDCA_U32* pACArray );

/* ��ѯǮ��ID�б� */
extern CDCA_U16 CDCASTB_GetSlotIDs( CDCA_U16 wTVSID, CDCA_U8* pbySlotID );

/* ��ѯǮ������ϸ��Ϣ */
extern CDCA_U16 CDCASTB_GetSlotInfo( CDCA_U16          wTVSID,
                                     CDCA_U8           bySlotID,
                                     SCDCATVSSlotInfo* pSlotInfo );

/* ��ѯ���ܿ�����״̬*/
/*pbyStatusֵ˵��:
    0: ����;
    ��0: ���� */
CDCA_U16 CDCASTB_GetCardFreezeStatus(CDCA_U16 wTvsID, CDCA_U8* pbyStatus);


/* ��ѯ��ͨ��Ȩ��Ŀ������� */
extern CDCA_U16 CDCASTB_GetServiceEntitles( CDCA_U16       wTVSID,
                                            SCDCAEntitles* pServiceEntitles );




/*CAͨ�ò�ѯ�ӿ�*/
/*
����˵��:
    byType  :��ѯ���͡�             
    pInData :�������ݵ�ַ�����ݿռ��ɵ����߷��䡣
    pOutData:������ݵ�ַ�����ݿռ��ɵ����߷��䡣
����ֵ��CDCA_RC_OK 	����ʾ�ɹ�
	    ����ֵ		����ʾʧ�ܣ�����鿴ͷ�ļ���"���ܵ��÷���ֵ����"
*/
extern CDCA_U16 CDCASTB_QuerySomething(CDCA_U8 byType,void * pInData, void * pOutData);


/*-- �߼�Ԥ����Ŀ�����¼��ѯ(�ڲ�������) --*/
/*
����˵����
	wTvsID	����Ӫ��ID
	pCurtainRecs	���߼�Ԥ����¼ָ�룬Ҫ�������СΪCDCA_MAXNUM_CURTAIN_RECORD��
	pNumber	�������Ч��¼��������pCurtainRecs[0]��ʼ�ĸ�����
	
����ֵ��CDCA_RC_OK 	����ʾ�ɹ�
	    ����ֵ		����ʾʧ�ܣ�����鿴ͷ�ļ���"���ܵ��÷���ֵ����"
	
*/
extern CDCA_U16 CDCASTB_GetCurtainRecords( CDCA_U16 wTvsID,
                                SCDCACurtainInfo* pCurtainRecs,
                                CDCA_U8* pNumber);


/*-------- ��Ȩ��Ϣ���� --------*/

/* ��ѯ��ȨID�б� */
extern CDCA_U16 CDCASTB_GetEntitleIDs( CDCA_U16  wTVSID,
                                       CDCA_U32* pdwEntitleIds );

/* ��ѯ����Ȩȷ���� */
extern CDCA_U16 CDCASTB_GetDetitleChkNums( CDCA_U16   wTvsID,
                                           CDCA_BOOL* bReadFlag,
                                           CDCA_U32*  pdwDetitleChkNums);

/* ��ѯ����Ȩ��Ϣ��ȡ״̬ */
extern CDCA_BOOL CDCASTB_GetDetitleReaded( CDCA_U16 wTvsID );

/* ɾ������Ȩȷ���� */
extern CDCA_BOOL CDCASTB_DelDetitleChkNum( CDCA_U16 wTvsID,
                                           CDCA_U32 dwDetitleChkNum );

/*------- ������Ӧ -------*/

/* ��ѯ������Ӧ��� */
extern CDCA_U16 CDCASTB_IsPaired( CDCA_U8* pbyNum,
                                  CDCA_U8* pbySTBID_List );
/* ��ѯ������ƽ̨��� */
extern CDCA_U16 CDCASTB_GetPlatformID( void );

/* ��ѯ�ն����ͱ�� */
extern CDCA_U16 CDCASTB_GetTerminalTypeID( void );

/*-------- IPPVӦ�� -------*/

/* IPPV��Ŀ���� */
extern CDCA_U16 CDCASTB_StopIPPVBuyDlg( CDCA_BOOL       bBuyProgram,
                                        CDCA_U16        wEcmPid,
                                        const CDCA_U8*  pbyPinCode,
                                        const SCDCAIPPVPrice* pPrice );

/* IPPV��Ŀ���������ѯ */
extern CDCA_U16 CDCASTB_GetIPPVProgram( CDCA_U16       wTvsID,
                                        SCDCAIppvInfo* pIppv,
                                        CDCA_U16*      pwNumber );


/*-------- �ʼ����� --------*/

/* ��ѯ�ʼ�ͷ��Ϣ */
extern CDCA_U16 CDCASTB_GetEmailHeads( SCDCAEmailHead* pEmailHead,
                                       CDCA_U8*        pbyCount,
                                       CDCA_U8*        pbyFromIndex );

/* ��ѯָ���ʼ���ͷ��Ϣ */
extern CDCA_U16 CDCASTB_GetEmailHead( CDCA_U32        dwEmailID,
                                      SCDCAEmailHead* pEmailHead );

/* ��ѯָ���ʼ������� */
extern CDCA_U16 CDCASTB_GetEmailContent( CDCA_U32           dwEmailID,
                                         SCDCAEmailContent* pEmailContent );

/* ɾ���ʼ� */
extern void CDCASTB_DelEmail( CDCA_U32 dwEmailID );

/* ��ѯ����ʹ����� */
extern CDCA_U16 CDCASTB_GetEmailSpaceInfo( CDCA_U8* pbyEmailNum,
                                           CDCA_U8* pbyEmptyNum );


/*-------- ��ĸ��Ӧ�� --------*/

/* ��ȡ��ĸ����Ϣ */
extern CDCA_U16 CDCASTB_GetOperatorChildStatus( CDCA_U16   wTVSID,
                                                CDCA_U8*   pbyIsChild,
                                                CDCA_U16*   pwDelayTime,
                                                CDCA_TIME* pLastFeedTime,
                                                char*      pParentCardSN,
                                                CDCA_BOOL *pbIsCanFeed );

/* ��ȡĸ��ι������ */
extern CDCA_U16 CDCASTB_ReadFeedDataFromParent( CDCA_U16 wTVSID,
                                                CDCA_U8* pbyFeedData,
                                                CDCA_U8* pbyLen     );

/* ι������д���ӿ� */
extern CDCA_U16 CDCASTB_WriteFeedDataToChild( CDCA_U16       wTVSID,
                                              const CDCA_U8* pbyFeedData,
                                              CDCA_U8        byLen    );


/*-------- ��ʾ������� --------*/

/* ˢ�½��� */
extern void CDCASTB_RefreshInterface( void );


/*-------- ˫��ģ��ӿ� -------*/
    

/*------------------------���Ͻӿ���CA_LIB�ṩ��STB---------------------------*/

/******************************************************************************/

/*------------------------���½ӿ���STB�ṩ��CA_LIB---------------------------*/

/*-------- �̹߳��� --------*/

/* ע������ */
extern CDCA_BOOL CDSTBCA_RegisterTask( const char* szName,
                                       CDCA_U8     byPriority,
                                       void*       pTaskFun,
                                       void*       pParam,
                                       CDCA_U16    wStackSize  );

/* �̹߳��� */
extern void CDSTBCA_Sleep(CDCA_U16 wMilliSeconds);


/*-------- �ź������� --------*/

/* ��ʼ���ź��� */
extern void CDSTBCA_SemaphoreInit( CDCA_Semaphore* pSemaphore,
                                   CDCA_BOOL       bInitVal );

/* �ź��������ź� */
extern void CDSTBCA_SemaphoreSignal( CDCA_Semaphore* pSemaphore );

/* �ź�����ȡ�ź� */
extern void CDSTBCA_SemaphoreWait( CDCA_Semaphore* pSemaphore );


/*-------- �ڴ���� --------*/

/* �����ڴ� */
extern void* CDSTBCA_Malloc( CDCA_U32 byBufSize );

/* �ͷ��ڴ� */
extern void  CDSTBCA_Free( void* pBuf );

/* �ڴ渳ֵ */
extern void  CDSTBCA_Memset( void*    pDestBuf,
                             CDCA_U8  c,
                             CDCA_U32 wSize );

/* �ڴ渴�� */
extern void  CDSTBCA_Memcpy( void*       pDestBuf,
                             const void* pSrcBuf,
                             CDCA_U32    wSize );


/*--------- �洢�ռ䣨Flash������ ---------*/

/* ��ȡ�洢�ռ� */
extern void CDSTBCA_ReadBuffer( CDCA_U8   byBlockID,
                                CDCA_U8*  pbyData,
                                CDCA_U32* pdwLen );

/* д��洢�ռ� */
extern void CDSTBCA_WriteBuffer( CDCA_U8        byBlockID,
                                 const CDCA_U8* pbyData,
                                 CDCA_U32       dwLen );


/*-------- TS������ --------*/

/* ����˽�����ݹ����� */
extern CDCA_BOOL CDSTBCA_SetPrivateDataFilter( CDCA_U8        byReqID,  
											   const CDCA_U8* pbyFilter,  
											   const CDCA_U8* pbyMask, 
											   CDCA_U8        byLen, 
											   CDCA_U16       wPid, 
											   CDCA_U8        byWaitSeconds );


/* �ͷ�˽�����ݹ����� */
extern void CDSTBCA_ReleasePrivateDataFilter( CDCA_U8  byReqID,
                                              CDCA_U16 wPid );
/* ����CW�������� */
extern void CDSTBCA_ScrSetCW( CDCA_U16       wEcmPID,  
							  const CDCA_U8* pbyOddKey,  
							  const CDCA_U8* pbyEvenKey, 
							  CDCA_U8        byKeyLen, 
							  CDCA_BOOL      bTapingEnabled );


/*--------- ���ܿ����� ---------*/

/* ���ܿ���λ */
extern CDCA_BOOL CDSTBCA_SCReset( CDCA_U8* pbyATR, CDCA_U8* pbyLen );

/* ���ܿ�ͨѶ */
extern CDCA_BOOL CDSTBCA_SCPBRun( const CDCA_U8* pbyCommand, 
								  CDCA_U16       wCommandLen,  
								  CDCA_U8*       pbyReply,  
								  CDCA_U16*      pwReplyLen  );


/*-------- ��Ȩ��Ϣ���� -------*/

/* ֪ͨ��Ȩ�仯 */
extern void CDSTBCA_EntitleChanged( CDCA_U16 wTvsID );


/* ����Ȩȷ����֪ͨ */
extern void CDSTBCA_DetitleReceived( CDCA_U8 bstatus );



/*-------- ��ȫ���� --------*/

/* ��ȡ������Ψһ��� */
extern void CDSTBCA_GetSTBID( CDCA_U16* pwPlatformID,
                              CDCA_U32* pdwUniqueID);

/* ��ȫоƬ�ӿ� */
extern CDCA_U16 CDSTBCA_SCFunction( CDCA_U8* pData);

/*-------- IPPVӦ�� -------*/

/* IPPV��Ŀ֪ͨ */
extern void CDSTBCA_StartIppvBuyDlg( CDCA_U8                 byMessageType,
                                     CDCA_U16                wEcmPid,
                                     const SCDCAIppvBuyInfo* pIppvProgram  );

/* ����IPPV�Ի��� */
extern void CDSTBCA_HideIPPVDlg(CDCA_U16 wEcmPid);

/*------- �ʼ�/OSD��ʾ���� -------*/

/* �ʼ�֪ͨ */
extern void CDSTBCA_EmailNotifyIcon( CDCA_U8 byShow, CDCA_U32 dwEmailID );

/* ��ʾ��ͨOSD��Ϣ */
extern void CDSTBCA_ShowOSDMessage( CDCA_U8     byStyle,
                                    const char* szMessage );

/* ����OSD��Ϣ*/
extern void CDSTBCA_HideOSDMessage( CDCA_U8 byStyle );

/*��ʾ����OSD��Ϣ�� ע�⣺pOSDInfoΪNULL����ʾȡ��OSD��ʾ*/
extern void  CDSTBCA_ShowOSDInfo(SCDCAOSDInfo *pOSDInfo);


/*-------- ��ĸ��Ӧ�� --------*/

/* ������ʾ��ȡι�����ݽ�� */
extern void  CDSTBCA_RequestFeeding( CDCA_BOOL bReadStatus );



/*-------- ǿ���л�Ƶ�� --------*/

/* Ƶ������ */
extern void CDSTBCA_LockService( const SCDCALockService* pLockService );

/* ���Ƶ������ */
extern void CDSTBCA_UNLockService( void );


/*-------- ��ʾ������� --------*/

/* ���������տ���Ŀ����ʾ */
/*wEcmPID==0��ʾ��wEcmPID�޹ص���Ϣ���Ҳ��ܱ�������Ϣ����*/
extern void CDSTBCA_ShowBuyMessage( CDCA_U16 wEcmPID,
                                    CDCA_U8  byMessageType );


/* ָ����ʾ, pFingerInfo=NULL ��ʾȡ��ָ��*/
extern void CDSTBCA_ShowFingerInfo ( CDCA_U16 wEcmPID,
                         const SCDCAFingerInfo* pFingerInfo );

/* ָ����ʾ��fingerMsg=NULL ��ʾȡ��ָ��*/
extern void CDSTBCA_ShowFingerMessageExt( CDCA_U16 wEcmPID,
                                      char* fingerMsg );

/* ֪ͨ��������ʾ��رո߼�Ԥ��*/
extern void  CDSTBCA_ShowCurtainNotify( CDCA_U16 wEcmPID,
                                    CDCA_U16  wCurtainCode);

/* ������ʾ */
extern void CDSTBCA_ShowProgressStrip( CDCA_U8 byProgress,
                                       CDCA_U8 byMark );

/*--------- ������֪ͨ --------*/

/* ������֪ͨ */
extern void  CDSTBCA_ActionRequest( CDCA_U16 wTVSID,
                                    CDCA_U8  byActionType );


/*--------- ˫��ģ��ӿ� --------*/

/*--------- �����ۿ�����֪ͨ --------*/
/*byType=0��ʾ���������ۿ�����,
 byType=1��ʾ���������ۿ�����,��ʱ��wWorkTime(��������ʱ�䣬��λ:Сʱ)��wStopTime(ֹͣʱ�䣬��λ:����)����Ҫ���ø������С�
 byType=2��ʾȡ����ǰ�����ۿ�����*/
extern void CDSTBCA_ContinuesWatchLimit(CDCA_U8 byType, CDCA_U16 wWorkTime, CDCA_U16 wStopTime);

/*--------- ��ȡ��ǰʱ�� --------*/
/*��ȡ�ɹ�����CDCA_TRUE,��ȡʧ�ܷ���CDCA_FALSE*/
//The bias is the difference, in minutes, between Coordinated Universal Time (UTC) and local time, UTC = local time + bias
extern CDCA_BOOL  CDSTBCA_GetCurrentTime( CDCA_S32  *pUTCTime, CDCA_S32 *pTimeZoneBias, CDCA_S32 *pDaylightBias,  CDCA_BOOL* pInDaylight);







/*-------- ���� --------*/

/* ��ȡ�ַ������� */
extern CDCA_U16 CDSTBCA_Strlen(const char* pString );

/* ������Ϣ��� */
extern void CDSTBCA_Printf(CDCA_U8 byLevel, const char* szMesssage );


/*---------------------------���Ͻӿ���STB�ṩ��CA_LIB------------------------*/

#ifdef  __cplusplus
}
#endif
#endif
/*EOF*/

