//本文件定义要求机顶盒实现并提供给CA模块使用的接口。
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
/*----------------------------------以下接口是STB提供给CAS调用--------------------------------------------*/
/*++
功能：申请机顶盒创建num个消息栈，每个消息栈最多有20条消息排队。一般后进先出原则。
参数：
	BU8  MSGQUE1;
	STBCA_MSGQUENE_Init(2);
返回:
    1:  执行成功；
    0:  失败
--*/
BU8 STBCA_MSGQUENE_Init(BU8 num);
/*++
功能：从第NO个消息队列中获取头条消息，非阻塞型。
参数：如果队列为空，则  参数msg 为空
返回:    成功获取到消息，队列当前消息为下条消息内容。
	CA_MSG *msg1= NULL;
	STBCA_GETCAMSG(0,msg1);
	if(msg1 != NULL )
	{
		//从第一个消息队列中获取并  分析消息
	}
	CA_MSG *msg2= NULL;
	STBCA_GETCAMSG(1,msg2);
	if(msg2 != NULL )
	{
		//从第二个消息队列中获取并  分析消息
	}
--*/
//void STBCA_GETCAMSG(BU8 NO,CA_MSG* msg);
CA_MSG* STBCA_GETCAMSG(BU8 NO);
/*++
功能：在第NO个消息队列中增加一条消息，如果队列满则删除所有的消息后增加消息。
参数：
	static CA_MSG msg1;
	msg1.msgtype = 1;
	msg1.msgparm1 = 0x33;
	msg1.msgparm2 = 0x44;
	
	STBCA_SENDCAMSG(0,&msg1);
	STBCA_SENDCAMSG(1,&msg1);
--*/
void STBCA_SENDCAMSG(BU8 NO,CA_MSG* msg);
//这个接口暂时不用。
void STBCA_ReleaseCAMSG(BU8 NO,CA_MSG* msg);

/*++
功能：挂起当前线程。
参数：
	dwMicroseconds:			毫秒。
--*/
void STBCA_Sleep( int dwMicroseconds );

/*++
功能：得到一个随机8bit数据，0--255之间。
参数：
	dwMicroseconds:			毫秒。
--*/
BU8 STBCA_RAND_GET( void );

/*++
功能：初始化信号量。
参数：
	pSemaphore:				指向信号量的指针。
	flag 为1表示初始后的信号有个信号等待。0则表示没。
返回:
 1: 成功执行
 0: 表示执行出错
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
功能：给信号量加信号。
参数：
	pSemaphore: 			指向信号量的指针。
返回:
   	
--*/
void STBCA_SemaphoreSignal( CA_Semaphore *pSemaphore );

/*++
功能：等待信号量,等待成功之后,信号量为无信号。
参数：
	pSemaphore: 			指向信号量的指针。
返回:
   同上		
--*/
void STBCA_SemaphoreWait( CA_Semaphore *pSemaphore );

/*++
功能：CAS模块向机顶盒注册线程。
参数：
	szName:					注册的线程名称。
	pTaskFun:				要注册的线程函数。
返回:
   1: 成功执行
 0: 表示执行出错
--*/
BU8	STBCA_RegisterTask(const char* szName, pThreadFunc pTaskFun);

/*++
功能：获得机顶盒分配给CAS的存储空间的起点地址和大小（以字节为单位）。
参数：
	ppStartAddr:			机顶盒分配给CAS的存储空间的开始地址。
	lSize:					输出机顶盒分配给CAS的储存空间的大小。
--*/
void STBCA_GetDataBufferAddr(BU32* lSize, BU32* ppStartAddr);

/*++
功能：读取保存在机顶盒中的信息。
参数：
	pStartAddr:				要读取的存储空间的开始地址。
	plDataLen:				输入为要读取的最长数据值；输出为实际读取的大小。
	pData:					存放输出数据。
返回:
   1: 成功执行
   0: 表示执行出错
--*/
BU8 STBCA_ReadDataBuffer(BU32* pStartAddr, BU32* plDataLen, BYTE* pData);

