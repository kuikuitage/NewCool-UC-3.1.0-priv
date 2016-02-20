#ifndef __dvbca_cas_include_h__
#define __dvbca_cas_include_h__

#define R_INDEX 	(0)
#define G_INDEX 	(1)
#define B_INDEX 	(2)
#define A_INDEX 	(3)

#define DVBCA_CAS_TASK_MAX (8)
#define DVBCA_TASK_PRIORITY_0 (45)
#define DVBCA_FILTER_MAX_NUM (6)
#define DVBCA_FILTER_DEPTH_SIZE (DMX_SECTION_FILTER_SIZE)

typedef struct
{
  /*!
    max support mail num
    */
  u8 max_mail_num;
  /*!
    mail policy see cas_data_overlay_policy_t
    */
  u8 mail_policy;
}cas_adt_dvbca_maii_cfg_t;

typedef enum dvbca_filter_status
{
    DVBCA_FILTER_STATUS_FREE,
    DVBCA_FILTER_STATUS_BUSY
}dvbca_filter_status_t;

typedef struct dvbca_ca_desc_s
{
    u16   ca_system_id        :16;
    u32   reserve             :3;
    u32   ca_pid              :13;
}dvbca_ca_desc_t;

/**
  Filter Struct
*/
typedef struct
{
	BOOL valid;
	u16 pid;
	u8 filter_data[DVBCA_FILTER_DEPTH_SIZE];
	u8 filter_mask[DVBCA_FILTER_DEPTH_SIZE];
	u16 req_handle;
	u32 start_ms;
	u8 *p_buf;
}filter_struct_t;


typedef struct dvbca_task_buff_s
{
    u32  task_prio;
    BOOL is_busy;
}dvbca_task_buff_t;

typedef struct
{
	/*!
		The callback of nvram reading function
	*/
	RET_CODE (*nvram_read)(u32 offset, u8 *p_buf, u32 *size);
	/*!
		The callback of nvram writing function
	*/
	RET_CODE (*nvram_write)(u32 offset, u8 *p_buf, u32 size);
	/*!
    	The callback of machine serial number getting
    */
  	RET_CODE (*machine_serial_get)(u8 *p_buf, u32 size);
	/*!
		The callback of software version of STB
	*/
	RET_CODE (*software_version_get)(u8 *p_buf, u32 size);
  	/*!
		The callback of query program check
	*/
	RET_CODE (*query_check)(u16 net_id, u16 ts_id, u16 sev_id);
	/*!
		flash_start_address
	*/
	u32 flash_start_add;
	/*!
		flash_size
	*/
	u32 flash_size;
	/*!
		video pid
	*/
	u16 v_pid;
	/*!
		audio pid
	*/
  	u16 a_pid;
	/*!
		NIT PDSD data
	*/
	u32 private_data_specifier;
	/*!
		task priority
    */
    u32 task_prio;
	/*!
		task priority
    */
    u32 task_prio_monitor;
	/*!
	    slot id
	*/
	u8 slot;
	/*!
	    smart card status
	*/
    u8 card_status;
}s_cas_dvbca_priv_t;

typedef struct
{
	DVBCA_UINT8 *Title;
	DVBCA_UINT8 *Content;
} dvbcas_urgent;

typedef struct
{

  //handle of smart card
  scard_device_t *p_smc_dev;
  //handle of demux
  dmx_device_t *p_dmx_dev;
  // semphore used for lock
  os_sem_t mg_port_sem;
  
  u8 stb_serial[10];
  u8 card_status;
  u8 card_ready_flg;
  u8 slot;

  filter_struct_t emm_filter[DVBCA_FILTER_MAX_NUM];
  filter_struct_t ecm_filter[DVBCA_FILTER_MAX_NUM];
  u8 filter_mode;

  dvbca_task_buff_t task_record[DVBCA_CAS_TASK_MAX];
  s_cas_dvbca_priv_t task_priv;
  s_cas_dvbca_priv_t monitor_priv;

  /*!
  The callback of nvram reading function
  */
  RET_CODE (*nvram_read)(u32 offset,u8 *p_buf, u32 *size);
  /*!
  The callback of nvram writing function
  */
  RET_CODE (*nvram_write)(u32 offset,u8 *p_buf, u32 size);

  /*!    
    The callback of nvram erase function
    */
  RET_CODE (*nvram_erase)(u32 offset, u32 size);

  u16 dvbca_ecm_pid;
  u8  dvbca_ecm_pid_num;

  u16 cur_ecm_pid;
  u16 cur_emm_pid;
  u16 cur_ecm_dmx_chan;
  u16 cur_v_pid;
  u16 cur_a_pid;
  u16 pre_v_pid;
  u16 pre_a_pid;
  u16 zone_code;
  cas_adt_dvbca_maii_cfg_t mail_cfg;
  u8 zone_code_flag;
  u8 internal_reset_flag;

  u8 reset_flag;
  u8 expiring_days;

  u16 used_chV;//used video channel
  u16 used_chA;
  cas_upgrade_freq_t upgrade_freq;
  msg_info_t dvbca_OsdMsg;
  finger_msg_t dvbca_msgFinger;
  os_sem_t ca_osd_lock;
  os_sem_t ca_finger_lock;
  os_sem_t dvbcas_lock;
  dvbcas_urgent urgent;
}cas_adt_dvbca_priv_t;

#endif

