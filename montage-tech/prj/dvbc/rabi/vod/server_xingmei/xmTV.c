#include <string.h>
#include <stdio.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "ui_common.h"
#include "download_api.h"
#include "lib_unicode.h"
#include "drv_dev.h"
#include "ufs.h"
#include "mdl.h"
#include "authorization.h"
#include "ap_cas.h"
#include "des.h"
#include "xmTV.h"
#include "iptv_interface.h"

#define  LOGIN_STATE_PASSWARD_ERROR 		(2001)
#define  LOGIN_STATE_ARREARS		 		(2002)
#define  LOGIN_STATE_UPCOMING_ARREARS 	(2004)
#define  LOGIN_STATE_OK				 		(0)

#define   TICK_PER_SECOND 		100
#define   URL_MAX_LENGTH           	(2048)
#define   NC_AUTH_STACK_SIZE     (16*1024)
#define   USER_ID_LENGTH		(64)

unsigned  char ipAddress[4];


//static const char * DEFAULT_SERVER_ADDRESS	= "http://101.200.173.180";
static const char * DEFAULT_SERVER_ADDRESS	= "http://192.168.0.37";

static const char * DEFAULT_SERVER_PORT  	= "8660";
static const char * DEFAULT_HOST_PORT  		= "8080";

static const char * REGISTER			= "getLoginCode";
static const char * LOGIN				= "login";
static const char * UPDATEUSERTOKEN		= "updateUserToken";
static const char * HEARTBEAT			= "epgHeart";
static const char * LOGOUT				= "logout";

static const char * MANAGEPLATFORM		= "aaa";
static const char * SERVICE				= "ott";
static const char * TERM				= "term";
static const char * USERID   			= "userid";
static const char * AUTHENTICATOR		= "Authenticator";

static const char * ACTION   			= "Action";
static const char * USERTOKEN   		= "usertoken";
static const char * HEARTTOKEN   		= "userToken";
static const char * NEWUSERTOKEN   		= "newUserToken";
static const char * TOKENEXPIREDTIME   	= "tokenExpiredTime";
static const char * OLDUSERTOKEN   		= "oldusertoken";
static const char * STREAMSTATUS   		= "streamStatus";
static const char * USERTOKENEXPIRED   	= "UserTokenExpired";
static const char * TYPE				= "type";
static const char * DEFAULTTYPE			= "AndroidStb";

//static const char *USERGROUPNMB 	= "UserGroupNMB";
//static const char *EPGGROUPNMB 		= "EPGGroupNMB";
//static const char *STBID 			= "STBID";
static const char *AUTHENTICATION	= "authentication";

//result
static const char * LOGINCODE			= "loginCode";
static const char * RETURNCODE			= "returnCode";
static const char * STATUS				= "status";


typedef struct _xm_auth_s
{
	BOOL 		inited;
	BOOL 		active;
	BOOL		caEntitlement;
	NC_STATUS_t state;
	unsigned char 			userid[USER_ID_LENGTH + 1];
	unsigned char  		mac[20];
	unsigned char  		stbId[64];

	unsigned char			userToken[64];
	unsigned char			encryToken[64];
	unsigned char 			key[24];
	unsigned char 			authenticator[1024];

	unsigned char			streamStatus[6];
	unsigned char			type[21];
	unsigned char 			serverIp[32];
	unsigned char 			serverPort[8];
	unsigned char 			hostIp[32];
	unsigned char 			hostPort[8];

	unsigned char 			url[URL_MAX_LENGTH];
	char 			first_insert;
	unsigned int	last_beat_tick;
	unsigned int	beating;
	unsigned int    beatFailureCount;
}XM_Auth_t;


typedef struct epg_s
{
	unsigned char			epgDomain[256]; //EPG 的域名
	unsigned char			epgDomainBackUp[256]; //EPG 的域名备份
	unsigned char 			managementDomain[256];//管理服务器的Domain
	unsigned char 			managementDomainBackUp[256];//管理服务器的Domain 备份
	unsigned char 			NTPDomain[256];//时钟同步服务器地址
	unsigned char			userToken[64];//AAA为该用户分配的临时身份证明
	unsigned char			userGroupNum[12];//用户对应的用户分组信息标识
	unsigned char 			epgGroupNum[12];//用户对应的EPG分组信息标识
	unsigned char 			epgHeartUrl[256];//EPG心跳接口地址
	unsigned char 			epgHeartTime[12];//心跳周期，默认为60(秒)
	unsigned char 			epgOfflineUrl[256];//EPG登出时调用。不返回数据
	unsigned char 			tokenUpdateUrl[256];//USERTOKEN是访问后台数据的重要标识，但USERTOKEN会有过期时间，建议在USERTOKEN过期前5到10秒进行一次更新请求
	unsigned char 			tokenExpireTime[12];//USERTOKEN更新周期，N秒后USETOKEN将会过期
	
	unsigned char 			userId[64];
	unsigned char  		stbId[64];
	unsigned char  		mac[20];
	unsigned char 			url[URL_MAX_LENGTH];

	int					iptvGroupNum;
	char					first_insert;

	os_sem_t 			lock;

}NC_EPG_t;



static XM_Auth_t _xm_auth;
static NC_EPG_t  _xm_epg;

volatile int MONITOR_ACTIVE;

#define AUTH_PRINTF  OS_PRINTF

static int XM_set_epg_info(GxJson_t json);
extern void triple_des_encrypt(char *out, char *in, long datalen, char *key);
extern void triple_des_decrypt(char *out, char *in, long datalen, char *key);;

