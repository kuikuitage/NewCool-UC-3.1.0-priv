/**********************************************************************/
/**********************************************************************/
#ifndef __CUSTOMER_CONFIG_H_
#define __CUSTOMER_CONFIG_H_


// internal macro for compile 


typedef enum
{
  CUSTOMER_DEFAULT,
  CUSTOMER_YINHE,
  CUSTOMER_KINGVON,
  CUSTOMER_JIZHONG,
  CUSTOMER_NEWSTART,
  CUSTOMER_AISAT,

  CUSTOMER_MAIKE,					// for maike dtmb
  CUSTOMER_MAIKE_HUNAN,				// for hunan maike dtmb
  CUSTOMER_MAIKE_HUNAN_LINLI,		// for hunan maike dtmb
  CUSTOMER_XINNEW_DEMO, 			// use this if seenew demo
  CUSTOMER_XINNEW_DESAI_HAICHENG, 	// use this if seenew demo
  CUSTOMER_DTMB_DESAI_JIMO,			// for seenew dtmb desai jimo
  CUSTOMER_DTMB_CHANGSHA_HHT,		// for seenew dtmb divi changchahuhutong
  CUSTOMER_DTMB_SHANGSHUIXIAN,		// for seenew dtmb dvb shangshuixian
  CUSTOMER_XINNEW_GAIZHOU,			// for seenew dtmb dvb shangshuixian

}CUSTOMER_ID_ENUM;

typedef enum
{
  COUNTRY_INDIA,
  COUNTRY_CHINA,
}COUNTRY;

typedef struct customer_cfg
{
  u8 customer_id;
  u8 cas_id;
  u8 country;
  BOOL b_WaterMark;
  u32  x_WaterMark;
  u32  y_WaterMark;
  u8 language_num;
  void (*cus_uio_init)(void);
  void (*cus_config_dac)(void *p_dev);
  void (*cus_audio_max_volume)(void *p_dev);
  void (*cus_reg_set)(void);
  void (*cus_bouquet_set)(void *p_data);
  void (*cus_ir_led_set)(void);
  void (*cus_config_ota_main_tp)(void);
}customer_cfg_t;

extern customer_cfg_t g_customer;
#define CAS_ID (g_customer.cas_id)
#define CUSTOMER_ID (g_customer.customer_id)

typedef enum
{
  UIO_LED_TYPE_4D = 0,
  UIO_LED_TYPE_3D,
} CUSTOMER_UIO_LED_TYPE;


//config customer cas
typedef enum
{
  /*
    desai config id
  */
  CONFIG_CAS_ID_DS = 1,
  /*
    only_1 config id
  */
  CONFIG_CAS_ID_ONLY_1,
    /*
    abv config id
  */
  CONFIG_CAS_ID_ABV,
  /*
    yxsb config id
  */
  CONFIG_CAS_ID_TF,
  /*
    cryptoguard config id
  */
  CONFIG_CAS_ID_CRYPG,
    /*
    quanzhi config id
  */
  CONFIG_CAS_ID_GS,
  /*
    sanzhouxunchi config id
  */
  CONFIG_CAS_ID_ADT_MG,
  /*
    topreal config id
  */
  CONFIG_CAS_ID_TR,
  /*
    sumavision config id
  */
  CONFIG_CAS_ID_SV,
  /*
	boyuan config id
  */
  CONFIG_CAS_ID_BY,
  /*
	dvbca config id
  */
  CONFIG_CAS_ID_DVBCA,
  /*
	qilian config id
  */
  CONFIG_CAS_ID_QL,
  /*
	shushitong config id
  */
  CONFIG_CAS_ID_DIVI,
  /*
    cas_unknow config id
  */
  CONFIG_CAS_UNKNOWN,
  /*
    config id max num
  */
  CONFIG_CAS_ID_MAX_NUM,
}cas_config_id;



#define CUSTOMER_STDID_SIZE 6

/*!
   [enum] for tool config the module enable or not
  */
typedef enum  _EXT_CUS_MODULE_CFG 
{
     EXTERNAL_MUSIC_PICTURE,       
     EXTERNAL_TTX_SUBTITLE,     
     EXTERNAL_SMALL_LIST_V2,          
     EXTERNAL_INFO_BAR_V2,    

     EXTERNAL_INSTALLTION_LIST_V2,   
     EXTERNAL_GLOBAL_VOLUME ,
     EXTERNAL_USB_DUMP_BREAK_LOG, 
     EXTERNAL_TRICK_PLAY ,
    
     EXTERNAL_UCASKEY_EDIT ,
     EXTERNAL_WATCH_DOG,    
     EXTERNAL_OTA,
     EXTERNAL_TKGS_UPDATE,  
    
     EXTERNAL_CUSTOMER_YINHE, 
     EXTERNAL_CUSTOMER_AISAT,
     EXTERNAL_CHINESE_OTA,
}EXT_CUS_MODULE_CFG;

