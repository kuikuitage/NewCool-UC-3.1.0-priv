/********************************************************************************************/
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_util.h"

#include "mem_manager.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "mdl.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "nit.h"

#include "service.h"
//#define NIT_DEBUG
#ifdef NIT_DEBUG
#define NIT_DBG OS_PRINTF
#else
#define NIT_DBG  DUMMY_PRINTF
#endif

void parse_nit(handle_t handle, dvb_section_t *p_sec)
{
  service_t *p_svc = handle;
  nit_t _nit = {0};
  nit_t *p_nit = &_nit;
  u8 *p_buf = p_sec->p_buffer;
  s16 network_desc_len = 0;
  s16 ts_loop_len = 0;
  s16 desc_len = 0;
  //u16 net_name_des_len = 0;
  os_msg_t msg = {0};
  u16 ts_id = 0;
  u16 offset = 0;
  u16 offset0 = 0;
  u8  tp_cnt = 0 ;
  u8  tp_ext_cnt = 0 ;
  u8  svc_cnt = 0;
  s16 svc_list_des_length = 0;

  u16 nxt_ts_start = 0;

  if(p_buf[0] != DVB_TABLE_ID_NIT_ACTUAL &&
    p_buf[0] != DVB_TABLE_ID_NIT_OTHER)
  {
    NIT_DBG("MDL: not nit\n");
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_NIT_ACTUAL;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    return;
  }

  if((p_buf[5] & 0x01)== 0)
  {
    NIT_DBG("MDL: this nit is not usable\n");
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_NIT_ACTUAL;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    return;
  }
  p_nit->p_origion_data = p_sec->p_buffer;
  p_nit->network_id = MAKE_WORD(p_buf[4], p_buf[3]);
  p_nit->version_num =(p_buf[5] & 0x3E) >> 1;
  p_nit->sec_number = p_buf[6];

  p_nit->last_sec_number = p_buf[7];
  
  network_desc_len = MAKE_WORD(p_buf[9], (p_buf[8] & 0x0F));
  offset = 10;
  offset0 = offset;
  offset += network_desc_len;

  //OS_PRINTF("network_desc_len[%d]\n", network_desc_len);
  
  while(network_desc_len > 0)
  {
    //net_name_des_len = p_buf[offset + 1];
    if(p_buf[offset0] == DVB_DESC_NETWORK_NAME)
    {
      //Skip tag
      offset0 ++;
      network_desc_len --;
      
      //Fetch desc length
      desc_len = p_buf[offset0];

      //Skip desc length
      offset0 ++;
      network_desc_len --;
  
      //get network name & skip it.
      if(desc_len != 0)
      {
        memcpy(p_nit->network_name, p_buf + offset0, desc_len);
      }
      else
      {
        memset(p_nit->network_name, 0, MAX_NETWORK_NAME_LEN);
      }

      NIT_DBG("nit network name: %s, length %d\n", &p_nit->network_name, desc_len);

      offset0 += desc_len;
      network_desc_len -= desc_len;
    }
    else if(p_buf[offset0] == DVB_DESC_SERVICE_UPDATE)
    {
      //Skip tag
      offset0 += 1;
      network_desc_len -=1;

      //skip length
      offset0 += 1;
      network_desc_len -=1;
      
      //
      p_nit->svc_update.force_flag = (p_buf[offset0]&0x80)>>7;
      p_nit->svc_update.ver_num  = p_buf[offset0]&0x7f;     

      //Skip flag
      offset0 += 1;
      network_desc_len -= 1;

      // length is fixed as 1
      NIT_DBG("service update len: %d\n", p_buf[offset0 + 2]);
    }
    else if(p_buf[offset0] == DVB_DESC_LOGIC_CHANNEL)
    {
      desc_len = p_buf[offset0 + 1];
      network_desc_len -= (2 + desc_len);
      offset0 += 2 + desc_len;
      // parse logical channel list here
    }
    else if(p_buf[offset0] == DVB_DESC_LINKAGE)
    {
      desc_len = p_buf[offset0 + 1];
      network_desc_len -= (2 + desc_len);
      offset0 += 2 + desc_len;
    }
    else
    {
     //offset0 ++;
     //network_desc_len --;
    desc_len = p_buf[offset0 + 1];
    network_desc_len -= (2 + desc_len);
    offset0 += 2 + desc_len;
    }
  }

  ts_loop_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
  NIT_DBG("ts_loop_len = %d\n", ts_loop_len);
  
  offset += 2;

  while(ts_loop_len > 0)
  {
    //OS_PRINTF("ts_loop_len[%d]\n", ts_loop_len);

    /*! ts id*/
    ts_id = MAKE_WORD(p_buf[offset + 1], p_buf[offset]);
    p_nit->tp_svc_list[tp_cnt].ts_id = ts_id;

    offset += 4;

    desc_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
    /*! Skip offset length*/
    offset += 2;
    
    ts_loop_len -= (desc_len + 6);
    /*! Saving next start position for next ts id*/
    nxt_ts_start = offset + desc_len; 
    svc_cnt   = 0 ;
    while(desc_len > 0)
    {
      NIT_DBG("TAG [%x]\n", p_buf[offset]);
      NIT_DBG("desc_len[%d]\n", desc_len);
      switch(p_buf[offset])
      {
        case DVB_DESC_SERVICE_LIST:
          /*!Jump to descriptor length*/
          offset += 1; 
          desc_len --;
          
          svc_list_des_length = p_buf[offset];
          desc_len -= svc_list_des_length;

          /*!Jump to service id */
          offset += 1;
          desc_len --;
          while(svc_list_des_length > 0)
          {
            //OS_PRINTF("svc_list_des_length[%d]\n", svc_list_des_length);
            //p_nit->tp_svc_list[tp_cnt].svc_id[svc_cnt] 
                    //= MAKE_WORD(p_buf[offset+1], p_buf[offset]);
            /*!Skip service id and service type */
            offset += 3;
            svc_cnt ++;
            svc_list_des_length -=3;
            if(svc_cnt > MAX_PROG_NUM)
            {
              break;
            }
          }
          //p_nit->tp_svc_list[tp_cnt].total_svc_num = svc_cnt;
          //tp_cnt ++;
          break;
        case DVB_DESC_SATELLITE_DELIVERY:
        {
          sat_tp_info_t *p_tp_info = &p_nit->tp_svc_list[tp_cnt].dvbs_tp_info;

          p_nit->tp_svc_list[tp_cnt].tp_type = NIT_DVBS_TP; 
          /*! Skip tag*/
          offset += 1;
          desc_len -=1;
          
          /*! Fetch desc length*/
          desc_len -=  p_buf[offset];

          /*! Skip length*/
          offset += 1;
          desc_len -=1;

          p_tp_info->frequency  = make32(&p_buf[offset]);

          offset += 4;
          p_tp_info->orbital_position 
                                    = MAKE_WORD(p_buf[offset+1], p_buf[offset]);

          offset += 2;
          p_tp_info->west_east_flag = (p_buf[offset]&0x80)>>7;
          p_tp_info->polarization = (p_buf[offset] & 0x60)>> 5;
          //circule left 
          if(p_tp_info->polarization >= 2)
          {
            p_tp_info->polarization -= 2;
          }
          p_tp_info->modulation_system = (p_buf[offset] & 0x04);
          p_tp_info->modulation_type = (p_buf[offset] & 0x03);
          //p_tp_info->modulation = (p_buf[offset] & 0x1F);

          offset += 1;
          p_tp_info->symbol_rate = make32(&p_buf[offset])>>4;  
          offset += 3;
          p_tp_info->fec_inner = (p_buf[offset] & 0x0F);
          offset += 1;
          tp_cnt ++;
        }
          break;
        case DVB_DESC_CABLE_DELIVERY:
        {
          cable_tp_info_t *p_tp_info = &p_nit->tp_svc_list[tp_cnt].dvbc_tp_info;
          
          p_nit->tp_svc_list[tp_cnt].tp_type = NIT_DVBC_TP;
          offset += 1;
          desc_len -=1;
          desc_len -=  p_buf[offset];
          offset += 1;
          desc_len -=1;
          p_tp_info->frequency = make32(&p_buf[offset]);
          offset += 4;
          offset += 1;
          p_tp_info->fec_outer = p_buf[offset] & 0xf;
          offset += 1;
          p_tp_info->modulation = p_buf[offset];
          offset += 1;
          p_tp_info->symbol_rate = MT_MAKE_DWORD(
                    MAKE_WORD((p_buf[offset + 3] & 0xf0), p_buf[offset + 2]),
                    MAKE_WORD(p_buf[offset + 1], p_buf[offset]));
          p_tp_info->symbol_rate >>= 4;
          
          offset += 3;
          p_tp_info->fec_inner = p_buf[offset] & 0xf;
          offset += 1;
          tp_cnt ++;
        }
          break;
        case DVB_DESC_TERRERTRIAL_DELIVERY:
        {
          terrestrial_2_tp_info_t *p_tp_info = &p_nit->tp_svc_list[tp_cnt].t2_tp_info;
          
          p_nit->tp_svc_list[tp_cnt].tp_type = NIT_DVBT2_TP;
          offset += 1;
          desc_len -=1;
          desc_len -=  p_buf[offset];
          offset += 1;
          desc_len -=1;
          p_tp_info->centre_frequency = make32(&p_buf[offset]);
          offset += 4;
          p_tp_info->bandwidth = (p_buf[offset] >> 5) & 0x07;
          switch(p_tp_info->bandwidth)
          {
            case 0x0:
              p_tp_info->bandwidth = 8;
              break;
            case 0x1:
              p_tp_info->bandwidth = 7;
              break;
            case 0x2:
              p_tp_info->bandwidth = 6;
              break;
            case 0x3:
              p_tp_info->bandwidth = 5;
              break;
            default:
              NIT_DBG("reserved,unknown\n");
              break;
          }
          offset += 7;//skip other data
          tp_cnt ++;
        }
          break;
          case DVB_DESC_EXTENSION:
            {
              u8 ext_tag = 0;
          u16 offset_final = 0;
              //extern
               /*!Jump to descriptor length*/
              offset += 1; 
              desc_len --;
              
              svc_list_des_length = p_buf[offset];
          offset ++;
              desc_len --;
              
              desc_len -= svc_list_des_length;
          offset_final = offset + svc_list_des_length;

              
              ext_tag = p_buf[offset];
              switch(ext_tag)
              {
                case 0x04:
                  {
              terrestrial_2_tp_info_t *p_tp_info = &p_nit->tp_svc_list[tp_ext_cnt].t2_tp_info;
                    offset ++;
                    p_tp_info->plp = p_buf[offset];
                    offset ++;
                    p_tp_info->t2_sys_id = MAKE_WORD(p_buf[offset + 1], p_buf[offset]);
                  }
                  break;
                 default:
                  break;
              }
          offset = offset_final;
          tp_ext_cnt ++;
            }
            break;
          case  DVB_DESC_LCD:
            {
              /*!Jump to descriptor length*/
              offset += 1; 
              desc_len --;
              
              svc_list_des_length = p_buf[offset];
              desc_len -= svc_list_des_length;

              /*!Jump to service id */
              offset += 1;
              desc_len --;
              while(svc_list_des_length > 0)
              {
                if(p_nit->lcd_cnt > MAX_LCD_NUM)
                {
                  OS_PRINTF("p_nit->lcd_cnt[%d]\n", p_nit->lcd_cnt);
                  break;
                }
                OS_PRINTF("svc_list_des_length[%d]\n", svc_list_des_length);
                p_nit->lcd[p_nit->lcd_cnt].svc_id
                        = MAKE_WORD(p_buf[offset+1], p_buf[offset]);
                /*!Skip service id and service type */
                offset += 2;
                svc_list_des_length -= 2;
                p_nit->lcd[p_nit->lcd_cnt].visible_service_flag = p_buf[offset] >> 7;
                p_nit->lcd[p_nit->lcd_cnt].logical_channel_number = 
                  (MAKE_WORD(p_buf[offset+1], p_buf[offset])) & 0x03FF;
                OS_PRINTF("lcd[%d],svc_id=%d,lcn=%d\n", p_nit->lcd_cnt, 
                  p_nit->lcd[p_nit->lcd_cnt].svc_id, 
                  p_nit->lcd[p_nit->lcd_cnt].logical_channel_number);
                offset += 2;
                svc_list_des_length -= 2;
                p_nit->lcd_cnt ++;
              }
            }
            break;
          case DVB_DESC_STUFFING:
          case DVB_DESC_PRIVATE_DATA:
          case DVB_DESC_FREQUENCY_LIST:
          case DVB_DESC_MULTILIGUAL_NETWORK_NAME:
          case DVB_DESC_CELL_LIST:
          case DVB_DESC_CELL_FREQUE:
          case DVB_DESC_DEFAULT_AUTHORITY:
          case DVB_DESC_TIME_SLICE:
          case DVB_DESC_LINKAGE:
             //Skip tag
            offset ++;
            desc_len -- ;
            //if(desc_len > tmp_desc_length)
            {
              desc_len -= (s16)p_buf[offset];
              offset   += p_buf[offset];   
            }
            
            offset ++;
            desc_len -- ;

          break;
          default:
          /*! 
            Jump to unknown desc length
           */
          offset ++;
          desc_len --;

          NIT_DBG("MDL: parse nit tmp_desc_length %d\n",p_buf[offset]);
          NIT_DBG("MDL: parse nit desc_len %d\n",desc_len);

          //Skip content of unkown descriptor
          //if(desc_len > tmp_desc_length)
          {
            desc_len -=  (s16)p_buf[offset];
            offset   +=  p_buf[offset];   
          }

          NIT_DBG("MDL: parse nit offset %d\n",  offset);
          NIT_DBG("MDL: parse nit desc_len %d\n",desc_len);

          offset ++;
          desc_len -- ;
          break;
        }

    }
    
    offset = nxt_ts_start ;
    if((tp_cnt >= MAX_TP_NUM) || (tp_ext_cnt >= MAX_TP_NUM))
    {
      NIT_DBG("tp cnt [%d],tp ext cnt [%d]\n", tp_cnt, tp_ext_cnt);
      break;
    }
  } 

  /*! Saving total tp number*/
  p_nit->total_tp_num = tp_cnt;
  p_nit->table_id = p_sec->table_id;
  
  //All the tasks receiving this message must have higher task priority 
  //than DVB task
  msg.content = DVB_NIT_FOUND;
  msg.is_ext = 0;
  msg.para1 =(u32)(p_nit);
  msg.para2 = sizeof(nit_t);  
  msg.context = p_sec->r_context;
#ifdef __LINUX__
  msg.is_ext = 1;
#endif
  p_svc->notify(p_svc, &msg);

  NIT_DBG("MDL: End parse nit\n");

  return;  
}