static void set_authenticator(void)
{
	unsigned char buffer[1024] = {0};
	unsigned char num[6] = {0};
	static unsigned char rand = 12;

	rand = ((rand + 1)*2)%256;
	sprintf(num, "%d",rand);
	strcat(buffer,num);
	strcat(buffer,"$");
	
	strcat(buffer,_xm_auth.encryToken);
	strcat(buffer,"$");

	strcat(buffer,_xm_auth.userid);
	strcat(buffer,"$");

	strcat(buffer,_xm_auth.stbId);
	strcat(buffer,"$");

	strcat(buffer,_xm_auth.hostIp);
	strcat(buffer,"$");

	strcat(buffer,_xm_auth.mac);
	strcat(buffer,"$");
	
	strcat(buffer,"newcool");
	strcat(buffer,"$");
	
	strcat(buffer,"SV");
	DEBUG(XM_AUTH,INFO,"authenticator_len = %d  buffer = %s \n",strlen(_xm_auth.authenticator),buffer);
	memset(_xm_auth.authenticator,0,sizeof(_xm_auth.authenticator));
	IPTV_3DesEncrypt(_xm_auth.key,buffer,_xm_auth.authenticator,sizeof(_xm_auth.authenticator));
}

static char *UrlInsertKeyValuePairsString(const char * key, const char * value)
{
	unsigned char buf[1024];
	snprintf(buf,1024,"%s=%s",key,value);
	if (!_xm_auth.first_insert)
	{
		strcat(_xm_auth.url,"&");
	}
	_xm_auth.first_insert = 0;
	strcat(_xm_auth.url,buf);
	return _xm_auth.url;
}

static char *_XMNewUrlRegister(void)
{
	memset(_xm_auth.url,0,sizeof(_xm_auth.url));
	_xm_auth.first_insert = 1;

	//address
	strcat(_xm_auth.url,_xm_auth.serverIp);
	strcat(_xm_auth.url,":");
	strcat(_xm_auth.url,_xm_auth.serverPort);
	strcat(_xm_auth.url,"/");

	//service
	strcat(_xm_auth.url,MANAGEPLATFORM);
	strcat(_xm_auth.url,"/");
	strcat(_xm_auth.url,SERVICE);
	strcat(_xm_auth.url,"/");
	//type
	strcat(_xm_auth.url,REGISTER);
	strcat(_xm_auth.url,"?");

	//userid
	UrlInsertKeyValuePairsString(USERID, _xm_auth.userid);

	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_auth.url);
	return _xm_auth.url;
}

static char *_XMNewUrlLogin(void)
{
	memset(_xm_auth.url,0,sizeof(_xm_auth.url));
	_xm_auth.first_insert = 1;

	//address
	strcat(_xm_auth.url,_xm_auth.serverIp);
	strcat(_xm_auth.url,":");
	strcat(_xm_auth.url,_xm_auth.serverPort);
	strcat(_xm_auth.url,"/");

	//service
	strcat(_xm_auth.url,MANAGEPLATFORM);
	strcat(_xm_auth.url,"/");
	strcat(_xm_auth.url,SERVICE);
	strcat(_xm_auth.url,"/");
	
	//type
	strcat(_xm_auth.url,LOGIN);
	strcat(_xm_auth.url,"?");

	//userid
	UrlInsertKeyValuePairsString(USERID, _xm_auth.userid);
	//authenticator
	UrlInsertKeyValuePairsString(AUTHENTICATOR, _xm_auth.authenticator);
	//type
	UrlInsertKeyValuePairsString("MAC", _xm_auth.mac);
	UrlInsertKeyValuePairsString(TYPE, _xm_auth.type);

	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_auth.url);
	return _xm_auth.url;
}

static char *_XMNewUrlUpdateUserToken(void)
{
	memset(_xm_auth.url,0,sizeof(_xm_auth.url));
	_xm_auth.first_insert = 1;

	//address
	strcat(_xm_auth.url,_xm_auth.serverIp);
	strcat(_xm_auth.url,":");
	strcat(_xm_auth.url,_xm_auth.serverPort);
	strcat(_xm_auth.url,"/");

	//service
	strcat(_xm_auth.url,MANAGEPLATFORM);
	strcat(_xm_auth.url,"/");
	strcat(_xm_auth.url,SERVICE);
	strcat(_xm_auth.url,"/");
	
	//type
	strcat(_xm_auth.url,UPDATEUSERTOKEN);
	strcat(_xm_auth.url,"?");

	//userid
	UrlInsertKeyValuePairsString(OLDUSERTOKEN, _xm_auth.userToken);
	//action
	UrlInsertKeyValuePairsString(ACTION, USERTOKENEXPIRED);

	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_auth.url);
	return _xm_auth.url;
}

static char *_XMNewUrlHeartbeat(void)
{
	memset(_xm_auth.url,0,sizeof(_xm_auth.url));
	_xm_auth.first_insert = 1;

	//address
	strcat(_xm_auth.url,_xm_auth.serverIp);
	strcat(_xm_auth.url,":");
	strcat(_xm_auth.url,_xm_auth.serverPort);
	strcat(_xm_auth.url,"/");

	//service
	strcat(_xm_auth.url,MANAGEPLATFORM);
	strcat(_xm_auth.url,"/");
	strcat(_xm_auth.url,TERM);
	strcat(_xm_auth.url,"/");
	
	//type
	strcat(_xm_auth.url,HEARTBEAT);
	strcat(_xm_auth.url,"?");

	//userid
	UrlInsertKeyValuePairsString(HEARTTOKEN, _xm_auth.userToken);
	//streamStatus
	UrlInsertKeyValuePairsString(STREAMSTATUS, _xm_auth.streamStatus);
	
	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_auth.url);
	return _xm_auth.url;
}

