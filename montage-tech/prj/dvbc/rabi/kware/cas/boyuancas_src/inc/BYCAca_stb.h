#ifndef BYCACA_STB_H_2013_02_20
#define BYCACA_STB_H_2013_02_20

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "BYCApubtype.h"
/*++
功能：机顶盒启动的时候调用，对CAS模块初始化。
参数：byReaderNo：		StreamGuard所属的智能卡读卡器号。
返回值：
	true				初始化成功。
	false				初始化失败。
--*/
BU8  BYCASTB_Init(BU8 byReaderNo);

/*++
功能：机顶盒获得CA模块的版本。
参数：pdwVer:				返回版本号。					
		   pCaSysSp:                       CA厂家名称
		   pCaSysCt:				CA所有权人
返回值：
	true				成功。
	false				失败。
说明：返回CA模块的版本号。举例: 如果是0x01020304意思就是1.2.3.4。
--*/
BU8 BYCASTB_GetVer(BU32 *pdwVer,BU8 *pCaSysSp,BU8 *pCaSysCt);

/*++
功能：调用CAS模块，获得智能卡内部ID。
参数：pdwCardID:			返回智能卡内部ID。						
返回值：
	true				成功
	false				失败
--*/
BU8 BYCASTB_GetCardID(BU32 *pdwCardID);

/*++
功能：机顶盒获得CASID后调用该接口判断CASID是否有效，如果有效则返回true，否则返回false。
参数：wCaSystemID:		机顶盒分析SI/PSI信息获得的CASID。
返回值：
	true				CASID有效
	false				CASID无效
--*/
BU8 BYCASTB_CASIDVerify(BU16 wCaSystemID);

/*++
功能：当用户插卡时机顶盒主程序调用，通知CAS模块
参数：byReaderNo:			读卡器的编号
--*/
BVOID BYCASTB_SCInsert(BVOID);

/*++
功能：当用户拔出智能卡时机顶盒主程序调用，通知CAS模块。
参数：byReaderNo:			读卡器的编号。
--*/
BVOID BYCASTB_SCRemove(BVOID);

/*++
功能：设置当前正在收看的节目的详细信息。
参数：	pServiceInfos:		SBYCAServiceInfo结构，描述节目的详细信息。
		byCount:			EcmPid的个数，目前只支持byCount=1 ，如果为0 表示停止ECM
说明：
	当用户换频道时，机顶盒需要调用CAS提供的此函数，将当前节目的原始信息及ECM PID传递给CAS模块，CAS模块会
	根据当前频道的信息处理与当前频道相关的操作，如果当前频道为不加扰的频道，则需要将ECM的PID设为0。
--*/
BVOID BYCASTB_SetCurEcmInfos(BU8 bType,const SBYCAServiceInfo* pServiceInfos);

/*++
功能：设置EMMPID，CAS模块将根据设置的EMMPID信息进行EMM数据接收处理。
参数：
	pwEmmPids:			CAT表中包含的descriptor中将有CA_system_ID及CA_PID，调用BYCASTB_CASIDVerify()校验有效的CA_PID即为EMM PID。
	nEmmCount:			EmmPid的个数，目前只支持nEmmCount＝1， 如果为0 表示停止EMM
说明:
	当机顶盒收到CAT表后，机顶盒需要调用CA提供的此函数，将EMM PID传过来，这样CA程序就可以根据此PID收取
	私有表，更新用户卡里的授权及为用户提供增值服务，当设置nEmmCount = 0或 pwEmmPids = NULL 时，CA模块会停止收取EMM。
--*/
BVOID BYCASTB_SetEmmPids(BU8 nEmmCount,BU16* pwEmmPids);

/*++
功能：机顶盒主程序在收到CAS私有数据或者Time_Out，调用这个接口将数据提供给CAS模块处理。
参数：
byReqID：				
当前请求收取私有表的请求号，与BYSTBCA_SetStreamGuardFilter里的byReqID对应
	bSuccess：				收取数据有没有成功，成功为true，失败为false
	wPID:					接收的数据的流的PID。
	byszReceiveData：		收取的私有数据
	wLen：					收取到的私有数据的长度
--*/
BVOID BYCASTB_StreamGuardDataGot(BU8 byReqID,BU8 bSuccess,BU16 wPID,BU8* byszReceiveData,BU32 wLen);

/*++
功能：打开或关闭调试信息。
参数：bDebugFlag:			打开调试信息标志，1:打开调试信息;0:关闭调试信息。
--*/
BVOID BYCASTB_AddDebugMsg(BU8 bDebugFlag);

/*
功能：机顶盒显示完一个OSD消息后，用这个接口来通知CA模块。
参数:	
	wDuration:				消息显示的具体时间，单位为秒。
--*/
BVOID BYCASTB_ShowOSDMsgOver(BU16 wDuration);

/*++
功能:机顶盒获取E-mail个数接口。
参数:
	*pEmailCount:			机顶盒中E-mail总个数。
	*pNewEmailCount:		机顶盒中未读过的E-mail个数。
返回值：
	1				成功
	0				失败
--*/
BU8 BYCASTB_GetEmailCount(BU16 *pEmailCount,BU16 *pNewEmailCount);

/*++
功能:机顶盒获取一个E-mail的内容。
参数:
	icount:				当前读过第几封邮件号，0-表示第一封。
	*pEmail:				获取的E-mail结构。
返回值:
  
--*/
BU8 BYCASTB_GetEmailContent(BU16 icount,MAIL_DATA *pEmail);

/*++
功能:机顶盒看完E-mail后，通知CAS的接口。
参数:
	icount:				当前读过第几封邮件号，0-表示第一封。
返回值:
	1:				设置E-mail为已读成功。
	0:					失败
--*/
BU8 BYCASTB_EmailRead(BU16 icount);

/*++
功能:机顶盒删除E-mail接口。
参数:
	icount:			如果为0，表示删除邮箱中所有email，1 表示第一封
返回值:
	1:				删除E-mail成功。
	0:				要删除的E-mail不存在。失败
--*/
BU8 BYCASTB_DelEmail(BU16 icount);

/*
	如果有产品，返回true;
	
	如果没有，返回false
	*pProductCount，返回实际产品个数
*/
BU8 BYCASTB_GetProductCount(BU16 *pProductCount);
/*
	返回产品内容，如果成功返回true,否则false
	pCaMenu_product->CaMenu_Product_Init 返回这个参数没有意义，可以不用考虑
*/
BU8 BYCASTB_GetCaMenuProduct(CAMenu_Product *pCaMenu_product);
/*
  为了实现区域漫游功能，需要用户调用的功能，当用户收到BAT数据后，
  调用该函数
*/
BVOID BYCA_ParseBAT(BU8* psibuffer);

/*
功能：调用CAS模块，设置当前的区域。
NIT插入的描述子结构为:
	tag                    							8bit(0x9c)
	length									8bit
	zoneID									16BIT 
返回：描述子长度。
*/

BVOID BYCAS_NitZONEDes(BU8* databuf);


/*
BVOID TestCardBufferTest(BU16 u16Count);
*/
/*++
功能：调用CAS模块，获得母卡卡号内部ID
参数：pdwCardID:			返回母卡卡号内部ID					
返回值：
	true				成功
	false				失败
--*/

BU8 BYCAS_GetMasterCardID(BU32 *pdwCardID);
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

