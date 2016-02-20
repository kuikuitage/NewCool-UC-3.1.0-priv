#include "sys_types.h"
#include "ads_ware.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "dmx.h"
#include "nim.h"
#include "ads_api.h"
#include "ads_xml_parser.h"
#include "mtos_printk.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "ads_drv_dvbad.h"
#include "sys_dbg.h"
#include "fcrc.h"
#include "ads_dvbad_adt.h"
#include "ui_common.h"


#define ADS_DRV_PRINTF	OS_PRINTF

#define DVBAD_ADS_PIC_DATA_SIZE	(480*576)
#define ADS_DVBAD_TASK_STKSIZE    (128 * KBYTES)
#define DVB_ADS_FILTER_MAX_NUM    (10)
#define DVBAD_ADS_DMX_BUF_SIZE  (64 * KBYTES + 188)
#define NIT_PID (0x10)
#define BAT_PID	(0x1F41)
#define PAT_PID (0x0)
#define NIT_TABLE_ID  (0x40)
#define BAT_PID		(0x1F41)
#define DATA_PID	(0x1F40)
#define DATA_TABLE_ID	(0xFD)
#define AD_HEAD_TABLE_ID	(0xFE)
#define AD_DATA_TABLE_ID	(0xFD)
#define DATA_TYPE_XML	0x01
#define DATA_TYPE_PIC_INFO	0x02
#define WRITE_FLASH_PIC_NUM		(10)
//#define AD_FLASH_SIZE			(1152*KBYTES)
#define AD_FLASH_SIZE			(512*KBYTES)

#ifdef WIN32
static u8 *p_test_buf = NULL;
#endif


typedef enum
{
    DVB_ADS_FREE = 0,
    DVB_ADS_USED
}dvbad_ads_status_t;

typedef enum
{
	DVBAD_DATA_FREE,
	DVBAD_DATA_TRUE,
	DVBAD_DATA_FINISH,
	DVBAD_DATA_FALSE,
}dvbad_data_return;

typedef struct
{
	dmx_device_t          	* p_dev;
	u16 					filter_id;
	dvbad_ads_status_t     	filter_status;
	dmx_chanid_t          	channel_filter_id;
	u16              	    channel_pid;
	u16              		filter_num;
	u8               		filter_data[DVBAD_FILTER_MAX_LEN];
	u8           			filter_mask[DVBAD_FILTER_MAX_LEN];
	u8             			* p_buf;
	u32          			start_time;
	u32          			timeout;
	void (*callback_func)(u16 pid, u8 *p_data, u16 data_len);
}dvbad_ads_filter_struct_t;

typedef enum{
	DVBAD_TYPE_LOGO=1,
	DVBAD_TYPE_MAINMENU,
	DVBAD_TYPE_SUBMENU,
	DVBAD_TYPE_STRIPE,
	DVBAD_TYPE_TOTAL,
}ADS_DVBAD_TYPE;

typedef struct
{
	ADS_DVBAD_TYPE dvbad_type;
	u16 ads_x;
	u16 ads_y;
	u16 ads_w;
	u16 ads_h;
}ads_pic_info;

typedef struct
{
	u16 id;
	ads_pic_info pic_info;
	u8  ads_pic_data[DVBAD_ADS_PIC_DATA_SIZE];
}pic_data;

typedef struct{
	u16 index_xml_table_id_start;
	u16 index_xml_table_id_end;
	u32 index_xml_size;
}ad_head_xml_info;

typedef struct{
	u16 pic_information_table_id_start;
	u16 pic_information_table_id_end;
	u32 pic_information_size;
}ad_head_pic_info;

typedef struct{
	ad_head_xml_info ads_xml_info;
	ad_head_pic_info ads_pic_info;
}down_ctrl_info;

typedef struct _filter_buf_node
{
	u8 *p_data;
	u32 data_len;
	struct _filter_buf_node* next;
}filter_buf_node;

typedef struct{
	u8 set_section_number;
	u8 set_last_section_number;
	u8 set_data_type;
	u16 set_table_id;
	BOOL new_table_flag;
}set_filter_status;

typedef struct dvbad_priv_t{
 	os_sem_t 					lock;
  	dvbad_ads_filter_struct_t 	filter[DVB_ADS_FILTER_MAX_NUM];
 	ST_ADS_DVBAD_FILTER_INFO 	filter_data[DVB_ADS_FILTER_MAX_NUM];
 	u8 							filter_buf[DVB_ADS_FILTER_MAX_NUM][DVBAD_ADS_DMX_BUF_SIZE];

	//ad data
	down_ctrl_info 				down_ctl;
	u8 * 						xml;//xml data buffer
	u32							xml_pos;//databuffer write pointer
	u8 * 						pic_info;
	u32							pic_info_pos;
	u8 *						pic_data;
	u32							pic_data_pos;
	dvbad_ads_pic_data			*p_ad_data;
	
}dvbad_priv_t;

extern BOOL add_fist_data( ads_data_node** p_node, ads_pic_attribute data);
extern ads_data_node* create_list_data(ads_pic_attribute data);
extern void delete_node(ads_data_node** pNode);
extern void dvb_set_ad_flag(get_ad_flag flag);
extern RET_CODE ads_xml_parse(const xmlChar *p_buf, u32 buf_len, dvbad_ads_pic_data *p_dvbad_ads_pic_attribute_data);
extern ads_module_priv_t *p_dvbad_priv;


static dvbad_priv_t s_dvbad;

static void exit_monitor(get_ad_flag flag)
{	
	DEBUG(ADS, NOTICE, "exit get ad monitor\n");
	dvb_set_ad_flag(flag);
	mtos_task_exit();		
}

static void nvm_ad_block_crc_init(void)
{
	RET_CODE ret;

	/* create crc32 table */
	ret = crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
	MT_ASSERT(SUCCESS == ret); 
}

static u32 nvm_ad_block_generate(u8 *buf, u32 len)
{
	return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, buf, len);
}

static RET_CODE add_data_to_list(ads_data_node **p_list, ads_pic_attribute pic_attr)
{
	ads_data_node *p_node;
	ads_data_node *p_data_node = NULL;
	if((NULL==p_list)||(NULL==*p_list))
	{
		DEBUG(ADS, INFO, "null == p_list\n");
		return ERR_FAILURE;
	}
	DEBUG(ADS, INFO, "\n");
	p_data_node = create_list_data(pic_attr);
	p_node = *p_list;
	while(NULL!=p_node->next)
		p_node = p_node->next;

	DEBUG(ADS, INFO, "type:%d\n", p_node->ads_pic_attribute_data.type);
	p_node->next = p_data_node;
	return SUCCESS;
}

static RET_CODE add_first_data_to_list(ads_data_node *p_list, ads_pic_attribute attr_data)
{
	if(NULL==p_list)
	{
		DEBUG(ADS, INFO, "null == p_list\n");
		return ERR_FAILURE;
	}
	memcpy(&p_list->ads_pic_attribute_data, &attr_data, sizeof(ads_pic_attribute));
	return SUCCESS;
}

static void dvbad_ads_lock(void)
{
    int ret = FALSE;
    ret = mtos_sem_take((os_sem_t *)&s_dvbad.lock, 0);

    if (!ret)
    {
        MT_ASSERT(0);
    }
}

static void dvbad_ads_unlock(void)
{
    int ret = FALSE;

    ret = mtos_sem_give((os_sem_t *)&s_dvbad.lock);
    if (!ret)
    {
        MT_ASSERT(0);
    }
}

static RET_CODE dvbad_set_sum(u8 *p_data, u32 data_len)
{
	u32 i;
	u8 sum = 0;
	if(NULL==p_data)
	{
		DEBUG(ADS, ERR, "NULL == p_data\n");
		return ERR_FAILURE;
	}

	for(i = 0;i<data_len-1;i++)
		sum += p_data[i];
	p_data[data_len-1] = sum;

	DEBUG(ADS, INFO, "sum=%d %d data_len=%d\n",sum, p_data[data_len-1],data_len);
	return SUCCESS;
}

static RET_CODE dvbad_check_sum(u8 *p_data, u32 data_len)
{
	u32 i;
	u8 sum = 0;
	if(NULL==p_data)
	{
		DEBUG(ADS, ERR, "null == p_data\n");
		return ERR_FAILURE;
	}
	
	for(i = 0; i<data_len-1; i++)
		sum += p_data[i];

	DEBUG(ADS, INFO, "sum=%d  %d data_len = %d\n",sum, p_data[data_len-1],data_len);
	if(sum == p_data[data_len-1])
		return SUCCESS;
	return ERR_FAILURE;
}

static RET_CODE dvbad_drv_erase(u32 AD_length)
{
  RET_CODE ret = ERR_FAILURE;
  u32 elength = AD_length;

  DEBUG(ADS, INFO, "Erase ad flash.size:%#x\n", elength);
  MT_ASSERT(elength != 0);
  if(NULL != p_dvbad_priv->nv_erase)
  {
    ret = p_dvbad_priv->nv_erase(elength);
    if(SUCCESS != ret)
    {
        DEBUG(ADS, ERR,"Erase flash data failed.\n");
        return ERR_FAILURE;
    }
  }
  else
  {
	DEBUG(ADS, ERR, "Erase ad flash failure\n");
	return ERR_FAILURE;
  }

  DEBUG(ADS, INFO, "Erase ad flash success\n");
  return SUCCESS;
}

static ads_data_node *dvbad_get_node_by_id(ads_data_node* p_data, u16 id)
{
	ads_data_node *p = p_data;
	while (id != p->ads_pic_attribute_data.file_number)
	{
		if (NULL== p->next)
		{
			DEBUG(ADS,ERR, "ERROR!(NULL== *p->dvbad_pic_attribute_node)\n");
			return NULL;
		}
		p = p->next;
	}
	return p;
}

static RET_CODE dvbad_drv_readflash(u32 flashOffset, u8 *extBuffer, u32 size)
{
  RET_CODE ret = ERR_FAILURE;
  u32 size_back = size;
  u32 offset = flashOffset;

  DEBUG(ADS, INFO, "nvrOffset=%x,size=%x\n",flashOffset,size);

  MT_ASSERT(extBuffer != NULL);
  if(NULL != p_dvbad_priv->nv_read && size_back != 0)
  {
    ret = p_dvbad_priv->nv_read(offset, extBuffer, (u32 *)&size_back);
    if(SUCCESS != ret)
    {
        DEBUG(ADS, INFO, "Read flash data failed.\n");
        return ERR_FAILURE;
     }
  }
  return  SUCCESS;
}

static RET_CODE dvbvad_drv_writeflash(u32 flashOffset, u8 * extBuffer, u32 size)
{
	RET_CODE ret = ERR_FAILURE;
	u32 size_back = size;
	u32 offset = flashOffset;

	DEBUG(ADS, INFO, "Offset=%x,extBuffer=%#x,size=%#x\n",flashOffset,extBuffer,size);

	if(NULL==extBuffer)
	{
		DEBUG(ADS, ERR, "NULL data write to flash\n");
		return ERR_FAILURE;
	}
	if(NULL != p_dvbad_priv->nv_write && size_back != 0)
	{
		ret = p_dvbad_priv->nv_write(offset, extBuffer, size_back);
		if(SUCCESS != ret)
		{
			DEBUG(ADS, ERR,"Write flash data failed.\n");
			return ERR_FAILURE;
		}
	}
	return SUCCESS;
}

