#ifndef BYCASTB_CA_H_2013_02_20
#define BYCASTB_CA_H_2013_02_20

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "BYCApubtype.h"

/*------------------------以下接口是STB提供给BYCA调用-----------------------------*/

/*++
功能：返回智能卡当前状态。
参数：pbyStatus：	返回智能卡状态，为BYCAS_SC_OUT、BYCAS_SC_IN两种之一。
--*/
BVOID BYSTBCA_GetSCStatus(BU8 * pbyStatus);

/*++
功能：获得机顶盒分配给CAS的存储空间的起点地址和大小（以字节为单位）。
参数：
	ppStartAddr:			机顶盒分配给CAS的存储空间的开始地址。
	lSize:					输出机顶盒分配给CAS的储存空间的大小。
--*/
BU8 BYSTBCA_GetDataBufferAddr(BU32* lSize,BU32 * ppStartAddr);

/*++
功能：读取保存在机顶盒中的信息。
参数：
	pStartAddr:				要读取的存储空间的开始地址。
	plDataLen:				输入为要读取的最长数据值；输出为实际读取的大小。
	pData:					存放输出数据。
--*/
BU8 BYSTBCA_ReadDataBuffer(BU32 pStartAddr,BU32* plDataLen,BU8* pData);

/*++
功能：向机顶盒的存储空间写信息。
参数：
	lStartAddr:				要写的存储空间的开始地址。
	plDataLen:				输入为要写的数据的长度；输出为写入的实际长度。
	pData:					要写的数据。
--*/
BU8 BYSTBCA_WriteDataBuffer(BU32 pStartAddr,BU32 DataLen,BU8* pData);

/*++
功能：挂起当前线程。
参数：
	dwmsec:			毫秒。
--*/
BU8 BYSTBCA_Sleep(BU32 dwmsec);

/*++
功能：初始化信号量,初始化后信号量没有信号。
参数：pSemaphore:				指向信号量的指针。
	    flag:						  初始化信号量值
--*/
BU8 BYSTBCA_SemaphoreInit(BY_Semaphore *pSemaphore,BU8 flag);

/*++
功能：释放一个信号量。
参数：pSemaphore: 			指向信号量的指针。
--*/
BU8 BYSTBCA_SemaphoreDestory(BY_Semaphore *pSemaphore );

/*++
功能：给信号量加信号。
参数：pSemaphore: 			指向信号量的指针。
--*/
BU8 BYSTBCA_SemaphoreSignal(BY_Semaphore *pSemaphore );

/*++
功能：等待信号量,等待成功之后,信号量为无信号。
参数：pSemaphore: 			指向信号量的指针。
--*/
BU8 BYSTBCA_SemaphoreWait(BY_Semaphore *pSemaphore );

/*++
功能：CAS模块向机顶盒注册线程。
参数：
	szName:					注册的线程名称。
	pTaskFun:				要注册的线程函数。
--*/
BU8 BYSTBCA_RegisterTask(const BS8* szName,pThreadFunc pTaskFun);

/*++
功能：设置过滤器接收CAS私有数据。
参数：
	byReqID:				请求收表的请求号。机顶盒应该保存该数据，接收到CAS私有数据后调用BYCASTB_StreamGuardDataGot()时应该将该数据返回。
	wEcmPID:				需要过滤的流的PID。
	szFilter:				过滤器的值，为一个8个字节的数组，相当于8*8的过滤器。
	szMask:					过滤器的掩码，为一个8个字节的数组，与8*8的过滤器对应，当szMask的某一bit位为0时表示要过滤的表对应的bit位不需要与szFilter里对应的bit位相同，当szMask某一位为1时表示要过滤的表对应的bit位一定要与szFilter里对应的bit位相同。
	byLen:					过滤器的长度，为8。
	nWaitSecs:			收此表最长的等待时间，如果为0则无限等待，单位秒。
--*/
BU8 BYSTBCA_SetStreamGuardFilter(BU8 byReqID,BU16 wPID,BU8* szFilter,BU8* szMask,BU8 byLen,BU16 nWaitSecs);

/*++
功能：CA程序用此函数设置解扰器。将当前周期及下一周期的CW送给解扰器。
参数：
	wEcmPID:				CW所属的ECMPID。
	szOddKey:				奇CW的数据。
	szEvenKey:				偶CW的数据。
	byKeyLen:				CW的长度。
	bTaingControl:			true：允许录像,false：不允许录像。
--*/
BU8 BYSTBCA_SetDescrCW(BU16 wEcmPid,BU8 byKeyLen,const BU8* szOddKey,const BU8* szEvenKey,BU8 bTaingControl);

