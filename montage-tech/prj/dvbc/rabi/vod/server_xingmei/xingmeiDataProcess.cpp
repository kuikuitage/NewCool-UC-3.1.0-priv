#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "mt_debug.h"
#include "sys_cfg.h"
#include "file_playback_sequence.h"
#ifndef __LINUX__
#include "list.h"
#include "drv_dev.h"
#include "block.h"
#include "fsioctl.h"
#include "ufs.h"
#else

#endif
#include "tinyxml.h"
#include "commonData.h"
#include  "tvLiveDataProvider.h"
#include "download_api.h"

#include "xingmeiDataProvider.h"
#include "sys_dbg.h"
#include "mplayer_sdk.h"
#include "lib_util.h"
#include "gx_jansson.h"
#include "xmTv.h"


#define  PRINT_MALLOC_FAILURE  		OS_PRINTF("malloc mem failure @ func: %s() line:%d\n",__FUNCTION__,__LINE__)
#define  I1B								"        "
#define  I2B							"                "
#define  I3B							"                       "


///根据需要定义这个url的格式，这个主要是为了保存参数，让下次函数调用的时候使用
/// 涉及到的函数有， iptv_get_format  / iptv_channel_info
///#define   ZGW_FAKE_PLAYURL_FORMAT	"http://www.iid%d-normal%d-jisu%d-high%d-super%d-aid%d-tid%d-wid%d-hahaha!!.html"

/*
static GxJson_t _server_get_json(const char* url)
{
	char * out_buffer = NULL;
	unsigned int out_bufferSize = 0;
	int ret = 0;
	GxJson_t json =NULL;
	ret = iptv_common_http_get_with_malloc_internal(url, NULL,NULL, 0, 5, &out_buffer, &out_bufferSize,0);
	DEBUG(XINGMEI,INFO,"get String[%s]\n",out_buffer);
	json = GxJson_LoadString(out_buffer);
	SY_FREE(out_buffer);

	return json;
}
*/
typedef struct
{
	XM_CATEGORY_LIST_T  			* category_list;
	XM_CATEGORY_TYPE_ALL_T			* category_type_all;
	XM_CHANNEL_LIST_T    			* program_list;
	XM_CHANNEL_INFO_T   			* program_info;
	XM_RECMD_INFO_T 				* recommend_info;
	//XM_AREA_INFO_LIST_T 			* area_info_list;
	XM_CHANNEL_PLAY_URL_T		* play_url;
	//XM_VIDEO_FORMAT_T				* format;



/*xm operate memory,  alloc when init and free when deinit*/
	XM_homepage						* op_homepage	;
	XM_movlist						* op_mov_list;
	XM_detail							* op_mov_detail;
	//XINGMEI_EpisodeList					* op_episode_list;
	XM_RecmdList						* op_recmd_list;
	XM_play_urlList					* op_playurl_list;


	BOOL                       		is_init;	/*whether this type of data provider has been initilized successfully*/
	BOOL							get_new_info_from_net_every_call_in;

/*current status*/
	VDO_ID_t						cur_video_id;
	char 								cur_plid[33];
    	int     							cur_chId;                          //频道id
    	int 								cur_episode_num;
}xingmei_vod_dp_priv;

static xingmei_vod_dp_priv *_ctrb;


//XM_homepage homepage;

static int _xm_get_home_cateory_entry_url(GxJson_t box,XM_homepage *p_homepage)
{
	const char *str;
	int i;
	int size = GxJson_GetArraySizeWithFmtKey(box, "category");
	DEBUG(XINGMEI,INFO,"category array size = %d\n",size);
	for (i=0;i<size;i++)
	{
		str = GxJson_GetStringWithFmtKey(box, "category[%d].name",i);
		DEBUG(XINGMEI,INFO,"category[%d].name = %s\n strlen[%d-%d]",i,str,strlen(str),strlen("entry"));
		if (strcasecmp(str,"entry") == 0)
		{
			str = GxJson_GetStringWithFmtKey(box, "category[%d].subCategoryUrl",i);
			if (str)
			{
				DEBUG(XINGMEI,INFO,"category name[%s] suburl[%s]\n","entry",str);
				strncpy(p_homepage->entry_url,str,sizeof(p_homepage->entry_url));
				return 0;
			}
			else
			{
				DEBUG(XINGMEI,INFO,"str == NULL");
				p_homepage->entry_url[0] = '\0';
			}
		}
	}
	return -1;
}




static int _xm_parse_first_category_info(GxJson_t box,XM_homepage *p_homepage)
{
	const char *str;
	int i;
	int size = GxJson_GetArraySizeWithFmtKey(box, "category");
	DEBUG(XINGMEI,INFO,"first category array size = %d\n",size);
	for(i=0;i<size;i++)
	{
		p_homepage->firstcategory[i].id=(int)GxJson_GetIntegerWithFmtKey(box,"category[%d].id",i);
		
		str=GxJson_GetStringWithFmtKey(box,"category[%d].name",i);
		if (str)
			memcpy(p_homepage->firstcategory[i].name,str,strlen(str));
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			p_homepage->firstcategory[i].name[0] = '\0';
		}
		str=GxJson_GetStringWithFmtKey(box,"category[%d].type",i);
		if (str)
			memcpy(p_homepage->firstcategory[i].type,str,strlen(str));
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			p_homepage->firstcategory[i].type[0] = '\0';
		}
		//subCategoryUrl	
		str=GxJson_GetStringWithFmtKey(box,"category[%d].subCategoryUrl",i);
		if (str)
			memcpy(p_homepage->firstcategory[i].subCategoryUrl,str,strlen(str));
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			p_homepage->firstcategory[i].subCategoryUrl[0] = '\0';
		}
		DEBUG(XINGMEI,INFO,"id[%d]name[%s],type[%d],subUrl[%s]\n",
				p_homepage->firstcategory[i].id,p_homepage->firstcategory[i].name,p_homepage->firstcategory[i].type,p_homepage->firstcategory[i].subCategoryUrl);

	}	
	p_homepage->category_size = size;
	if (p_homepage->category_size)
		return p_homepage->category_size;
	
	return -1;
}

