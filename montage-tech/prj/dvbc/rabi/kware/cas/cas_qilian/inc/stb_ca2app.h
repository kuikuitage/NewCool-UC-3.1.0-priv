//本文件定义CA模块实现提供给STB调用的接口
// 2011 0801 LHCAS VER0.2

#ifndef _STB_CAS2APP
#define _STB_CAS2APP

#include "stb_Public.h"

#ifdef  __cplusplus
extern "C" {
#endif 

/*-----------------------------------以下接口是CAS提供给STB调用----------------------------------*/

/*++
功能：机顶盒启动的时候调用，对CAS模块初始化。
参数：
	
返回值：
	true				初始化成功。
	false				初始化失败。
--*/
BU8 STBCA_Init(void);
/*++
功能：机顶盒获得CA模块的版本。
参数：
	pdwVer:				返回版本号。					
返回值：
	true				成功。	
说明：
	返回CA模块的版本号。举例: 如果是0x01020304意思就是1.2.3.4。
--*/
void	STBCA_GetVer(BU32 *pdwVer);
/*++
功能：机顶盒获得CA模块的厂家信息。
参数：
	pdwVer:				返回厂家信息				
返回值：
	true				成功。	
--*/
void STBCA_GetServiceInfo(char *pdwVer);
/*++
功能：当用户插卡时机顶盒主程序调用，通知CAS模块
参数：VOID
返回:	  cas初始化成功返回true
--*/
bool STBCA_SCInsert( void);

/*++
功能：当用户拔出智能卡时机顶盒主程序调用，通知CAS模块。
--*/
void STBCA_SCRemove(void);

/*++
功能：调用CAS模块，获得智能卡内部ID。
参数：pdwCardID 卡号
返回值：
	0				失败,无卡
	1    读取成功。
--*/
bool STBCA_GetSmartCardID(BU32 *pdwCardID,BU16* Zone_ID);

//功能：机顶盒在解析PMT CAT 时分析当前加扰信息是否是	CAS供应商提供
//
BU8    STBCA_IsLYCA( BU16 wCaSystemID );
/*++
功能：用户换台时调用的CAS接口。此功能让APP实现，搜到ECM pid后调用STBCA_SetCurECMInfo
参数:  当前节目的pmtpid，节目序号
--*/

/*++
功能：当机顶盒收到CAT表后,通知	CAS当前的CAS 的EMM pid。
--*/
void STBCA_SetEmmPid(BU16 pid);
/*++
功能：当机顶盒收到PMT表后,通知	CAS当前的CAS的ECM pid。当参数全为0x1fff 时，ca demux
		停止申请过滤器。
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
功能：机顶盒主程序在收到CAS私有数据或者Time_Out，调用这个接口将数据提供给CAS模块处理。
参数：
	ReqID：				当前请求收取私有表的请求号，与STBCA_SetStreamGuardFilter里的ReqID对应
	bSuccess：				收取数据有没有成功，成功为true，失败为false
	wPID:					接收的数据的流的PID。
	b8ReceiveData		收取的私有数据
	wLen：					收取到的私有数据的长度
--*/
BVOID  STBCA_StreamGuardDataGot(BU8 ReqID,BU8 bSuccess,BU16 wPID,BU8* b8ReceiveData,BU32 wLen);

/*++
功能：调用CAS模块，ca停止请求Dexmux section数据，如果想恢复可调用STBCA_DemuxRun
参数：void	
*/
void STBCA_DemuxStop(void );
/*++
功能:相反
*/
void STBCA_DemuxRun(void );

/*++
功能：调用CAS模块，   删除邮件操作
参数：0，表示删掉所有的邮件，1表示删掉第一封。。。
*/
BU8 STBCA_DelEmail(BU16 savedNO);

/*
功能：调用CAS模块，读取邮件数量信息,最多40封。
参数：第一个参数返回邮件总数量，第2个参数返回新邮件数量
返回：0 表示读取失败。
*/
BU8 STBCA_GetEmailCount(BU16 *pEmailCount,BU16 *pNewEmailCount);

/*
功能：调用CAS模块，读取邮件。
参数：第一个参数表示要读邮件序号，从0开始 小于EMail_MAXNUM，第2个参数返回邮件内容
返回：0 表示读取失败，或者邮件不存在。
*/
BU8 STBCA_GetEmailContent(BU16 b16index,Email_Data *pEmail);
/*
功能：调用CAS模块，从0开始 设置存储序号为saveNO 的邮件为已读
返回：0 表示读取失败，或者邮件不存在。
*/

BU8 STBCA_EmailRead(BU16 savedNO);
/*
功能：调用CAS模块，在系统启动后主程序调用，检查邮箱状态。
		通知机顶盒OSD菜单提示是否有新邮箱、　邮件已满。
*/
 void STBCA_CHECK_MailBox(void);
//NIT表的区域描述符 0x9c
//因为这个描述子长度不是固定的，所以传数据是整个描述符，第1个数据就是标准9c
// 搜到这个描述符后，ca库可以自己申请BAT,自己解析，外部应用demux需具有过滤bat功能。
void STBCA_CurrentNitDes(BU8* databuf );

/*
功能：调用CAS模块，获取当前智能卡用户订购的产品信息
    CAMenu_Product类型表示产品类型。
返回：0 表示读取失败    
*/

BU8 STBCA_GetCaMenuProduct(CAMenu_Product *pCaMenu_product);
/*
功能：调用CAS模块，获取当前智能卡用户订购的产品总数
    CAMenu_Product类型表示产品类型。
返回：0 表示读取失败    
*/
BU8 STBCA_GetProductCount(BU16 *pProductCount);
/*
功能：调用CAS模块，关闭cas打印信息
*/
/*
功能：调用CAS模块，获取当前智能卡 信息，判断是否为子卡
返回：0 表示读取失败 ，当前卡为母卡 
                 1表示为子卡，pdwCardID为母卡号码
*/

bool STBCA_isDaughterCard(BU32 *pdwCardID);
/*
功能：调用CAS模块，获取当前ca系统使用的时间，格式为MJD，五个字节。
返回:五个字节的数组。
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