static char *_XMNewUrlLogout(void)
{
	memset(_xm_auth.url,0,sizeof(_xm_auth.url));
	_xm_auth.first_insert = 1;

	//address
	strcat(_xm_auth.url,_xm_auth.serverIp);
	strcat(_xm_auth.url,":");
	strcat(_xm_auth.url,_xm_auth.serverPort);
	strcat(_xm_auth.url,"/");
	//type
	strcat(_xm_auth.url,LOGOUT);
	strcat(_xm_auth.url,"?");
	return _xm_auth.url;
}

static void FreeUrl(void)
{
	memset(_xm_auth.url,0,sizeof(_xm_auth.url));
}

/*need invoke GxJson_Free*/
const char *xx = 
"{"
"\"category\":"
"["
"{\"id\":2,\"name\":\"xxxx\",\"poster\":\"null\",\"logo\":\"\",\"imgDirection\":0,\"listplay\":0,\"showtype\":0,\"templatecode\":null,\"defaultflag\":0,\"programsum\":\"0\",\"hpbcUrl\":\"http://192.168.5.3:8290/epgserver/channel/getHotChannel?parent=1&cid=2&pid=2\",\"subCategoryUrl\":\"http://192.168.5.3:8290/epgserver/category/getCategory?pName=xx2&cid=2&parent=1&path=/channel/channelList\",\"programListUrl\":\"http://192.168.5.3:8290/epgserver/channel/channelList?cid=2&parent=1&showtype=0&listplay=0&secondaryCategoryId=0\",\"type\":\"live\"},"
"{\"id\":4,\"name\":\"entry\",\"poster\":\"null\",\"logo\":\"\",\"imgDirection\":0,\"listplay\":0,\"showtype\":0,\"templatecode\":null,\"defaultflag\":0,\"programsum\":\"1\",\"hpbcUrl\":\"http://192.168.5.3:8290/epgserver/program/getHPBC?parent=1&cid=4&pid=4\",\"subCategoryUrl\":\"http://192.168.5.3:8290/epgserver/category/getCategory?pName=entry&cid=4&parent=1&path=/program/getlist\",\"programListUrl\":\"http://192.168.5.3:8290/epgserver/program/getlist?cid=4&parent=1&showtype=0&listplay=0&secondaryCategoryId=0\",\"type\":\"vod\"},"
"{\"id\":13,\"name\":\"xx2\",\"poster\":\"null\",\"logo\":\"\",\"imgDirection\":0,\"listplay\":0,\"showtype\":0,\"templatecode\":null,\"defaultflag\":0,\"programsum\":\"0\",\"hpbcUrl\":\"http://192.168.5.3:8290/epgserver?parent=1&cid=13&pid=13\",\"subCategoryUrl\":\"http://192.168.5.3:8290/epgserver/category/getCategory?pName=xx3&cid=13&parent=1&path=/program/getProgramListByCategoryId\",\"programListUrl\":\"http://192.168.5.3:8290/epgserver/program/getProgramListByCategoryId?cid=13&parent=1&showtype=0&listplay=0&secondaryCategoryId=0\",\"type\":\"buxian\"}"
"],"
"\"hotest\": [],\"recommand\": [],\"recommendTV\": [],\"adpicture\":[],\"bgImage\":\"null\",\"areaUrl\":\"http://192.168.5.3:8290/epgserver/tools/getAreaInfoByIp\",\"weatherUrl\":\"http://192.168.5.3:8290/epgserver/tools/getWeatherByDX\",\"updatePeriod\":\"2\",\"searchUrl\":\"http://192.168.5.3:8290/epgserver/program/search?parent=1\",\"textUrl\":\"http"
".168.5.3:8290/epgserver/program/getText?parent=1&secondaryCategoryId=0\",\"favoritesUrl\":\"http://192.168.5.3:8290/epgserver/program/favorites?parent=1\",\"orderProductUrl\":\"http://192.168.5.3:8290/epgserver/program/product?parent=1\",\"bookmarkUrl\":\"http://192.168.5.3:8290/epgserver/program/bookmark?parent=1\",\"hotProgramsUrl\":\"http://192.168.5.3:8290/epgserver/program/getHotPrograms?parent=1\",\"orderHistoryUrl\":\"http://192.168.5.3:8290/epgserver/program/getOrderHistory?parent=1\",\"hotChannelUrl\":\"http://192.168.5.3:8290/epgserver/channel/getHotChannel?parent=1\","
"\"queryJsonUrl\":\"http://192.168.5.3:8290/epgserver/query.json\",\"downlineAppUrl\":\"http://192.168.5.3:8290/epgserver/application/getDownlineApplicationByCpspid?cpspid=1\",\"propertyUserUrl\":\"http://192.168.5.3:8290/epgserver/propertyuser/getPropertyUserByCustomerId?customerid=1\",\"memberInfoUrl\":\"http://192.168.5.3:8290/epgserver/propertyuser/getMemberInfoByCustomerId?customerid=1\",\"updatePropertyInfoUrl\":\"http://192.168.5.3:8660/aaa/ott/updatePropertyInfo?customerid=1\",\"updateMemberInfoUrl\":\"http://192.168.5.3:8660/aaa/ott/updateMemberInfo?customerid=1\","
"\"updateMemberIntegralUrl\":\"http://192.168.5.3:8660/aaa/ott/updateMemberIntegral?customerid=1\",\"updateOnlineDurationUrl\":\"http://192.168.5.3:8660/aaa/ott/updateOnlineDuration?customerid=1\",\"exchangeCommodityUrl\":\"http://192.168.5.3:8660/aaa/ott/exchangeCommodity?customerid=1\",\"updateLotteryCount\":\"http://192.168.5.3:8660/aaa/ott/updateLotteryCount?customerid=1\",\"vedioPlayCountUrl\":\"http://192.168.5.3:8290/epgserver/log/getVedioPlayCountByUserIdAndProgramIdAndDate?userid=0000091a0004d847\"}";