static int _xm_parse_category_types_info(GxJson_t box,int cat_index,XM_homepage *p_homepage)
{
	const char *str;
	int i;
	int size = GxJson_GetArraySizeWithFmtKey(box, "category");
	DEBUG(XINGMEI,INFO,"first category array size = %d\n",size);
	DEBUG(XINGMEI,INFO,"tag index = %d\n",cat_index);
	for(i=0;i<size;i++)
	{
		XM_tag *tag = &p_homepage->firstcategory[cat_index].tags[i];
		tag->id = (int)GxJson_GetIntegerWithFmtKey(box,"category[%d].id",i);
		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"category[%d].name",i)))
			strncpy(tag->name,str,sizeof(tag->name));
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			tag->name[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"category[%d].type",i)))
			strncpy(tag->type,str,sizeof(tag->type));	
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			tag->type[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"category[%d].programListUrl",i)))
			strncpy(tag->programListUrl,str,sizeof(tag->programListUrl));		
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			tag->programListUrl[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"category[%d].subCategoryUrl",i)))
			strncpy(tag->subCategoryUrl,str,sizeof(tag->subCategoryUrl));			
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			tag->subCategoryUrl[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"category[%d].hpbcUrl",i)))
			strncpy(tag->hpbcUrl,str,sizeof(tag->hpbcUrl));			
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			tag->hpbcUrl[0] = '\0';
		}

		DEBUG(XINGMEI,INFO,"category type id[%d] name[%s],type[%d],subUrl[%s],proglist[%s],res[%d]size_i[%d]\n",
				tag->id,tag->name,tag->type,tag->subCategoryUrl,tag->programListUrl,cat_index,i);

	}	
	p_homepage->firstcategory[cat_index].tagSize = size;
	if (p_homepage->firstcategory[cat_index].tagSize)
		return p_homepage->firstcategory[cat_index].tagSize;
	
	return -1;
}

static int _xm_parse_program_list_info(GxJson_t box,XM_movlist*mList)
{
	const char *str;
	int i;
	int size = GxJson_GetArraySizeWithFmtKey(box, "list");
	mList->totalNum = GxJson_GetIntegerWithFmtKey(box,"totalNum");
	mList->currStartIdx = GxJson_GetIntegerWithFmtKey(box,"currStartIdx");
	DEBUG(XINGMEI,INFO,"list total[%d],curstartIndex[%d],got list size = %d\n",mList->totalNum,mList->currStartIdx,size);
	for(i=0;i<size;i++)
	{
		XM_mov *mov = &mList->data[i];
		mov->id = GxJson_GetIntegerWithFmtKey(box,"list[%d].id",i);
		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"list[%d].detailUrl",i)))
			strncpy(mov->detailUrl,str,sizeof(mov->detailUrl));
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			mov->detailUrl[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"list[%d].imageUrl",i)))
			strncpy(mov->image,str,sizeof(mov->image));	
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			mov->image[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"list[%d].name",i)))
			strncpy(mov->name ,str,sizeof(mov->name));	
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			mov->name[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"list[%d].type",i)))
			strncpy(mov->type,str,sizeof(mov->type));		
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			mov->type[0] = '\0';
		}

		
		DEBUG(XINGMEI,INFO,"move id[%d] name[%s],type[%s],image[%s],detailUrl[%s]\n",
				mov->id,mov->name,mov->type,mov->image,mov->detailUrl);
	}
	mList->validCount = size;
	if (mList->validCount >= 0)
		return mList->validCount;
	return -1;
}
static int _xm_parse_program_detail_info(GxJson_t box,XM_detail*m_detail)
{

	const char *str;
	int i;



	
	//DEBUG(XINGMEI,INFO,"list total[%d],curstartIndex[%d],got list size = %d\n",mList->totalNum,mList->currStartIdx,size);
	
	m_detail->id = GxJson_GetIntegerWithFmtKey(box,"id");
	if (NULL != (str = GxJson_GetStringWithFmtKey(box,"name")))
		strncpy(m_detail->name ,str,sizeof(m_detail->name));
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->name[0] = '\0';
	}

	if (NULL != (str = GxJson_GetStringWithFmtKey(box,"type")))
		strncpy(m_detail->type,str,sizeof(m_detail->type));	
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->type[0] = '\0';
	}

	if (NULL != (str = GxJson_GetStringWithFmtKey(box,"image")))
		strncpy(m_detail->image,str,sizeof(m_detail->image));	
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->image[0] = '\0';
	}

	if (NULL != (str =GxJson_GetStringWithFmtKey(box,"actor")))
		strncpy(m_detail->actor,str,sizeof(m_detail->actor));	
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->actor[0] = '\0';
	}

	if (NULL != (str =GxJson_GetStringWithFmtKey(box,"director")))
		strncpy(m_detail->director,str,sizeof(m_detail->director));
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->director[0] = '\0';
	}

	m_detail->duration= GxJson_GetIntegerWithFmtKey(box,"duration");
	if (NULL != (str = GxJson_GetStringWithFmtKey(box,"originalcountry")))
		strncpy(m_detail->originalcountry,str,sizeof(m_detail->originalcountry));
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->originalcountry[0] = '\0';
	}

	if (NULL != (str = GxJson_GetStringWithFmtKey(box,"description")))
		strncpy(m_detail->description,str,sizeof(m_detail->description));
	else
	{
		//DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->description[0] = '\0';
	}

	m_detail->releaseyear = GxJson_GetIntegerWithFmtKey(box,"releaseyear");
	if (NULL != (str = GxJson_GetStringWithFmtKey(box,"terminalStateUrl")))
		strncpy(m_detail->terminalStateUrl,str,sizeof(m_detail->terminalStateUrl));
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->terminalStateUrl[0] = '\0';
	}

	if (NULL != (str = GxJson_GetStringWithFmtKey(box,"recommandUrl")))
		strncpy(m_detail->recommandUrl,str,sizeof(m_detail->recommandUrl));
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->recommandUrl[0] = '\0';
	}

	if(strncmp(m_detail->type,"series",sizeof("series"))==0)
	{
		m_detail->seriesList.size= GxJson_GetArraySizeWithFmtKey(box,"seriesList");
		for(i=0;i<m_detail->seriesList.size;i++)
		{
			m_detail->seriesList.data[i].id=GxJson_GetIntegerWithFmtKey(box,"seriesList[%d].id",i);
			if (NULL != (str = GxJson_GetStringWithFmtKey(box,"seriesList[%d].name",i)))
				strncpy(m_detail->seriesList.data[i].name ,str,sizeof(m_detail->seriesList.data[i].name));	
			else
			{
				DEBUG(XINGMEI,INFO,"str == NULL");
				m_detail->seriesList.data[i].name[0] = '\0';
			}
			m_detail->seriesList.data[i].bookmarktime=GxJson_GetIntegerWithFmtKey(box,"seriesList[%d].bookmarktime",i);
			m_detail->seriesList.data[i].duration=GxJson_GetIntegerWithFmtKey(box,"seriesList[%d].duration",i);
			if (NULL != (str = GxJson_GetStringWithFmtKey(box,"seriesList[%d].bookmarkUrl",i)))
				strncpy(m_detail->seriesList.data[i].bookmarkUrl ,str,sizeof(m_detail->seriesList.data[i].bookmarkUrl));
			else
			{
				DEBUG(XINGMEI,INFO,"str == NULL");
				m_detail->seriesList.data[i].bookmarkUrl[0] = '\0';
			}
			if (NULL != (str = GxJson_GetStringWithFmtKey(box,"seriesList[%d].terminalStateUrl",i)))
				strncpy(m_detail->seriesList.data[i].terminalStateUrl ,str,sizeof(m_detail->seriesList.data[i].terminalStateUrl));
			else
			{
				DEBUG(XINGMEI,INFO,"str == NULL");
				m_detail->seriesList.data[i].terminalStateUrl[0] = '\0';
			}
		}
	}
