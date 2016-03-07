/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"
#include "commonData.h"
#include "xingmei_api.h"
#include "xingmeiDataProvider.h"
#include "ui_iptv_prot.h"

//#include "lib_char.h"
#if ENABLE_NETWORK
#include "ui_iptv_api.h"

u16 xm_evtmap(u32 event);

extern iconv_t g_cd_utf8_to_utf16le;
extern iconv_t g_cd_utf16le_to_utf8;

static u16 chrn (char *str, char c)
{
    u16 cnt = 0;
    while (*str != '\0')
    {
        if (*str == c)
        {
            cnt++;
        }
        str++;
    }
    return cnt;
}

static void set_vdo_id(VDO_ID_t *id,int res_id,int cat_id,int pg_id,int type)
{
	id->type = type;
	id->res_id = res_id;
	id->cat_id = cat_id;
	id->program_id = pg_id;
	
	memset (id->tvQid,0,sizeof (id->tvQid));
	memset (id->qpId,0,sizeof (id->qpId));
	memset (id->sourceCode,0,sizeof (id->sourceCode));
	
}

static al_iptv_name_res_list_t *xm_gen_name_res_list(XM_CATEGORY_LIST_T *p_param)
{
	al_iptv_name_res_list_t *p_data = NULL;
    XM_Channel *p_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

	if(NULL == p_param)
		return NULL;

	DEBUG(XM_GEN,INFO,"[RES IN]total res = %d\n",p_param->total);
	for(i = 0 ;i < p_param->total; i++)
	{
		DEBUG(XM_GEN,INFO,"[RES IN]program_id[%d] = %d\n",i,p_param->array[i].id);
		DEBUG(XM_GEN,INFO,"[RES IN]type[%d] = %s\n",i,p_param->array[i].type);
		DEBUG(XM_GEN,INFO,"[RES IN]name[%d] = %s\n",i,p_param->array[i].name);
	}
	
	if ((p_param->array != NULL) && (p_param->total > 0))
    {
        p_data = (al_iptv_name_res_list_t *)SY_MALLOC(sizeof(al_iptv_name_res_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_res = (u16)p_param->total;
		p_data->resList = (al_iptv_name_res_item_t *)SY_MALLOC(p_data->total_res * sizeof(al_iptv_name_res_item_t));
		MT_ASSERT(p_data->resList != NULL);
        memset(p_data->resList, 0, p_data->total_res * sizeof(al_iptv_name_res_item_t));
		for (i = 0; i < p_data->total_res; i++)
        {
            p_item = p_param->array + i;
	     	p_data->resList[i].res_id = p_item->id;
            if (p_item->name != NULL)
            {
                src_len = strlen(p_item->name) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->name;
                outbuf = (char *)SY_CALLOC(1,dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->resList[i].name = (u16 *)outbuf;
				iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
           }
        }
    }

	DEBUG(XM_GEN,INFO,"[RES OUT]total res = %d\n",p_data->total_res);
	for(i = 0 ;i < p_data->total_res; i++)
	{
		DEBUG(XM_GEN,INFO,"[RES OUT]id[%d] = %d\n",i,p_data->resList[i].res_id);
		DEBUG(XM_GEN,INFO,"[RES OUT]name[%d] = %s\n",i,p_data->resList[i].name);
	}
	
    return p_data;
}
 
static void xm_free_name_res_list(u32 param)
{
	al_iptv_name_res_list_t *p_data = (al_iptv_name_res_list_t *)param;
	u16 i;

	if (p_data)
	{
		if (p_data->resList)
		{
			for (i = 0; i < p_data->total_res; i++)
			{
				if (p_data->resList[i].name)
				{
					SY_FREE(p_data->resList[i].name);
				}
			}
			SY_FREE(p_data->resList);
		}
		SY_FREE(p_data);
	}
}

static al_iptv_cat_res_list_t *xm_gen_cat_res_list(XM_CATEGORY_TYPE_ALL_T *p_param)
{
	al_iptv_cat_res_list_t *p_data = NULL;
	XM_tag *xm_subtag;
    XM_Channel * xm_channel;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i,j;

	XM_CATEGORY_LIST_T * category_list = p_param->category;
	DEBUG(XM_GEN,INFO,"[CAT IN] cat counts =  %d\n",category_list->total);
	for(i=0; i<category_list->total; i++)
	{
		DEBUG(XM_GEN,INFO,"[CAT IN]CAT index = %d name = %s tags number = %d\n",i,category_list->array[i].name,category_list->array[i].tagSize);
		for(j = 0;j < category_list->array[i].tagSize; j++)
			DEBUG(XM_GEN,INFO,"id = %d name = %s\n",category_list->array[i].tags[j].id,category_list->array[i].tags[j].name);
	}


	if((NULL == p_param )||(NULL == p_param->category) || (0 == p_param->category->total))
		return NULL;

    if ((p_param->category->array != NULL) && (p_param->category->total > 0))
    {
        p_data = (al_iptv_cat_res_list_t *)SY_MALLOC(sizeof(al_iptv_cat_res_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_res = (u16)p_param->category->total;
        p_data->resList = (al_iptv_cat_res_item_t *)SY_MALLOC(p_data->total_res * sizeof(al_iptv_cat_res_item_t));
        MT_ASSERT(p_data->resList != NULL);
        memset(p_data->resList, 0, p_data->total_res * sizeof(al_iptv_cat_res_item_t));

        for (i = 0; i < p_data->total_res; i++)
        {
            xm_channel = p_param->category->array + i;
			if(NULL == xm_channel)
			{
				DEBUG(XM_GEN,NOTICE,"p_param->category->array [%d] == NULL\n",i);
				continue;
			}
            p_data->resList[i].res_id = xm_channel->id;
            xm_channel = p_param->category->array + i;
			src_len = strlen(xm_channel->name) + 1;
			if(1 < src_len)
			{
	            dest_len = src_len * sizeof(u16);
    	        inbuf = xm_channel->name;
        	    outbuf = (char *)SY_CALLOC(1,dest_len);
           		MT_ASSERT(outbuf != NULL);
            	p_data->resList[i].res_name = (u16 *)outbuf;
            	iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
			}
			if(NULL != p_data->resList[i].catList)
			{
				SY_FREE(p_data->resList[i].catList);
			}
			p_data->resList[i].total_cat = xm_channel->tagSize;
			if (p_data->resList[i].total_cat > 0)
			{
				p_data->resList[i].catList = (al_iptv_cat_item_t *)SY_MALLOC(p_data->resList[i].total_cat * sizeof(al_iptv_cat_item_t));
				MT_ASSERT(p_data->resList[i].catList != NULL);
		        memset(p_data->resList[i].catList, 0, p_data->resList[i].total_cat * sizeof(al_iptv_cat_item_t));
	           	for(j = 0; j < xm_channel->tagSize; j++)
            	{
    	  			xm_subtag = xm_channel->tags + j;
					src_len = strlen(xm_subtag->name) + 1;
					if(1 < src_len)
					{
		            	dest_len = src_len * sizeof(u16);
		    	        inbuf = xm_subtag->name;
		        	    outbuf = (char *)SY_CALLOC(1,dest_len);
		           		MT_ASSERT(outbuf != NULL);
		            	p_data->resList[i].catList[j].name= (u16 *)outbuf;
					   	iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
					}
					p_data->resList[i].catList[j].cat_id = xm_subtag->id;
					p_data->resList[i].catList[j].total_vdo = 1000;//temp value
				}
            }
            else
            {
                p_data->resList[i].total_cat = 0;
                p_data->resList[i].catList = NULL;
            }

			p_data->resList[i].total_filter = 0;
			p_data->resList[i].filter = NULL;
			
	    }
    }
	
	for (i = 0; i < p_data->total_res; i++)
	{
		DEBUG(XM_GEN,INFO,"res_name = %s\n",p_data->resList[i].res_name);	
		for(j = 0; j < p_data->resList[i].total_cat; j++)
		{
			DEBUG(XM_GEN,INFO,"p_data->resList[%d].catList[%d].name = %s\n",i,j,p_data->resList[i].catList[j].name);
			DEBUG(XM_GEN,INFO,"p_data->resList[%d].catList[%d].total_vdo = %d\n",i,j,p_data->resList[i].catList[j].total_vdo);
			DEBUG(XM_GEN,INFO,"p_data->resList[%d].catList[%d].key = %s\n",i,j,p_data->resList[i].catList[j].key);
		}
	}
    return p_data;
}

static void xm_free_cat_res_list(u32 param)
{
    al_iptv_cat_res_list_t *p_data = (al_iptv_cat_res_list_t *)param;
    u16 i, j;

    if (p_data)
    {
        if (p_data->resList)
        {
            for (i = 0; i < p_data->total_res; i++)
            {
                SY_FREE(p_data->resList[i].res_name);
                if (p_data->resList[i].catList)
                {
                    for (j = 0; j < p_data->resList[i].total_cat; j++)
                    {
                        if (p_data->resList[i].catList[j].name)
                        {
                            SY_FREE(p_data->resList[i].catList[j].name);
                        }
                        if (p_data->resList[i].catList[j].key)
                        {
                            SY_FREE(p_data->resList[i].catList[j].key);
                        }
                    }
                    SY_FREE(p_data->resList[i].catList);
                }
            }
            SY_FREE(p_data->resList);
        }
        SY_FREE(p_data);
    }
}

static al_iptv_vdo_list_t *xm_gen_vdo_list(XM_CHANNEL_LIST_T *p_param)
{
    al_iptv_vdo_list_t *p_data = NULL;
    XM_mov	*palbum;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

	DEBUG(XM_GEN,INFO,"[VDO LIST IN]pagecount  =%d,totalcount= %d,number = %d,category_id = %d,page_num = %d,page_size = %d,identify = %d,query_mode = %d\n",p_param->pagecount,p_param->totalcount,p_param->number,p_param->category_id,p_param->page_num,p_param->page_size,p_param->identify ,p_param->query_mode);
	for (i = 0; i < p_param->totalcount; i++)
	{
		DEBUG(XM_GEN,INFO,"[DO LIST IN[%d]:] id = %d,name = %s,type = %s,iamge = %s,detailUrl = %s",i,p_param->array[i].id,p_param->array[i].name,p_param->array[i].type,p_param->array[i].image,p_param->array[i].detailUrl);
	}

    p_data = (al_iptv_vdo_list_t *)SY_MALLOC(sizeof(al_iptv_vdo_list_t));
    MT_ASSERT(p_data != NULL);
    if(( NULL != p_param ) && (p_param->array != NULL) && (p_param->number > 0))
    {
		p_data->total_page = (u32)p_param->pagecount;
        p_data->page_num = (u32)p_param->page_num;
        p_data->total_vdo = (u32)p_param->totalcount;
        p_data->vdo_cnt = (u16)p_param->number;
		
        p_data->vdoList = (al_iptv_vdo_item_t *)SY_MALLOC(p_data->vdo_cnt * sizeof(al_iptv_vdo_item_t));
        MT_ASSERT(p_data->vdoList != NULL);
        memset(p_data->vdoList, 0, p_data->vdo_cnt * sizeof(al_iptv_vdo_item_t));
		for (i = 0; i < p_data->vdo_cnt; i++)
        {
            palbum = p_param->array + i;
   			p_data->vdoList[i].vdo_id.program_id = (u32)palbum->id;
            p_data->vdoList[i].b_single_page = (u8)0; 
			// TODO: b_single_page

			src_len = strlen(palbum->name) + 1;
            if (1 < src_len)
            {            
                dest_len = src_len * sizeof(u16);
                inbuf = palbum->name;
                outbuf = (char *)SY_CALLOC(1,dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->vdoList[i].name = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);         
			}
     
			if (palbum->image[0] != '\0')
            {
                dest_len = strlen(palbum->image) + 1;
                outbuf = (char *)SY_MALLOC(dest_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, (char *)palbum->image);
                p_data->vdoList[i].img_url = (u8 *)outbuf;
            }
        }
    }
    else
    {
        p_data->total_page = 0;
        p_data->page_num = 0;
        p_data->total_vdo = 0;
        p_data->vdo_cnt = 0;
        p_data->vdoList = NULL;
    }
	
	DEBUG(XM_GEN,INFO,"[VDO LIST OUT]vdo_cnt = %d\n",p_data->vdo_cnt);
	for (i = 0; i < p_data->vdo_cnt; i++)
	{
		DEBUG(XM_GEN,INFO,"[VDO LIST OUT]program_id = %d name = %s img_url = %s\n",
							p_data->vdoList[i].vdo_id.program_id,
							p_data->vdoList[i].name,
							p_data->vdoList[i].img_url);	
	}
    return p_data;
}

static void xm_free_vdo_list(u32 param)
{
    al_iptv_vdo_list_t *p_data = (al_iptv_vdo_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->vdoList)
        {
            for (i = 0; i < p_data->vdo_cnt; i++)
            {
                if (p_data->vdoList[i].name)
                {
                    SY_FREE(p_data->vdoList[i].name);
                }
                if (p_data->vdoList[i].img_url)
                {
                    SY_FREE(p_data->vdoList[i].img_url);
                }
            }
            SY_FREE(p_data->vdoList);
        }
        SY_FREE(p_data);
    }
}

static al_iptv_vdo_info_t *xm_gen_vdo_info(XM_CHANNEL_INFO_T *p_param)
{
    al_iptv_vdo_info_t *p_data = NULL;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    int i = 0;
	
#if 1	//Test the format
	p_param->mov_detail->duration = 1000;
	p_param->mov_detail->releaseyear = 1999;
	p_param->format.total = 3;
	p_param->format.format[0] = 480;
	p_param->format.format[1] = 720;
	p_param->format.format[2] = 1080;
#endif

	DEBUG(XM_GEN,INFO,"[VDO INFO IN]id = %d,cat_id = %d,episode_total = %d,page_size = %d,page_index = %d\n",p_param->id,p_param->cat_id,p_param->episode_total,p_param->this_page_size,p_param->this_page_index);
	DEBUG(XM_GEN,INFO,"[VDO INFO IN]vdo_format list total = %d\n",p_param->format.total);
	for(i = 0;i < p_param->format.total;i++)
	{
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]vdo_format list[%d] = %d\n",i,p_param->format.format[i]);
	}
	for(i =0;i < p_param->episode_total;i++)
	{
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].id=%d\n",i,p_param->mov_detail[i].id);
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].name=%s\n",i,p_param->mov_detail[i].name);
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].type=%s\n",i,p_param->mov_detail[i].type);
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].image=%s\n",i,p_param->mov_detail[i].image);
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].actor=%s\n",i,p_param->mov_detail[i].actor);
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].director=%s\n",i,p_param->mov_detail[i].director);
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].duration=%d\n",i,p_param->mov_detail[i].duration);
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].originalcountry=%s\n",i,p_param->mov_detail[i].originalcountry);
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].description=%s\n",i,p_param->mov_detail[i].description);
		DEBUG(XM_GEN,INFO,"[VDO INFO IN]episode[%d].releaseyear=%d\n",i,p_param->mov_detail[i].releaseyear);
	}
	
	DEBUG(XM_GEN,INFO,"[VDO INFO IN]mov_detail->name = %s\n",p_param->mov_detail->name);
	DEBUG(XM_GEN,INFO,"[VDO INFO IN]mov_detail->originalcountry = %s\n",p_param->mov_detail->originalcountry);
	DEBUG(XM_GEN,INFO,"[VDO INFO IN]mov_detail->director = %s\n",p_param->mov_detail->director);
	DEBUG(XM_GEN,INFO,"[VDO INFO IN]mov_detail->actor = %s\n",p_param->mov_detail->actor);
	DEBUG(XM_GEN,INFO,"[VDO INFO IN]mov_detail->releaseyear = %d\n",p_param->mov_detail->releaseyear);
	DEBUG(XM_GEN,INFO,"[VDO INFO IN]mov_detail->type = %s\n",p_param->mov_detail->type);
	DEBUG(XM_GEN,INFO,"[VDO INFO IN]mov_detail->description = %s\n",p_param->mov_detail->description);
	DEBUG(XM_GEN,INFO,"[VDO INFO IN]mov_detail->image = %s\n",p_param->mov_detail->image);

	DEBUG(XM_GEN,INFO,"[VDO INFO IN]is_description = %d,identify = %d\n",p_param->is_description,p_param->identify);;

	if((NULL == p_param) || (NULL == p_param->mov_detail))
			return NULL;
	else
	{
	    p_data = (al_iptv_vdo_info_t *)SY_MALLOC(sizeof(al_iptv_vdo_info_t));
		MT_ASSERT(NULL != p_data);
		memset(p_data, 0, sizeof(al_iptv_vdo_info_t));
		
	    p_data->res_id = (u32)p_param->cat_id;
		p_data->time= (u32)p_param->mov_detail->duration;
	    //p_data->tvsets= (u16)(p_param->album->tvsets > p_param->album->tvCount? p_param->album->tvsets:p_param->album->tvCount);
	    p_data->tvsets = (u16)p_param->episode_total;

    	src_len = strlen(p_param->mov_detail->name) + 1;
	    if(1 < src_len )
	    {
	    	dest_len = src_len * sizeof(u16);
	    	inbuf = p_param->mov_detail->name;
	    	outbuf = (char *)SY_CALLOC(1,dest_len);
	    	MT_ASSERT(outbuf != NULL);
	    	p_data->name = (u16 *)outbuf;
	    	iconv(g_cd_utf8_to_utf16le, (char**)&inbuf, &src_len, (char**) &outbuf, &dest_len);
	    }
   
		src_len = strlen(p_param->mov_detail->originalcountry) + 1;
	    if(1 < src_len) // TODO: tag ->> area
	    {
		    dest_len = src_len * sizeof(u16);
		    inbuf = p_param->mov_detail->originalcountry;
		    outbuf = (char *)SY_CALLOC(1,dest_len);
		    MT_ASSERT(outbuf != NULL);
		    p_data->area = (u16 *)outbuf;
		    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
	    }

		src_len = strlen(p_param->mov_detail->director) + 1;
	   	if(1 < src_len)
		{
		    dest_len = src_len * sizeof(u16);
		    inbuf = p_param->mov_detail->director;
		    outbuf = (char *)SY_CALLOC(1,dest_len);
		    MT_ASSERT(outbuf != NULL);
		    p_data->director = (u16 *)outbuf;
		    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
	    }

		src_len = strlen(p_param->mov_detail->actor) + 1;
	    if(1 < src_len)
	    {
		    dest_len = src_len * sizeof(u16);
		    inbuf = p_param->mov_detail->actor;
		    outbuf = (char *)SY_CALLOC(1,dest_len);
		    MT_ASSERT(outbuf != NULL);
		    p_data->actor = (u16 *)outbuf;
		    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
		}
	
	    if(0 != p_param->mov_detail->releaseyear) // Äê´ú
	    {
			char temp_years[8] = {0};
			sprintf(temp_years,"%d",p_param->mov_detail->releaseyear);//INT to string
			src_len = strlen(temp_years) + 1;
		    dest_len = src_len * sizeof(u16);
		    inbuf = (char*)temp_years;
		    outbuf = (char *)SY_CALLOC(1,dest_len);
		    MT_ASSERT(outbuf != NULL);
		    p_data->years = (u16 *)outbuf;	//utf8->utf16
		    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
	    }

		src_len = strlen(p_param->mov_detail->type) + 1;
	    if(1 < src_len)
	    {
			if(0 == strcmp(p_param->mov_detail->type,"Program"))
			{
				p_data->vdo_id.type = 1;//Movie
				DEBUG(XM_GEN,INFO,"[VDO INFO OUT] MOVIE TYPE = %d\n",p_data->vdo_id.type);
			}
			else if( 0 == strcmp(p_param->mov_detail->type,"series"))
			{
				p_data->vdo_id.type = 2;//Series
				DEBUG(XM_GEN,INFO,"[VDO INFO OUT] TV TYPE = %d\n",p_data->vdo_id.type);
			}
	    }
		src_len = strlen(p_param->mov_detail->description) + 1;
	    if(1 < src_len)
	    {
		    dest_len = src_len * sizeof(u16);
		    inbuf = p_param->mov_detail->description;
		    outbuf = (char *)SY_CALLOC(1,dest_len);
		    MT_ASSERT(outbuf != NULL);
		    p_data->description = (u16 *)outbuf;
		    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
	    }

		src_len = strlen(p_param->mov_detail->image) + 1;
	    if(1 < src_len)
	    {
		    dest_len = strlen(p_param->mov_detail->image) + 1;
		    outbuf = (char *)SY_MALLOC(dest_len);
		    MT_ASSERT(outbuf != NULL);
		    strcpy((char *)outbuf, (char *)p_param->mov_detail->image);
		    p_data->img_url = (u8 *)outbuf;
	    }
		
	    if (p_param->format.total > 0)
	    {
	    	p_data->format = SY_MALLOC(sizeof(al_iptv_source_format_t));
			MT_ASSERT(p_data->format != NULL);
			memset(p_data->format,0,sizeof(al_iptv_source_format_t));
	      	p_data->format->formatList = SY_MALLOC(sizeof(int)*p_param->format.total + 1);
			memset(p_data->format->formatList,0,sizeof(int)*p_param->format.total + 1);
	       	MT_ASSERT(p_data->format->formatList!= NULL);
			p_data->format->total_format = p_param->format.total;
			for (i=0;i<p_param->format.total;i++)
			{
				*(p_data->format->formatList+i) = p_param->format.format[i];
			}
	    }

		DEBUG(XM_GEN,INFO,"[VDO INFO OUT]name = %s\n",p_data->name);
		DEBUG(XM_GEN,INFO,"[VDO INFO OUT]area = %s\n", p_data->area);
		DEBUG(XM_GEN,INFO,"[VDO INFO OUT]actor = %s\n",	p_data->actor);
		DEBUG(XM_GEN,INFO,"[VDO INFO OUT]director = %s\n", p_data->director);
		DEBUG(XM_GEN,INFO,"[VDO INFO OUT]years = %s\n",	p_data->years);
		DEBUG(XM_GEN,INFO,"[VDO INFO OUT]attr = %s\n",  p_data->attr);
		DEBUG(XM_GEN,INFO,"[VDO INFO OUT]description = %s\n",p_data->description);
		DEBUG(XM_GEN,INFO,"[VDO INFO OUT]img_url = %s\n", p_data->img_url);

		for (i=0;i<p_data->format->total_format;i++)
		{
			DEBUG(XM_GEN,INFO,"[VDO OUT] p_data->format->formatList[%d]  = %d\n", i,p_data->format->formatList[i]);
		}
		
	}
    return p_data;
}