/*!
   [struct]  for tool config system function ,include module
  */
typedef struct _EXT_CUS_SYS_CFG
{
   BOOL  customer_module_cfg;

   //for add sys config below.
}EXT_CUS_SYS_CFG;


typedef enum
{
  /*!
    CVBS (RGB) format for video out.
  */
  VDAC_CVBS_RGB = 0,
  /*!
  Signal CVBS video out, need enable dac 3 on concerto platform
  */
  VDAC_SIGN_CVBS,
    /*!
  Signal CVBS low power video out, use dac 3 for cvbs video out, need HW support
  */
  VDAC_SIGN_CVBS_LOW_POWER,
  /*!
  CVBS+CVBS format for video out.
  */
  VDAC_DUAL_CVBS,
    /*!
    SVDIEO format for  video out.
    */
  VDAC_SIGN_SVIDEO,
    /*!
   CVBS + S_VIDEO format for  video out.
    */
  VDAC_CVBS_SVIDEO,
    /*!
    YUV (SD) + CVBS format for  video out.
    */
  VDAC_CVBS_YPBPR_SD,
    /*!
    YUV (HD) + CVBS format for  video out.
    */
  VDAC_CVBS_YPBPR_HD,
    /*!
    reserved
    */
  VDAC_RESERVED1,
      /*!
    reserved
    */
  VDAC_RESERVED2,
}vdac_info_t;

typedef enum
{
  SPDIF_ON = 0,
  SPDIF_OFF,    
}spdif_info_t;

typedef enum
{
  LNB_LOAD_ON = 0,
  LNB_LOAD_OFF,    
}lnb_load_t;

typedef enum
{
  FP_GPIO = 0,
  FP_MCU
}fp_type_t;

#define MAX_LED_MAP_NUM 128
#define MAX_LANG_NUM 13

typedef struct
{
  BOOL scart_enable;
  u8 pin_tvav;
  u8 pin_tvmode;
  u8 pin_tvratio;
  BOOL sog_enable;
}scart_info_t;

typedef enum
{
     DISP_TIME,
     DISP_OFF,
     DISP_NONE
}fp_disp_t;

typedef enum
{
  //ratio 480p
  RATIO_480I_576I,
  //ratio 576p
  RATIO_480P_576P,
  //ratio 720p
  RATIO_720_P,
  //ratio 1080i
  RATIO_1080_i,
  //ratio 1080p
  RATIO_1080_P
}resolution_ratio;

typedef enum
{
  //pal
  TV_SYSTEM_PAL,
  //ntsc
  TV_SYSTEM_NTSC,
  //auto
  TV_SYSTEM_AUTO,
}tv_system;

typedef struct
{
  unsigned long addr;  
  unsigned long data; 
}reg_data_t;

typedef struct
{
  u8 num;
  reg_data_t array_data[10];  
}reg_cfg_t;

typedef struct
{
  hal_fp_type_t fp_type;
  pan_hw_info_t pan_info;
  led_bitmap_t led_bitmap[MAX_LED_MAP_NUM];
  u16 map_size;  
  vdac_info_t vdac_info;
  spdif_info_t spdif_info;
  lnb_load_t lnb_load;
  scart_info_t scart_info;
  fp_disp_t fp_disp;
  u8 standby_mode;
  u8 loop_through;
  u8 tuner_clock; //0-demo 1-other
  u8 convention;  
  u8 detect_pin_pol;
  u8 vcc_enable_pol;
  BOOL enable_mute_pin;
  u8 mute_pin;
  BOOL mute_gpio_level;
  reg_cfg_t reg;
  u8 led_num;
  u32 reserve1; 
  u32 reserve2; 
  u32 reserve3; 
}hw_cfg_t;
/*!
    dm load the block data in mem
  */
void dm_load_customer_sys_cfg(void);

/*!
    dm get  the module enable flag  in customer config mem
  */
u8 handle_dm_get_customer_module_cfg(EXT_CUS_MODULE_CFG customer_cfg);

/*!
    dm load the hw cfg block data in mem
  */
void dm_load_customer_hw_cfg(void);

/*!
   dm get the hw config info from flash
  */
hw_cfg_t dm_get_hw_cfg_info(void);

void customer_config_init(void);

u8 customer_config_language_num_get();
#endif // END __CUSTOMER_CONFIG_H_