//product

	m_detail->orderProduct= GxJson_GetIntegerWithFmtKey(box,"orderProduct");
	m_detail->bookmarktime = GxJson_GetIntegerWithFmtKey(box,"bookmarktime");		
	m_detail->favorites= GxJson_GetIntegerWithFmtKey(box,"favorites");
	if (NULL != (str = GxJson_GetStringWithFmtKey(box,"favoriteUrl")))
		strncpy(m_detail->favoriteUrl,str,sizeof(m_detail->favoriteUrl));	
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->favoriteUrl[0] = '\0';
	}

	if (NULL != (str = GxJson_GetStringWithFmtKey(box,"bookmarkUrl")))
		strncpy(m_detail->bookmarkUrl,str,sizeof(m_detail->bookmarkUrl));
	else
	{
		DEBUG(XINGMEI,INFO,"str == NULL");
		m_detail->bookmarkUrl[0] = '\0';
	}


	DEBUG(XINGMEI,INFO,"move id[%d] name[%s],type[%s],terminalStateUrl[%s]\n",
			m_detail->id,m_detail->name,m_detail->type,m_detail->terminalStateUrl);
	DEBUG(XINGMEI,INFO,"image[%s]\n",m_detail->image);

	return 0;
}
#if 1
static int _xm_parse_program_recommand_info(GxJson_t box,XM_RecmdList*m_recommand)
{
	const char *str;
	int i;
	int size = GxJson_GetArraySizeWithFmtKey(box, "recommand");
	
	DEBUG(XINGMEI,INFO,"got list size = %d\n",size);
	for(i=0;i<size;i++)
	{
		XM_recommand *recmd= &m_recommand->data[i];
		recmd->id = GxJson_GetIntegerWithFmtKey(box,"recommand[%d].id",i);
		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"recommand[%d].detailUrl",i)))
			strncpy(recmd->detailUrl,str,sizeof(recmd->detailUrl));
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			recmd->detailUrl[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"recommand[%d].imageUrl",i)))
			strncpy(recmd->image,str,sizeof(recmd->image)); 
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			recmd->image[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"recommand[%d].name",i)))
			strncpy(recmd->name ,str,sizeof(recmd->name));	
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			recmd->name[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"recommand[%d].type",i)))
			strncpy(recmd->type,str,sizeof(recmd->type));		
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			recmd->type[0] = '\0';
		}

		
		DEBUG(XINGMEI,INFO,"recmd id[%d] name[%s],type[%s],image[%s],detailUrl[%s]\n",
				recmd->id,recmd->name,recmd->type,recmd->image,recmd->detailUrl);
	}
	m_recommand->num= size;
	if (m_recommand->num >= 0)
		return m_recommand->num;
	return -1;

}
#endif
static int _xm_parse_program_play_info(GxJson_t box,XM_play_urlList*playUrl)
{

	const char *str;
	int i;
	int size = GxJson_GetArraySizeWithFmtKey(box, "urlobj");
	playUrl->terminalState = GxJson_GetIntegerWithFmtKey(box,"terminalState");
	
	DEBUG(XINGMEI,INFO,"terminalState[%d],got list size = %d\n",playUrl->terminalState,size);
	for(i=0;i<size;i++)
	{
		XM_urlobj *urlobj = &playUrl->urlobj[i];
		urlobj->id= GxJson_GetIntegerWithFmtKey(box,"urlobj[%d].id",i);
		urlobj->hdtv= GxJson_GetIntegerWithFmtKey(box,"urlobj[%d].hdtv",i);
		if (NULL != (str =GxJson_GetStringWithFmtKey(box,"urlobj[%d].name",i)))
			strncpy(urlobj->name ,str,sizeof(urlobj->name));	
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			urlobj->name[0] = '\0';
		}

		if (NULL != (str =GxJson_GetStringWithFmtKey(box,"urlobj[%d].playUrl",i)))
			strncpy(urlobj->playUrl,str,sizeof(urlobj->playUrl));		
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			urlobj->playUrl[0] = '\0';
		}

		if (NULL != (str = GxJson_GetStringWithFmtKey(box,"urlobj[%d].timeshiftUrl",i)))
			strncpy(urlobj->timeshiftUrl,str,sizeof(urlobj->timeshiftUrl));	
		else
		{
			DEBUG(XINGMEI,INFO,"str == NULL");
			urlobj->timeshiftUrl[0] = '\0';
		}

		DEBUG(XINGMEI,INFO,"urlobj hdtv[%d] name[%s],playUrl[%s],timeshiftUrl[%s]\n",
				urlobj->hdtv,urlobj->name,urlobj->playUrl,urlobj->timeshiftUrl);
	}
	playUrl->num= size;
	//if (mList->validCount >= 0)
	//	return mList->validCount;
	//return -1;


	return 0;
}