static void xm_free_vdo_info(u32 param)
{
	al_iptv_vdo_info_t *p_data = (al_iptv_vdo_info_t *)param;
	if (p_data)
	{
		if (p_data->name)
		{
			SY_FREE(p_data->name);
		}
		if (p_data->area)
		{
			SY_FREE(p_data->area);
		}
		if (p_data->director)
		{
			SY_FREE(p_data->director);
		}
		if (p_data->actor)
		{
			SY_FREE(p_data->actor);
		}
		if (p_data->score)
		{
			SY_FREE(p_data->score);
		}
		if (p_data->description)
		{
			SY_FREE(p_data->description);
		}
		if (p_data->attr)
		{
			SY_FREE(p_data->attr);
		}
		if (p_data->years)
		{
			SY_FREE(p_data->years);
		}
		if (p_data->img_url)
		{
			SY_FREE(p_data->img_url);
		}
		if (p_data->orgnList)
		{
			SY_FREE(p_data->orgnList);
		}
		if (p_data->format)
		{
			if (p_data->format->formatList)
				SY_FREE(p_data->format->formatList);

			SY_FREE(p_data->format);
		}
		SY_FREE(p_data);
	}
}

static al_iptv_info_url_list_t *xm_gen_info_url(XM_CHANNEL_INFO_T *p_param)
{ 
	al_iptv_info_url_list_t *p_data = NULL;
    XM_detail *pepisode;
	char *outbuf;
    size_t dest_len,src_len;
    char *inbuf;
    u16 i;

    DEBUG(UI_PLAY_API,INFO,"album type[%s]\n",p_param->mov_detail->type);
    p_data = (al_iptv_info_url_list_t *)SY_MALLOC(sizeof(al_iptv_info_url_list_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data, 0, sizeof(al_iptv_info_url_list_t));

    if (0 == strcmp(p_param->mov_detail->type,"Program")) //mov
    {
    	p_data->count = 1;
		p_data->page_count = 1;
		p_data->page_total = 1;
    	p_data->urlList = (al_iptv_info_url_item_t *)SY_MALLOC(p_data->count * sizeof(al_iptv_info_url_item_t));
		memset( p_data->urlList,0,p_data->count * sizeof(al_iptv_info_url_item_t));
		MT_ASSERT(p_data->urlList != NULL);

		src_len = strlen(p_param->mov_detail->name ) + 1;
		dest_len = src_len * sizeof(u16);
		inbuf =p_param->mov_detail->name ;
		outbuf = (char *)SY_CALLOC(1,dest_len);
		MT_ASSERT(outbuf != NULL);

		p_data->urlList[0].urltitle = (u16 *)outbuf;
		iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
		return p_data;
    }

    p_data->count = (u32)p_param->this_page_size;
    if (p_param->episode_total > 0 && p_param->this_page_size > 0)
    {
        p_data->page_count = (u32)(p_param->episode_total + p_param->this_page_size -1)/p_param->this_page_size;
	
	    p_data->page_total = (u32)p_param->episode_total;

	    DEBUG(UI_PLAY_API,INFO,"url total  %d\n",p_param->episode_total);
	    DEBUG(UI_PLAY_API,INFO,"url page_count  %d\n",p_data->page_count);
	    DEBUG(UI_PLAY_API,INFO,"url page_total  %d\n",p_data->page_total);
	    
	    p_data->urlList = (al_iptv_info_url_item_t *)SY_MALLOC(p_data->count * sizeof(al_iptv_info_url_item_t));
	    memset( p_data->urlList,0,p_data->count * sizeof(al_iptv_info_url_item_t));
	    MT_ASSERT(p_data->urlList != NULL);

	    for (i = 0; i < p_data->count; i++)
	    {
			pepisode = p_param->mov_detail + i;
			//memcpy((char *) p_data->urlList[i].tvQid, (char *)pepisode->tvQid,sizeof(p_data->urlList[0].tvQid));
			//memcpy((char *) p_data->urlList[i].vid, (char *)pepisode->vid,sizeof(p_data->urlList[0].vid));

			DEBUG(UI_PLAY_API,INFO,"url name[%s]\n",pepisode->name );
			//url title
			if(pepisode->name != NULL)
			{
				src_len = strlen(pepisode->name ) + 1;
				dest_len = src_len * sizeof(u16);
				inbuf = pepisode->name ;
				outbuf = (char *)SY_CALLOC(1,dest_len);
				MT_ASSERT(outbuf != NULL);
				p_data->urlList[i].urltitle = (u16 *)outbuf;
				iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
			}
		}
    	return p_data;
	}
	return NULL;
}

static void xm_free_info_url(u32 param)
{
    al_iptv_info_url_list_t *p_data = (al_iptv_info_url_list_t *)param;
    u16 i;
    if (p_data)
    {
        if (p_data->urlList)
        {
            for (i = 0; i < p_data->count; i++)
            {
                if (p_data->urlList[i].urltitle)
                {
                    SY_FREE(p_data->urlList[i].urltitle);
                } 
            }
            SY_FREE(p_data->urlList);
        }
        SY_FREE(p_data);
    }
}



static al_iptv_source_format_t *xm_gen_source_format(XM_VIDEO_FORMAT_T * format)
{
	al_iptv_source_format_t *p_data = NULL;

	//if(format != NULL && format->total != 0)
	{
		p_data = (al_iptv_source_format_t *)SY_MALLOC(sizeof(al_iptv_source_format_t));
		p_data->formatList = (int*)SY_MALLOC(sizeof(int)*format->total);
		p_data->total_format  = 3;
		p_data->formatList[0] = 1080;
		p_data->formatList[1] = 720;
		p_data->formatList[2] = 480;	
	}

	return p_data;
#if 0
	al_iptv_source_format_t *p_data = NULL;
	int i;
	p_data = (al_iptv_source_format_t *)SY_MALLOC(sizeof(al_iptv_source_format_t));
	MT_ASSERT(p_data != NULL);

	p_data->total_format = 0;
	p_data->formatList = (int *)SY_MALLOC(IPTV_MAX_FORMAT_COUNT * sizeof(*p_data->formatList));
	MT_ASSERT(p_data->formatList != NULL);
	for(i=0;i<format->total;i++)
	{
		p_data->formatList[i] = format->format[i];
	}
	p_data->total_format = format->total;
	return p_data;
#endif
}

static void xm_free_source_format(u32 param)
{
    al_iptv_source_format_t *p_data = (al_iptv_source_format_t *)param;

    if (p_data)
    {
        if (p_data->formatList)
        {
            SY_FREE(p_data->formatList);
        }
        SY_FREE(p_data);
    }
	return;
}

static al_iptv_play_url_list_t *xm_gen_play_url(XM_CHANNEL_PLAY_URL_T *p_param)
{
    al_iptv_play_url_list_t *p_data = NULL;
    XM_urlobj * p_item;
    u16 i;

	DEBUG(XM_GEN,INFO,"[PLAY URL IN]number = %d\n",p_param->number);
	for(i = 0; i < p_param->number; i++)
	{
		DEBUG(XM_GEN,INFO,"[PLAY URL IN][%d] id = %d,name = %s,hdtv = %d,playUrl =%s,timeShiftUrl = %s,\n",
			i,p_param->playurls[i].id,p_param->playurls[i].name,p_param->playurls[i].hdtv,p_param->playurls[i].playUrl,p_param->playurls[i].timeshiftUrl);
	}
	DEBUG(XM_GEN,INFO,"[PLAY URL IN]terminalState = %d\n",p_param->terminalState);
	DEBUG(XM_GEN,INFO,"[PLAY URL IN]format = %d\n",p_param->format);
	DEBUG(XM_GEN,INFO,"[PLAY URL IN]status = %d\n",(int)p_param->status);
	DEBUG(XM_GEN,INFO,"[PLAY URL IN]identify = %d",p_param->identify);

    if ((p_param->playurls != NULL) && (p_param->number > 0))
    {
        p_data = (al_iptv_play_url_list_t *)SY_MALLOC(sizeof(al_iptv_play_url_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_url = (u16)p_param->number;
        p_data->pp_urlList = (al_iptv_play_url_info_t*)SY_MALLOC(p_data->total_url * sizeof(p_data->pp_urlList[0]));
        MT_ASSERT(p_data->pp_urlList != NULL);

        for (i = 0; i < p_data->total_url; i++)
        {
            p_item = p_param->playurls+ i;

            if ((strlen(p_item->playUrl) != 0)
					&& (strlen( p_item->name) != 0)
					&& (strlen(p_item->timeshiftUrl) != 0))
            {
				p_data->pp_urlList->hdtv = p_item->hdtv;
				p_data->pp_urlList->id = p_item->id;
				strncpy(p_data->pp_urlList->name,p_item->name,strlen(p_item->name));
				strncpy(p_data->pp_urlList->playUrl,p_item->playUrl,strlen(p_item->playUrl));
				strncpy(p_data->pp_urlList->timeshiftUrl,p_item->timeshiftUrl,strlen(p_item->timeshiftUrl));
            }
        }
    }

	DEBUG(XM_GEN,INFO,"[PLAY URL OUT]total = %d\n",p_data->total_url);
	for(i = 0; i < p_data->total_url; i++)
	{
		DEBUG(XM_GEN,INFO,"[PLAY URL IN][%d] id = %d,name = %s,hdtv = %d,playUrl =%s,timeShiftUrl = %s,\n",
			i,p_data->pp_urlList[i].id,p_data->pp_urlList[i].name,p_data->pp_urlList[i].hdtv,p_data->pp_urlList[i].playUrl,p_data->pp_urlList[i].timeshiftUrl);
	}
	 return p_data;
   
}

static void xm_free_play_url(u32 param)
{
    al_iptv_play_url_list_t *p_data = (al_iptv_play_url_list_t *)param;

    if (p_data)
    {
        if (p_data->pp_urlList)
        {            
            SY_FREE(p_data->pp_urlList);
        }
        SY_FREE(p_data);
    }
}

static al_iptv_recomand_info_t *xm_gen_remend_info(XM_RECMD_INFO_T *p_param)
{

	al_iptv_recomand_info_t *p_data = NULL;
	XM_recommand *p_item;
	u8 i = 0;
	char *inbuf,*outbuf;
	size_t src_len,dest_len;

	DEBUG(XM_GEN,INFO,"[RECMD IN]have_geted = %d,channel_num = %d,pagecount = %d\n",p_param->have_geted,p_param->channel_num,p_param->pagecount);
	DEBUG(XM_GEN,INFO,"[RECMD IN]res_id = %d,cat_id = %d,program_id = %d,type = %d\n",p_param->id.res_id,p_param->id.cat_id,p_param->id.program_id,p_param->id.type);
	for(i = 0; i < p_param->totalcount; i++)
	{
		DEBUG(XM_GEN,INFO,"[RECMD IN][%d] id = %d,name = %s,type = %s,img = %s,detailUrl = %s\n",i,p_param->recmd[i].id,p_param->recmd[i].name,p_param->recmd[i].type,p_param->recmd[i].image,p_param->recmd[i].detailUrl);
	}

	p_data = (al_iptv_recomand_info_t *)SY_MALLOC(sizeof(al_iptv_recomand_info_t));
	MT_ASSERT(p_data != NULL);
	memset(p_data, 0, sizeof(al_iptv_recomand_info_t));

	p_data->number = (u32)p_param->totalcount;//gw::totalCount or channelCount
	if(p_data->number == 0)
	{
		return p_data;
	}

	p_data->recmd  = (iptv_recmd_channel_t *)SY_MALLOC(p_data->number * sizeof(iptv_recmd_channel_t));
	memset(p_data->recmd,0,p_data->number * sizeof(iptv_recmd_channel_t));
	MT_ASSERT(p_data->recmd != NULL);

	for (i = 0; i < p_data->number; i++)
	{
		p_item = p_param->recmd+ i;

		p_data->recmd[i].id.program_id = p_param->recmd[i].id;
		if(p_item->name[0] != '\0')
		{
			src_len = strlen(p_item->name) + 1;
			dest_len = src_len * sizeof(u16);
			inbuf = p_item->name;
			outbuf = (char *)SY_CALLOC(1,dest_len);
			MT_ASSERT(outbuf != NULL);
			p_data->recmd[i].title= (u16 *)outbuf;
			iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
		}
			
		if(p_item->type[0] != '\0')
		{
			if(0 == strcmp(p_item->type,"Program"))
				p_data->recmd[i].id.type = 1;
			else if(0 == strcmp(p_item->type,"series"))
				p_data->recmd[i].id.type = 2;
			else
				p_data->recmd[i].id.type = 0;
		}

		if(p_item->image[0] != '0')
		{	
			dest_len = strlen(p_item->image) + 1;
			outbuf = (char *)SY_MALLOC(dest_len);
			MT_ASSERT(outbuf != NULL);
			strcpy((char *)outbuf, (char *)p_item->image);
			p_data->recmd[i].img = (u8 *)outbuf;
		}
	}

	DEBUG(XM_GEN,INFO,"[RECMD OUT]number = %d\n",p_data->number);
	for(i = 0; i < p_data->number; i++)
	{
		DEBUG(XM_GEN,INFO,"[RECMD OUT][%d]title = %s,img = %s,program_id = %d,type = %d",
			i,p_data->recmd[i].title,p_data->recmd[i].img,p_data->recmd[i].id.program_id,p_data->recmd[i].id.type);
	}

	return p_data;
}

static void xm_free_remend_info(u32 param)
{
    al_iptv_recomand_info_t *p_data = (al_iptv_recomand_info_t *)param;
    u16 i;
    if (p_data)
    {
        if (p_data->recmd)
        {
            for (i = 0; i < p_data->number; i++)
            {
                if (p_data->recmd[i].title)
                {
                    SY_FREE(p_data->recmd[i].title);
                }
                if (p_data->recmd[i].img)
                {
                    SY_FREE(p_data->recmd[i].img);
                }
            }
            SY_FREE(p_data->recmd);
        }
        SY_FREE(p_data);
    }
}

static al_iptv_search_word_t *xm_gen_search_word(XM_SEARCH_WORD_T *p_param)
{
    return NULL;
#if 0
    al_iptv_search_word_t *p_data = NULL;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

    if ((p_param->search != NULL) && (p_param->search->wordSize > 0))
    {
        p_data = (al_iptv_search_word_t *)SY_MALLOC(sizeof(al_iptv_search_word_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_word = (u16)p_param->search->wordSize;
        p_data->word_list = (al_iptv_word_name_t *)SY_MALLOC(p_data->total_word * sizeof(al_iptv_word_name_t));
        MT_ASSERT(p_data->word_list != NULL);
        memset(p_data->word_list, 0, p_data->total_word * sizeof(al_iptv_word_name_t));

        for (i = 0; i < p_data->total_word; i++)
        {
            src_len = strlen(p_param->search->wordList[i].word) + 1;
            dest_len = src_len * sizeof(u16);
            inbuf = p_param->search->wordList[i].word;
            outbuf = (char *)SY_MALLOC(dest_len);
            MT_ASSERT(outbuf != NULL);
            p_data->word_list[i].word = (u16 *)outbuf;
            iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        }
    }

    return p_data;
#endif	
}

static void xm_free_search_word(u32 param)
{
#if 0
    al_iptv_search_word_t *p_data = (al_iptv_search_word_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->word_list)
        {
            for (i = 0; i < p_data->total_word; i++)
            {
                if (p_data->word_list[i].word)
                {
                    SY_FREE(p_data->word_list[i].word);
                }
            }
            SY_FREE(p_data->word_list);
        }
        SY_FREE(p_data);
    }
#endif
}



static RET_CODE  iptv_dp_event_callback(IPTV_EVENT_TYPE event, u32 param)
{
    //return 0;
    DEBUG(UI_PLAY_API,INFO,"@@@iptv_dp_event_callback = %x\n",iptv_dp_event_callback);
#if 1

    event_t evt = {0};
    XM_CHANNEL_INFO_T *p_param = NULL;
    switch(event)
    {
        case IPTV_INIT_SUCCESS:
        {
            evt.id = IPTV_EVT_INIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_INIT_FAIL:
        {
            evt.id = IPTV_EVT_INIT_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_DEINIT_SUCCESS:
        {
            evt.id = IPTV_EVT_DEINIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_CATEGORY_ARRIVAL:
        {
            if((XM_CATEGORY_LIST_T *)param != NULL)
            {
             XM_CATEGORY_LIST_T *p_param = (XM_CATEGORY_LIST_T *)param;
 
             DEBUG(UI_PLAY_API,INFO,"@@@IPTV_CATEGORY_ARRIVAL status=%d\n", p_param->status);
             if (p_param->status == IPTV_DATA_SUCCESS)
             {
                 al_iptv_name_res_list_t *p_data = xm_gen_name_res_list(p_param);
 
                 if (p_data != NULL)
                 {
                     evt.id = IPTV_EVT_NEW_RES_NAME_ARRIVE;
                     evt.data1 = (u32)p_data;
                     ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                     break;
                 }
             }
            }
            evt.id = IPTV_EVT_GET_RES_NAME_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
        break;

        case IPTV_CATEGORY_TYPE_ARRIVAL:
        {
           if((XM_CATEGORY_TYPE_ALL_T *)param != NULL)
           {
            XM_CATEGORY_TYPE_ALL_T *p_param = (XM_CATEGORY_TYPE_ALL_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_CATEGORY_TYPE_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                al_iptv_cat_res_list_t *p_data = xm_gen_cat_res_list(p_param);

                if (p_data != NULL)
                {
                    evt.id = IPTV_EVT_NEW_RES_CATGRY_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                    break;
                }
            }
           }
            evt.id = IPTV_EVT_GET_RES_CATGRY_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_NEW_PAGE_ARRIVAL:
        {
            XM_CHANNEL_LIST_T *p_param = (XM_CHANNEL_LIST_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_NEW_PAGE_ARRIVAL status=%d\n", p_param->status);
			DEBUG(UI_PLAY_API,INFO,"p_param->identify = %d,vdo_identify_code = %d\n",p_param->identify,vdo_identify_code);
            if (p_param->identify == vdo_identify_code)
            {
                    DEBUG(UI_PLAY_API,INFO,"@@@IPTV_QUERY_MODE_CATGRY status[%d], query_mode[%d]\n",p_param->status,p_param->query_mode);
                    if (p_param->status == IPTV_DATA_SUCCESS)
                    {
                        al_iptv_vdo_list_t *p_data = xm_gen_vdo_list(p_param);

			   if (p_param->query_mode == IPTV_QUERY_MODE_CATGRY)
                        	evt.id = IPTV_EVT_NEW_PAGE_VDO_ARRIVE;
			   else if (p_param->query_mode == IPTV_QUERY_MODE_PINYIN)
                        	evt.id = IPTV_EVT_NEW_SEARCH_VDO_ARRIVE;
			   else
                        	evt.id = IPTV_EVT_NEW_PAGE_VDO_ARRIVE;
			   	
                        evt.data1 = (u32)p_data;
                        evt.data2 = p_param->identify;
                        ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                        break;
                    }
 		     if (p_param->query_mode == IPTV_QUERY_MODE_CATGRY)
                        evt.id = IPTV_EVT_GET_PAGE_VDO_FAIL;
 		     else if (p_param->query_mode == IPTV_QUERY_MODE_PINYIN)
                     	  evt.id = IPTV_EVT_GET_SEARCH_VDO_FAIL;
 		     else
                     	  evt.id = IPTV_EVT_GET_PAGE_VDO_FAIL;
                        	
                    evt.id = IPTV_EVT_GET_SEARCH_VDO_FAIL;
                    evt.data2 = p_param->identify;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
            }
            else
            {
                DEBUG(UI_PLAY_API,INFO,"@@@identify=%d vdo_identify_code=%d\n", p_param->identify, vdo_identify_code);
            }
        }
            break;

        case IPTV_VIDEO_INFO_ARRIVAL:
        {
            if((XM_CHANNEL_INFO_T *)param ==NULL)
            {  
                evt.id = IPTV_EVT_GET_VDO_INFO_FAIL;
                ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                break;
            }
            p_param = (XM_CHANNEL_INFO_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_VIDEO_INFO_ARRIVAL status=%d description = %d\n", 
				p_param->status,p_param->is_description);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                if (p_param->is_description == FALSE)
                {
                    al_iptv_info_url_list_t *p_data = xm_gen_info_url(p_param);
                    evt.id = IPTV_EVT_NEW_INFO_URL_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                }
                else
                {
                    al_iptv_vdo_info_t *p_data = xm_gen_vdo_info(p_param);
                    evt.id = IPTV_EVT_NEW_VDO_INFO_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                }
            }
            else
            {
                evt.id = IPTV_EVT_GET_VDO_INFO_FAIL;
                ap_frm_send_evt_to_ui(APP_IPTV, &evt);
            }
        }
            break;

        case IPTV_RECOMMEND_INFO_ARRIVAL:
        {
          if((XM_RECMD_INFO_T *)param!=NULL)
          {
            XM_RECMD_INFO_T *p_param = (XM_RECMD_INFO_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_RECOMMEND_INFO_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                al_iptv_recomand_info_t *p_data = xm_gen_remend_info(p_param);
                if (p_data != NULL)
                {
                    evt.id = IPTV_EVT_NEW_RECMND_INFO_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                    break;
                }
            }
          }
            evt.id = IPTV_EVT_GET_RECMND_INFO_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_FORMAT_TYPE_ARRIVAL:
        {
            al_iptv_source_format_t *p_data;
	     XM_VIDEO_FORMAT_T *p_param = (XM_VIDEO_FORMAT_T*)param;
            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_FORMAT_TYPE_ARRIVAL param.total[%d]\n", p_param->total);
            p_data = xm_gen_source_format(p_param);
            if (p_data != NULL)
            {
                evt.id = IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE;
                evt.data1 = (u32)p_data;
                ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                break;
            }
            evt.id = IPTV_EVT_GET_SOURCE_FORMAT_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_PLAY_URL_ARRIVAL:
        {
          if((XM_CHANNEL_PLAY_URL_T *)param!= NULL)
          {
            XM_CHANNEL_PLAY_URL_T *p_param = (XM_CHANNEL_PLAY_URL_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_PLAY_URL_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                al_iptv_play_url_list_t *p_data = xm_gen_play_url(p_param);

                if (p_data != NULL)
                {
                    evt.id = IPTV_EVT_NEW_PLAY_URL_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                    break;
                }
            }
          }
            evt.id = IPTV_EVT_GET_PLAY_URL_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_SEARCH_WORD_ARRIVAL:
        {
            if((XM_SEARCH_WORD_T *)param != NULL)
            {
             XM_SEARCH_WORD_T *p_param = (XM_SEARCH_WORD_T *)param;
 
             if (p_param->status == IPTV_DATA_SUCCESS)
             {
		 DEBUG(UI_PLAY_API,INFO,"@@@ IPTV_SEARCH_WORD_ARRIVAL\n");
                 al_iptv_search_word_t *p_data = xm_gen_search_word(p_param);
 
                 if (p_data != NULL)
                 {
                     evt.id = IPTV_EVT_NEW_SEARCH_WORD_ARRIVE;
                     evt.data1 = (u32)p_data;
                     ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                     break;
                 }
             }
            }
            evt.id = IPTV_EVT_GET_SEARCH_WORD_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        default:
            break;
    }

    return SUCCESS;
#endif	
}

void xm_free_msg_data(u16 msg, u32 para1, u32 para2)
{
#if 1

    if (msg < MSG_IPTV_EVT_BEGIN || msg >= MSG_IPTV_EVT_END)
    {
        return;
    }

    switch(msg)
    {
        case MSG_IPTV_EVT_NEW_SEARCH_WORD_ARRIVE:
        {
            xm_free_search_word(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE:
        {
            xm_free_name_res_list(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE:
        {
            xm_free_cat_res_list(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE:
        case MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE:
        {
            xm_free_vdo_list(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE:
        {
            xm_free_vdo_info(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE:
        {
            xm_free_info_url(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE:
        {
            xm_free_remend_info(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE:
        {
            xm_free_source_format(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE:
        {
            xm_free_play_url(para1);
        }
            break;
        default:
            break;
    }
#endif	
}

const VodDpInterface_t *pVod = NULL; //vod interface
void xm_dp_init(void)
{
#ifndef WIN32
	DO_CMD_TASK_CONFIG_T config;

	pVod = GetXingMeiDpInterface();

	DEBUG(UI_PLAY_API,INFO,"@@@xm_dp_init\n");
	memset(&config, 0, sizeof(DO_CMD_TASK_CONFIG_T));
	vdo_identify_code = 0;
	config.priority = VOD_DP_PRIORITY;
	config.http_priority = YT_HTTP_PRIORITY;
	config.stack_size = 520*1024;
	config.dp = BSW_IPTV_DP;
	config.sub_dp = SUB_DP_ID;
	config.read_flash = NULL;
	config.write_flash = NULL;
	config.mac = SY_MALLOC(MAC_CHIPID_LEN);
	config.id = SY_MALLOC(MAC_CHIPID_LEN);
	ui_get_stb_mac_addr_chip_id(config.mac, config.id);
	if (pVod->init)
		pVod->init(&config);
	pVod->registerCb(iptv_dp_event_callback);
#endif
	ui_iptv_set_dp_state(IPTV_DP_STATE_INITING);
#ifndef WIN32
	SY_FREE(config.id);
	SY_FREE(config.mac);
#endif
}

void xm_dp_deinit(void)
{
#if 1

    if (pVod != NULL)
    {
#ifndef WIN32
	pVod->deinit();
#endif

    }
#endif	
}

void xm_dp_register_msg(void)
{
#if 1

	DEBUG(UI_PLAY_API,INFO,"@@@xm_register_msg\n");
	fw_register_ap_evtmap(APP_IPTV, xm_evtmap);
	fw_register_ap_msghost(APP_IPTV, ROOT_ID_IPTV);
	fw_register_ap_msghost(APP_IPTV, ROOT_ID_IPTV_SEARCH);
#endif	
}

void xm_dp_unregister_msg(void)
{
#if 1

	DEBUG(UI_PLAY_API,INFO,"@@@xm_unregister_msg\n");
	fw_unregister_ap_evtmap(APP_IPTV);
	fw_unregister_ap_msghost(APP_IPTV, ROOT_ID_IPTV);
	fw_unregister_ap_msghost(APP_IPTV, ROOT_ID_IPTV_SEARCH);
#endif
}


inline const VodDpInterface_t * ui_xm_return_instance(void)
{
	(void)chrn;
#if 1
	(void)set_vdo_id;
	(void)xm_gen_search_word;
	(void)xm_free_search_word;
	(void)xm_gen_name_res_list;
	(void)xm_free_name_res_list;
	(void)xm_gen_cat_res_list;
	(void)xm_gen_vdo_list;
	(void)xm_free_vdo_list;
	(void)xm_gen_vdo_info;
	(void)xm_free_vdo_info;
	(void)xm_gen_info_url;
	(void)xm_free_info_url;
	(void)xm_gen_remend_info;
	(void)xm_free_remend_info;
	(void)xm_gen_source_format;
	(void)xm_free_source_format;
	(void)xm_gen_play_url;
	(void)xm_free_play_url;
	(void)xm_free_cat_res_list;
	
	return pVod;

#else

	return pVod;
#endif

}



BEGIN_AP_EVTMAP(xm_evtmap)
CONVERT_EVENT(IPTV_EVT_INIT_SUCCESS, MSG_IPTV_EVT_INIT_SUCCESS)
CONVERT_EVENT(IPTV_EVT_INIT_FAIL, MSG_IPTV_EVT_INIT_FAIL)
CONVERT_EVENT(IPTV_EVT_DEINIT_SUCCESS, MSG_IPTV_EVT_DEINIT_SUCCESS)
CONVERT_EVENT(IPTV_EVT_NEW_RES_NAME_ARRIVE, MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_RES_NAME_FAIL, MSG_IPTV_EVT_GET_RES_NAME_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_RES_CATGRY_ARRIVE, MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_RES_CATGRY_FAIL, MSG_IPTV_EVT_GET_RES_CATGRY_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_PAGE_VDO_ARRIVE, MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_PAGE_VDO_FAIL, MSG_IPTV_EVT_GET_PAGE_VDO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_SEARCH_VDO_ARRIVE, MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_SEARCH_VDO_FAIL, MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_VDO_INFO_ARRIVE, MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_VDO_INFO_FAIL, MSG_IPTV_EVT_GET_VDO_INFO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_INFO_URL_ARRIVE, MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_INFO_URL_FAIL, MSG_IPTV_EVT_GET_INFO_URL_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_PLAY_URL_ARRIVE, MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_PLAY_URL_FAIL, MSG_IPTV_EVT_GET_PLAY_URL_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_RECMND_INFO_ARRIVE, MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_RECMND_INFO_FAIL, MSG_IPTV_EVT_GET_RECMND_INFO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE, MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_SOURCE_FORMAT_FAIL, MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_SEARCH_WORD_ARRIVE, MSG_IPTV_EVT_NEW_SEARCH_WORD_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_SEARCH_WORD_FAIL, MSG_IPTV_EVT_GET_SEARCH_WORD_FAIL)
END_AP_EVTMAP(xm_evtmap)

#endif