static RET_CODE	add_flash_data_to_list(ad_head_flash ad_head_data,u8 *p_buf, u32 *read_data_len,ads_data_node **p)
{
	ads_pic_attribute pic_attr;
	ads_data_node *p_node = NULL;
	u16 text_size;

	DEBUG(ADS, INFO, "read_data_len:%d\n", *read_data_len);
	text_size = (((u16)p_buf[*read_data_len])<<8)|(u16)p_buf[*read_data_len+1];
	DEBUG(ADS, INFO, "text_size:%#x\n, p_buf[0]:%d,p_buf[1]:%d, text_size addr:%d\n",
						text_size, p_buf[*read_data_len], p_buf[*read_data_len+1],
						*read_data_len);
	*read_data_len += 2;

	memcpy(&pic_attr, p_buf+*read_data_len, sizeof(ads_pic_attribute));
	DEBUG(ADS, INFO, "file_number:%d, ads_pic_attribute_data addr:%d\n", pic_attr.file_number, *read_data_len);
	*read_data_len += sizeof(ads_pic_attribute);
	if(NULL==(*p)->p_ads_pic_data)
	{
		if(SUCCESS!=add_first_data_to_list(*p, pic_attr))
			return ERR_FAILURE;
	}
	else
	{
		
		if(SUCCESS!=add_data_to_list(p, pic_attr))
			return ERR_FAILURE;
	}
	p_node = dvbad_get_node_by_id(*p, pic_attr.file_number);
	if(NULL==p_node)
	{
		DEBUG(ADS, ERR, "null==p_node\n");
		return ERR_FAILURE;
	}
	DEBUG(ADS, INFO, "p_node:%d, p_ads_pic_data_addr:%d\n", p_node, p_node->p_ads_pic_data);
	p_node->text_size = text_size;
	if(p_node->text_size)
	{
		p_node->ads_pic_attribute_data.text = mtos_malloc(p_node->text_size);
		if(NULL==p_node->ads_pic_attribute_data.text)
		{
			DEBUG(ADS, ERR, "malloc failure\n");
			return ERR_FAILURE;
		}
		DEBUG(ADS, INFO, "text_size:%d\n", p_node->text_size);
		memcpy(p_node->ads_pic_attribute_data.text, p_buf+*read_data_len, p_node->text_size);
		DEBUG(ADS, INFO, "test addr:%d\n", *read_data_len);
		*read_data_len += p_node->text_size;
	}
	DEBUG(ADS, INFO, "pic_info_addr:%d\n", *read_data_len);
	memcpy(&p_node->p_pic_info, p_buf+*read_data_len, sizeof(ads_pic_infomation));
	*read_data_len += sizeof(ads_pic_infomation);
	DEBUG(ADS, INFO, "p_ads_pic_data_addr:%d, pic_original_size:%d\n", p_node->p_ads_pic_data, p_node->p_pic_info.pic_original_size);
	p_node->p_ads_pic_data = mtos_malloc(p_node->p_pic_info.pic_original_size);
	DEBUG(ADS, INFO, "\n");
	if(NULL==p_node->p_ads_pic_data)
	{
		DEBUG(ADS, ERR, "malloc failure\n");
		return ERR_FAILURE;
	}
	memcpy(p_node->p_ads_pic_data, p_buf+*read_data_len, p_node->p_pic_info.pic_original_size);
	*read_data_len += p_node->p_pic_info.pic_original_size;

	DEBUG(ADS, INFO, "*********************************\n");
	DEBUG(ADS, INFO, "one pic parser end.num:%d\n", ad_head_data.ad_pic_num);
	DEBUG(ADS, INFO, "file_number:%d\n", p_node->ads_pic_attribute_data.file_number);
	DEBUG(ADS, INFO, "text_size:%d\n", p_node->text_size);
	DEBUG(ADS, INFO, "id.num:%d\n", p_node->p_pic_info.pic_id);
	DEBUG(ADS, INFO, "pic_size:%d\n", p_node->p_pic_info.pic_original_size);
	DEBUG(ADS, INFO, "*********************************\n");
	return SUCCESS;
}

static RET_CODE read_ad_from_flash(dvbad_ads_pic_data *p_ads_pic_data)
{
	u32 read_data_len;
	u8 *p_buf = NULL;
	ads_data_node *p = NULL;
	ads_data_node *p_node = NULL;
	u32 crc32_read;
	u32 crc32_check;
	ad_head_flash ad_head_data;
	RET_CODE ret = ERR_FAILURE;
#ifndef WIN32	
	ret = dvbad_drv_readflash(0, (u8*)&ad_head_data, sizeof(ad_head_flash));
	if(SUCCESS!=ret)
	{
		DEBUG(ADS, ERR, "read flash failure\n");
		return DVBAD_UPDATE_FAILURE;
	}
	DEBUG(ADS, INFO, "ad_pic_num:%d\n",ad_head_data.ad_pic_num);
	DEBUG(ADS, INFO, "sum=%d\n",ad_head_data.sum);
	DEBUG(ADS, INFO, "version_number=%d\n",ad_head_data.version_number);
	DEBUG(ADS, INFO, "AD_version=%d\n",ad_head_data.AD_version);
	DEBUG(ADS, INFO, "buff_size=%d\n",ad_head_data.buff_size);
	DEBUG(ADS, INFO, "version_number=%d\n",ad_head_data.save_all_pic_flag);
	
	if(SUCCESS!=dvbad_check_sum((u8 *)&ad_head_data, sizeof(ad_head_flash)))
	{
		DEBUG(ADS, ERR, "read ad data from flash and check sum failure\n");
		return DVBAD_UPDATE_FAILURE;
	}
	
	p_buf = mtos_malloc(ad_head_data.buff_size);
	if(NULL==p_buf)
	{
		DEBUG(ADS, ERR, "NULL == p_buf");
		return ERR_FAILURE;
	}
	memset(p_buf, 0, ad_head_data.buff_size);
	DEBUG(ADS, INFO, "buff_size=%d\n",ad_head_data.buff_size);
	ret = dvbad_drv_readflash(0, p_buf,ad_head_data.buff_size);
	if(SUCCESS!=ret)
	{
		DEBUG(ADS, ERR, "read flash failure\n");
		return ERR_FAILURE;
	}
#else
	memcpy(p_buf, p_test_buf, ad_head_data.buff_size);
	mtos_free(p_test_buf);
#endif
	crc32_read = (u32)p_buf[ad_head_data.buff_size-1]+
				 (((u32)p_buf[ad_head_data.buff_size-2])<<8)+
				 (((u32)p_buf[ad_head_data.buff_size-3])<<16)+
				 (((u32)p_buf[ad_head_data.buff_size-4])<<24);
	DEBUG(ADS, INFO, "pic_num:%d, crc32_read:%#x, start get crc32_check\n", ad_head_data.ad_pic_num, crc32_read);
	nvm_ad_block_crc_init();
	crc32_check = nvm_ad_block_generate(p_buf, ad_head_data.buff_size-4);
	if(crc32_read!=crc32_check)
	{
		DEBUG(ADS, ERR, "crc32 check failure\n");
		mtos_free(p_buf);
		return ERR_FAILURE;
	}
	p = &p_ads_pic_data->dvbad_pic_attribute_node;
	if(NULL!=p->next)
	{
		delete_node(&p->next);
		DEBUG(ADS, INFO, "p:%d, addr:%d\n", p, &p_ads_pic_data->dvbad_pic_attribute_node);
	}
	if(NULL!=p->p_ads_pic_data)
	{
		DEBUG(ADS, INFO, "free p_ads_pic_data\n");
		mtos_free(p->p_ads_pic_data);
		p->p_ads_pic_data = NULL;
	}
	memset(p, 0, sizeof(ads_data_node));
	p = &p_ads_pic_data->dvbad_pic_attribute_node;
	memcpy(&p_ads_pic_data->ad_head_data, p_buf, sizeof(ad_head_flash));
	read_data_len = sizeof(ad_head_flash);
	memcpy(&p_ads_pic_data->ad_bat_data, p_buf+read_data_len, sizeof(bat_data));
	read_data_len += sizeof(bat_data);
	memcpy(&p_ads_pic_data->dvbad_ads_rcv_config, p_buf+read_data_len, sizeof(ads_rcv_config));
	read_data_len += sizeof(ads_rcv_config);
	DEBUG(ADS, INFO, "start parser flash data.pic_num:%d, buff size:%#x\n", ad_head_data.ad_pic_num, ad_head_data.buff_size);
	while(ad_head_data.ad_pic_num>0)
	{
		if(SUCCESS!=add_flash_data_to_list(ad_head_data, p_buf, &read_data_len, &p))
		{
			DEBUG(ADS, ERR, "add_flash_data_to_list failure\n");
			mtos_free(p_buf);
			return ERR_FAILURE;
		}
		ad_head_data.ad_pic_num--;
		DEBUG(ADS, INFO, "read one pic end.ad_pic_num:%d\n", ad_head_data.ad_pic_num);
	}
	
	DEBUG(ADS, INFO, "read ad from flash\n");
	p_node = &p_ads_pic_data->dvbad_pic_attribute_node;
	while(NULL!=p_node)
	{
		DEBUG(ADS, INFO, "id:%d; type:%d\n", p_node->p_pic_info.pic_id, p_node->ads_pic_attribute_data.type);
		p_node = p_node->next;
	}
	mtos_free(p_buf);
	return SUCCESS;
}