#if 0
/*for test*/
static  void *  xingmei_epg_test(void * hdl)
{
	xingmei_vod_dp_priv * priv = (xingmei_vod_dp_priv *)hdl;
	XM_homepage *p_homepage = priv->op_homepage;
	int  ret = -1;
	GxJson_t json = NULL;

	//const char* p_name;
	DEBUG(XINGMEI,INFO,"call in \n");
	DEBUG(XINGMEI,INFO,"get entry url!! \n");
	/*get first page entry*/
	json = XM_HomeRequest();
	if (json == NULL)
	{
		DEBUG(XINGMEI,ERR,"XM_HomeRequest OBJ = NULL \n");
		return NULL;
	}
	ret = _xm_get_home_cateory_entry_url(json, p_homepage); //ertry
	GxJson_Free(json);
	if (ret < 0)
	{
		DEBUG(XINGMEI,ERR,"get entry url failure \n");
		return NULL;
	}

	/*get first category(resource)*/
	DEBUG(XINGMEI,INFO,"get first category name and subcategory url!! \n");	
	json = XM_CateGoryRequest(p_homepage->entry_url);				//一级
	if (json == NULL)
	{
		DEBUG(XINGMEI,ERR,"XM_CateGoryRequest OBJ = NULL \n");
		return NULL;
	}
	ret = _xm_parse_first_category_info(json,p_homepage);
	GxJson_Free(json);
	if (ret < 0)
	{
		DEBUG(XINGMEI,ERR,"get first category failure \n");
		return NULL;
	}
	/*get sub category(cat) as types*/
	DEBUG(XINGMEI,INFO,"get second category name !! \n");
	for (int i=0;i<p_homepage->category_size;i++)
	{
		json = XM_CateGoryRequest(p_homepage->firstcategory[i].subCategoryUrl);
		if (json)
		{
			ret = _xm_parse_category_types_info(json, i, p_homepage);
			GxJson_Free(json);
			if (ret < 0)
			{
				DEBUG(XINGMEI,ERR,"get [%s]'s type failure!!! \n",p_homepage->firstcategory[i].name);
			}
			XM_Channel *ch = &p_homepage->firstcategory[i];
			/*get program list*/
			for (int ii=0;ii<ch->tagSize;ii++)
			{
				XM_movlist *mlist = priv->op_mov_list;
				json = XM_ProgramListRequest(ch->tags[ii].programListUrl, 0, 11);
				if (json)
				{
					ret = _xm_parse_program_list_info(json,mlist);
					GxJson_Free(json);					
					if (ret <= 0)
					{
						DEBUG(XINGMEI,ERR,"get [%s]'s proglist failure!!! \n",ch->name);
					}	
					/*get program detail info*/
					for (int iii=0;iii<mlist->validCount;iii++)
					{
						XM_detail *md = priv->op_mov_detail;
						json = XM_ProgramDetailRequest(mlist->data[iii].detailUrl);
						if (json)
						{
							ret = _xm_parse_program_detail_info(json,md);
							GxJson_Free(json);
							if (ret < 0)
							{
								DEBUG(XINGMEI,ERR,"get [%s]'s program detail info failure!!! \n",mlist->data[iii].name);
							}								
						}
						/*get recommen info */
						XM_RecmdList * rmd_list = priv->op_recmd_list;						
						json = XM_ProgramRecommendUrlRequest(md->recommandUrl);
						if (json)
						{
							_xm_parse_program_recommand_info(json, rmd_list);
							GxJson_Free(json);
						}

						/*get play info */
						XM_play_urlList *playUrl = priv->op_playurl_list;
						json = XM_ProgramPlayInfoRequest(md->terminalStateUrl);
						if (json)
						{
							_xm_parse_program_play_info(json,playUrl);
							GxJson_Free(json);							
						}
					}
				}

			}
		}

	}
	return 0;
}
#endif