/*++
功能：向机顶盒的存储空间写信息。
参数：
	lStartAddr:				要写的存储空间的开始地址。
	plDataLen:				输入为要写的数据的长度；输出为写入的实际长度。
	pData:					要写的数据。
返回:
   1: 成功执行
   0: 表示执行出错
--*/
BU8 STBCA_WriteDataBuffer(BU32 pStartAddr, BU32 plDataLen, const BYTE* pData);

/*++
功能：设置过滤器接收CAS私有数据。
参数：
	ReqID:				请求收表的table id。机顶盒应该保存该数据，接收到CAS私有数据后调用STBCA_StreamGuardDataGot()处理。ecm的tableid有两种0x82、0x83
	wPID:				需要过滤的流的PID。
	szFilter:				过滤器的值，为一个8个字节的数组。
	szMask:					过滤器的掩码，为一个8个字节的数组。
	u8Len:					过滤器的长度，为8。
	nWaitSeconds:			收此表最长的等待时间，如果为0则无限等待，单位秒。
返回:
   同上		
	
--*/
BU8 STBCA_SetStreamGuardFilter(BYTE ReqID, WORD wPID, const BYTE* szFilter, const BYTE* szMask,BYTE u8Len, int nWaitSeconds);


/*++
功能：打印调试信息。
参数：
	pszMsg:					调试信息内容。
--*/
void STBCA_AddDebugMsg(const char *pszMsg);

/*++
	功能: 通知菜单，显示邮件图标
	 参数
	 0,表示显示新邮件图标
	 1，表示显示邮箱已满
	 2, 表示没有新邮件图标
*/
void STBCA_EmailNotify(BU8 type);
/*++
功能：返回智能卡当前状态。
返回值：0表示没卡插入；1表示有卡	
--*/
BU8 STBCA_GetSCStatus(void );

/*++
功能：ca库申请复位智能卡。
--*/
void STBCA_SMARTCARD_ReSet(void);
/*++
功能：实现与智能卡之间的通讯。采用7816标准。
参数：
	num:			通讯编号，无实际意义。
	bSendData:			待发送的一串命令字节。
	bReplyLen:			发送时，是指定数据长度；读数据时读取的数据长度。
	bReplyData:         		返回的数据串.
返回值   :
	0   : 物理通讯失败
	1   : 成功
--*/
BU8 STBCA_SCAPDU(BU8 num, BYTE* bSendData,BU32* bReplyLen, BYTE* bReplyData);



/*
功能：显示用户不能观看节目等提示信息，下面两个函数对应使用。
参数：	
	b8MesageNo:				OSD 要显示的消息类型
--*/
void   STBCA_ShowOSDPromptMsg(BYTE b8MesageNo);


/*--------------------------- 类型2的界面：用户可以取消显示的界面 -----------------------*/
/*
功能：显示OSD消息，可以是同步显示，也可以是异步显示。注意到期提示要优先显示
参数:	
	szOSD:					用于显示的OSD字符。
	wDuration:			为0时为到期提示。
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
说明：
	用户可以通过遥控器取消当前OSD显示。
	gx方案的消息:
		如果到期提示在显示，其它osd消息不可以覆盖。
		如果用户退出到期消息，方可显示其它OSD消息。
		如果其它消息在显示，收到到期提示消息后，要及时显示到期提示内容。
--*/
void   STBCA_ShowOSDTXTMsg(const char* szOSD, CAMSG_Display_Type wDuration);

/*++
功能：显示指纹信息，以图标方式显示指纹,显示位置一般和邮件其它图标不重叠
参数：szOSD == NULL 时清除屏幕已显示的消息。
--*/
void STBCA_FingerSHOW(const char* szOSD);
/*++
功能:     设置当前节目的解扰密钥
参数：节目serverID
--*/

void STBCA_ScrSetCW( BU16  serverID, const BU8* pb8OddKey, const BU8* pb8EvenKey,BU8 KeyLen);

/*
功能：在特定的频点寻找区域信息。
参数:	区域调制器的频率KHZ;
返回:  1，表示频点已锁到，否则未锁定该频点。
注意:  信号锁定后，此时ca会申请过滤BAT数据。
--*/
BU8 STBCA_SetZoneFreq( BU32 Freq );

#ifdef  __cplusplus
}
#endif
#endif  
/***********************


******************************/