const char *xx2 = 
"{"
"\"category\":"
"["
"{\"id\":2,\"name\":\"xxxx\",\"poster\":\"null\",\"logo\":\"\",\"imgDirection\":0,\"listplay\":0,\"showtype\":0,\"templatecode\":null,\"defaultflag\":0,\"programsum\":\"0\",\"hpbcUrl\":\"http://192.168.5.3:8290/epgserver/channel/getHotChannel?parent=1&cid=2&pid=2\",\"subCategoryUrl\":\"http://192.168.5.3:8290/epgserver/category/getCategory?pName=xx2&cid=2&parent=1&path=/channel/channelList\",\"programListUrl\":\"http://192.168.5.3:8290/epgserver/channel/channelList?cid=2&parent=1&showtype=0&listplay=0&secondaryCategoryId=0\",\"type\":\"live\"},"
"{\"id\":4,\"name\":\"entry\",\"poster\":\"null\",\"logo\":\"\",\"imgDirection\":0,\"listplay\":0,\"showtype\":0,\"templatecode\":,\"defaultflag\":0,\"programsum\":\"1\",\"hpbcUrl\":\"http://192.168.5.3:8290/epgserver/program/getHPBC?parent=1&cid=4&pid=4\",\"subCategoryUrl\":\"http://192.168.5.3:8290/epgserver/category/getCategory?pName=entry&cid=4&parent=1&path=/program/getlist\",\"programListUrl\":\"http://192.168.5.3:8290/epgserver/program/getlist?cid=4&parent=1&showtype=0&listplay=0&secondaryCategoryId=0\",\"type\":\"vod\"},"
"{\"id\":13,\"name\":\"xx2\",\"poster\":\"null\",\"logo\":\"\",\"imgDirection\":0,\"listplay\":0,\"showtype\":0,\"templatecode\":null,\"defaultflag\":0,\"programsum\":\"0\",\"hpbcUrl\":\"http://192.168.5.3:8290/epgserver?parent=1&cid=13&pid=13\",\"subCategoryUrl\":\"http://192.168.5.3:8290/epgserver/category/getCategory?pName=xx3&cid=13&parent=1&path=/program/getProgramListByCategoryId\",\"programListUrl\":\"http://192.168.5.3:8290/epgserver/program/getProgramListByCategoryId?cid=13&parent=1&showtype=0&listplay=0&secondaryCategoryId=0\",\"type\":\"buxian\"}"
"]"
"}";

/*



	
"\"hotest\": [],\"recommand\": [],\"recommendTV\": [],\"adpicture\":[],\"bgImage\":\"null\",\"areaUrl\":\"http://192.168.5.3:8290/epgserver/tools/getAreaInfoByIp\",\"weatherUrl\":\"http://192.168.5.3:8290/epgserver/tools/getWeatherByDX\",\"updatePeriod\":\"2\",\"searchUrl\":\"http://192.168.5.3:8290/epgserver/program/search?parent=1\",\"textUrl\":\"http"
".168.5.3:8290/epgserver/program/getText?parent=1&secondaryCategoryId=0\",\"favoritesUrl\":\"http://192.168.5.3:8290/epgserver/program/favorites?parent=1\",\"orderProductUrl\":\"http://192.168.5.3:8290/epgserver/program/product?parent=1\",\"bookmarkUrl\":\"http://192.168.5.3:8290/epgserver/program/bookmark?parent=1\",\"hotProgramsUrl\":\"http://192.168.5.3:8290/epgserver/program/getHotPrograms?parent=1\",\"orderHistoryUrl\":\"http://192.168.5.3:8290/epgserver/program/getOrderHistory?parent=1\",\"hotChannelUrl\":\"http://192.168.5.3:8290/epgserver/channel/getHotChannel?parent=1\","
"\"queryJsonUrl\":\"http://192.168.5.3:8290/epgserver/query.json\",\"downlineAppUrl\":\"http://192.168.5.3:8290/epgserver/application/getDownlineApplicationByCpspid?cpspid=1\",\"propertyUserUrl\":\"http://192.168.5.3:8290/epgserver/propertyuser/getPropertyUserByCustomerId?customerid=1\",\"memberInfoUrl\":\"http://192.168.5.3:8290/epgserver/propertyuser/getMemberInfoByCustomerId?customerid=1\",\"updatePropertyInfoUrl\":\"http://192.168.5.3:8660/aaa/ott/updatePropertyInfo?customerid=1\",\"updateMemberInfoUrl\":\"http://192.168.5.3:8660/aaa/ott/updateMemberInfo?customerid=1\","
"\"updateMemberIntegralUrl\":\"http://192.168.5.3:8660/aaa/ott/updateMemberIntegral?customerid=1\",\"updateOnlineDurationUrl\":\"http://192.168.5.3:8660/aaa/ott/updateOnlineDuration?customerid=1\",\"exchangeCommodityUrl\":\"http://192.168.5.3:8660/aaa/ott/exchangeCommodity?customerid=1\",\"updateLotteryCount\":\"http://192.168.5.3:8660/aaa/ott/updateLotteryCount?customerid=1\",\"vedioPlayCountUrl\":\"http://192.168.5.3:8290/epgserver/log/getVedioPlayCountByUserIdAndProgramIdAndDate?userid=0000091a0004d847\"}";
*/