// TODO:获取分类及其tag信息
static  void *  xingmei_get_category_list(void * hdl)
{
	xingmei_vod_dp_priv * priv = (xingmei_vod_dp_priv *)hdl;
	XM_homepage *p_homepage = priv->op_homepage;
	XM_CATEGORY_LIST_T *category_list = priv->category_list;
	int  ret = -1;
	GxJson_t obj = NULL;
	//const char* p_name;
	DEBUG(XINGMEI,INFO,"call in \n");
	DEBUG(XINGMEI,INFO,"get entry url!! \n");
	obj = XM_HomeRequest();
	if (obj == NULL)
	{
		DEBUG(XINGMEI,ERR,"XM_HomeRequest OBJ = NULL \n");
		return NULL;
	}
	ret = _xm_get_home_cateory_entry_url(obj, p_homepage); //ertry
	GxJson_Free(obj);
	if (ret < 0)
	{
		DEBUG(XINGMEI,ERR,"get entry url failure \n");
		return NULL;
	}
	
	DEBUG(XINGMEI,INFO,"get first category name and subcategory url!! \n");	
	obj = XM_CateGoryRequest(p_homepage->entry_url);				//一级
	if (obj == NULL)
	{
		DEBUG(XINGMEI,ERR,"XM_CateGoryRequest OBJ = NULL \n");
		return NULL;
	}
	ret = _xm_parse_first_category_info(obj,p_homepage);
	GxJson_Free(obj);
	if (ret < 0)
	{
		DEBUG(XINGMEI,ERR,"get first category failure \n");
		return NULL;
	}
	DEBUG(XINGMEI,INFO,"get second category name !! \n");
	for (int i=0;i<p_homepage->category_size;i++)
	{
		obj = XM_CateGoryRequest(p_homepage->firstcategory[i].subCategoryUrl);
		if (obj)
		{
			ret = _xm_parse_category_types_info(obj, i, p_homepage);
			GxJson_Free(obj);
			if (ret < 0)
			{
				DEBUG(XINGMEI,ERR,"get [%s]'s type failure!!! \n",p_homepage->firstcategory[i].name);
			}
		}

	}
	/* alloc all category buffer and get data*/
	category_list->array = (XM_Channel *)SY_CALLOC(1,sizeof(XM_Channel) * p_homepage->category_size);
	if (category_list->array == NULL)
	{
		PRINT_MALLOC_FAILURE;
		return NULL;
	}
	category_list->total = p_homepage->category_size;
	//category_list->identify = ;
	category_list->status =IPTV_DATA_SUCCESS;
	DEBUG(XINGMEI,INFO,"total[%d]\n",category_list->total);
	for(int i=0;i<category_list->total;i++)
	{
		category_list->array[i] = p_homepage->firstcategory[i];
		DEBUG(XINGMEI,INFO,"index = %d name = %s tags number = %d\n",i,category_list->array[i].name,category_list->array[i].tagSize);
		for(int c = 0;c<category_list->array[i].tagSize;c++)
			DEBUG(XINGMEI,INFO,"id = %d name = %s\n",category_list->array[i].tags[c].id,category_list->array[i].tags[c].name);
	}
	DEBUG(XINGMEI,INFO,"\n");
	return category_list;

}



// TODO: tag 在获取分类时已经传递，这个函数暂时不用
static  void *  xingmei_get_category_type_list(void * hdl, int cat_id)
{
	xingmei_vod_dp_priv * priv = (xingmei_vod_dp_priv *)hdl;
	XM_CATEGORY_TYPE_ALL_T *category_type = priv->category_type_all;

	DEBUG(XINGMEI,ERR,"\n");

	MT_ASSERT(category_type != NULL);
	MT_ASSERT(priv->category_list != NULL);
	DEBUG(XINGMEI,INFO,"\n");

	category_type->category 	= priv->category_list;
	category_type->status		= IPTV_DATA_SUCCESS;
	return priv->category_type_all;
}


static  void * xingmei_get_program_page_by_label(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	xingmei_vod_dp_priv * priv = (xingmei_vod_dp_priv *)hdl;
	XM_movlist *p_playlist = priv->op_mov_list;
	XM_CHANNEL_LIST_T	* program_list = priv->program_list;
	int  ret = -1;
	char *url = NULL;
	
	DEBUG(XINGMEI,INFO,"\n");
 	if (program_list->array != NULL && program_list->page_size < p_req->page_size)  
	{
		SY_FREE(program_list->array);
		program_list->array = NULL;
	}
	program_list->category_id = p_req->cat_id;
	program_list->page_num = p_req->page_index;
	program_list->page_size = p_req->page_size;
  	for(int x=0;x < priv->category_list->total;x++)
	{
		if(priv->category_list->array[x].id == p_req->cat_id)
		{
			DEBUG(XINGMEI,INFO,"tagsize=[%d]x=[%d]\n",priv->category_list->array[x].tagSize,x);
			for(int y=0;y < priv->category_list->array[x].tagSize;y++)
			{
				if(priv->category_list->array[x].tags[y].id == p_req->cat_id2) 
				{
					url = (char *)&priv->category_list->array[x].tags[y].programListUrl;
					DEBUG(XINGMEI,INFO,"x=[%d]y=[%d],url = %s\n",x,y,url);	
					break;
				}
			}
			break;
		}
	}
	if(url == NULL)
	{
		DEBUG(XINGMEI,ERR,"cat_id or type_id err!cant find in list!!\n");
		return NULL;
	}

	GxJson_t json = XM_ProgramListRequest(url, (program_list->page_num-1)*program_list->page_size, 
											(program_list->page_num)*program_list->page_size);
	if (json)
	{
		ret = _xm_parse_program_list_info(json,p_playlist);
		GxJson_Free(json);
		if (ret <= 0)
		{
			DEBUG(XINGMEI,ERR,"get cat_id[%d]type_id[%d]'s proglist failure!!! \n",p_req->cat_id,p_req->cat_id2);
			return NULL;
		}	
	}
	DEBUG(XINGMEI,INFO,"%d\n",p_playlist->validCount);
	if (p_playlist->validCount == 0)
	{
		DEBUG(XINGMEI,ERR,"get program list count = 0!\n");
		return NULL;
	}

	if (program_list->array == NULL)
		program_list->array = (XM_mov*)SY_CALLOC(1,sizeof(program_list->array[0]) * p_req->page_size);
	if (program_list->array == NULL)
	{
		OS_PRINTF("malloc mem failure @ func: %s() line:%d\n",__FUNCTION__,__LINE__);
		return NULL;
	}
	
	program_list->totalcount= p_playlist->totalNum;
	program_list->number = p_playlist->validCount;
	program_list->pagecount = (program_list->totalcount / program_list->page_size + 1) ;
	DEBUG(XINGMEI,ERR,"totalcount [%d]number[%d]!!! \n",program_list->totalcount,program_list->number);
	
	for(int i=0;i<program_list->number ;i++)
	{
		program_list->array [i] = p_playlist->data[i];
	}
	program_list->status = IPTV_DATA_SUCCESS;
	program_list->identify = p_req->identify;
	DEBUG(XINGMEI,INFO,"identify[%d]\n",p_req->identify);
  	return (void *)program_list;	
}
//tmp
static  void * xingmei_get_program_page_by_screening(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	return NULL;
}