static RET_CODE dvbad_save_to_flash()
{
    RET_CODE ret = ERR_FAILURE;
    u8 *flash_data = NULL;
	u8 p_write_crc_buffer[4];
	u8 *p_buf = NULL;
	u32 crc_32_write;
	u32 flash_data_size;
	dvbad_priv_t *ad = &s_dvbad;
	ads_data_node *p;

	DEBUG(ADS, INFO, "Start save ad to flash\n");
	if(NULL==ad->p_ad_data)
	{
		DEBUG(ADS, ERR, "Have no data save to flash\n");
		return ERR_FAILURE;
	}
#ifndef WIN32
    ret = dvbad_drv_erase(p_dvbad_priv->flash_size);
    if(ret != SUCCESS)
        return ERR_FAILURE;
#endif
	flash_data_size = 0;
    flash_data = mtos_malloc(AD_FLASH_SIZE);
    if(flash_data == NULL)
    {
        DEBUG(ADS, ERR, "**memory malloc wrong\n");
        return ERR_FAILURE;
    }
    memset(flash_data, 0, AD_FLASH_SIZE);

	flash_data_size = sizeof(ad_head_flash);
	memcpy(flash_data+flash_data_size, &ad->p_ad_data->ad_bat_data, sizeof(bat_data));
	flash_data_size += sizeof(bat_data);
	memcpy(flash_data+flash_data_size, &ad->p_ad_data->dvbad_ads_rcv_config, sizeof(ads_rcv_config));
	flash_data_size += sizeof(ads_rcv_config);

	p = &ad->p_ad_data->dvbad_pic_attribute_node;
	ad->p_ad_data->ad_head_data.save_all_pic_flag = FALSE;
	ad->p_ad_data->ad_head_data.ad_pic_num = 0;
    while(NULL != p)
    {
    	if((flash_data_size+sizeof(ads_pic_attribute) + 2 + p->text_size+
					sizeof(ads_pic_infomation)+p->p_pic_info.pic_original_size+4)>=AD_FLASH_SIZE)
		{
			DEBUG(ADS, ERR, "flash_data_size > AD_FLASH_SIZE! Have no enough flash\n");
			break;
		}
		flash_data[flash_data_size] = (u8)((p->text_size&0xFF00)>>8);
		flash_data[flash_data_size+1] = (u8)(p->text_size&0x00FF);
		DEBUG(ADS, INFO, "text_size:%#x,flash_data[0]:%d, flash_data[1]:%d, flash_data_size:%d\n", 
						p->text_size, flash_data[flash_data_size], flash_data[flash_data_size+1], flash_data_size);
		flash_data_size += 2;
		memcpy(flash_data+flash_data_size, &p->ads_pic_attribute_data, sizeof(ads_pic_attribute));
		flash_data_size += sizeof(ads_pic_attribute);
		if(p->text_size)
		{
			memcpy(flash_data+flash_data_size, p->ads_pic_attribute_data.text, p->text_size);
			flash_data_size += p->text_size;
		}
		memcpy(flash_data+flash_data_size, &p->p_pic_info, sizeof(ads_pic_infomation));
		DEBUG(ADS, INFO, "id:%d, p_pic_info addr:%d\n",p->p_pic_info.pic_id, flash_data_size);
		flash_data_size += sizeof(ads_pic_infomation);
		
		memcpy(flash_data+flash_data_size, p->p_ads_pic_data, p->p_pic_info.pic_original_size);
		DEBUG(ADS, INFO, " p->p_pic_info.pic_original_size:%d\n",  p->p_pic_info.pic_original_size);
 		flash_data_size += p->p_pic_info.pic_original_size;

		ad->p_ad_data->ad_head_data.ad_pic_num++;
		
		DEBUG(ADS, INFO, "*********************************\n");
		DEBUG(ADS, INFO, "pack end.num:%d\n", ad->p_ad_data->ad_head_data.ad_pic_num);
		DEBUG(ADS, INFO, "file_number:%d\n", p->ads_pic_attribute_data.file_number);
		DEBUG(ADS, INFO, "ad type:%d\n", p->ads_pic_attribute_data.type);
		DEBUG(ADS, INFO, "channel_type:%d\n", p->ads_pic_attribute_data.channel_type);
		DEBUG(ADS, INFO, "p_format:%d\n", p->ads_pic_attribute_data.p_format);
		DEBUG(ADS, INFO, "text_size:%d\n", p->text_size);
		DEBUG(ADS, INFO, "id.num:%d\n", p->p_pic_info.pic_id);
		DEBUG(ADS, INFO, "pic_size:%d\n", p->p_pic_info.pic_original_size);
		DEBUG(ADS, INFO, "*********************************\n");
		p = p->next;
		if(NULL==p)
		{
			ad->p_ad_data->ad_head_data.save_all_pic_flag = TRUE;
		}
    }
	if(ad->p_ad_data->ad_head_data.ad_pic_num<=0)
	{
		DEBUG(ADS, ERR, "Have now pic to save!\n");
		return ERR_FAILURE;
	}
	ad->p_ad_data->ad_head_data.buff_size = flash_data_size+4;
	if(SUCCESS!=dvbad_set_sum((u8 *)&ad->p_ad_data->ad_head_data, sizeof(ad_head_flash)))
	{
		mtos_free(flash_data);
		DEBUG(ADS, ERR, "dvbad_set_sum!\n");
		return ERR_FAILURE;
	}
	DEBUG(ADS, INFO, "pic_num:%d, AD_version:%d\n", ad->p_ad_data->ad_head_data.ad_pic_num, ad->p_ad_data->ad_head_data.AD_version);
	memcpy(flash_data, &ad->p_ad_data->ad_head_data, sizeof(ad_head_flash));
	if(flash_data_size > p_dvbad_priv->flash_size)
	{
		DEBUG(ADS, ERR, "flash is small: 0x%x, 0x%x",flash_data_size, p_dvbad_priv->flash_size);
		mtos_free(flash_data);
		return ERR_FAILURE;
	}

	p_buf = mtos_malloc(flash_data_size);
	if(NULL==p_buf)
	{
		DEBUG(ADS, ERR, "malloc failure\n");
		mtos_free(flash_data);
		return ERR_FAILURE;
	}
	memcpy(p_buf, flash_data, flash_data_size);
    nvm_ad_block_crc_init();
    crc_32_write = nvm_ad_block_generate(p_buf, flash_data_size);
    p_write_crc_buffer[0] = (u8)(crc_32_write >>24)&0xff;
    p_write_crc_buffer[1] = (u8)(crc_32_write >>16)&0xff;
    p_write_crc_buffer[2] = (u8)(crc_32_write >>8)&0xff;
    p_write_crc_buffer[3] = (u8)(crc_32_write >>0)&0xff;
	memcpy(flash_data+flash_data_size, p_write_crc_buffer, 4);
#ifndef WIN32
	DEBUG(ADS, INFO, "crc_32_write:%#x\n", crc_32_write);
    ret = dvbvad_drv_writeflash(0, flash_data, flash_data_size+4);
    if(ret != SUCCESS)
    {
        DEBUG(ADS, ERR, "ad write flash wrong\n");
		mtos_free(p_buf);
        mtos_free(flash_data);
        return ERR_FAILURE;
    }
#else
	p_test_buf = mtos_malloc(flash_data_size+4);
	memcpy(p_test_buf, flash_data, flash_data_size+4);
#endif


	mtos_free(p_buf);
    mtos_free(flash_data);
	DEBUG(ADS, INFO, "Save ad to flash succcess\n");
    return SUCCESS;

}

static RET_CODE get_log_pid(u16 *pid)
{
	dvbad_priv_t *ad = &s_dvbad;
	ads_data_node *p_node = &(ad->p_ad_data->dvbad_pic_attribute_node);

	while(DVBAD_STB_LOG!=p_node->ads_pic_attribute_data.type)
	{
		if(NULL==p_node->next)
			return ERR_FAILURE;
		p_node = p_node->next;
	}
	
	*pid = p_node->ads_pic_attribute_data.file_number;
	return SUCCESS;
}

static ads_data_node *dvbad_get_data_node_by_serviceid(ads_data_node *p_node, DVBAD_POS_TYPE ad_type, u32 service_id)
{
	ads_data_node *p = p_node;
	int count = 0;
	int chose = 0;

	if(NULL==p_node)
	{
		DEBUG(ADS, ERR, "null==p_node\n");
		return NULL;
	}
	DEBUG(ADS, INFO, "service_id:%d\n", service_id);


	while (p->next != NULL)
	{
		if(ad_type == p->ads_pic_attribute_data.type)
		{
			if(service_id==(u16)atoi((char*)p->ads_pic_attribute_data.type_name))
				count ++;
		}
		p = p->next;
	}
	if (count == 0)
		return NULL;


	chose = rand()%count;
	chose += 1; /*% means 0~count-1*/
	DEBUG(ADS, ERR, "count:%d chose:%d\n", count,chose);	
	count = 0;
	p = p_node;
	while (p->next != NULL)
	{
		if(ad_type == p->ads_pic_attribute_data.type)
		{
			if(service_id==(u16)atoi((char*)p->ads_pic_attribute_data.type_name))
				count ++;
		}

		if (count == chose)
			return p;
		p = p->next;
	}
	return NULL;
}


static ads_data_node *dvbad_get_data_node(ads_data_node *p_node, DVBAD_POS_TYPE ad_type)
{
	ads_data_node *p = p_node;

	if(NULL==p_node)
	{
		DEBUG(ADS, ERR, "null==p_node\n");
		return NULL;
	}
	DEBUG(ADS, INFO, "get data node now.ad_type:%d\n", ad_type);
	DEBUG(ADS, INFO, "p ->ads_pic_attribute_data.type:%d\n", p ->ads_pic_attribute_data.type);
	while(ad_type!=p ->ads_pic_attribute_data.type)
	{
		if(NULL==p->next)
		{
			DEBUG(ADS, ERR, "get data node failure\n");
			return NULL;
		}
		p  = p ->next;
	}

	DEBUG(ADS, INFO, "type:%d\n", p->ads_pic_attribute_data.type);
	return p;
}

RET_CODE dvbad_get_logo_flag(void)
{
	dvbad_priv_t *ad = &s_dvbad;
	ads_data_node *p_node = &(ad->p_ad_data->dvbad_pic_attribute_node);
	
	dvbad_ads_lock();
	p_node =  dvbad_get_data_node(p_node, DVBAD_STB_LOG);
	if(NULL!=p_node)
	{
		dvbad_ads_unlock();
		return SUCCESS;
	}
	dvbad_ads_unlock();
	return ERR_FAILURE;
}

RET_CODE dvbad_get_logo_size(dvbad_log_save_info *p_logo_info)
{
	dvbad_priv_t *ad = &s_dvbad;
	ads_data_node *p_node = &(ad->p_ad_data->dvbad_pic_attribute_node);

	if(NULL==p_logo_info)
		return FALSE;
	dvbad_ads_lock();
	p_node = dvbad_get_data_node(p_node, DVBAD_STB_LOG);
	if(NULL == p_node)
	{
		dvbad_ads_unlock();
		return FALSE;
	}

	p_logo_info->uiLogoSize = p_node->p_pic_info.pic_original_size;
	p_logo_info->ucLogType = p_node->ads_pic_attribute_data.p_format;
	dvbad_ads_unlock();

	DEBUG(ADS, INFO, "get logo size end.uiLogoSize:%d,ucLogType:%d\n", p_logo_info->uiLogoSize, p_logo_info->ucLogType);
	return TRUE;
}

RET_CODE dvbad_get_logo(u8 *p_log_data, u32 size)
{
	dvbad_priv_t *ad = &s_dvbad;
	ads_data_node *p_node = &(ad->p_ad_data->dvbad_pic_attribute_node);

	if(NULL==p_log_data)
		return FALSE;
	dvbad_ads_lock();
	p_node = dvbad_get_data_node(p_node, DVBAD_STB_LOG);
	if(NULL == p_node)
	{
		dvbad_ads_unlock();
		return FALSE;
	}
	if(NULL==p_log_data)
	{
		DEBUG(ADS, ERR, "malloc failure\n");
		dvbad_ads_unlock();
		return FALSE;
	}

	memcpy(p_log_data, p_node->p_ads_pic_data, size);
	dvbad_ads_unlock();
	DEBUG(ADS, INFO, "get logo end\n");
	return TRUE;
}

RET_CODE dvbad_get_ad(dvbad_program_para para, dvbad_show_pic_info *p_pic, DVBAD_POS_TYPE ad_type)
{
	dvbad_priv_t *ad = &s_dvbad;
	ads_data_node *p_node = &(ad->p_ad_data->dvbad_pic_attribute_node);

	dvbad_ads_lock();
	if((DVBAD_STB_CURRENT_FOLLOW == ad_type)||(DVBAD_STB_CHANNEL_INFO == ad_type))
	{
		DEBUG(ADS, INFO, "current pro usServiceId:%d\n", para.usServiceId);
		p_node = dvbad_get_data_node_by_serviceid(p_node, ad_type, (u32)para.usServiceId);
		if(NULL == p_node)
		{	//reset p_node or it will be NULL
			p_node = &(ad->p_ad_data->dvbad_pic_attribute_node);
			p_node = dvbad_get_data_node_by_serviceid(p_node, ad_type, (u32)0);
		}
	}
	else
	{
		p_node = dvbad_get_data_node(p_node, ad_type);
	}
	if(NULL == p_node)
	{
		dvbad_ads_unlock();
		return ERR_FAILURE;
	}
	p_pic->data_len = p_node->p_pic_info.pic_original_size;
	p_pic->h = p_node->ads_pic_attribute_data.h;
	p_pic->w = p_node->ads_pic_attribute_data.w;
	p_pic->x = p_node->ads_pic_attribute_data.x;
	p_pic->y = p_node->ads_pic_attribute_data.y;
	p_pic->pic_id = p_node->ads_pic_attribute_data.file_number;
	p_pic->pic_type = p_node->ads_pic_attribute_data.p_format;
	p_pic->pos_type = p_node->ads_pic_attribute_data.type;
	p_pic->pic_data= mtos_malloc(p_pic->data_len);
	memcpy(p_pic->pic_data, p_node->p_ads_pic_data, p_pic->data_len);
	dvbad_ads_unlock();
	DEBUG(ADS, INFO, "pic_id:%d, pic_type:%d,len:%d, type:%d\n",
					p_pic->pic_id, p_pic->pic_type, p_pic->data_len,
					p_pic->pos_type);
	return SUCCESS;
}

