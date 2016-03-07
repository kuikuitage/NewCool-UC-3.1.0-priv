/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"
#include "commonData.h"
#include "iptv_interface.h"
#include "ui_iptv_api.h"

#include "iqy_api.h"
#include "xingmei_api.h"

const VodPlayerInterface_t* GetIqyPlayerInterface(void);
const VodPlayerInterface_t* GetXingMeiPlayerInterface(void);

static u8 iptv_nc_id;
int cur_cat_id;	//Record the category id of level 2

inline static const VodDpInterface_t * ui_iptv_get_instance(void)
{
	switch(iptv_nc_id)
	{
		case IPTV_ID_IQY:
			#ifdef IPTV_SUPPORT_IQY
			return ui_iqy_return_instance();
			#endif
			break;
		case IPTV_ID_XM:
			#ifdef IPTV_SUPPORT_XM
			return ui_xm_return_instance();
			#endif
			break;
		default:
			break;
	}
	return NULL;
}

inline const VodPlayerInterface_t * ui_iptv_get_player_instance(void)
{
	switch(iptv_nc_id)
	{
		case IPTV_ID_IQY:
			#ifdef IPTV_SUPPORT_IQY
			return GetIqyPlayerInterface();
			#endif
			break;
		case IPTV_ID_XM:
			#ifdef IPTV_SUPPORT_XM
			return GetXingMeiPlayerInterface();
			#endif
			break;
		default:
			break;
	}
	return NULL;
}

void ui_iptv_dp_set_iptvId(u8 iptv_id)
{
	iptv_nc_id = iptv_id;
}

inline iptv_module_id_t ui_iptv_dp_get_iptvId(void)
{
	return iptv_nc_id;
}



void ui_iptv_dp_init(void)
{
	switch(iptv_nc_id)
	{
		case IPTV_ID_IQY:
			#ifdef IPTV_SUPPORT_IQY
			iqy_dp_init();
			#endif
			break;
		case IPTV_ID_XM:
			#ifdef IPTV_SUPPORT_XM
			xm_dp_init();
			#endif
			break;
		default:
			break;
	}
}
void ui_iptv_dp_deinit(void)
{
	switch(iptv_nc_id)
	{
		case IPTV_ID_IQY:
			#ifdef IPTV_SUPPORT_IQY
			iqy_dp_deinit();
			#endif
			break;
		case IPTV_ID_XM:
			#ifdef IPTV_SUPPORT_XM
			xm_dp_deinit();
			#endif
			break;
		default:
			break;
	}
}
void ui_iptv_register_msg(void)
{
	switch(iptv_nc_id)
	{
		case IPTV_ID_IQY:
			#ifdef IPTV_SUPPORT_IQY
			DEBUG(UI_IPTV,INFO,"@@@register_msg\n");
			iqy_dp_register_msg();
			#endif
			break;
		case IPTV_ID_XM:
			#ifdef IPTV_SUPPORT_XM
			DEBUG(UI_IPTV,INFO,"@@@register_msg\n");
			xm_dp_register_msg();
			#endif
			break;
		default:
			break;
	}
}
void ui_iptv_unregister_msg(void)
{
	switch(iptv_nc_id)
	{
		case IPTV_ID_IQY:
			#ifdef IPTV_SUPPORT_IQY
			iqy_dp_unregister_msg();
			#endif
			break;
		case IPTV_ID_XM:
			#ifdef IPTV_SUPPORT_XM
			xm_dp_unregister_msg();
			#endif
			break;
		default:
			break;
	}
}
void ui_iptv_free_msg_data(u16 msg, u32 para1, u32 para2)
{
	switch(iptv_nc_id)
	{
		case IPTV_ID_IQY:
			#ifdef IPTV_SUPPORT_IQY
			iqy_free_msg_data(msg, para1, para2);
			#endif
			break;
		case IPTV_ID_XM:
			#ifdef IPTV_SUPPORT_XM
			xm_free_msg_data(msg, para1, para2);
			#endif
			break;
		default:
			break;
	}
}

static u16 g_dp_state = IPTV_DP_STATE_UNINIT;
static u8  search_keyword[MAX_KEYWORD_LENGTH*3+1];
static BOOL g_plid_flag = TRUE;
volatile u32 vdo_identify_code;
static u32 search_res_id;



inline u16 ui_iptv_get_dp_state(void)
{
    DEBUG(UI_PLAY_API,INFO,"@@@ui_iptv_get_dp_state dp_state=%d\n", g_dp_state);
    return g_dp_state;
}

inline void ui_iptv_set_dp_state(u16 state)
{
    g_dp_state = state;
}

