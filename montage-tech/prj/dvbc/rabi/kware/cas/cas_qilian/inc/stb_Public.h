// 2011 0801 LHCAS VER0.2
// LHCAS使用的基本变量类型定义，宏参数。

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
/* 信号量定义 */
typedef long  CA_Semaphore ;
/*  线程函数地址类型  */
typedef void (*pThreadFunc) (void);
// 消息类型
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

//下面是长度值定义     
#define AD_PROMPTOSD_MAXLEN			256
#define EMail_TIME						5
#define	EMail_Title_MAXLEN		50
#define	Email_Content_MAXLEN	300
#define	EMail_MAXNUM					40
#define CAMENU_PRODUCT_MAX					20
#define CAMENU_PRODUCTNAME_MAXLEN		40
/*----------------------------------------------------------------------*/
/*                       下面是OSD 显示的消息类型值定义                               */
/*----------------------------------------------------------------------*/
#define STBCA_NO_ERROR												0	
// 智能卡已获取授权，  或者智能卡插入后被正确识别   ,CA OSD 无显示
#define STBCA_ERROR_InitCardCommand								1   //智能卡初始化错误
#define STBCA_ERROR_ReadCardCommand								2	// 智能卡无法识别
#define STBCA_ERROR_NO_CARD												3	//请检查智能卡
#define STBCA_ERROR_EMM_RET												4  // 无效的EMM 数据
#define STBCA_ERROR_ECM_RET												5	//无效的ECM数据
#define STBCA_ECM_RET_READCOMM_ERROR								6
#define STBCA_EMMECM_RET_P1P2_ERROR								7    //无效指令
#define STBCA_EMMECM_RET_P3_ERROR									8	// 数据格式无效
#define STBCA_EMMECM_RET_DES_ERROR									9
#define STBCA_ECM_RET_NOENTITLE_ERROR							10			//当前服务没有授权
#define STBCA_EMMECM_RET_STORE_ERROR								11
#define STBCA_EMMECM_RET_CHECK_ERROR								12			// CRC   错误,当前节目没有授权
#define STBCA_EMM_NOKEY_OR_NOFILE_ERROR						13
#define STBCA_ECM_OUTDATE_ERROR										14
#define STBCA_EMMECM_RET_UNKNOW_ERROR				  		15
#define STBCA_NO_ECM_INFO												16
#define STBCA_ZONE_FORBID_ERROR										17  // 区域限播无效
#define STBCA_UserCdn_Limit_Ctrl_Enable									18			// 条件禁止播放
#define STBCA_UserCdn_Limit_Ctrl_Disable									19			// 条件禁播无效
#define STBCA_ZoneCdn_Limit_Ctrl_Enable									20			// 条件禁止播放
#define STBCA_ZoneCdn_Limit_Ctrl_Disable								21			// 条件禁播无效


//机卡配对
#define STBCA_CARD_NEED_PAIRING										22   // 没有机卡配对
#define STATUE_CA_SET_PAIR												23			//  is pairing            正在设置配对信息，请等待
#define STBCA_CARD_PAIRING_OK											24	// 机卡配对成功，清除31、16  24 25的消息
#define STBCA_PAIRED_OtherCARD_ERROR									25 /*该机器与其它卡配对*/
#define STBCA_PAIRED_OtherMachine_ERROR								26 /*该卡与此机器不配对*/

//字母卡配对STATUE_CA_TIME_VAILD
#define	STATUE_CA_CHILD_SC									27 //字母卡等待配对
#define STATUE_CA_TIME_VAILD                                         			28// 字母卡配对时间已更新
#define STATUE_CA_NEED_PAIR									29  //"已过有效期,请把卡插入主机配对"
//漫游
#define STATUE_CA_CARD_ZONE_INVALID							30		//您的智能卡不再服务区
#define  STATUE_CA_CARD_ZONE_CHECK_ENTER                 			31                    /* 正在寻找网络，请等待*/
#define  STATUE_CA_CARD_ZONE_CHEKC_OK                       			32         // 当stb得到该消息后，区域检测完毕，
				//			强制高频头锁定当前节目频率,播放当前节目。

#define  STATUE_CA_OSD_CLS                     			0XFF  //无caosd 提示

/*----------------------------------------------------------------------*/
/*                       下面是一些数据结构定义                         */
/*----------------------------------------------------------------------*/

typedef struct tag_mail_Data_struct{
	BU8		 	MessageFlag; 	/*	0 -- 不需要存储 1 -- 需要存储 2 -- 未阅读 3-- 已经阅读 4 -- 删除 0xff 没有初始化*/
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
		// 兼容老的版本，0为到期提示类型
	MSG_DISPLAY_ICON=1,	
		//有新邮件的图标显示
	MSG_POP_TEXT=2,	
		//弹出显示框，需要手动清除
	MSG_DISPLAY_MAILBOX =4,
		//弹出邮箱菜单
	MSG_SCROLL_TOP = 7,
		//在屏幕顶部滚动字符
	MSG_SCROLL_BOTTOM=8,
		//在屏幕底部滚动字符
	MSG_POP_TOP=14,
		//在屏幕顶部显示字符
	MSG_POP_BOTTOM=15
	   //在屏幕底部显示字符
}CAMSG_Display_Type;

#ifdef  __cplusplus
}
#endif

#endif	