static RET_CODE ads_save_pic_info(ads_data_node *p_ads_pic_data_node, ads_pic_infomation data)
{
	ads_data_node *p = p_ads_pic_data_node;

	while (data.pic_id != p->ads_pic_attribute_data.file_number)
	{
		if (NULL== p->next)
		{
			DEBUG(ADS,ERR, "ERROR!(NULL== *p->dvbad_pic_attribute_node)\n");
			return FALSE;
		}
		p = p->next;
	}
	memcpy(&p->p_pic_info, &data, sizeof(ads_pic_infomation));
	return TRUE;
}

static RET_CODE ads_pic_info_parser(u8 *data, ads_data_node *ads_pic_data_node)
{
#define ONE_PIC_INFO_LENGTH		(19)
	u16 pic_num = (((u16)data[0]<<8)|(u16)data[1])&0xFFFF;
	u16 i;
	ads_pic_infomation pic_info;

	for(i = 0; i<pic_num;i++)
	{
		memset(&pic_info, 0, sizeof(ads_pic_infomation));
		pic_info.pic_num = (((u16)data[0]<<8)|(u16)data[1])&0xFFFF;
		pic_info.pic_id = (((u16)data[2+(ONE_PIC_INFO_LENGTH*i)]<<8|
																				(u16)data[3+(ONE_PIC_INFO_LENGTH*i)]))&0xFFFF;
		pic_info.pic_original_size = (((u32)data[4+(ONE_PIC_INFO_LENGTH*i)]<<24)|
																				((u32)data[5+(ONE_PIC_INFO_LENGTH*i)]<<16)|
																				((u32)data[6+(ONE_PIC_INFO_LENGTH*i)]<<8)|
																				((u32)data[7+(ONE_PIC_INFO_LENGTH*i)]))&0xFFFFFFFF;
		pic_info.pic_segment_length = (((u16)data[12+(ONE_PIC_INFO_LENGTH*i)]<<8)|
																				((u16)data[13]+(ONE_PIC_INFO_LENGTH*i)))&0xFFFF;
		pic_info.pic_segment_num = (((u16)data[14+(ONE_PIC_INFO_LENGTH*i)]<<8)|
																				((u16)data[15+(ONE_PIC_INFO_LENGTH*i)]))&0xFFFF;
		pic_info.pic_compressed_flag = data[16+(ONE_PIC_INFO_LENGTH*i)]>>7&0x01;
		pic_info.pic_compressed_size= (((u32)data[17+(ONE_PIC_INFO_LENGTH*i)]<<24)|
																				((u32)data[18+(ONE_PIC_INFO_LENGTH*i)]<<16)|
																				((u32)data[19+(ONE_PIC_INFO_LENGTH*i)]<<8)|
																				((u32)data[20+(ONE_PIC_INFO_LENGTH*i)]));
		if (FALSE==ads_save_pic_info(ads_pic_data_node, pic_info))
		{
			DEBUG(ADS, ERR, "ads_save_pic_info return FALSE\n");
			return FALSE;
		}
	}		
	return TRUE;
}

static void dvbad_get_parser_down_ctrl_info(u8* p_data, u32 data_len, down_ctrl_info *ads_down_ctrl_info)
{
#define XML_TABLE_ID_START	8	
	MT_ASSERT((NULL!=p_data)&&(NULL!=ads_down_ctrl_info));
	ads_down_ctrl_info->ads_xml_info.index_xml_table_id_start= 
							(((u16)p_data[XML_TABLE_ID_START]<<8)|(u16)p_data[XML_TABLE_ID_START+1])&0xFFFF;
	ads_down_ctrl_info->ads_xml_info.index_xml_table_id_end= 
							(((u16)p_data[XML_TABLE_ID_START+2]<<8)|(u16)p_data[XML_TABLE_ID_START+3])&0xFFFF;
	ads_down_ctrl_info->ads_xml_info.index_xml_size= 
							(((u32)p_data[XML_TABLE_ID_START+4]<<24)|((u32)p_data[XML_TABLE_ID_START+5]<<16)|
							((u32)p_data[XML_TABLE_ID_START+6]<<8)|(u32)p_data[XML_TABLE_ID_START+7])&0xFFFFFFFF;
	ads_down_ctrl_info->ads_xml_info.index_xml_size++;
	ads_down_ctrl_info->ads_pic_info.pic_information_table_id_start= 
							(((u16)p_data[XML_TABLE_ID_START+8]<<8)|(u16)p_data[XML_TABLE_ID_START+9])&0xFFFF;
	ads_down_ctrl_info->ads_pic_info.pic_information_table_id_end= 
							(((u16)p_data[XML_TABLE_ID_START+10]<<8)|(u16)p_data[XML_TABLE_ID_START+11])&0xFFFF;
	ads_down_ctrl_info->ads_pic_info.pic_information_size= 
							(((u32)p_data[XML_TABLE_ID_START+12]<<24)|((u32)p_data[XML_TABLE_ID_START+13]<<16)|
							((u32)p_data[XML_TABLE_ID_START+14]<<8)|(u32)p_data[XML_TABLE_ID_START+15])&0xFFFFFFFF;

}

static BOOL ads_get_head_data(u8 *p_data, u8 *p_xml_data, u32 *xml_len, u8 *p_info_data, u32 *info_len)
{
	u32 data_len;
	u8 data_type;
	if(NULL==p_data)
	{
		DEBUG(ADS, ERR, "(NULL==p_data).error");
		return FALSE;
	}

	data_type = p_data[8];
	if(DATA_TYPE_XML == data_type)
	{
		data_len = *xml_len;
		*xml_len = ((u16)p_data[9]<<8|(u16)p_data[10])&0x0FFF;
		memcpy(p_xml_data+data_len, p_data+11,  *xml_len);
		*xml_len += data_len;
	}
	else if(DATA_TYPE_PIC_INFO == data_type)
	{
		data_len = *info_len;
		*info_len = ((u16)p_data[9]<<8|(u16)p_data[10])&0x0FFF;
		memcpy(p_info_data+data_len, p_data+11,  *info_len);
		*info_len += data_len;
	}
	else 
	{
		DEBUG(ADS, ERR, "wrong data_type!!\n");
		return FALSE;
	}
	OS_PRINTF("\n");
	DEBUG(ADS, INFO, "ads_get_head_data return true DATA_TYPE_PIC_INFO %d\n", data_type);
	return TRUE;
}

static RET_CODE ads_save_pic_data(ads_data_node *p_ads_pic_data_node, u8 *p_data, u32 pic_data_len, u16 pid)
{
	ads_data_node *p = p_ads_pic_data_node;

	while (pid != p->ads_pic_attribute_data.file_number)
	{
		if (NULL== p->next)
		{
			DEBUG(ADS,ERR, "ERROR!(NULL== *p->dvbad_pic_attribute_node)\n");
			return ERR_FAILURE;
		}
		p = p->next;
	}

	p->p_ads_pic_data = mtos_malloc(pic_data_len);
	memcpy(p->p_ads_pic_data, p_data, pic_data_len);
	DEBUG(ADS, INFO, "save pic data end.pic_data_len:%d, id:%d\n",pic_data_len, p->ads_pic_attribute_data.file_number);
	return SUCCESS;
}


static RET_CODE ads_get_next_pid(ads_data_node *p_node, u16 pid, u16 *next_pid)
{
	ads_data_node *p = p_node;

	while (pid != p->ads_pic_attribute_data.file_number)
	{
		if (NULL== p->next)
		{
			DEBUG(ADS,ERR, "ERROR!(NULL== *p->dvbad_pic_attribute_node)\n");
			return ERR_FAILURE;
		}
		p = p->next;
	}
	if(NULL!=p->next)
	{
		*next_pid = p->next->ads_pic_attribute_data.file_number;
		DEBUG(ADS, INFO, "next pid is %d\n", *next_pid);
		return SUCCESS;
	}

	return ERR_FAILURE;
}

static RET_CODE ads_get_pic_data(u8 *p_data, u8 *p_pic_data, u32 *pic_data_len)
{
	u32 data_len = *pic_data_len;

	if(NULL==p_data)
	{
		DEBUG(ADS, ERR, "NULL==p_data.ERROR\n");
		return FALSE;
	}

	*pic_data_len = ((u16)(p_data[9]&0x0F)<<8|(u16)p_data[10]);
	memcpy(p_pic_data+data_len, p_data+11,  *pic_data_len);
	*pic_data_len += data_len;

	DEBUG(ADS, INFO, "get pic data end\n");
	return TRUE;
}

static filter_buf_node  *create_filter_buf_list(u8 *p_data, u32 data_len)
{
	filter_buf_node *p_node = NULL;
	
	if((data_len<=0)||(NULL==p_data))
	{
		DEBUG(ADS, ERR, "Error p_data:%d, data_len:%d!!!\n", p_data, data_len);
		return NULL;
	}
    p_node = mtos_malloc(sizeof(filter_buf_node));
	MT_ASSERT(NULL!=p_node);
	
	p_node->p_data = mtos_malloc(data_len);
	if(NULL==p_node->p_data)
	{
		DEBUG(ADS, ERR, "malloc failure!!!\n");
		return NULL;
	}
	memcpy(p_node->p_data, p_data, data_len);
	p_node->data_len = data_len;
    p_node->next = NULL;  
    return p_node;
}

static void delete_head_node(filter_buf_node** pNode)  
{
	MT_ASSERT((pNode != NULL)&&(NULL!=* pNode));
	mtos_free((*pNode)->p_data);
	if ((*pNode)->next)
		delete_head_node(&(*pNode)->next);   
	mtos_free((*pNode)); 
	*pNode = NULL;
}

static RET_CODE add_filter_data(filter_buf_node** p_node, u8 *p_data, u32 data_len)
{  
    filter_buf_node* p_data_node = NULL;
	filter_buf_node *p_add_node;
    if((NULL==p_node)||(NULL==*p_node))
    {
       DEBUG(ADS, ERR, "null!!\n");
	   	return ERR_FAILURE;
    }
          
    p_data_node = create_filter_buf_list(p_data, data_len);
    MT_ASSERT(NULL != p_data_node);  
	p_add_node = *p_node;
	while(NULL != p_add_node->next)
		p_add_node = p_add_node->next;
	p_add_node->next = p_data_node;
	return SUCCESS;
}  

static RET_CODE add_data_to_filter_list(filter_buf_node** p_node, u8 *p_data, u32 data_len)
{
	if(NULL==*p_node)
	{
		DEBUG(ADS, INFO, "add new p_data\n");
		*p_node = create_filter_buf_list(p_data, data_len);
		MT_ASSERT(NULL!=*p_node);
		return SUCCESS;
	}
	DEBUG(ADS, INFO, "add p_data\n");
	return add_filter_data(p_node, p_data, data_len);
}

