#ifndef _GS_CA_PROCESS_H_
#define _GS_CA_PROCESS_H_

/* ������ */
//��ȷ
/*!
  abc
  */
#define GS_ERR_OK                      0x00
//EMM�����п��Ŵ���
 /*!
  abc
  */
#define GS_ERR_INVALID_ADDRESS         0x01
//����CRC����
 /*!
  abc
  */
#define GS_ERR_CRC                     0x02
//��δ��ʼ��
 /*!
  abc
  */
#define GS_ERR_NOT_INI                 0x03
//�������������
 /*!
  abc
  */
#define GS_ERR_INVALID_TYPE            0x04
//ECM����
 /*!
  abc
  */
#define GS_ERR_ECM                     0x05
//δ֪����
 /*!
  abc
  */
#define GS_ERR_UNKNOWN                 0x06
//����̫��
 /*!
  abc
  */
#define GS_ERR_DATA_LENGTH             0x07
//�Ѵ�����������ݣ������ECM����������CW
 /*!
  abc
  */
#define GS_ERR_OLD_DATA                0x08
//ͨѶ����
 /*!
  abc
  */
#define GS_ERR_COMM                    128

//�����
 /*!
  abc
  */
#define GS_CLA                            0xE5
//ָ����
 /*!
  abc
  */
#define GS_INS_GET_CA_SYSTEM_ID		      0x22
 /*!
  abc
  */
#define GS_INS_GET_SMARTCARD_ID		      0x24
 /*!
  abc
  */
#define GS_INS_GET_CARD_NO			          0x26
 /*!
  abc
  */
#define GS_INS_GET_CARD_ID			          0x28
 /*!
  abc
  */
#define GS_INS_GET_AREA_CODE		          0x2A
 /*!
  abc
  */
#define GS_INS_CHECK_SMART			          0x7C
 /*!
  abc
  */
#define GS_INS_PROCESS_ECM			          0x7E
 /*!
  abc
  */
#define GS_INS_PROCESS_EMM			          0x36
 /*!
  abc
  */
#define GS_INS_GET_STB_SN			          0x38
 /*!
  abc
  */
#define GS_INS_GET_VERSION_INFO		      0x3A
 /*!
  abc
  */
#define GS_INS_GET_SUBSCRIBER_INFO		      0x3C
 /*!
  abc
  */
#define GS_INS_GET_EXPIRE_DATE 		      0x2C
 /*!
  abc
  */
#define GS_INS_GET_CHILD_AUTH_INFO           0x46
 /*!
  abc
  */
#define GS_INS_SET_CHILD_AUTH_INFO           0x48
 /*!
  abc
  */
#define GS_INS_READ_CHILD_AUTH_INFO          0x4a
 /*!
  abc
  */
#define GS_INS_GET_CARD_TYPE                 0x4c
 /*!
  abc
  */
#define GS_INS_GET_STAGE_DATA                0x82
 /*!
  abc
  */
#define GS_INS_CHECK_SMARTCARD_APDU          0x84
 /*!
  abc
  */
#define GS_INS_PROCESS_ECM_APDU              0x86
//������
 /*!
  abc
  */
#define GS_PARA_DEFAULT                      0

//����ֵ
 /*!
  abc
  */
#define GS_NO_SW1SW2                         0x0000
 /*!
  abc
  */
#define GS_SW1SW2_OK                         0x9000
 /*!
  abc
  */
#define GS_SW1SW2_MEMORY_ERROR               0x6581
 /*!
  abc
  */
#define GS_SW1SW2_WRONG_LC_LENGTH            0x6700
 /*!
  abc
  */
#define GS_SW1SW2_INCORRECT_DATA             0x6A80
 /*!
  abc
  */
#define GS_SW1SW2_INCORRECT_PARA             0x6A86
 /*!
  abc
  */
#define GS_SW1SW2_WRONG_LE_LENGTH            0x6C00
 /*!
  abc
  */
#define GS_SW1SW2_INS_NOT_SUPPORTED          0x6D00
 /*!
  abc
  */
#define GS_SW1SW2_CLA_NOT_SUPPORTED          0x6E00
 /*!
  abc
  */
#define GS_SW1SW2_GENERAL_ERROR              0x6F00
 /*!
  abc
  */
#define GS_SW1SW2_EXPIRED                    0x6FA0
 /*!
  abc
  */
#define GS_SW1SW2_STB_SN_NOT_MATCH           0x7000
 /*!
  abc
  */
