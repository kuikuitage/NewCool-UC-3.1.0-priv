/******************************************************************************/

/******************************************************************************/
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"  

#include "lib_util.h"
#include "mem_manager.h"
#include "mtos_printk.h"
#include "mtos_mem.h"

#include "class_factory.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#include "data_base.h"
#include "data_base16v2.h"


#define LINE_MAX_SIZE 300 //largest number of letters in a line

typedef enum tag_line_type
{
  LINE_COMMENTS = 0,
  LINE_SEPARATOR,
  LINE_BLOCK_NUM, 
  LINE_BLOCK_ID, 
  LINE_BLOCK_NAME,
  LINE_BLOCK_VERSION,
  LINE_TOTAL_TYPE    // ADD line type above, leave this as the last one

}line_type_t;

const u8 *line_type[] = //map with line_type_t
{
  "//",           //annotation
  "----",         //new ID sepeartor
  "blknum",
  "id",           //block ID
  "name",         //block name
  "version",      //block version
  
};

typedef struct 
{
  u32 id;
  u8 name[9];
  u32 version;
  
}upg_block_info_t;


typedef struct 
{
  u8 blk_num;
  u8 blk_cnt;
  upg_block_info_t m_block[MAX_BLOCK_NUM]; 
  
} upg_info;


static u32 getline(u8 *buf, u8 *line,u32 *offset)
{
  BOOL is_end = 1;
  u32 ch;
  u32 i = 0;
  s32 ret = 0;
  u8 *tem = buf + *offset;
  u8 *tem1 = NULL;
  while(1)
  {
    ch = *tem++;
    *offset += 1;
    if ('\n' == ch)   
    {
       break;
    }   
      
    else if ('#' == ch)//if it's end 
    {

      tem1 = tem -1;

      while(1)
      {
        if(*tem1-- == '\n')
        {
            is_end = 1;
            break;
        }
        else if(*tem1 == '/')
        {
            is_end = 0;
            break;
        }
            
      }
      if(!is_end)
      {
         line[i++] = (u8)ch;
      }
      else
      {
         ret = -2;
         break;
      }
    }
    else
      line[i++] = (u8)ch;
    
    if (i >= LINE_MAX_SIZE)
    {
      OS_PRINTF("line is too long!!");
      ret = -1;
      break;
    }
  }

  //remove the space, tab or dos format in the end
  while((i>0) && ((line[i-1] == '\r')||(line[i-1] == ' ')||(line[i-1] == '\t')))
  {
    line[i-1] = '\0';
    i--;
  }
  return ret;
}

