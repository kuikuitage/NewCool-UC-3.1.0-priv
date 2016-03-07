
#include <string.h>
#include "sys_types.h"
#include "mtos_misc.h"
#include <stdio.h>
#include "sys_define.h"
#include "ui_common.h"
#include "download_api.h"
#include "lib_unicode.h"
#include "gx_jansson.h"
#include "drv_dev.h"
#include "ufs.h"
#include "mdl.h"
#include "authorization.h"
#include "ap_cas.h"
#include "time.h"
#include "sys_dbg.h"

struct  iptv_common_interface_block{
	int http_inited;
};

static struct iptv_common_interface_block iptv_common;


int iptv_common_porting_http_get(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char* buffer,              //response
                     unsigned int bufferSize,
                     BOOL if_http_gzip)
{
	int ret;
	ufs_file_t fd = {0};
	u16 unicode[48] = {0};
	u32 len;
	u8 err;
	u8 file_name[40] = {0};
	unsigned long s,ms,us;
	mtos_systime_get(&s,&ms,&us);
	
	sprintf((char*)file_name,"r:rcv%ld%03ld%03ld",s,ms,us);
	OS_PRINTF("get url[%s]:\r\n############file_name[%s]\r\n",url,file_name);
	str_asc2uni(file_name, unicode);
	if (0 ==  iptv_common.http_inited)
	{
		unsigned char * mem = SY_MALLOC(1*1024);
		Nw_Download_Init_Download_Manager();
		Nw_Download_Init(TASK_AIQIYI_HTTP, mem,1*1024);
		iptv_common.http_inited = 1;
	}

	if (if_http_gzip)
	{
		if (strncasecmp(url,"https",5) != 0)
			ret = Nw_DownloadURLTimeout_Gzip(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
		else
			ret = Nw_DownloadURLTimeout(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
	}
	else
	{
		ret = Nw_DownloadURLTimeout(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
	}
	if (ret <= 0)
	{
		OS_PRINTF("get server resp failure!\n");
		ufs_delete(unicode,0);		
		return -1;
	}

	err = ufs_open(&fd, (tchar_t*)unicode, UFS_READ);
	if (err != UFS_OK)
	{
	    ufs_delete(unicode,0);
	    return -1;
	}

	ufs_read(&fd,buffer, bufferSize, &len);
	ufs_close(&fd);
	ufs_delete(unicode,0);

	return len;
}


int iptv_common_http_get_with_malloc_internal(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char** out_buffer,              //response
                     unsigned int *out_bufferSize,
                     BOOL if_http_gzip)
{
	int ret;
	ufs_file_t fd = {0};
	u16 unicode[48] = {0};
	u32 len;
	u8 err;
	u8 file_name[40] = {0};
	int file_len = 0;
	int time_out_array[4] = {5,5,10,10};
	int i;
	int *buf_tmp;
	unsigned long s,ms,us;
	mtos_systime_get(&s,&ms,&us);
	
	sprintf((char*)file_name,"r:rcv%ld%03ld%03ld",s,ms,us);
	str_asc2uni(file_name, unicode);
	OS_PRINTF("get url mac[%s]:\r\n############file_name[%s]\r\n",url,file_name);

	if (0 ==  iptv_common.http_inited)
	{
		unsigned char * mem = SY_MALLOC(1*1024);
		Nw_Download_Init_Download_Manager();
		Nw_Download_Init(TASK_AIQIYI_HTTP, mem,1*1024);
		iptv_common.http_inited = 1;

	}
	for (i=0;i<ARRAY_SIZE(time_out_array);i++)
	{
		if (if_http_gzip)
		{
			if (strncasecmp(url,"https",5) != 0)
				ret = Nw_DownloadURLTimeout_Gzip(url, file_name, time_out_array[i], NULL, NULL, extraHeaders, body, bodyLen);
			else
				ret = Nw_DownloadURLTimeout(url, file_name, time_out_array[i], NULL, NULL, extraHeaders, body, bodyLen);
		}
		else
		{
			ret = Nw_DownloadURLTimeout(url, file_name, time_out_array[i], NULL, NULL, extraHeaders, body, bodyLen);
		}
		if (ret <= 0)
		{
			OS_PRINTF("get server resp failure!\n");
		       ufs_delete(unicode,0);		
		}
		else
		{
			i = -1;
			break;
		}
	}
	if (i != -1)
	{
		OS_PRINTF("http get failure 5 times!\n");
		return -1;
	}

	err = ufs_open(&fd, (tchar_t*)unicode, UFS_READ);
	if (err != UFS_OK)
	{
	    ufs_delete(unicode,0);
	    return -1;
	}

	file_len = fd.file_size;
	buf_tmp = SY_CALLOC(1,file_len+4);
	if (buf_tmp == NULL)
	{
	       ufs_delete(unicode,0);
		return -1;
	}
	ufs_read(&fd,buf_tmp, file_len, &len);
	*out_bufferSize = file_len;
	*out_buffer = (void*)buf_tmp;
	ufs_close(&fd);
	ufs_delete(unicode, 0);	
	return len;
}


int iptv_common_http_get_with_malloc_internal_with_no_loop(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char** out_buffer,              //response
                     unsigned int *out_bufferSize,
                     BOOL if_http_gzip)
{
	int ret;
	ufs_file_t fd = {0};
	u16 unicode[48] = {0};
	u32 len;
	u8 err;
	u8 file_name[40] = {0};
	int file_len = 0;
	int *buf_tmp;
	unsigned long s,ms,us;
	mtos_systime_get(&s,&ms,&us);
	
	sprintf((char*)file_name,"r:rcv%ld%03ld%03ld",s,ms,us);
	str_asc2uni(file_name, unicode);
	OS_PRINTF("get url mac no loop[%s]:\r\n############file_name[%s]\r\n",url,file_name);

	if (0 ==  iptv_common.http_inited)
	{
		unsigned char * mem = SY_MALLOC(1*1024);
		Nw_Download_Init_Download_Manager();
		Nw_Download_Init(TASK_AIQIYI_HTTP, mem,1*1024);
		iptv_common.http_inited = 1;

	}
	if (if_http_gzip)
	{
		if (strncasecmp(url,"https",5) != 0)
			ret = Nw_DownloadURLTimeout_Gzip(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
		else
			ret = Nw_DownloadURLTimeout(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
	}
	else
	{
		ret = Nw_DownloadURLTimeout(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
	}
	
	if (ret <= 0)
	{
		OS_PRINTF("get server resp failure!\n");
	       ufs_delete(unicode,0);		
		return -1;
	}

	err = ufs_open(&fd, (tchar_t*)unicode, UFS_READ);
	if (err != UFS_OK)
	{
	    ufs_delete(unicode,0);
	    return -1;
	}

	file_len = fd.file_size;
	buf_tmp = SY_CALLOC(1,file_len+4);
	if (buf_tmp == NULL)
	{
	       ufs_delete(unicode,0);
		return -1;
	}
	ufs_read(&fd,buf_tmp, file_len, &len);
	*out_bufferSize = file_len;
	*out_buffer = (void*)buf_tmp;
	ufs_close(&fd);
	ufs_delete(unicode, 0);	
	return len;
}



///
int iptv_common_http_post_with_malloc_internal(const char* url,
                                           const char* postBody,
                                           unsigned int postBodyLen,
                                           const char* extraHeaders,
                                           unsigned int timeout,
                                           char** out_buffer,              //response
                                           unsigned int *out_bufferSize,
                                           BOOL if_http_gzip)
{

	int ret;
	ufs_file_t fd = {0};
	u16 unicode[48] = {0};
	u8 err;
	u8 file_name[40] = {0};
	int file_len = 0;
	int *buf_tmp;
	u32 len;
	unsigned long s,ms,us;
	mtos_systime_get(&s,&ms,&us);
	
	sprintf((char*)file_name,"r:snd%ld%03ld%03ld",s,ms,us);
	OS_PRINTF("post url[%s]:\r\n############file_name[%s]\r\n",url,file_name);
	str_asc2uni(file_name, unicode);

	if (0 ==  iptv_common.http_inited)
	{
		unsigned char * mem = SY_MALLOC(1*1024);
		Nw_Download_Init_Download_Manager();
		Nw_Download_Init(TASK_AIQIYI_HTTP, mem,1*1024);
		iptv_common.http_inited = 1;

	}


	if (if_http_gzip)
	{
		if (strncasecmp(url,"https",5) != 0)
			ret = Nw_DownloadURL_POST_Gzip(url, postBody, postBodyLen, file_name, extraHeaders, NULL, NULL, timeout);
		else
			ret = Nw_DownloadURL_POST_ex(url, postBody, postBodyLen, file_name, extraHeaders, NULL, NULL, timeout);
	}
	else
	{
		ret = Nw_DownloadURL_POST_ex(url, postBody, postBodyLen, file_name, extraHeaders, NULL, NULL, timeout);
	}
	
	if (ret <= 0)
	{
		OS_PRINTF("post get server resp failure!\n");
	       ufs_delete(unicode,0);
		return -1;
	}

	err = ufs_open(&fd, (tchar_t*)unicode, UFS_READ);
	if (err != UFS_OK)
	{
	       ufs_delete(unicode,0);
		return -1;
	}
	file_len = fd.file_size;
	buf_tmp = SY_CALLOC(1,file_len+4);
	if (buf_tmp == NULL)
	{
	       ufs_delete(unicode,0);
		return -1;
	}
	ufs_read(&fd,buf_tmp, file_len, &len);
	*out_bufferSize = file_len;
	*out_buffer = (void*)buf_tmp;
	ufs_close(&fd);
	ufs_delete(unicode, 0);
	return len;
}