static GxJson_t GetUrlJsonResult(const char* url)
{
	char *out_buff = NULL;
	unsigned int out_bufferSize = 0;
	int ret;
	GxJson_t json;
	ret = iptv_common_http_get_with_malloc_internal(url, NULL,NULL, 0, 5, &out_buff,&out_bufferSize, 1);
	//DEBUG(XM_AUTH,INFO,"got Json string size[%d] read size[%d]: %s\n",out_bufferSize,ret,out_buff);
	if (ret <= 0)
	{
		DEBUG(XM_AUTH,ERR,"http get failure!~\n");
		SY_FREE(out_buff);
		return NULL;
	}
	json = GxJson_LoadString(out_buff);
	SY_FREE(out_buff);
	
	return json;
}

static int XM_RegisterStb(void)
{
	GxJson_t json;
	long long value;

	_XMNewUrlRegister();
	json = GetUrlJsonResult(_xm_auth.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
		return -1;
	}

	value = GxJson_GetInteger(json, LOGINCODE);
	if(value != 0)
	{
		DEBUG(XM_AUTH,INFO,"encryToken = %lld \n",value);	
		sprintf(_xm_auth.encryToken,"%lld",value);
		DEBUG(XM_AUTH,INFO,"encryToken = %s \n",_xm_auth.encryToken);	
		set_authenticator();
	}
	GxJson_Free(json);
	return 0;
}

static int XM_Login(void)
{
	GxJson_t json;
	const char * str;

	_XMNewUrlLogin();
	json = GetUrlJsonResult(_xm_auth.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
		return -1;
	}
	DEBUG(XM_AUTH,INFO,"\n");
	
	XM_set_epg_info(json);
	DEBUG(XM_AUTH,INFO,"\n");
	str = GxJson_GetString(json, RETURNCODE);
	if((str != NULL) && (strcasecmp(str,"0") !=0) )
	{
		DEBUG(XM_AUTH,ERR,"XM_Login fail returnCode = %s\n",str);
		GxJson_Free(json);
		return -1;
	}

	str = GxJson_GetString(json, USERTOKEN);
	if (str != NULL)
	{
		strncpy( _xm_auth.userToken,str,sizeof ( _xm_auth.userToken));
		DEBUG(XM_AUTH,INFO,"userToken = %s \n",_xm_auth.userToken);
		GxJson_Free(json);
		return 0;		
	}
	_xm_auth.beating = GxJson_GetInteger(json, "EPGHeartTime") * TICK_PER_SECOND;
	GxJson_Free(json);
	return -1;
}

static int XM_Heartbeat(void)
{
	GxJson_t json;
	int  v;	

	_XMNewUrlHeartbeat();
	json = GetUrlJsonResult(_xm_auth.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
		return -1;
	}
	v = (int)GxJson_GetInteger(json, STATUS);
	DEBUG(XM_AUTH,INFO,"XM_UpdateUserToken status = %d\n",v);	
	if(v == 1)
	{
		GxJson_Free(json);
		return 0;
	}
	GxJson_Free(json);	
	return -1;
}

static int XM_UpdateUserToken(void)
{
	GxJson_t json;
	const char *str;

	_XMNewUrlUpdateUserToken();
	json = GetUrlJsonResult(_xm_auth.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"http get info failure!\n");
		return -1;
	}

	str = GxJson_GetString(json,RETURNCODE);
	if((str != NULL)&&(strcmp(str, "0") != 0))
	{
		DEBUG(XM_AUTH,ERR,"result = %s\n",str);
		GxJson_Free(json);
		return -1;
	}

	str = GxJson_GetString(json, TOKENEXPIREDTIME);
	if(str != NULL)
	{
		DEBUG(XM_AUTH,ERR,"tokenExpiredTime = %s \n",str);
		strncpy(_xm_epg.tokenExpireTime,str,sizeof(_xm_epg.tokenExpireTime));
	}

	str = GxJson_GetString(json, NEWUSERTOKEN);
	if(str != NULL)
	{
		strncpy(_xm_auth.userToken, str,sizeof (_xm_auth.userToken));
		DEBUG(XM_AUTH,ERR,"_xm_auth.userToken = %s!  %s\n",_xm_auth.userToken, str);
		GxJson_Free(json);
		return 0;
	}
	GxJson_Free(json);
	return -1;
}

static int XM_Logout(void)
{
	GxJson_t json;

	_XMNewUrlLogout();
	json = GetUrlJsonResult(_xm_auth.url);
	GxJson_Free(json);
	return 0;
}


/*
"Result:len=1011 {"
userid":"0000091a0004d847",
"password":"vbBdyZ2diI3+ApSynym4QQ==",
"status":"1",
"smp_stbid":"0cf0b407592f",
"onlinestatus":"1",
"usertoken":"00000000091a0004d84720160229152336761",
"epggroupnmb":"16",
"installflag":"1",
"iptvgroupid":1,
"effectiveTime":1488211200000,
"cpspid":1,"epgdomain":"http://192.168.0.37:8290/epgserver/program/index",
"epgdomainbackup":"",
"upgradedomain":"http://192.168.0.37:8081/upgrade/default/version.xml",
"upgradedomainbackup":"",
"managementdomain":"",
"managementdomainbackup":"ftp://msn:StarMsn@192.168.0.37//stbupgrade",
"ntpdomain":"192.168.0.37",
"ntpdomainbackup":"",
"returnCode":"0",
"description":"鐧诲綍鎴愬姛",
"EPGOfflineUrl":"http://192.168.0.37:8660/aaa/term/offline",
"EPGHeartUrl":"http://192.168.0.37:8660/aaa/term/epgHeart",
"EPGHeartTime":"60","EPGBookmarkUrl":"http://192.168.0.37:8660/aaa/ott/bookmark",
"EPGLoadingImgUrl":"http://192.168.0.37:8081/uploadImage/img_1385372235216.jpg",
"TokenUpdateUrl":"http://192.168.0.37:8660/aaa/ott/updateUserToken",
"TokenExpireTime":"18
*/
	