static filter_buf_node *get_next_head_node(filter_buf_node *p_list, u8 old_section_number,
												u8 old_data_type, u16 old_extension_id)
{
	u8 section_number;
	u8 data_type;
	u16 table_extension_id;
	filter_buf_node *p_node;

	p_node = p_list;
	while(NULL!=p_node)
	{
		table_extension_id = (((u16)p_node->p_data[3]<<8)|((u16)p_node->p_data[4]));
		section_number = p_node->p_data[6];
		data_type = p_node->p_data[8];
		if((table_extension_id==old_extension_id)&&(section_number==old_section_number)&&
										(data_type==old_data_type))
		{
			DEBUG(ADS, INFO, "data_type:%d,table_extension_id:%d,section_number:%d", data_type, table_extension_id, section_number);
			return p_node;
		}
		p_node = p_node->next;
	}

	return NULL;
}

static RET_CODE parser_head_data(filter_buf_node *p_list)
{
	dvbad_priv_t *ad = &s_dvbad;
	filter_buf_node *p_node;
	u8 section_number;
	u8 last_section_number;
	u8 data_type;
	u16 table_extension_id;

	if(NULL==p_list)
	{
		DEBUG(ADS, ERR, "null plist\n");
		return ERR_FAILURE;
	}
	section_number = 0;
	data_type = DATA_TYPE_XML;
	table_extension_id = ad->down_ctl.ads_xml_info.index_xml_table_id_start;
	if (ad->xml == NULL)
	{
		ad->xml = mtos_malloc(ad->down_ctl.ads_xml_info.index_xml_size);
		memset((void*)ad->xml, 0, ad->down_ctl.ads_xml_info.index_xml_size);
		ad->xml_pos = 0;
	}
	if (ad->pic_info == NULL)
	{
		ad->pic_info = mtos_malloc(ad->down_ctl.ads_pic_info.pic_information_size);
		memset((void*)ad->pic_info, 0, ad->down_ctl.ads_pic_info.pic_information_size);
		ad->pic_info_pos = 0;
	}
	if (ad->xml == NULL || ad->pic_info == NULL)
	{
		DEBUG(ADS,ERR,"malloc failure!!!\n");
		return ERR_FAILURE;
	}
	while(1)
	{
		p_node = get_next_head_node(p_list, section_number, data_type, table_extension_id);
		if(NULL==p_node)
		{
			DEBUG(ADS, ERR, "None of next head node\n");
			return ERR_FAILURE;
		}
		if(FALSE==ads_get_head_data(p_node->p_data,ad->xml, &ad->xml_pos, ad->pic_info, &ad->pic_info_pos))
		{
			mtos_free(ad->xml);
			mtos_free(ad->pic_info);
			ad->xml = NULL;
			ad->pic_info = NULL;
			DEBUG(ADS, ERR, "ads_get_head_data return FALSE!\n");
			break;
		}
		last_section_number = p_node->p_data[7];
		if(section_number<last_section_number)
			section_number++;
		else if((table_extension_id<ad->down_ctl.ads_xml_info.index_xml_table_id_end)||
			((table_extension_id>ad->down_ctl.ads_xml_info.index_xml_table_id_end)&&
			(table_extension_id<ad->down_ctl.ads_pic_info.pic_information_table_id_end)))
		{
			table_extension_id++;
			section_number= 0;
		}
		else if((table_extension_id==ad->down_ctl.ads_xml_info.index_xml_table_id_end)||
						(table_extension_id==ad->down_ctl.ads_pic_info.pic_information_table_id_end))
		{
			if(DATA_TYPE_XML == data_type)
			{
				
				DEBUG(ADS, INFO, "sizeof(dvbad_ads_pic_data) %d\n", sizeof(dvbad_ads_pic_data));
				if(NULL==ad->p_ad_data)
				{
					mtos_free(ad->xml);
					mtos_free(ad->pic_info);
					ad->xml= NULL;
					ad->pic_info = NULL;
					DEBUG(ADS, ERR, "mtos_malloc failure\n");
					return DVBAD_FUN_ERR_PARA;
				}
				ad->xml_pos++;
				if(SUCCESS!=ads_xml_parse(ad->xml, ad->xml_pos, ad->p_ad_data))
				{
					DEBUG(ADS, ERR, "ads_xml_parsefailure\n");
					return DVBAD_FUN_ERR_PARA;
				}
				DEBUG(ADS, INFO, "id:%d,type:%d,x:%d,y:%d,w:%d,h:%d\n", ad->p_ad_data->dvbad_pic_attribute_node.ads_pic_attribute_data.file_number,
										ad->p_ad_data->dvbad_pic_attribute_node.ads_pic_attribute_data.type,
										ad->p_ad_data->dvbad_pic_attribute_node.ads_pic_attribute_data.x,
										ad->p_ad_data->dvbad_pic_attribute_node.ads_pic_attribute_data.y,
										ad->p_ad_data->dvbad_pic_attribute_node.ads_pic_attribute_data.w,
										ad->p_ad_data->dvbad_pic_attribute_node.ads_pic_attribute_data.h);
				if(ad->down_ctl.ads_xml_info.index_xml_table_id_end==ad->down_ctl.ads_pic_info.pic_information_table_id_start)
					section_number++;
				else 
					section_number = 0;
				table_extension_id = ad->down_ctl.ads_pic_info.pic_information_table_id_start;
				data_type = DATA_TYPE_PIC_INFO;
			}
			else if(DATA_TYPE_PIC_INFO == data_type)
			{
				if(FALSE==ads_pic_info_parser(ad->pic_info, &(ad->p_ad_data->dvbad_pic_attribute_node)))
				{
					DEBUG(ADS, ERR, "ads_pic_info_parser return FALSE\n");
					return DVBAD_FUN_ERR_PARA;
				}
				mtos_free(ad->xml);
				mtos_free(ad->pic_info);
				ad->xml = NULL;
				ad->pic_info = NULL;
				DEBUG(ADS, INFO, "parser head data success\n");
				return SUCCESS;
			}			
		}
	}
	
	return ERR_FAILURE;
}

static filter_buf_node* get_data_node_by_id(filter_buf_node *p_list, u8 section_number, u16 pic_id)
{
	u8 data_section_number;
	u16 data_pic_id;
	filter_buf_node *p_node;

	p_node = p_list;
	while(NULL!=p_node)
	{
		data_pic_id = (((u16)p_node->p_data[3]<<8)|((u16)p_node->p_data[4]));
		data_section_number = p_node->p_data[6];
		if((data_pic_id==pic_id)&&(data_section_number==section_number))
		{
			DEBUG(ADS, INFO, "pic_id:%d,section_number:%d\n", pic_id, section_number);
			return p_node;
		}
		p_node = p_node->next;
	}
	DEBUG(ADS, ERR, "Have none pic_id node\n");
	return NULL;
}

static u32 get_one_pic_size(u16 pic_pid, ads_data_node *p_node)
{
	ads_data_node *p = p_node;

	while (pic_pid != p->ads_pic_attribute_data.file_number)
	{
		if (NULL== p->next)
		{
			DEBUG(ADS,ERR, "ERROR!(NULL== *p->dvbad_pic_attribute_node)\n");
			return FALSE;
		}
		p = p->next;
	}

	DEBUG(ADS, INFO, "get pic size end:%d\n", p->p_pic_info.pic_original_size);
	return p->p_pic_info.pic_original_size;
}

static RET_CODE save_filter_pic_data(filter_buf_node *p_list, ads_data_node *p_data_node, u16 pic_id)
{
	dvbad_priv_t *ad = &s_dvbad;
	filter_buf_node *p_buf_node;
	u8 section_number;
	u32 pic_data_size;
	RET_CODE ret;
	u8 last_section_number;

	section_number = 0;
	if (NULL==ad->pic_data)
	{
		DEBUG(ADS, INFO, "malloc pic data\n");
		pic_data_size = get_one_pic_size(pic_id, &(ad->p_ad_data->dvbad_pic_attribute_node));
		ad->pic_data = mtos_malloc(pic_data_size);
		memset((void*)ad->pic_data, 0, pic_data_size);
		ad->pic_data_pos = 0;
	}
	
	if (NULL==ad->pic_data)
	{
		DEBUG(ADS, ERR, "malloc failure!\n");
		mtos_free(ad->pic_data);
		ad->pic_data = NULL;
		return ERR_FAILURE;
	}
	
	do
	{
		p_buf_node = get_data_node_by_id(p_list, section_number, pic_id);
		if(NULL==p_buf_node)
		{
			DEBUG(ADS, ERR, "no pic_id node\n");
			mtos_free(ad->pic_data);
			ad->pic_data = NULL;
			return ERR_FAILURE;
		}
		ads_get_pic_data(p_buf_node->p_data, ad->pic_data, &ad->pic_data_pos);
		last_section_number = p_buf_node->p_data[7];
		if(section_number==last_section_number)
		{
			ret = ads_save_pic_data(p_data_node, ad->pic_data, ad->pic_data_pos, pic_id);
			mtos_free(ad->pic_data);
			ad->pic_data = NULL;
			return ret;
		}
		section_number++;
	}while(section_number<=last_section_number);

	return ERR_FAILURE;
}

static RET_CODE get_free_filter_id(u16 *filter_id)
{
	u16 i;
	dvbad_priv_t *ad = &s_dvbad;

	for(i=0;i<DVB_ADS_FILTER_MAX_NUM;i++)
	{
		if(DVB_ADS_FREE==ad->filter[i].filter_status)
		{
			*filter_id = i;
			return SUCCESS;
		}
	}
	DEBUG(ADS, INFO,"have no free filter\n");
	return ERR_BUSY;
}

static BOOL dvbad_set_filter_data_table(u16 filter_id, u8 table_id, u16 extension_id, u8 section_number)
{
	ST_ADS_DVBAD_FILTER_INFO filter_info;
	ST_AD_DVBAD_FUNC_TYPE ret = DVBAD_FUN_ERR_PARA;

	DEBUG(ADS, INFO, "table_id:%d,extension_id:%d,section_number:%d\n",table_id,extension_id,section_number);
	filter_info.filter_id = filter_id;
	filter_info.usChannelPid = DATA_PID;
	filter_info.ucFilterLen = 12;
	memset((void*)filter_info.aucFilter, 0, sizeof(filter_info.aucFilter));
	memset((void*)filter_info.aucMask, 0, sizeof(filter_info.aucMask));
	filter_info.aucFilter[0] = table_id;
	filter_info.aucFilter[3] = (u8)((extension_id&0xFF00)>>8);
	filter_info.aucFilter[4] = (u8)(extension_id&0x00FF); 
	filter_info.aucFilter[6] = section_number;
	filter_info.aucMask[0] = 0xFF;
	filter_info.aucMask[3] = 0xFF;
	filter_info.aucMask[4] = 0xFF;
	filter_info.aucMask[6] = 0xFF;
	filter_info.ucWaitSeconds = 10;
	filter_info.pfDataNotifyFunction = NULL;

	ret = DVBAD_SetFilter(&filter_info);
	if(ret != DVBAD_FUN_OK)
	{
		ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
		MT_ASSERT(0);
	}
	DEBUG(ADS, INFO,"dvbad_set_filter_data_table return true\n");
	return TRUE;
}