static  void * xingmei_get_program_page_by_search(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	return NULL;
}

static  void * xingmei_get_search_word_by_pinyin(void * hdl, void * pinYin)
{
	return NULL;
}


static  void * xingmei_get_new_program_page(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	//xingmei_vod_dp_priv * priv = (xingmei_vod_dp_priv *)hdl;
	//priv->cur_chId = p_req->cat_id;
	if (p_req->query_mode == IPTV_QUERY_MODE_CATGRY)
		return xingmei_get_program_page_by_label(hdl, p_req);
	else if (p_req->query_mode == IPTV_QUERY_MODE_SCREENING)
		return xingmei_get_program_page_by_screening(hdl, p_req);
	else
		return xingmei_get_program_page_by_search(hdl, p_req);
	return NULL;
}


// TODO: 获取节目页
static  void * xingmei_get_program_page(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	return NULL;
}


static void * xingmei_get_program_detail_info(void * hdl, IPTV_VIDEO_INFO_REQ_T * p_req)
{
#if 1
	xingmei_vod_dp_priv * priv = (xingmei_vod_dp_priv *)hdl;
	XM_detail *mov_detail = priv->op_mov_detail;
	int err;
	int c;
	char *url = NULL;
	char type[32];
	MT_ASSERT(priv != NULL);
	MT_ASSERT(mov_detail != NULL);
	MT_ASSERT(priv->program_info != NULL);
	
	DEBUG(XINGMEI,INFO,"program_id[%d]\n",p_req->id.program_id);
	DEBUG(XINGMEI,INFO,"number[%d]\n",priv->program_list->number);
#if 0 
	if(p_req->id.type == 1)
		memcpy(&type,"Program",sizeof("Program"));
	else if(p_req->id.type == 2)
		memcpy(&type,"series",sizeof("series"));
	else
	{
		DEBUG(XINGMEI,ERR,"get  ERR TYPE\n");
		return NULL;
	}
#endif
	for(int z=0;z < priv->program_list->number;z++)
	{
		DEBUG(XINGMEI,INFO,"detail url =[%s]z=[%d]\n",priv->program_list->array[z].detailUrl,z);
		DEBUG(XINGMEI,INFO,"id =[%d]\n",priv->program_list->array[z].id);
	}

 	for (int i=0;i < priv->program_list->number;i++)
 	{
		if((priv->program_list->array[i].id == p_req->id.program_id)&&(strcmp(priv->program_list->array[i].type,type)==0))
		{
			url = priv->program_list->array[i].detailUrl;
			break;
		}
	}
	//DEBUG(XINGMEI,INFO,"url =[%s]\n",url);
	// //TODO: TEMP
	//url = priv->program_list->array[0].detailUrl;  // panhui temp
	DEBUG(XINGMEI,INFO,"\n url =[%s]\n",url);

	if (priv->program_info->mov_detail != NULL )  
	{
		SY_FREE(priv->program_info->mov_detail);
		priv->program_info->mov_detail = NULL;
	}

	priv->program_info->mov_detail = (XM_detail*)SY_CALLOC(1,sizeof(*mov_detail) );

	if(priv->program_info->mov_detail ==NULL)
	{
		OS_PRINTF("malloc mem failure @ func: %s() line:%d\n",__FUNCTION__,__LINE__);
		return NULL;
	}


	priv->cur_video_id = p_req->id;
	
	DEBUG(XINGMEI,INFO,"\n");
#if 1
	for (c=0,err = -1;c<2 && err != XM_NORMAL;c++)	
	{				
		GxJson_t json = XM_ProgramDetailRequest(url);
		if (json)
		{
			err = _xm_parse_program_detail_info(json,mov_detail);
		}
	}
	if (err != XM_NORMAL)
	{	
		DEBUG(XINGMEI,ERR,"get detail info fail\n");
		return NULL;
	}
#endif
	DEBUG(XINGMEI,INFO,"terminalStateUrl[%s]\n",mov_detail->terminalStateUrl);
	
	//*priv->program_info->mov_detail = *mov_detail;
	memcpy(priv->program_info->mov_detail,mov_detail,sizeof(XM_detail));

	if (strcmp(priv->program_info->mov_detail->type,"series") != 0)
	{
		DEBUG(XINGMEI,INFO,"album type == XINGMEI_TYPE_MOVE!just return album\n");
		priv->program_info->status = IPTV_DATA_SUCCESS;
		priv->program_info->is_description = 1;

		/* get count*/
		priv->program_info->episode_total 		= 0;
		priv->program_info->this_page_index	= p_req->page_index;
		priv->program_info->this_page_size		= 0;

		return priv->program_info;		
	}
	//连续剧
	DEBUG(XINGMEI,INFO,"\n");
	priv->program_info->episode_total 		= mov_detail->seriesList.size;
	priv->program_info->this_page_index	= p_req->page_index;
	priv->program_info->this_page_size		= p_req->page_size;
	priv->program_info->status = IPTV_DATA_SUCCESS;
	priv->program_info->is_description = 1;
	
	DEBUG(XINGMEI,INFO,"\n");
	return (void *)priv->program_info;
#endif 
	return NULL;

}