void ui_iptv_dp_del(void)
{
	const VodDpInterface_t * pvod = ui_iptv_get_instance();
	pvod->delCmdTask();
}


void ui_iptv_get_res_list(void)
{
	const VodDpInterface_t * pvod = ui_iptv_get_instance();
	if (pvod != NULL)
	{
#ifndef WIN32
		pvod->getCategoryList(0);
#endif
	}
}

void ui_iptv_get_catgry_list(void)
{
	const VodDpInterface_t * pvod = ui_iptv_get_instance();
	if (pvod != NULL)
	{
#ifndef WIN32
		pvod->getCategoryTypeList(0,0);
#endif
	}
}

extern iconv_t g_cd_utf8_to_utf16le;
extern iconv_t g_cd_utf16le_to_utf8;

void ui_iptv_get_video_list(u32 res_id, u16 *cat_name, u8 *key, u32 page_num)
{
	char *inbuf, *outbuf;
	IPTV_UPPAGE_REQ_T req = {0};
	size_t src_len, dest_len;
	const VodDpInterface_t * pvod = ui_iptv_get_instance();
	DEBUG(UI_PLAY_API,INFO,"res_id = %d,cat_name = %s,key = %s,page_num = %d/n",res_id,cat_name,key,page_num);
	if (pvod != NULL)
	{
		req.cat_id = (int)res_id;//一级ID
		extern int cur_cat_id;
		req.cat_id2 = cur_cat_id;//新媒根据二级的CAT_NAME获取到二级的CAT_ID
		DEBUG(UI_PLAY_API,INFO,"cur_cat_id = %d\n",cur_cat_id);
		if (cat_name)
		{
			inbuf = (char *)cat_name;
			outbuf = (char *)req.types;
			src_len = (uni_strlen(cat_name) + 1) * sizeof(u16);
			dest_len = sizeof(req.types);
			iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
		}
		
		if (key)
		{
			strcpy(req.keys, key);
		}
		req.page_size = 12;
		req.page_index = (int)page_num;
		req.query_mode = IPTV_QUERY_MODE_CATGRY;
		vdo_identify_code++;
		req.identify = vdo_identify_code;
		req.cb = ui_iptv_vdo_idntfy_cmp;
#ifndef WIN32
		pvod->updatePage(&req);
#endif
	}
}


void ui_iptv_get_video_list_for_filter(u32 res_id, u16 *cat_name, u8 *key, u32 page_num)
{
	char *inbuf, *outbuf;
	IPTV_UPPAGE_REQ_T req = {0};
	size_t src_len, dest_len;
	const VodDpInterface_t * pvod = ui_iptv_get_instance();

	if (pvod != NULL)
	{
		req.cat_id = (int)res_id;

		if (cat_name)
		{
			inbuf = (char *)cat_name;
			outbuf = (char *)req.types;
			src_len = (uni_strlen(cat_name) + 1) * sizeof(u16);
			dest_len = sizeof(req.types);
			iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
		}

		if (key)
		{
			strcpy(req.keys, key);
		}

		req.page_index = (int)page_num;
		req.query_mode = IPTV_QUERY_MODE_SCREENING;
		vdo_identify_code++;
		req.identify = vdo_identify_code;
		req.cb = ui_iptv_vdo_idntfy_cmp;
#ifndef WIN32
		pvod->updatePage(&req);
#endif
	}
}

void ui_iptv_search_set_res_id(u32 res_id)
{
	search_res_id = res_id;
}

void ui_iptv_search_set_keyword(u16 *keyword)
{
    char *inbuf, *outbuf;
    size_t src_len, dest_len;

    inbuf = (char *)keyword;
    outbuf = (char *)search_keyword;
    src_len = (uni_strlen(keyword) + 1) * sizeof(u16);
    dest_len = sizeof(search_keyword);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
}

void ui_iptv_search_word(void)
{
	u8  searchWord[MAX_KEYWORD_LENGTH*3+1] = {0};
	const VodDpInterface_t * pvod = ui_iptv_get_instance();

	if (pvod != NULL)
	{
		DEBUG(UI_PLAY_API,INFO,"search word \n");
		strcpy(searchWord, search_keyword);
#ifndef WIN32
		pvod->getSearchWord(searchWord);
#endif
	}
}

void ui_iptv_search(u32 page_num, u16 index)
{
	IPTV_UPPAGE_REQ_T req = {0};
	const VodDpInterface_t * pvod = ui_iptv_get_instance();

	if (pvod != NULL)
	{
		req.cat_id = (int)search_res_id;
		strcpy(req.pinyin, search_keyword);
		req.page_index = page_num;
		req.searchWordIndex = index;

		req.query_mode = IPTV_QUERY_MODE_PINYIN;
		vdo_identify_code++;
		req.identify = vdo_identify_code;
		req.cb = ui_iptv_vdo_idntfy_cmp;
#ifndef WIN32
		pvod->updatePage(&req);
#endif
	}
}