static int XM_set_epg_info(GxJson_t json)
{
	const char *str = NULL;
	
	MT_ASSERT(json!=NULL);
	mtos_sem_take(&_xm_epg.lock, 0);
	if ((str = GxJson_GetString(json, "epgdomain")) != NULL)
	{
		strncpy(_xm_epg.epgDomain,str,sizeof(_xm_epg.epgDomain));
	}

	if ((str = GxJson_GetString(json, "epgdomainbackup")) != NULL)
	{
		strncpy(_xm_epg.epgDomainBackUp,str,sizeof(_xm_epg.epgDomainBackUp));
	}

	if ((str = GxJson_GetString(json, "usertoken")) != NULL)
	{
		strncpy(_xm_epg.userToken,str,sizeof(_xm_epg.userToken));
	}

	if ((str = GxJson_GetString(json, "userid")) != NULL)
	{
		strncpy(_xm_epg.userId,str,sizeof(_xm_epg.userId));
	}
	
	if ((str = GxJson_GetString(json, "usergroupnum")) != NULL)
	{
		DEBUG(XM_AUTH,INFO,"\n");	
		strncpy(_xm_epg.userGroupNum,str,sizeof(_xm_epg.userGroupNum));
	}

	if ((str = GxJson_GetString(json, "epggroupnmb")) != NULL)
	{
		strncpy(_xm_epg.epgGroupNum,str,sizeof(_xm_epg.epgGroupNum));
	}
	
	if ((str = GxJson_GetString(json, "smp_stbid")) != NULL)
	{
		strncpy(_xm_epg.stbId,str,sizeof(_xm_epg.stbId));
	}

	strcpy(_xm_epg.mac,_xm_auth.mac);
	mtos_sem_give(&_xm_epg.lock);
	return TRUE;
}




static char *UrlEpgInsertKeyValuePairsString(const char * key, const char * value)
{
	static unsigned char buf[512];
	snprintf(buf,sizeof(buf),"%s=%s",key,value);
	if (!_xm_epg.first_insert)
	{
		strcat(_xm_epg.url,"&");
	}
	_xm_epg.first_insert = 0;
	strcat(_xm_epg.url,buf);
	return _xm_epg.url;
}

static char *UrlEpgInsertKeyValuePairsInteger(const char * key, int value)
{
	static unsigned char buf[512];
	snprintf(buf,sizeof(buf),"%s=%d",key,value);
	if (!_xm_epg.first_insert)
	{
		strcat(_xm_epg.url,"&");
	}
	_xm_epg.first_insert = 0;
	strcat(_xm_epg.url,buf);
	return _xm_epg.url;
}


static char *_XMNewUrlHomeRequest(void)
{
	memset(_xm_epg.url,0,sizeof(_xm_epg.url));
	_xm_epg.first_insert = 1;

	//address
	mtos_sem_take(&_xm_epg.lock, 0);
	strcat(_xm_epg.url,_xm_epg.epgDomain);
	strcat(_xm_epg.url,"?");
	
	UrlEpgInsertKeyValuePairsString("pName", "ottepg");
	UrlEpgInsertKeyValuePairsString(USERTOKEN, _xm_epg.userToken);
	UrlEpgInsertKeyValuePairsString(TYPE, _xm_auth.type);	

	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_epg.url);
	mtos_sem_give(&_xm_epg.lock);
	return _xm_auth.url;
}

static char *_XMNewUrlCategoryRequest(const char *domain)
{
	memset(_xm_epg.url,0,sizeof(_xm_epg.url));
	
	//address
	mtos_sem_take(&_xm_epg.lock, 0);
	strcat(_xm_epg.url,domain);
	
	UrlEpgInsertKeyValuePairsString(USERTOKEN, _xm_epg.userToken);
	UrlEpgInsertKeyValuePairsString(TYPE, _xm_auth.type);
	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_epg.url);
	mtos_sem_give(&_xm_epg.lock);
	return _xm_auth.url;
}


static char *_XMNewUrlProgramListRequest(const char *domain,unsigned int start, unsigned int total)
{
	memset(_xm_epg.url,0,sizeof(_xm_epg.url));
	
	//address
	mtos_sem_take(&_xm_epg.lock, 0);
	strcat(_xm_epg.url,domain);
	UrlEpgInsertKeyValuePairsInteger("start",(int)start);
	UrlEpgInsertKeyValuePairsInteger("total",(int)total);
	UrlEpgInsertKeyValuePairsString(USERTOKEN, _xm_epg.userToken);
	UrlEpgInsertKeyValuePairsString(TYPE, _xm_auth.type);
	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_epg.url);
	mtos_sem_give(&_xm_epg.lock);
	return _xm_auth.url;
}


static char *_XMNewUrlProgramDetailInfoRequest(const char *domain)
{
	memset(_xm_epg.url,0,sizeof(_xm_epg.url));
	
	//address
	mtos_sem_take(&_xm_epg.lock, 0);
	strcat(_xm_epg.url,domain);
	UrlEpgInsertKeyValuePairsString(USERTOKEN, _xm_epg.userToken);
	UrlEpgInsertKeyValuePairsString(TYPE, _xm_auth.type);
	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_epg.url);
	mtos_sem_give(&_xm_epg.lock);
	return _xm_auth.url;
}


static char *_XMNewUrlPlayInfoRequest(const char *domain)
{
	memset(_xm_epg.url,0,sizeof(_xm_epg.url));
	
	//address
	mtos_sem_take(&_xm_epg.lock, 0);
	strcat(_xm_epg.url,domain);
	UrlEpgInsertKeyValuePairsString(USERTOKEN, _xm_epg.userToken);
	UrlEpgInsertKeyValuePairsString(TYPE, _xm_auth.type);
	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_epg.url);
	mtos_sem_give(&_xm_epg.lock);
	return _xm_auth.url;
}