static void *xingmei_get_url(void * hdl, IPTV_VIDEO_URL_REQ_T * p_req)
{
	xingmei_vod_dp_priv * priv = (xingmei_vod_dp_priv *)hdl;
	XM_play_urlList *p_playurl_list = priv->op_playurl_list;
	XM_CHANNEL_PLAY_URL_T *p_play_url = priv->play_url;
	XM_detail *mov_detail = priv->op_mov_detail;
	int i;
	char * url =mov_detail->terminalStateUrl;
	int c;
	int err;



	DEBUG(XINGMEI,INFO,"url=[%s]\n",url);
	for (c=0,err = -1;c<2 && err != XM_NORMAL;c++)	
	{
		GxJson_t json = XM_ProgramPlayInfoRequest(url);
			if (json)
			{
				err = _xm_parse_program_play_info(json,p_playurl_list);
				GxJson_Free(json);							
			}
	}
	if (err != XM_NORMAL)
	{
		
		return NULL;
	}
	DEBUG(XINGMEI,INFO,"\n");

	p_play_url->number = p_playurl_list->num;

	if (p_play_url->playurls == NULL)
	{
		p_play_url->playurls = (XM_urlobj*)SY_CALLOC(1, sizeof(*p_play_url->playurls) * p_play_url->number );
		if (p_play_url->playurls == NULL)
		{
			DEBUG(XINGMEI,INFO,"malloc failure!\n");
			return NULL;
		}
	}

	p_play_url->terminalState = p_playurl_list->terminalState;
	
	for(i=0;i<p_play_url->number;i++)
	{
		p_play_url->playurls[i] = p_playurl_list->urlobj[i];
	}

	p_play_url->format =p_req->format;//???
	p_play_url->identify = p_req->identify;
	p_play_url->status = IPTV_DATA_SUCCESS;
	DEBUG(XINGMEI,INFO,"\n");
	return (void *)p_play_url;

}


static void  _xingmei_get_format(void * hdl,XM_VIDEO_FORMAT_T* format,const char *tvQid,const char *vid)
{
	return;
}

static void* xingmei_get_format(void * hdl, IPTV_VIDEO_URL_REQ_T * p_req)
{
	return NULL;
}


static void *xingmei_get_recommend_info(void * hdl,  IPTV_RECOMMEND_REQ_T * p_req)
{
	xingmei_vod_dp_priv * priv = (xingmei_vod_dp_priv *)hdl;
	int err = -1;
	int i;
	//int c;
	int count = 0;
	XM_RecmdList *p_recomdlist=priv->op_recmd_list;
	XM_detail *mov_detail = priv->op_mov_detail;
	//char * url = NULL;
	DEBUG(XINGMEI,INFO,"\n");

	if (priv->recommend_info->recmd == NULL)
	{
		priv->recommend_info->recmd = (XM_recommand*)SY_CALLOC(1,sizeof(priv->recommend_info->recmd[0])*RECMD_MAX_NUMBER);
		if (priv->recommend_info->recmd == NULL)
		{
			DEBUG(XINGMEI,ERR,"malloc failure!\n");
			return NULL;
		}
	}


	DEBUG(XINGMEI,INFO,"recommandurl[%s]\n",mov_detail->recommandUrl);
	/*get recommen info */
							
	GxJson_t json = XM_ProgramRecommendUrlRequest(mov_detail->recommandUrl);
	if (json)
	{
		err = _xm_parse_program_recommand_info(json, p_recomdlist);
		GxJson_Free(json);
	}
	if (err < 0)
	{
		DEBUG(XINGMEI,ERR,"getRelatedRecommendAlbumList faliure! err[%d]\n",err);
		return NULL;
	}

	//DEBUG(XINGMEI,INFO,"get total[%d],size[%d]\n",priv->op_album_list->total,priv->op_album_list->size);

	count = p_recomdlist->num > RECMD_MAX_NUMBER?RECMD_MAX_NUMBER: p_recomdlist->num;
	for (i=0;i<count;i++)
	{
		//DEBUG(XINGMEI,INFO,"name[%s]\n",priv->op_album_list->data[i].name);
	}
	priv->recommend_info->totalcount = count;
	priv->recommend_info->channel_num= count;	
	priv->recommend_info->pagecount = count;
	priv->recommend_info->identify     = p_req->identify;
	priv->recommend_info->status	 = IPTV_DATA_SUCCESS;
	priv->recommend_info->have_geted = 1;

	for (i=0;i<priv->recommend_info->channel_num;i++)
			{
				priv->recommend_info->recmd[i] = p_recomdlist->data[i];
				DEBUG(XINGMEI,INFO,"name[%s]\n",priv->recommend_info->recmd[i].name);
			}
	return priv->recommend_info;
}


extern "C"  VDO_ID_t *xingmei_get_current_video_id(void)
{
	return NULL;
}
 
extern "C"  int	xingmei_get_current_channel_id(void)
{
	return 0;
}

extern "C"  char* xingmei_get_current_plid(void)
{
	return NULL;
}

extern "C"  int xingmei_get_current_episode_num(void)
{
	return 0;
}