void ui_iptv_get_video_info(VDO_ID_t *vdo_id, u8 cat_id)//cat_id is type of the Program or series for xinmei
{
#if 1//SY DELETE
	const VodDpInterface_t * pvod = ui_iptv_get_instance();
	IPTV_VIDEO_INFO_REQ_T req = {0};
	if (pvod != NULL)
	{
		req.cat_id = cat_id;
		req.page_index = 1;
		req.id = *vdo_id;

		req.id.program_id= vdo_id->program_id;
		
		req.is_description = TRUE;
		req.page_size = 10;
		req.page_index = 1;
#ifndef WIN32
		pvod->getVideoInfo(&req);
#endif
	}
#endif	
}

void ui_iptv_get_info_url(VDO_ID_t * vdo_id, u8 cat_id, u16 *origin, u32 page_num, u32 page_size)
{
#if 1  //SY DELETE

	char *inbuf, *outbuf;
	IPTV_VIDEO_INFO_REQ_T req = {0};
	size_t src_len, dest_len;
	const VodDpInterface_t * pvod = ui_iptv_get_instance();

	if (pvod != NULL)
	{
		req.id = *vdo_id;

		inbuf = (char *)origin;
		outbuf = (char *)req.origin;
		src_len = (uni_strlen(origin) + 1) * sizeof(u16);
		dest_len = sizeof(req.origin);
		iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

		req.cat_id = cat_id;
		req.is_description = FALSE;

		req.page_index = page_num;
		req.page_size = page_size;
#ifndef WIN32
		pvod->getVideoInfo(&req);
#endif
	}
#endif	
}

void ui_iptv_get_recmnd_info(VDO_ID_t * vdo_id, u32 res_id, u16 *area)
{
	char *inbuf, *outbuf;
	IPTV_RECOMMEND_REQ_T req;
	size_t src_len, dest_len;
	const VodDpInterface_t * pvod = ui_iptv_get_instance();

	if (pvod != NULL)
	{
		memset(&req,0,sizeof(req));
		req.id = *vdo_id;
		req.cat_id = res_id;
		inbuf = (char *)area;
		outbuf = (char *)req.area;
		src_len = (uni_strlen(area) + 1) * sizeof(u16);
		dest_len = sizeof(req.area);
		iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
#ifndef WIN32
		pvod->getRecommendInfo(&req);
#endif
	}
}

void ui_iptv_get_play_format(u8 *tvQid,u8 *vid,int episode_num)
{
	IPTV_VIDEO_URL_REQ_T req;
	const VodDpInterface_t * pvod = ui_iptv_get_instance();

	if (pvod != NULL)
	{
		memset((void *)&req, 0, sizeof(IPTV_VIDEO_URL_REQ_T));
		memcpy(req.tvQid,tvQid,sizeof(req.tvQid));
		memcpy(req.vid,vid,sizeof(req.vid));
		req.episode_num = episode_num;
#ifndef WIN32
		pvod->getPlayUrlFormat(&req);
#endif
	}
}

void ui_iptv_get_play_url(IPTV_API_FORMAT_T format,u8 *tvQid,u8 *vid,int episode_num)
{
	IPTV_VIDEO_URL_REQ_T req;
	const VodDpInterface_t * pvod = ui_iptv_get_instance();

	if (pvod != NULL)
	{
		memset((void *)&req, 0, sizeof(IPTV_VIDEO_URL_REQ_T));
		memcpy(req.tvQid,tvQid,sizeof(req.tvQid));
		memcpy(req.vid,vid,sizeof(req.vid));
		req.episode_num  = episode_num;
		req.format = format;
#ifndef WIN32
		pvod->getPlayUrl(&req);
#endif
	}
}

void ui_iptv_set_page_size(u16 page_size)
{
	const VodDpInterface_t * pvod = ui_iptv_get_instance();

	if (pvod != NULL)
	{
		pvod->setPageSize((int)page_size);
	}
}

int ui_iptv_vdo_idntfy_cmp(u32 unidntf_code)
{
    return vdo_identify_code - unidntf_code;
}

void ui_iptv_set_flag_to_plid(BOOL flag)   //从播单播放时候，传进来
{
    DEBUG(IPLAY, INFO,"g_plid_flag = %d \n",flag);
    g_plid_flag = flag;
}

BOOL ui_iptv_get_flag_to_plid(void)
{
    DEBUG(IPLAY, INFO,"g_plid_flag = %d \n",g_plid_flag);
    return g_plid_flag;
}