static unsigned getsize(const u8 *line)
{
  unsigned i=0;
  unsigned sum=0,d=0;
  size_t len;
  u8 tmp[30]={0,};

  len = strlen(line);
  for (i=0; i<len; i++)
  {
    if (('=' != line[i])&&(' ' != line[i]))
    break;
  }

  if (0 == strncasecmp(&line[i],"0x", 2)) //hex string to int
  {
    memcpy(tmp, &line[i+2], len-(i+2));
    i=0;
    while(tmp[i] != '\0')
    {
      switch (tmp[i])
      {
        case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
          d = tmp[i]-'a'+10;
          break;
        case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
          d = tmp[i]-'A'+10;
          break;
        default:
          d = tmp[i]-'0';
          break;
      }
      sum = sum*16 + d;
      i++;
    } 
  }
  else
  {
    memcpy(tmp,&line[i], len-i);
    i=0;
    while(tmp[i]!='\0')
    {
      sum = sum*10 + tmp[i] - '0';
      i++;
    }
  }
  
  return sum;
}
static void getstring(upg_info *upg_priv,u32 type,char* line)
{
  u32 i; 	
  size_t len = strlen(line);
    
  for (i=0; i<len; i++)
  {
    if ((' ' != line[i])&&('=' != line[i]))
      break;
  }
  if (i >= len)
  {
    return; //ignore
  }
  
  len -= i;
  switch(type)
  {
    case LINE_BLOCK_NAME:
      len = (len>8)?8:len;
      memcpy(upg_priv->m_block[upg_priv->blk_cnt].name, &line[i], len);
      break;
    default:
      break;
  }
}
static u32 ParseLine(char *line ,upg_info *upg_priv)
{
  size_t len = strlen(line);
  unsigned i,j;
  char *p;
  char *pl;

  for (j=0; j<len; j++)
  {
    if ((' ' != line[j]) && ('\t' != line[j]) //ignore space and tab
      && ('\r' != line[j])) //ignore dos sign
      break;
  }
  if (j>=len)
  {
    return 0; //ignore the a line full of space or tab
  }

  pl = &line[j];
  
  for (i=0; i<LINE_TOTAL_TYPE; i++)
  {
    if (0 == strncasecmp(pl,line_type[i],strlen(line_type[i])))//find cmd
       break;
  }

  if (i >= LINE_TOTAL_TYPE)
  {
    return -1;
  }
  p = &pl[strlen(line_type[i])];
  switch(i)
  {
    case LINE_COMMENTS:
      //OS_PRINTF("comment: %s\n", line);
      break;
    case LINE_SEPARATOR:
      //OS_PRINTF("separator: %s\n", line);
      break;
    case LINE_BLOCK_NUM:
      upg_priv->blk_num = getsize(p);
      break;
    case LINE_BLOCK_ID:
      upg_priv->blk_cnt ++;
      upg_priv->m_block[upg_priv->blk_cnt].id = getsize(p);
      break;
    case LINE_BLOCK_VERSION:
      upg_priv->m_block[upg_priv->blk_cnt].version = getsize(p);
      break;
    case LINE_BLOCK_NAME:
      getstring(upg_priv,LINE_BLOCK_NAME,p);
      break;
    default:
      OS_PRINTF("unknown type: %s\n", p);
      break;
  }
  
  return 0;
}
static void get_upg_info(u8 *buf,upg_info *upg_priv)
{
    BOOL end_parse  = 0;
    u8 line[LINE_MAX_SIZE] = {0};
    u32 offset = 0;
    u32 ret = 0;
    u32 i, num=0;
     
    while(1)
    {
        memset(line, 0x00, LINE_MAX_SIZE);

        ret = getline(buf,line,&offset);

        if (-1 == ret)
        {
          OS_PRINTF("line %d is too long!\n", num);
          
        }
        else if (-2 == ret)
        {
          end_parse = 1;
        }
        num++;
        if ((strlen(line)>0) && (-1 == ParseLine(line,upg_priv)))
        {
          OS_PRINTF("unknown cmd at line %d:%s\n", num,line);

        }
        if(end_parse)
         break; 
        
    }   

    OS_PRINTF("block num %d\n",upg_priv->blk_num);   

    for(i = 1;i < 11 ;i++)
        OS_PRINTF("block id %02x, name %s,version %d\n",upg_priv->m_block[i].id,upg_priv->m_block[i].name,upg_priv->m_block[i].version);
                  
}
static void get_upginfo_from_usb(u8 *buf,upg_info *upg_priv)
{
    u8 *p_headr = NULL;
    u8 i = 0;
    u16 blk_num = 0;
    u32 upginfo_off = 0;

    p_headr = buf + 0x200000 + 16 + 8 +4;

    blk_num =  MAKE_WORD(*((buf) + 16 + 8 + 0x200000), *((buf) + 1 + 16 + 8 + 0x200000));
            
    for(i = 0;i < blk_num;i++)
    {
        if(*p_headr == USB_UPGINFO_BLOCK_ID)
        {
            upginfo_off = MT_MAKE_DWORD( MAKE_WORD(*(p_headr + 4), *(p_headr + 5)),  MAKE_WORD(*(p_headr + 6), *(p_headr + 7)));     
            OS_PRINTF("upginfo offset : %x \n",upginfo_off);
            break;
        }
        else
        {
            p_headr += sizeof(dmh_block_info_t);
        }
        
    }
    buf = buf + 0X200000 + upginfo_off;

    get_upg_info(buf,upg_priv);
    
}
static void get_upginfo_from_flsh(upg_info *upg_priv)
{
    u32 size = 0;
    u8 *buf = NULL;
    dmh_block_info_t usb_upginfo_head;


    dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID), USB_UPGINFO_BLOCK_ID, &usb_upginfo_head);
    
    size = usb_upginfo_head.size;//little endian
    size =   MT_MAKE_DWORD(MAKE_WORD((size & 0xFF000000) >> 24,(size & 0x00FF0000) >> 16),MAKE_WORD((size & 0x0000FF00) >> 8,(size & 0x000000FF)));  
    
    buf  = mtos_malloc(size);

    dm_read(class_get_handle_by_id(DM_CLASS_ID), USB_UPGINFO_BLOCK_ID, 0, 0,size,buf);

    get_upg_info(buf,upg_priv);

    mtos_free(buf);
}

BOOL version_compare(u8 *pbuf)
{
    u32 ver_usb = 0;
    u32 ver_flsh = 0;
    upg_info *usb_info = NULL;
    upg_info *flash_info = NULL;

    usb_info = malloc(sizeof(upg_info));
    flash_info = malloc(sizeof(upg_info));

    
    get_upginfo_from_usb(pbuf,usb_info);
    get_upginfo_from_flsh(flash_info);

    ver_flsh = flash_info->m_block[1].version;
    ver_usb = usb_info->m_block[1].version;

    if(NULL != usb_info)
        free(usb_info);
    if(NULL != flash_info)
        free(flash_info);

    OS_PRINTF("ver_usb = %d,ver_flsh = %d\n",ver_usb,ver_flsh);

    if(ver_usb > ver_flsh)
    {
        return 1;//newr version
    }
    else
    {
        return 0;//not a new version
    }

}