void request_nit(dvb_section_t *p_sec, u32 table_id, u32 para)
{
  u16 network_id = para & 0xFFFF;
  u8 sec_number = (para & 0xFF0000) >> 16;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  
  p_sec->pid         = DVB_NIT_PID;
  p_sec->table_id    = table_id;
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,
    sizeof(u8) * MAX_FILTER_MASK_BYTES);
  p_sec->filter_code[0] = table_id;
  p_sec->filter_mask[0] = 0xFF;
  p_sec->filter_mode    = FILTER_TYPE_SECTION;
#ifndef WIN32
if(network_id != 0xFFFF)
{
  p_sec->filter_code[1] = network_id >> 8;
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = network_id & 0xFF;
  p_sec->filter_mask[2] = 0xFF;
}

  p_sec->filter_code[4] = sec_number;
  p_sec->filter_mask[4] = 0xFF;  
  p_sec->filter_mask_size = 5;
#else
if(network_id != 0xFFFF)
{
  p_sec->filter_code[3] = network_id >> 8;
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = network_id & 0xFF;
  p_sec->filter_mask[4] = 0xFF;
}
  p_sec->filter_code[6] = sec_number;
  p_sec->filter_mask[6] = 0xFF;  
  p_sec->filter_mask_size = 7;
#endif
  
  p_sec->timeout = NIT_TIMEOUT;
  
  p_sec->direct_data = FALSE;
  p_sec->crc_enable = 1;
  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}