/*++
功能：打印调试信息。
参数：Param1,Param2,pszMsg:					调试信息内容。
--*/
void BYSTBCA_AddDebugMsg(BU32 Param1,BU32 Param2,const BS8 *pszMsg);
/*++
功能：当用户插卡时,先调用该函数复位卡
返回:     true  成功，false失败
参数：pATR 最大64 字节
--*/
BU8 BYCASTB_SCReset(BU8 *pATR);

/*++
功能：实现与智能卡之间的通讯。采用7816标准。
参数：
	byReaderNo:				对应的读卡器编号。
	pbyLen:					输入为待发送命令字节的长度;输出为返回字节长度。
	byszCommand:			待发送的一串命令字节。
	szReply:				返回的数据，空间长度恒为256字节。
--*/
BU8 BYSTBCA_SCAPDU(BU8 byReaderNo,const BU8* byszCommand,BU16* pbyLen,BU8* byszReply);

/*------------ 类型1的界面：不允许用户通过遥控器取消显示的界面 -----------------------*/
/*
功能：显示用户不能观看节目等提示信息，下面两个函数对应使用。
参数：	
	byMesageNo:				要显示的消息编号。
--*/
BVOID   BYSTBCA_ShowPromptMessage(BU8 byMesageNo);
BVOID   BYSTBCA_HidePromptMessage(BVOID);

/*
功能：显示指纹信息。
参数：
	wEcmPid					ECM PID
	dwCardID:				卡号。
	wDuration:				持续秒数。
--*/
BVOID	BYSTBCA_ShowFingerPrinting(BU16 wEcmPid,BU32 dwCardID,BU32 wDuration);

/*--------------------------- 类型2的界面：用户可以取消显示的界面 -----------------------*/
/*
功能：以左滚字幕的方式显示OSD消息，可以是同步显示，也可以是异步显示。
参数:	
	szOSD:					用于显示的OSD信息。
	wDuration:				0:显示一次后立即返回;其他值为显示的具体持续秒数。
说明：
	用户可以通过遥控器取消当前OSD显示。
--*/
BVOID	BYSTBCA_ShowOSDMsg(const BS8* szTitle,const BS8* szOSD,BU16 wDuration);
/*--------------------------- 类型3的界面：用户可以取消显示的界面 -----------------------*/
/*
功能：弹出显示OSD消息，可以是同步显示，也可以是异步显示。
参数:	
	szOSD:					用于显示的OSD信息。
说明：
	用户可以通过遥控器取消当前OSD显示。
--*/
BVOID	BYSTBCA_ShowOSDFrameMsg(const BS8* szTitle,const BS8* szOSD);
/*++
功能：显示新邮件的标志。
参数：
	byShow:					标志类型。含义如下：
							BYCAS_NEW_EAMIL						新邮件通知。
							BYCAS_NEW_EMAIL_NO_ROOM		有新邮件，但Email空间不够。
							BYCAS_EMAIL_NONE					没有新邮件。
--*/
BVOID BYSTBCA_EmailNotify(BU8 byShow);
/*
功能: 提供随机数据
参数: blen 长度pbBuf 返回blen个随机数
*/
BU8	BYSTBCA_Random(BU8 blen,BU8 *pbBuf);
/*
功能：通知APP锁定某个频道信号，在该特定的频点寻找漫游信息。
参数:	KHZ;
返回:  1，表示频点已锁到，否则未锁定该频点。
--*/
BU8 BYCA_SetZoneFreq( BU32 Freq );

/*
功能：以滚字幕的方式显示OSD消息，可以是同步显示，也可以是异步显示。
参数:	
	ADScroll_Info:
说明：
	用户可以通过遥控器。
--*/
BVOID	BYSTBCA_ShowPromptOSDMsg(const ADScroll_Info* padInfo);/* 这个暂时可以不用实现*/

/*--------------------------- 应急广播 -----------------------*/
/*++
功能：应急广播, 切换到指定频道。
参数：
	wOriNetID:				原始网络ID。
	wTSID:					传送流ID。
	wServiceID:				业务ID。
	pUTCTime--5 bytes:				UTC时间。
--*/
BVOID BYCASTBCA_UrgencyBroad(BU16 wOriNetID,BU16 wTSID,BU16 wServiceID,BU8 *pUTCTime);

/*++
功能：取消应急广播，切换回应急广播前用户观看的频道。
参数：无。
--*/
BVOID BYCASTBCA_CancelUrgencyBroad(BVOID);

/*-------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