#ifdef __cplusplus
extern "C" {
#endif 

#ifdef __cplusplus
}
#endif 

extern "C"  int xingmei_lib_init(void)
{
	return 0;
}


int xingmei_iptv_dp_init(XM_DP_HDL_T * hdl)
{
	(void)xingmei_get_program_page_by_label;
	(void)xingmei_get_program_page_by_screening;
	(void)xingmei_get_program_page_by_screening;
	(void)xingmei_get_program_page_by_search;
	(void)xingmei_get_program_page;
	(void)_xingmei_get_format;


	xingmei_vod_dp_priv *priv = NULL;

	/*init priv data mem and clean to 0 */
	priv = (xingmei_vod_dp_priv *)SY_CALLOC(1,sizeof(xingmei_vod_dp_priv));
	MT_ASSERT(priv != NULL);

	priv->category_list =  (XM_CATEGORY_LIST_T*)SY_CALLOC(1,sizeof (*priv->category_list));
	MT_ASSERT(priv->category_list != NULL);

	priv->category_type_all	=  (XM_CATEGORY_TYPE_ALL_T*)SY_CALLOC(1,sizeof (*priv->category_type_all));
	MT_ASSERT(priv->category_type_all != NULL);
	
	priv->program_list =  (XM_CHANNEL_LIST_T*)SY_CALLOC(1,sizeof (*priv->program_list));
	MT_ASSERT(priv->program_list != NULL);
	
	priv->program_info =  (XM_CHANNEL_INFO_T*)SY_CALLOC(1,sizeof (*priv->program_info));
	MT_ASSERT(priv->program_info != NULL);

	priv->recommend_info =  (XM_RECMD_INFO_T*)SY_CALLOC(1,sizeof (*priv->recommend_info));
	MT_ASSERT(priv->recommend_info != NULL);

	priv->play_url =  (XM_CHANNEL_PLAY_URL_T*)SY_CALLOC(1,sizeof (*priv->play_url));
	MT_ASSERT(priv->play_url != NULL);


	//-------------------------------------------------------------
	

	
	priv->op_homepage =  (XM_homepage*)SY_CALLOC(1,sizeof (*priv->op_homepage));
	MT_ASSERT(priv->op_homepage != NULL);

	priv->op_mov_list =  (XM_movlist*)SY_CALLOC(1,sizeof (*priv->op_mov_list));
	MT_ASSERT(priv->op_mov_list != NULL);

	priv->op_mov_detail =  (XM_detail*)SY_CALLOC(1,sizeof (*priv->op_mov_detail));
	MT_ASSERT(priv->op_mov_detail != NULL);

	priv->op_recmd_list =  (XM_RecmdList*)SY_CALLOC(1,sizeof (*priv->op_recmd_list));
	MT_ASSERT(priv->op_recmd_list != NULL);

	priv->op_playurl_list =  (XM_play_urlList*)SY_CALLOC(1,sizeof (*priv->op_playurl_list));	
	MT_ASSERT(priv->op_playurl_list != NULL);
	/* init call back function */
	hdl->get_category_list 		= (get_category_list_func )xingmei_get_category_list;
	hdl->get_category_type_list	= (get_category_type_list_func)xingmei_get_category_type_list;
	hdl->update_page 			= (update_page_func )xingmei_get_new_program_page;
	hdl->get_recommend_info 	= (get_recommend_info_func )xingmei_get_recommend_info;
	hdl->get_video_info 		= (get_video_info_func )xingmei_get_program_detail_info;
	hdl->get_playurl 			= (get_playurl_func )xingmei_get_url;
	hdl->get_playurl_format 	= (get_playurl_format_func )xingmei_get_format;
	hdl->get_area = NULL;
	hdl->get_search_word		= (get_search_word_func )xingmei_get_search_word_by_pinyin;

	priv->is_init = TRUE ;

	hdl->priv = (void *)priv;
	_ctrb = priv;

	
	return 0;
}


int xingmei_iptv_dp_deinit(XM_DP_HDL_T * hdl)
{
	xingmei_vod_dp_priv *priv = (xingmei_vod_dp_priv *)hdl->priv;
	DEBUG(IPTVDP,INFO,"\n");
	if(priv == NULL)
		return 0;

	if (priv->category_list->array != NULL)
	{
		SY_FREE(priv->category_list->array);
		priv->category_list->array = NULL;
	}

	
	if(priv->category_list != NULL)
	{
		SY_FREE(priv->category_list);
		priv->category_list = NULL;
	}

	if(priv->category_type_all != NULL)
	{
		SY_FREE(priv->category_type_all);
		priv->category_type_all = NULL;
	}

	if(priv->program_list != NULL)
	{
		SY_FREE(priv->program_list);
		priv->program_list = NULL;
	}
	
	if(priv->program_info != NULL)
		{
			SY_FREE(priv->program_info);
			priv->program_info = NULL;
		}

	if(priv->recommend_info != NULL)
		{
			SY_FREE(priv->recommend_info);
			priv->recommend_info = NULL;
		}









	if(priv->op_homepage != NULL)
	{
		SY_FREE(priv->op_homepage);
		priv->op_homepage = NULL;
	}

	if(priv->op_mov_list != NULL)
	{
		SY_FREE(priv->op_mov_list);
		priv->op_mov_list = NULL;
	}

	if(priv->op_mov_detail != NULL)
	{
		SY_FREE(priv->op_mov_detail);
		priv->op_mov_detail = NULL;
	}
	
	if(priv->op_recmd_list != NULL)
	{
		SY_FREE(priv->op_recmd_list);
		priv->op_recmd_list = NULL;
	}

	
	if(priv != NULL)
	{
		SY_FREE(priv);
		priv = NULL;
	}

	return 0;
}