static char *_XMNewUrlRecommendInfoRequest(const char *domain)
{
	memset(_xm_epg.url,0,sizeof(_xm_epg.url));
	
	//address
	mtos_sem_take(&_xm_epg.lock, 0);
	strcat(_xm_epg.url,domain);
	UrlEpgInsertKeyValuePairsString(USERTOKEN, _xm_epg.userToken);
	UrlEpgInsertKeyValuePairsString(TYPE, _xm_auth.type);
	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_epg.url);
	mtos_sem_give(&_xm_epg.lock);
	return _xm_auth.url;
}

	



static char *_XMNewUrlHomeAuth(void)
{
	memset(_xm_epg.url,0,sizeof(_xm_epg.url));
	_xm_epg.first_insert = 1;

	//address
	strcat(_xm_epg.url,_xm_auth.serverIp);
	strcat(_xm_epg.url,":");
	strcat(_xm_epg.url,_xm_auth.serverPort);
	strcat(_xm_epg.url,"/");

	//service
	strcat(_xm_epg.url,MANAGEPLATFORM);
	strcat(_xm_epg.url,"/");
	strcat(_xm_epg.url,SERVICE);
	strcat(_xm_epg.url,"/");

	strcat(_xm_epg.url,AUTHENTICATION);
	strcat(_xm_epg.url,"?");
	
	mtos_sem_take(&_xm_epg.lock, 0);
	UrlEpgInsertKeyValuePairsString(USERTOKEN, _xm_epg.userToken);
	UrlEpgInsertKeyValuePairsString(TYPE, "AndroidStb");
	mtos_sem_give(&_xm_epg.lock);
	
	
	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_epg.url);
	return _xm_epg.url;
}

static char *_XMNewUrlHomeInfo(void)
{
	memset(_xm_epg.url,0,sizeof(_xm_epg.url));
	_xm_epg.first_insert = 1;

	//address
	strcat(_xm_epg.url,_xm_auth.serverIp);
	strcat(_xm_epg.url,":");
	strcat(_xm_epg.url,"8290");
	strcat(_xm_epg.url,"/");

	//service
	strcat(_xm_epg.url,"epgserver/program/index?pName=ottepg");
	
	DEBUG(XM_AUTH,INFO,"URL:%s\n",_xm_epg.url);
	return _xm_epg.url;
}


static int XM_home_auth(void)
{
	GxJson_t json;

	_XMNewUrlHomeAuth();
	
	json = GetUrlJsonResult(_xm_epg.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
		return -1;
	}
	
	GxJson_Free(json);
	return TRUE;
}

static int XM_home_info(void)
{
	GxJson_t json;

	_XMNewUrlHomeInfo();
	
	json = GetUrlJsonResult(_xm_epg.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
		return -1;
	}
	GxJson_Free(json);
	return TRUE;
}


GxJson_t XM_HomeRequest(void)
{
	GxJson_t json;
	(void)XM_home_auth;
	(void)XM_home_info;
	_XMNewUrlHomeRequest();
	json = GetUrlJsonResult(_xm_epg.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
	}
	return json;
}

GxJson_t XM_CateGoryRequest(const char * domain)
{
	GxJson_t json;
	_XMNewUrlCategoryRequest(domain);
	json = GetUrlJsonResult(_xm_epg.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
	}
	return json;
}


GxJson_t XM_ProgramListRequest(const char * domain,unsigned int start, unsigned int total)
{
	GxJson_t json;
	_XMNewUrlProgramListRequest(domain,start,total);
	json = GetUrlJsonResult(_xm_epg.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
	}
	return json;
}


GxJson_t XM_ProgramDetailRequest(const char * domain)
{
	GxJson_t json;
	_XMNewUrlProgramDetailInfoRequest(domain);
	json = GetUrlJsonResult(_xm_epg.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
	}
	return json;
}


GxJson_t XM_ProgramRecommendUrlRequest(const char * domain)
{
	GxJson_t json;
	_XMNewUrlRecommendInfoRequest(domain);
	json = GetUrlJsonResult(_xm_epg.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
	}
	return json;
}


GxJson_t XM_ProgramPlayInfoRequest(const char * domain)
{
	GxJson_t json;
	_XMNewUrlPlayInfoRequest(domain);
	json = GetUrlJsonResult(_xm_epg.url);
	if (json == NULL)
	{
		DEBUG(XM_AUTH,ERR,"get info failure!\n");
	}
	return json;
}






extern BOOL xmian_check_network_stat(void);
extern BOOL get_vol_entitlement_from_ca(void);
extern void send_event_to_ui_from_authorization(u32 event);