#define GS_SW1SW2_CARD_NOT_INIT              0x7001
 /*!
  abc
  */
#define GS_SW1SW2_INCORRECT_SUPER_CAS_ID     0x7002
 /*!
  abc
  */
#define GS_SW1SW2_INCORRECT_SMARTCARD_ID     0x7003
 /*!
  abc
  */
#define GS_SW1SW2_PPV_EXCEED                 0x7004
 /*!
  abc
  */
#define GS_SW1SW2_NOT_PAIRED                 0x7005
 /*!
  abc
  */
#define GS_SW1SW2_ECM_NOT_AUTHORIZED         0x7006
 /*!
  abc
  */
#define GS_SW1SW2_LIMIT_AREA                 0x7007
 /*!
  abc
  */
#define GS_SW1SW2_LIMIT_CARD                 0x7008
 /*!
  abc
  */
#define GS_SW1SW2_NOT_CHILD_MODE             0x7009
 /*!
  abc
  */
#define GS_SW1SW2_NO_CHILD_AUTH              0x700a
 /*!
  abc
  */
#define GS_SW1SW2_CHILD_NOT_AUTHORIZED       0x700b
 /*!
  abc
  */
#define GS_SW1SW2_CHILD_AUTH_TIMEOUT         0x700c
 /*!
  abc
  */
#define GS_SW1SW2_CHILD_AUTH_EXPIRED         0x700d

 /*!
  abc
  */
typedef enum
{
/*!
    ABC
  */
  SC_REMOVE_START,
  /*!
    ABC
  */
  SC_REMOVE_OVER,
  /*!
    ABC
  */
  SC_INSERT_START,
    /*!
    ABC
  */
  SC_INSERT_OVER,
  /*!
    ABC
  */
  SC_STATUS_END
}e_smartcard_insert_t;

/*!
  abc
  */
typedef struct
{
/*!
  abc
  */
  unsigned char mail_id;
/*!
  abc
  */
  unsigned char year_high_byte;
/*!
  abc
  */
  unsigned char year_low_byte;
/*!
  abc
  */
  unsigned char month;
/*!
  abc
  */
  unsigned char day;
/*!
  abc
  */
  unsigned char hour;
/*!
  abc
  */
  unsigned char minute;
/*!
  abc
  */
  unsigned char second;
/*!
  abc
  */
  unsigned char sender_length;//������
/*!
  abc
  */
  unsigned char sender[256];
/*!
  abc
  */
  unsigned char caption_length;//����
/*!
  abc
  */
  unsigned char caption[256];
/*!
  abc
  */
  unsigned char content_length;//����
/*!
  abc
  */
  unsigned char content[256];
}gs_mail_t;

/* ��ʱ��������λΪms */
 /*!
  abc
  */
void GS_delay(int ms);
 /*!
  abc
  */
unsigned short GS_crc16ccitt(unsigned char *pBuffer, unsigned long uBufSize);

/*APDU �������*/
//APDU ��Ҫ�� CLA, INS, P1, P2, [lc], [data], [le]���, ���Է�Ϊ����4�����:
//1��û�����ݣ���Ҳû�з�������
 /*!
  abc
  */
unsigned char GS_apdu_package_v1(
    unsigned char *cache,
    unsigned char cla,
    unsigned char ins,
    unsigned char p1,
    unsigned char p2
    );
//2��û�����ݣ������з�������
 /*!
  abc
  */
unsigned char GS_apdu_package_v2(
    unsigned char *cache,
    unsigned char cla,
    unsigned char ins,
    unsigned char p1,
    unsigned char p2,
    unsigned char le
    );

//3�������ݣ�û�з�������
 /*!
  abc
  */
unsigned char GS_apdu_package_v3(
    unsigned char *cache,
    unsigned char cla,
    unsigned char ins,
    unsigned char p1,
    unsigned char p2,
    unsigned char lc,
    unsigned char *input_data
    );
//4�������ݣ�Ҳ�з�������,���ڲ�����le�ֽڣ��������ַ�ʽһ��
 /*!
  abc
  */
unsigned char GS_apdu_package_v4(
    unsigned char *cache,
    unsigned char cla,
    unsigned char ins,
    unsigned char p1,
    unsigned char p2,
    unsigned char lc,
    unsigned char *input_data
    );
 /*!
  abc
  */
void GS_set_cw(u8 *cw);
 /*!
  abc
  */
char GS_memncmp(unsigned char *buf1, unsigned char *buf2, unsigned char length);
#endif
