/********************************************************************************************/
/********************************************************************************************/
#ifndef __SDT_H_
#define __SDT_H_
/*!
  \file sdt.h
  This file defines the common data information available in SDT table. These 
  information cover all the descriptors existing in SDT table.
  
  Development policy:
  SDT part should be developed together with DVB module. And by requesting SDT API,
  filter SDT information operation will be performed and the ts packet information
  with SDT data will be fed into parse sdt api to get the sdt section data
  */
/*!
  Max service name length
  */
#define MAX_SVC_NAME_LEN (32)

/*!
  sdt descriptor head length
  */
#define SDT_DESC_HEAD_LEN   0x5

/*!
  sdt section head length(following the section_length field and including the CRC.)
  */
#define SDT_SEC_HEAD_LEN    12

/*!
  crc size
  */
#define CRC_SIZE            4

/*!
  max sdt sec length
  */
#define MAX_SDT_SEC_LEN     1024  


/*!
  SDT service descriptor definition
  */
typedef struct
{
  /*!
    Service id
    */
  u16 svc_id ;
  /*!
    service type
    */
  u8 service_type;
  /*!
    EIT schedule flag
    */
  u8 eit_sch_flag ;
  /*!
    EIT PF flag
    */
  u8 eit_pf_flag ;
  /*!
    Channel volum compensation
    */
  u8 chnl_vlm_cmpt;  
  /*!
    Running status
    */
  u8 run_sts ;
  /*!
    Service scrambled flag, 
    1: scramble
    0: free
    */
  u8 is_scrambled : 3;
  /*!
    mosaic descriptor found
    */
  u8 mosaic_des_found : 1;
  /*!
    logic screen descriptor found
    */
  u8 logic_screen_des_found : 1;
  /*!
    linkage descriptor found
    */
  u8 linkage_des_found : 1;
  /*!
    linkage descriptor found
    */
  u8 nvod_reference_des_found : 1;
  /*!
    linkage descriptor found
    */
  u8 time_shifted_svc_des_found : 1;
  /*!
    nvod reference des cnt or time shifted svc reference svc id
    */
  u16 nvod;
  /*!
    CA system id
    */
  u16 ca_system_id;
  /*!
    Service name information
    */
  u8 name[MAX_SVC_NAME_LEN];
  /*!
    mosaic descriptor(service_type = 0x06) or
    nvod reference descriptor
    */
  void *p_nvod_mosaic_des;
} sdt_svc_descriptor_t;

/*!
  SDT table structure
  */
typedef struct
{
  /*!
    Stream id
    */
  u16 stream_id;
  /*!
    SDT section length
    */
  u16 sec_length;
  /*!
    Section number of SDT
    */
  u8 sec_number;
  /*!
    Last section number
    */
  u8 last_sec_number;
  /*!
    Original network id
    */
  u16 org_network_id;
  /*!
    Module id for deal sdt request conflicts
    */
  u8 module_id;
  /*!
    PF flag
    */
  u8 pf_flag;
  /*!
    Version number
    */
  u8 version_num;
  /*!
    Service number
    */
  u16 svc_count;
  /*!
    Service descriptor information
    */
  sdt_svc_descriptor_t svc_des[DVB_MAX_SDT_SVC_NUM];
  /*!
    origion data
    */
  u8 *p_origion_data;
} sdt_t;

/*!
  Parse sdt section

  \param[in] handle Service handle
  \param[in] p_sec: Section information filtered by PTI driver
  */
void parse_sdt(handle_t handle, dvb_section_t *p_sec);

/*!
  Parse sdt section for dvbc project

  \param[in] handle Service handle
  \param[in] p_sec: Section information filtered by PTI driver
  */
void parse_sdt_dvbc(handle_t handle, dvb_section_t *p_sec);

/*!
  Request SDT section in actual stream
  \param[in] p_sec information for request sdt section
  \param[in] table_id table id of sdt when request sdt information
  \param[in] para2 section information
  */
void request_sdt(dvb_section_t *p_sec, u32 table_id, u32 para2);

#endif // End for __ABS_SDT_H_