static ST_AD_DVBAD_FUNC_TYPE dvbad_set_filter_for_adhead(u16 filter_id, u16 table_id, u8 data_type, u8 section_number )
{
	ST_AD_DVBAD_FUNC_TYPE ret;
	ST_ADS_DVBAD_FILTER_INFO filter_info;
	memset(&filter_info,0,sizeof(filter_info));
	
	filter_info.filter_id = filter_id;
	filter_info.usChannelPid = DATA_PID;
	filter_info.ucFilterLen = 12;
	memset((void*)filter_info.aucFilter, 0, sizeof(filter_info.aucFilter));
	memset((void*)filter_info.aucMask, 0, sizeof(filter_info.aucMask));
	filter_info.aucFilter[0] = 0xFE;
	filter_info.aucFilter[3] = (u8)((table_id&0xFF00)>>8);
	filter_info.aucFilter[4] = (u8)(table_id&0x00FF); 
	filter_info.aucFilter[6] = section_number;
	filter_info.aucFilter[8] = data_type;
	filter_info.aucMask[0] = 0xFF;
	filter_info.aucMask[3] = 0xFF;
	filter_info.aucMask[4] = 0xFF;
	filter_info.aucMask[6] = 0xFF;
	filter_info.aucMask[8] = 0xFF;
	filter_info.ucWaitSeconds = 10;
	filter_info.pfDataNotifyFunction = NULL;
	ret = DVBAD_SetFilter(&filter_info);
	DEBUG(ADS, INFO,"end.ret:%d, section_number:%d, data_type:%d, table_id:%d\n", ret, section_number, data_type, table_id);
	return ret;
}

static BOOL dvbad_set_filter_ctrl_info(u16 filter_id)
{
	ST_AD_DVBAD_FUNC_TYPE ret = DVBAD_FUN_ERR_PARA;
	ST_ADS_DVBAD_FILTER_INFO filter_info;
	memset(&filter_info,0,sizeof(filter_info));

	filter_info.filter_id = filter_id;
	filter_info.usChannelPid = DATA_PID;
	filter_info.ucFilterLen = 12;
	filter_info.aucFilter[0] = 0xFE;
	filter_info.aucFilter[3] = 0;
	filter_info.aucFilter[4] = 0; 
	filter_info.aucFilter[6] = 0;
	filter_info.aucFilter[7] = 0;
	filter_info.aucMask[0] = 0xFF;
	filter_info.aucMask[3] = 0xFF;
	filter_info.aucMask[4] = 0xFF;
	filter_info.aucMask[6] = 0xFF;
	filter_info.aucMask[7] = 0xFF;

	filter_info.ucWaitSeconds = 10;
	filter_info.pfDataNotifyFunction = NULL;

	ret = DVBAD_SetFilter(&filter_info);
	if(ret != DVBAD_FUN_OK)
	{
		ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
		MT_ASSERT(0);
	}
	DEBUG(ADS, INFO,"dvbad_set_filter_CTRL_INFO return true\n");
	return TRUE;
}

void dvbad_ads_filter_free(dvbad_ads_filter_struct_t *ds_ads_filter)
{
	u32 ret = 0;

	ret = dmx_chan_stop(ds_ads_filter->p_dev, ds_ads_filter->channel_filter_id);
	CHECK_RET(ADS,ret);

	ret = dmx_chan_close(ds_ads_filter->p_dev, ds_ads_filter->channel_filter_id);
	CHECK_RET(ADS,ret);
}


static ST_AD_DVBAD_FUNC_TYPE reset_head_data_filter(u8 section_number, u8 last_section_number, 
										u16 table_id, u8 data_type)
{
	u16 i;
	u16 filter_id;
	static s32 remaining_section;
	static set_filter_status filter_set;
	static u32 old_table_id;
	ST_AD_DVBAD_FUNC_TYPE ret;
	dvbad_priv_t *ad = &s_dvbad;

	DEBUG(ADS, INFO, "start get_filter_data, data_type %d\n", data_type);
	DEBUG(ADS, INFO, "index_xml_table_id_start %d\n", (ad->down_ctl.ads_xml_info.index_xml_table_id_start));
	DEBUG(ADS, INFO, "index_xml_table_id_end %d\n", (ad->down_ctl.ads_xml_info.index_xml_table_id_end));
	DEBUG(ADS, INFO, "pic_information_table_id_start %d\n", (ad->down_ctl.ads_pic_info.pic_information_table_id_start));
	DEBUG(ADS, INFO, "pic_information_table_id_end %d\n", (ad->down_ctl.ads_pic_info.pic_information_table_id_end));
	DEBUG(ADS, INFO, "AD_version:%d\n", ad->p_ad_data->ad_head_data.AD_version);
	DEBUG(ADS, INFO, "section_number:%d, table_id:%d, old_table_id:%d, set_table_id:%d\n", section_number, table_id, old_table_id, filter_set.set_table_id);
	DEBUG(ADS, INFO, "remaining_section:%d\n", remaining_section);
	DEBUG(ADS, INFO, "set_section_number:%d, set_data_type:%d, new_table_flag:%d, set_last_section_number:%d\n", 
				filter_set.set_section_number, filter_set.set_data_type, filter_set.new_table_flag, filter_set.set_last_section_number);

	if((0==section_number)&&(table_id==ad->down_ctl.ads_xml_info.index_xml_table_id_start))
	{
		filter_set.set_section_number = 0;
		filter_set.set_data_type = data_type;
		filter_set.new_table_flag = TRUE;
		filter_set.set_last_section_number = last_section_number;
		filter_set.set_table_id = table_id;
		remaining_section = last_section_number-section_number+1;
		old_table_id= table_id;
		DEBUG(ADS, INFO, "reset remaining_section:%d\n", remaining_section);
	}
	if(old_table_id<table_id)
	{
		DEBUG(ADS, INFO, "old_table_id:%d, table_id:%d\n", old_table_id, table_id);
		remaining_section += last_section_number-section_number+1;
		old_table_id = table_id;
		filter_set.set_last_section_number = last_section_number;
		filter_set.new_table_flag = TRUE;
	}
	remaining_section--;
	DEBUG(ADS, INFO, "remaining_section:%d\n", remaining_section);
	MT_ASSERT(remaining_section>=0);
	if((0==remaining_section)&&(old_table_id==ad->down_ctl.ads_pic_info.pic_information_table_id_end))
	{
		DEBUG(ADS, INFO, "head filtration finsh\n");
		return DVBAD_FUN_FILTER_COMPELET;
	}
	if(FALSE==filter_set.new_table_flag)
	{
		DEBUG(ADS, INFO, "new_table_new_table_flag==FALSE\n");
		return DVBAD_FUN_OK;
	}
	for(i=0;i<DVB_ADS_FILTER_MAX_NUM;i++)
	{
		if(DVB_ADS_FREE==ad->filter[i].filter_status)
		{
			DEBUG(ADS, INFO, "free filter:%d\n", i);
		}
	}
	for(i = 0; i<DVB_ADS_FILTER_MAX_NUM; i++)
	{
		if(SUCCESS!=get_free_filter_id(&filter_id))
			break;
		if(filter_set.set_section_number<filter_set.set_last_section_number)
			filter_set.set_section_number++;
		else if(filter_set.set_section_number==filter_set.set_last_section_number)
		{
			if((filter_set.set_table_id==ad->down_ctl.ads_xml_info.index_xml_table_id_end)&&(ad->down_ctl.ads_xml_info.index_xml_table_id_end==ad->down_ctl.ads_pic_info.pic_information_table_id_start))
			{
				filter_set.set_section_number++;
				filter_set.set_last_section_number = 0;
				filter_set.new_table_flag = FALSE;
				filter_set.set_data_type = DATA_TYPE_PIC_INFO;
			}
			else if((filter_set.set_table_id>=ad->down_ctl.ads_xml_info.index_xml_table_id_start)&&
							(filter_set.set_table_id<ad->down_ctl.ads_xml_info.index_xml_table_id_end))
			{
				filter_set.set_table_id++;
				filter_set.set_section_number = 0;
				filter_set.new_table_flag = FALSE;
				filter_set.set_last_section_number = 0;
			}
			else if((filter_set.set_table_id>=ad->down_ctl.ads_xml_info.index_xml_table_id_end)&&
						(filter_set.set_table_id<ad->down_ctl.ads_pic_info.pic_information_table_id_end))
			{
				filter_set.set_table_id++;
				filter_set.set_section_number = 0;
				filter_set.new_table_flag = FALSE;
				filter_set.set_last_section_number = 0;
				filter_set.set_data_type = DATA_TYPE_PIC_INFO;
			}
			else
			{
				DEBUG(ADS, INFO, "set filter end.Wait to get data\n");
				return DVBAD_FUN_OK;
			}
		}
		ret = dvbad_set_filter_for_adhead(filter_id, filter_set.set_table_id, filter_set.set_data_type, filter_set.set_section_number);
		if(DVBAD_FUN_OK!=ret)
		{
			DEBUG(ADS, ERR, "set_filter_for_adhead return ERR\n");
			return ret;
		}
		if(FALSE==filter_set.new_table_flag)
		{
			DEBUG(ADS, INFO, "New table_id\n");
			return DVBAD_FUN_OK;
		}
	}

	DEBUG(ADS, INFO, "end.None of filter is free\n");
	return DVBAD_FUN_OK;
}

static ST_AD_DVBAD_FUNC_TYPE reset_data_filter(ads_data_node *p_node, u8 *p_data, filter_buf_node *p_list)
{
	static u8 set_section_number = 0;
	static s32 sum_section = 0;
	u16 filter_id;
	u16 i;
	u16 pic_pid;
	u16 next_pid;
	u8 section_number;
	u8 last_section_number;

	if((NULL==p_node)||(NULL==p_data))
	{
		DEBUG(ADS, INFO, "NULL pointer\n");
		return DVBAD_DATA_FALSE;
	}


	section_number = p_data[6];
	last_section_number = p_data[7];
	pic_pid = (((u16)p_data[3])<<8)|((u16)p_data[4]);
	DEBUG(ADS, INFO, "reset data filter. section_number:%d,last_section_number;%d,pic_pid:%d\n", section_number, last_section_number, pic_pid);
	if(0==section_number)
	{
		set_section_number = 0;
		sum_section = 0;
	}
	sum_section++;
	if((last_section_number+1)==sum_section)
	{
		if(SUCCESS!=save_filter_pic_data(p_list, p_node, pic_pid))
		{
			DEBUG(ADS, ERR, "save pic data failure\n");
			return DVBAD_DATA_FALSE;
		}
		if(SUCCESS!=ads_get_next_pid(p_node, pic_pid, &next_pid))
			return DVBAD_DATA_FINISH;
		if(SUCCESS!=get_free_filter_id(&filter_id))
			return DVBAD_DATA_TRUE;
		dvbad_set_filter_data_table(filter_id, AD_DATA_TABLE_ID, next_pid, 0);
		return DVBAD_DATA_TRUE;
	}
	
	for(i = 0; i<DVB_ADS_FILTER_MAX_NUM; i++)
	{
		if(SUCCESS!=get_free_filter_id(&filter_id))
			return DVBAD_DATA_TRUE;
		
		if(set_section_number<last_section_number)
		{
			set_section_number++;
			dvbad_set_filter_data_table(filter_id, AD_DATA_TABLE_ID, pic_pid, set_section_number);
		}
	}
	
	return DVBAD_DATA_TRUE;
}