/*

0：未开通
1：正常
2：用户半停机
3：用户全停机
4：欠费半停机

6：预拆
7：拆机
2002：欠费停机
2004：余额不足，即将停机
9995：更新usertoken失败
2001：登录失败，如密码错误

*/
static void auth_monitor(void)
{
	int ret;
	(void)FreeUrl;
	(void)XM_Logout;
	(void)XM_UpdateUserToken;
	unsigned int current_tick;
	
	while(1)
	{
		if (MONITOR_ACTIVE == 0)
		{
			mtos_task_sleep(5000);
			continue;
		}
		if (!xmian_check_network_stat())
		{
			mtos_task_sleep(2000);
			continue;
		}
		if (NC_AUTH_UNKNOW == _xm_auth.state)
		{
			DEBUG(XM_AUTH,INFO,"[auth_monitor] NC_AUTH_UNKNOW, register!\n");
			ret = XM_RegisterStb();
			if (ret != 0)
			{
				DEBUG(XM_AUTH,INFO,"[auth_monitor] NC auth register stb failure!");
				mtos_task_sleep(5000);
				continue;
			}
			_xm_auth.state = NC_AUTH_REGISTERED;
		}
		if (NC_AUTH_REGISTERED == _xm_auth.state)
		{
			DEBUG(XM_AUTH,INFO,"[auth_monitor] have registered, Login!\n");
			ret = XM_Login();
			if (ret != 0)
			{
				DEBUG(XM_AUTH,INFO,"[auth_monitor] NC auth stb login failure!");
				mtos_task_sleep(5000);
				continue;
			}
			_xm_auth.state = NC_AUTH_LOGIN;
			_xm_auth.last_beat_tick = 0;
		}
		if (NC_AUTH_LOGIN == _xm_auth.state)
		{
			current_tick = mtos_ticks_get();
			if ((current_tick - _xm_auth.last_beat_tick >= _xm_auth.beating) 
				|| (_xm_auth.last_beat_tick == 0))
			{
				ret = XM_Heartbeat();
				if (ret != 0)
				{
					_xm_auth.beatFailureCount++;
					if(_xm_auth.beatFailureCount > 3)
					{
						_xm_auth.beatFailureCount = 0;
						_xm_auth.state = NC_AUTH_UNKNOW;
					}
					DEBUG(XM_AUTH,INFO,"[auth_monitor] NC auth stb beating failure! \n");
				}
				else
				{
					_xm_auth.beatFailureCount = 0; // clear error count
					_xm_auth.caEntitlement = TRUE;
					DEBUG(XM_AUTH,INFO,"[auth_monitor] get entitlement from ca success! \n");
				}
				_xm_auth.last_beat_tick = current_tick;
			}
			/*need update token*/
			
		}
		mtos_task_sleep(2000);
	}
}

void NC_AuthStart(void)
{
	MONITOR_ACTIVE = 1;
}
void NC_AuthClose(void)
{
	MONITOR_ACTIVE = 0;
}
NC_STATUS_t NC_AuthGetState(void)
{
	if (FALSE == _xm_auth.caEntitlement)
		return NC_AUTH_BEATING_FAILURE;
	return _xm_auth.state;
}

int NC_AuthInit(const char *stbId,const unsigned char* mac,int mac_len)
{
	unsigned int *p_stack = NULL;
	int ret = 0;
	BOOL err;
	unsigned char * mem;
	ip_address_set_t p_set;
	unsigned char macaddr[6] = {0};
	u32 high,low;

	DEBUG(XM_AUTH,INFO,"NC_AuthInit call in:\n");
	if (_xm_auth.inited)
	{
		DEBUG(XM_AUTH,ERR,"  have inited,return!\n");
		return 0;
	}

	_xm_auth.inited = TRUE;
	_xm_auth.caEntitlement = TRUE;
	_xm_auth.beating = 60 * TICK_PER_SECOND;

	mem = mtos_malloc(NC_AUTH_STACK_SIZE);
	if (mem == NULL)
		return -1;
	Nw_Download_Init(NC_AUTHORIZATION_HTTP, mem,16*1024);

	macaddr[0] = 0xc;
	macaddr[1] = 0xf0;
	macaddr[2] = 0xb4;
	macaddr[3] = 0x7;
	macaddr[4] = 0x59;
	macaddr[5] = 0x2f;

	//mac address
	sprintf(_xm_auth.key,"%s","111111");
	sprintf(_xm_auth.mac,"%02X:%02X:%02X:%02X:%02X:%02X",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
	sprintf(_xm_auth.stbId,"%02x%02x%02x%02x%02x%02x",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);


	//strcpy(_xm_auth.userid,"4606000080000030");
	hal_get_chip_unique_numb(&high, &low);
	sprintf(_xm_auth.userid, "%08lx%08lx",high,low);
	DEBUG(XM_AUTH,INFO,"[debug] chip id[h:0x%08x,L0x%08x]  _xm_auth.userid = %s\n",high,low,_xm_auth.userid);
	strcpy(_xm_auth.userid,"0000091a0004d849");

	strcpy(_xm_auth.streamStatus,"1");
	strcpy(_xm_auth.type,DEFAULTTYPE);

	strcpy(_xm_auth.serverIp,DEFAULT_SERVER_ADDRESS);
	strcpy(_xm_auth.serverPort, DEFAULT_SERVER_PORT);

	strcpy(_xm_auth.hostPort, DEFAULT_HOST_PORT);
	sys_status_get_ipaddress(&p_set);
	sprintf(_xm_auth.hostIp,"%d.%d.%d.%d",p_set.sys_ipaddress.s_a1,p_set.sys_ipaddress.s_a2,p_set.sys_ipaddress.s_a3,p_set.sys_ipaddress.s_a4);
	DEBUG(XM_AUTH,INFO,"http://%d.%d.%d.%d \n",p_set.sys_ipaddress.s_a1,p_set.sys_ipaddress.s_a2,p_set.sys_ipaddress.s_a3,p_set.sys_ipaddress.s_a4);

	p_stack = (unsigned int *)mtos_malloc(NC_AUTH_STACK_SIZE);
	err = mtos_task_create((u8 *)"NC_AuthMonitor",
						(void (*)(void *))auth_monitor,
						(void *)0,
						NC_AUTHORIZATION_MONITOR,
						(u32*)p_stack,
						NC_AUTH_STACK_SIZE);
	if (!err)
	{
		DEBUG(XM_AUTH,INFO,"auth create task failure!\n");
		ret = -1;
	}
	mtos_sem_create(&_xm_epg.lock,TRUE);
	return ret;
}