static RET_CODE parser_bat_data(u8 *p_data, u32 data_len)
{
	dvbad_priv_t *ad = &s_dvbad;
	bat_data ad_bat_data;
	u16 bouquet_length;
	u8 version_number;
	u32 ad_version;
	u8 rivate_id[16];
	u8 date_Length;
	u8 descriptor_length;
	u8 linkage_type;
	u16 filter_id;
	u16 pid;

	if(NULL==p_data)
	{
		DEBUG(ADS, ERR, " p_data == NULL\n");
		return ERR_FAILURE;
	}
	version_number = p_data[5]&0x3E;
	bouquet_length = ((u16)p_data[8]&0x0F<<8)|((u16)p_data[9]);
	ad_bat_data.valid_flag= p_data[12]&0x01;
	ad_version = ((u16)p_data[13]<<8)|((u16)p_data[14]);
	date_Length = p_data[11];
	memset(rivate_id, 0, sizeof(rivate_id));
	memcpy(rivate_id, p_data+15, date_Length-3);
	descriptor_length = p_data[11+date_Length+2];
	ad_bat_data.stream_id = ((u16)p_data[11+date_Length+3]<<8)&(u16)p_data[11+date_Length+4];
	ad_bat_data.network_id = ((u16)p_data[11+date_Length+5]<<8)&(u16)p_data[11+date_Length+6];
	ad->p_ad_data->ad_bat_data.service_id = ((u16)p_data[11+date_Length+7]<<8)&(u16)p_data[11+date_Length+8];
	linkage_type = p_data[11+date_Length+9];
	if(0xA1==linkage_type)
	{
		ad_bat_data.frequency = ((u32)p_data[11+date_Length+9]<<24)|
								((u32)p_data[11+date_Length+10]<<16)|
								((u32)p_data[11+date_Length+11]<<8)|
								(u32)p_data[11+date_Length+12];
		ad_bat_data.modulation = (u32)p_data[11+date_Length+13];
		ad_bat_data.symbol_rate = ((u32)p_data[11+date_Length+14]<<24)|
								  ((u32)p_data[11+date_Length+15]<<16)|
							      ((u32)p_data[11+date_Length+16]<<8)|
								  ((u32)p_data[11+date_Length+17]&0xF0);
		DEBUG(ADS, ERR, "linkage_type = %d\n",linkage_type);
	}
	DEBUG(ADS, INFO, "update ad.ad_pic_num:%d, p_ad_data->version_number:%d;version_number:%d, ad_head_data.AD_version:%d;AD_version:%d, save_all_pic_flag:%d\n", 
			ad->p_ad_data->ad_head_data.ad_pic_num,
			ad->p_ad_data->ad_head_data.version_number,
			version_number,
			ad->p_ad_data->ad_head_data.AD_version,
			ad_version,
			ad->p_ad_data->ad_head_data.save_all_pic_flag);

	if((ad->p_ad_data->ad_head_data.ad_pic_num<=0)||((ad->p_ad_data->ad_head_data.version_number!=version_number)&&
						(ad->p_ad_data->ad_head_data.AD_version!=ad_version))||(FALSE==ad->p_ad_data->ad_head_data.save_all_pic_flag))
	{
		DEBUG(ADS, INFO, "update ad.ad_pic_num:%d, p_ad_data->version_number:%d;version_number:%d, ad_head_data.AD_version:%d;AD_version:%d, save_all_pic_flag:%d\n", 
					ad->p_ad_data->ad_head_data.ad_pic_num, ad->p_ad_data->ad_head_data.version_number, version_number, ad->p_ad_data->ad_head_data.AD_version, ad_version, ad->p_ad_data->ad_head_data.save_all_pic_flag);
		ad->p_ad_data->ad_head_data.version_number = version_number;
		ad->p_ad_data->ad_head_data.AD_version = ad_version;
		if(SUCCESS!=get_free_filter_id(&filter_id))
			return ERR_FAILURE;
		dvbad_set_filter_ctrl_info(filter_id);
	}
	else
	{
		memcpy(&ad->p_ad_data->ad_bat_data, &ad_bat_data, sizeof(bat_data));
		ad->p_ad_data->ad_head_data.version_number = version_number;
		ad->p_ad_data->ad_head_data.AD_version = ad_version;
		if(NULL!=ad->p_ad_data->dvbad_pic_attribute_node.p_ads_pic_data)
		{
			DEBUG(ADS, INFO, "Is the latest version\n");
			exit_monitor(GET_AD_TRUE);
			return SUCCESS;
		}
		if(SUCCESS==read_ad_from_flash(ad->p_ad_data))
		{
			DEBUG(ADS, INFO, "get ad from flash end.");
			if(SUCCESS==get_log_pid(&pid))
				dvb_set_ad_flag(GET_AD_TRUE);
			dvbad_set_filter_bat();
		}
		else
		{
			DEBUG(ADS, INFO, "read flash ad failure\n");
			ad->p_ad_data->ad_head_data.version_number = version_number;
			ad->p_ad_data->ad_head_data.AD_version = ad_version;
			if(SUCCESS!=get_free_filter_id(&filter_id))
				return ERR_FAILURE;
			dvbad_set_filter_ctrl_info(filter_id);
		}
	}
	DEBUG(ADS, INFO, "version_number:%d, valid_flag:%d, ad_version:%d, rivate_id:%s, date_Length:%d\n", 
								version_number, ad->p_ad_data->ad_bat_data.valid_flag, ad_version, rivate_id, date_Length);
	DEBUG(ADS, INFO, "stream_id:%d,network_id:%d, service_id:%d, descriptor_length:%d\n", 
											ad->p_ad_data->ad_bat_data.stream_id, ad->p_ad_data->ad_bat_data.network_id, ad->p_ad_data->ad_bat_data.service_id, descriptor_length);
	return SUCCESS;
}

static void ads_dvbad_data_monitor(void *p_param)
{
	s32 ret = FALSE;
	u8 index = 0;
	u8 *p_data=NULL;
	u32 data_len = 0;
	u16 table_extension_id;
	u16 table_id = 0;
	u8 data_type = 0x01;
	u8 section_number;
	u8 last_section_number;
	u16 pid = 0x1ff;
	u16 filter_id;
	u16 bouquet_id;
	u32 time_out = 0;
	filter_buf_node *p_filter_data_list=NULL;
	dvbad_data_return data_ret;
	ST_AD_DVBAD_FUNC_TYPE reset_ret;

	dvbad_priv_t *ad = &s_dvbad;
	while(1)
	{
		time_out++;
		mtos_task_sleep(20);
		if(time_out>600)
		{
			DEBUG(ADS, ERR, "time out\n");
			exit_monitor(GET_AD_FAILURE);
		}
		dvbad_ads_lock();
		for(index = 0; index < DVB_ADS_FILTER_MAX_NUM; index++)
		{
			if(ad->filter[index].filter_status == DVB_ADS_FREE)
				continue;

			ret=dmx_si_chan_get_data(ad->filter[index].p_dev,
										ad->filter[index].channel_filter_id,
										&p_data,
										&data_len);
			if (ret == 0)
			{				
				time_out = 0;
				bouquet_id = table_extension_id = (((u16)p_data[3]<<8)|((u16)p_data[4]));
				if((BAT_PID == ad->filter[index].channel_pid)&&(0xFF00==bouquet_id))
				{
					dvbad_ads_unlock();
					ad->filter[index].filter_status = DVB_ADS_FREE;
					dvbad_ads_filter_free(&ad->filter[index]);
					DEBUG(ADS,INFO,"bouquet_id = %d \n",bouquet_id);
					parser_bat_data(p_data, data_len);
					dvbad_ads_lock();
				}
				else if((DATA_PID == ad->filter[index].channel_pid)&&(AD_HEAD_TABLE_ID==p_data[0]))
				{
					section_number = p_data[6];
					last_section_number = p_data[7];
					data_type = p_data[8];
					DEBUG(ADS, INFO, "parser,index:%d, p_data[0]:%d, table_extension_id %d, section_number %d, last_section_number %d, data_type %d\n", 
											index,p_data[0], table_extension_id, section_number, last_section_number, data_type);
					switch(table_extension_id)
					{
						case 0:
							dvbad_get_parser_down_ctrl_info(p_data, data_len, &ad->down_ctl);
							dvbad_ads_unlock();
							table_id = ad->down_ctl.ads_xml_info.index_xml_table_id_start;
							data_type = DATA_TYPE_XML;
							ad->filter[index].filter_status = DVB_ADS_FREE;
							dvbad_ads_filter_free(&ad->filter[index]);
							if(SUCCESS!=get_free_filter_id(&filter_id))
								break;
							dvbad_set_filter_for_adhead(filter_id, table_id, DATA_TYPE_XML, 0);
						break;
						default:
							if(SUCCESS!=add_data_to_filter_list(&p_filter_data_list, p_data, data_len))
							{
								DEBUG(ADS, ERR, "add_data_to_filter_list failure\n");
								ad->filter[index].filter_status = DVB_ADS_FREE;
								dvbad_ads_filter_free(&ad->filter[index]);
								dvbad_ads_unlock();
								exit_monitor(GET_AD_FAILURE);
								break;
							}
							dvbad_ads_unlock();
							ad->filter[index].filter_status = DVB_ADS_FREE;
							dvbad_ads_filter_free(&ad->filter[index]);
							reset_ret = reset_head_data_filter(section_number, last_section_number, table_extension_id, data_type);
							if(DVBAD_FUN_FILTER_COMPELET==reset_ret)
							{
								DEBUG(ADS, INFO, "get all head data.start parser head data\n");
								DEBUG(ADS, INFO, "AD_version:%d\n", ad->p_ad_data->ad_head_data.AD_version);
								if(SUCCESS!=parser_head_data(p_filter_data_list))
								{
									DEBUG(ADS, ERR, "parser head data failure\n");
									exit_monitor(GET_AD_FAILURE);
								}
								else
								{
									if(SUCCESS!=get_free_filter_id(&filter_id))
									{
										dvbad_set_filter_bat();
										break;
									}
									dvbad_set_filter_data_table(filter_id, AD_DATA_TABLE_ID, ad->p_ad_data->dvbad_pic_attribute_node.p_pic_info.pic_id, 0);
								}
								delete_head_node(&p_filter_data_list);
							}
							else if(DVBAD_FUN_ERR_PARA==reset_ret)
							{
								if (ad->xml != NULL)
								{
									mtos_free(ad->xml);
									ad->xml = NULL;
								}
								if (ad->pic_info != NULL)
								{
									mtos_free(ad->pic_info);
									ad->pic_info = NULL;
								}
								DEBUG(ADS, ERR, "get_filter_data return false\n");
								delete_head_node(&p_filter_data_list);
								exit_monitor(GET_AD_FAILURE);
							}
						break;
					}
					dvbad_ads_lock();

				}
				else if((DATA_PID == ad->filter[index].channel_pid)&&(AD_DATA_TABLE_ID==p_data[0]))
				{
					if(SUCCESS!=add_data_to_filter_list(&p_filter_data_list, p_data, data_len))
					{
						DEBUG(ADS, ERR, "add_data_to_filter_list failure\n");
						ad->filter[index].filter_status = DVB_ADS_FREE;
						dvbad_ads_filter_free(&ad->filter[index]);
						dvbad_ads_unlock();
						exit_monitor(GET_AD_FAILURE);
						break;
					}
					ad->filter[index].filter_status = DVB_ADS_FREE;
					dvbad_ads_filter_free(&ad->filter[index]);
					dvbad_ads_unlock();
					data_ret=reset_data_filter(&(ad->p_ad_data->dvbad_pic_attribute_node), p_data, p_filter_data_list);
					if(DVBAD_DATA_FINISH==data_ret)
					{
						DEBUG(ADS, INFO, "filte data finish\n");
						delete_head_node(&p_filter_data_list);
						if(SUCCESS!=dvbad_save_to_flash())
							DEBUG(ADS, ERR, "save ad data to flash failure\n");
						if(SUCCESS==get_log_pid(&pid))
							dvb_set_ad_flag(GET_AD_TRUE);
						else 
							DEBUG(ADS, INFO, "Have no logo\n");
						exit_monitor(GET_AD_TRUE);
					}
					else if(DVBAD_DATA_FALSE==data_ret)
					{
						DEBUG(ADS, ERR,"reset_data_filter return err\n");
						delete_head_node(&p_filter_data_list);
						exit_monitor(GET_AD_FAILURE);
					}
    				dvbad_ads_lock();
				}
			}
		}
		dvbad_ads_unlock();
	}
}

ST_AD_DVBAD_FUNC_TYPE DVBAD_SetFilter(ST_ADS_DVBAD_FILTER_INFO *pstFilterInfo)
{
	BOOL ret = FALSE;
	dmx_device_t *p_dev = NULL;
	dmx_slot_setting_t slot = {0,};
	dmx_filter_setting_t  filter_param;
	u32 index;
	dvbad_priv_t *ad = &s_dvbad;
	MT_ASSERT(pstFilterInfo != NULL);
	MT_ASSERT(pstFilterInfo->ucFilterLen < 13);

	DEBUG(ADS, INFO, "filter_id:%d\n", pstFilterInfo->filter_id);
	ADS_DRV_PRINTF("usChannelPid           :0x%x \n",pstFilterInfo->usChannelPid);
	ADS_DRV_PRINTF("ucFilterLen               :%d \n",pstFilterInfo->ucFilterLen);

	index = pstFilterInfo->filter_id;
	dvbad_ads_lock();
	if(ad->filter[index].filter_status == DVB_ADS_USED)
	{
		dvbad_ads_filter_free(&ad->filter[index]);
		ad->filter[index].filter_status = DVB_ADS_FREE;
	}
	//set filter
	p_dev = (dmx_device_t *)dev_find_identifier(NULL
								, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	MT_ASSERT(NULL != p_dev->p_base);
	MT_ASSERT(p_dev != NULL);

	memset(&slot, 0, sizeof(dmx_slot_setting_t));
	slot.in = DMX_INPUT_EXTERN0;
	slot.pid   =pstFilterInfo->usChannelPid;
	slot.type = DMX_CH_TYPE_SECTION;
	ret = dmx_si_chan_open(p_dev, &slot, &ad->filter[index].channel_filter_id);
	CHECK_RET(ADS,ret);

	ret = dmx_si_chan_set_buffer(p_dev,ad->filter[index].channel_filter_id,
													ad->filter_buf[index],
													DVBAD_ADS_DMX_BUF_SIZE);
	CHECK_RET(ADS,ret);

	memset(&filter_param,0,sizeof(filter_param));
#ifdef WIN32
	memcpy(filter_param.value,pstFilterInfo->aucFilter,DMX_SECTION_FILTER_SIZE);
	memcpy(filter_param.mask,pstFilterInfo->aucMask,DMX_SECTION_FILTER_SIZE);
#else
	filter_param.value[0] = pstFilterInfo->aucFilter[0];
	filter_param.value[1] = pstFilterInfo->aucFilter[3];
	filter_param.value[2] = pstFilterInfo->aucFilter[4];
	filter_param.value[3] = pstFilterInfo->aucFilter[5];
	filter_param.value[4] = pstFilterInfo->aucFilter[6];
	filter_param.value[5] = pstFilterInfo->aucFilter[7];
	filter_param.value[6] = pstFilterInfo->aucFilter[8];
	filter_param.value[7] = pstFilterInfo->aucFilter[9];
	filter_param.value[8] = pstFilterInfo->aucFilter[10];
	filter_param.value[9] = pstFilterInfo->aucFilter[11];
	filter_param.value[10] = pstFilterInfo->aucFilter[12];
	filter_param.value[11] = pstFilterInfo->aucFilter[13];

	filter_param.mask[0] = pstFilterInfo->aucMask[0];
	filter_param.mask[1] = pstFilterInfo->aucMask[3];
	filter_param.mask[2] = pstFilterInfo->aucMask[4];
	filter_param.mask[3] = pstFilterInfo->aucMask[5];
	filter_param.mask[4] = pstFilterInfo->aucMask[6];
	filter_param.mask[5] = pstFilterInfo->aucMask[7];
	filter_param.mask[6] = pstFilterInfo->aucMask[8];
	filter_param.mask[7] = pstFilterInfo->aucMask[9];
	filter_param.mask[8] = pstFilterInfo->aucMask[10];
	filter_param.mask[9] = pstFilterInfo->aucMask[11];
	filter_param.mask[10] = pstFilterInfo->aucMask[12];
	filter_param.mask[11] = pstFilterInfo->aucMask[13];
#endif
/*	ADS_DRV_PRINTF("\n data        mask: \n");
	for(i = 0; i < 12; i++)
	{
		ADS_DRV_PRINTF(" %02x     %02x \n",filter_param.value[i],filter_param.mask[i]);
	}
*/
	filter_param.continuous = TRUE;
	filter_param.en_crc = TRUE;
	filter_param.req_mode = DMX_REQ_MODE_SECTION_DOUBLE;
	filter_param.ts_packet_mode = DMX_ONE_MODE;

	ret = dmx_si_chan_set_filter(p_dev,
						ad->filter[index].channel_filter_id,
						&filter_param);
	CHECK_RET(ADS,ret);

	ret = dmx_chan_start(p_dev,ad->filter[index].channel_filter_id);
	CHECK_RET(ADS,ret);

	dvbad_ads_unlock();
	ad->filter[index].p_dev = p_dev;
	ad->filter[index].channel_pid = pstFilterInfo->usChannelPid;
	ad->filter[index].start_time = mtos_ticks_get();
	ad->filter[index].filter_status = DVB_ADS_USED;
	
	return DVBAD_FUN_OK;
}

BOOL dvbad_set_filter_bat()
{
	ST_AD_DVBAD_FUNC_TYPE ret = DVBAD_FUN_ERR_PARA;
	ST_ADS_DVBAD_FILTER_INFO filter_info;
	memset(&filter_info,0,sizeof(filter_info));

	filter_info.filter_id = 0;
	filter_info.usChannelPid = BAT_PID;
	filter_info.ucFilterLen = 12;
	filter_info.aucFilter[3] = 0xFF;
	filter_info.aucFilter[4] = 0; 
	filter_info.aucFilter[6] = 0;
	filter_info.aucFilter[10] = 0xF0;
	filter_info.aucMask[3] = 0xFF;
	filter_info.aucMask[4] = 0xFF;
	filter_info.aucMask[6] = 0xFF;
	filter_info.aucMask[10] = 0xFF;

	filter_info.ucWaitSeconds = 10;
	filter_info.pfDataNotifyFunction = NULL;

	ret = DVBAD_SetFilter(&filter_info);
	if(ret != DVBAD_FUN_OK)
	{
		DEBUG(ADS, ERR, "[ERR! ret:%d\n",ret);
		MT_ASSERT(0);
	}
	DEBUG(ADS, INFO,"dvbad_set_filter_bat return true\n");
	return TRUE;
}

RET_CODE get_AD_version(u16 *param)
{
	dvbad_priv_t *ad = &s_dvbad;
	
	if((NULL==param)||(NULL==ad->p_ad_data))
	{
		DEBUG(ADS, INFO,"NO AD\n");
		return FALSE;
	}
	
	*param = ad->p_ad_data->dvbad_pic_attribute_node.ads_pic_attribute_data.version;
	DEBUG(ADS, INFO,"version = %d \n",*param);
	return TRUE;
}

BOOL dvb_ad_nim_lock(u32 freq, u32 symbolrate, u16 qammode)
{
    s32 ret = FALSE;
    nim_device_t *p_nim_dev = NULL;
    nim_channel_info_t channel_info = {0};

    DEBUG(ADS, INFO, "freq %d, symbolrate %d, qammode %d\n", freq, symbolrate, qammode);
    p_nim_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    if(NULL == p_nim_dev)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    channel_info.frequency = freq;
    channel_info.param.dvbc.symbol_rate = symbolrate;
    channel_info.param.dvbc.modulation = NIM_MODULA_QAM64;
    switch(qammode)
    {
        case 16:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM16;
            break;

        case 32:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM32;
            break;

        case 64:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM64;
            break;

        case 128:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM128;
            break;

        case 256:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM256;
            break;

        default:
            break;
    }

    ret = nim_channel_connect(p_nim_dev, &channel_info, FALSE);
    
    DEBUG(ADS, INFO, "nim_channel_connect lock return %d\n", ret);
    
    if(channel_info.lock != 1)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        ret = FALSE;
    }
    else
    {
      ret = TRUE;
    }
    return ret;
}


static RET_CODE get_ad_head_from_fialsh()
{
	RET_CODE ret;
	dvbad_priv_t *ad = &s_dvbad;

	memset(&ad->p_ad_data->ad_head_data, 0, sizeof(ad_head_flash));
	#ifndef WIN32
	ret = dvbad_drv_readflash(0, (u8*)&ad->p_ad_data->ad_head_data, sizeof(ad_head_flash));
	if(SUCCESS!=ret)
	{
		DEBUG(ADS, ERR, "get_ad_head failure\n");
		return DVBAD_UPDATE_FAILURE;
	}
	#endif	
	if(SUCCESS!=dvbad_check_sum((u8 *)&ad->p_ad_data->ad_head_data, sizeof(ad_head_flash)))
	{
		DEBUG(ADS, ERR, "get_ad_head and check sum failure\n");
		return DVBAD_UPDATE_FAILURE;
	}
	DEBUG(ADS, INFO, "get_ad_head end.ad_version:%d\n", ad->p_ad_data->ad_head_data.AD_version);
	return SUCCESS;
}

void dvbad_ads_client_init()
{
    s16 ret = FALSE;
    u32 *p_stack = NULL;
	dvbad_priv_t *ad = &s_dvbad;
    ret = mtos_sem_create((os_sem_t *)&s_dvbad.lock,1);
    if(!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    mtos_printk("[drv] %s task:%d \n",__func__,p_dvbad_priv->task_prio_end);
	ad->p_ad_data = mtos_malloc(sizeof(dvbad_ads_pic_data));
	MT_ASSERT(NULL!=ad->p_ad_data);
	memset(ad->p_ad_data, 0, sizeof(dvbad_ads_pic_data));
		
	get_ad_head_from_fialsh();
	p_stack = (u32 *)mtos_malloc(ADS_DVBAD_TASK_STKSIZE);
    MT_ASSERT(p_stack != NULL);

    ret = mtos_task_create((u8 *)"ads_monitor",
                                           (void *)ads_dvbad_data_monitor,
                                           NULL,
                                          // p_dvbad_priv->task_prio_start - 1,
                                           p_dvbad_priv->task_prio_end,
                                           p_stack,
                                           ADS_DVBAD_TASK_STKSIZE);
    if(!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
}

void DVBAD_SemaphoreInit(IN DVBAD_Semaphore* puiSemaphore , u8 ucInitVal)
{
    BOOL ret = FALSE;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == puiSemaphore)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    //ADS_DRV_PRINTF("[drv] puiSemaphore = 0x%x, val= %d ( 0 or 1) \n",puiSemaphore,ucInitVal);

    ret = mtos_sem_create((os_sem_t *)puiSemaphore, ucInitVal);
    if (!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    //ADS_DRV_PRINTF("[drv] semaphore 0x%x = %d \n",puiSemaphore,*puiSemaphore);
    //OS_PRINTF("[drv] init semaphore = 0x%x \n",*puiSemaphore);
}

