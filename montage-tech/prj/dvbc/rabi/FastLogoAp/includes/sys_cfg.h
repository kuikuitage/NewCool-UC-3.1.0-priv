/******************************************************************************/
/******************************************************************************/
#ifndef __SYS_CFG_H_
#define __SYS_CFG_H_


/*!
  BISS Data block_biss
  */
#define CUSTOMER_BLOCK_ID          0xAB
/*
adv logo block id
*/
#define RW_ADS_LOGO_BLOCK_ID         0xC5

#define SYS_BG_DATA_BLOCK_ID         0xC6

#ifdef CHIP
#define SYS_CPU_CLOCK                   (54 * 1000 * 1000)
#else
#define SYS_CPU_CLOCK                   (54 * 1000 * 1000)
#endif


#define CA_DATA_SIZE           (64*KBYTES)

#define P4V_CADATA_1_BLOCK_ID        0xA9

#define P4V_CADATA_2_BLOCK_ID        0xAA

#define DM_MAINCODE_BLOCK_ID         0x86
#define DM_APP_BLOCK_ID                      0x86
#define STATIC_SAVE_DATA_BLOCK_ID 0xA2
#define STATIC_SAVE_DATA_RESERVE_BLOCK_ID 0xA3

/*!
   Task priority
  */
typedef enum
{
  SYS_INIT_TASK_PRIORITY = 6,
  SYS_TIMER_TASK_PRIORITY = 7,
  ETH_NET_TASK_PRIORITY = 8,

#ifndef TEMP_SUPPORT_DS_AD
  MDL_SUBT_TASK_PRIORITY = 9,
#endif

  USB_MASS_STOR_TASK_PRIO,
  USB_BUS_TASK_PRIO,

  AP_FRM_TASK_PRIORITY,
  UI_FRM_TASK_PRIORITY, //13
  AP_DMX_TASK_PRIORITY,

  FILE_PLAYBACK_PRIORITY,
  DRV_SMC_TASK_PRIORITY,
#if defined DESAI_56_CA || defined DESAI_52_CA
  AP_NIT_TASK_PRIORITY,
  #endif
  DRV_CAS_ADAPTER_TASK_PRIORITY,
  AP_HIGHEST_TASK_PRIORITY,
  AP_NVOD_TASK_PRIORITY,
  FP_TASK_PRIORITY,
  AP_UIO_TASK_PRIORITY, //19
  AP_PLAYBACK_TASK_PRIORITY,

  // Due to jpeg animation may excuted with mp3 playback at sametime, and mp3 chain use eva thread, so we need
  // make jpeg animation thread priority higher than eva.
  JPEG_CHAIN_ANIM_TASK_PRIORITY,
  EVA_SYS_TASK_PRIORITY_START, //21
  EVA_SYS_TASK_PRIORITY_END = EVA_SYS_TASK_PRIORITY_START + 5,

#ifdef WIN32
  BLOCK_SERV_PRIORITY,
#endif
  DRV_HDMI_TASK_PRIORITY,

  AP_CA_TASK_PRIORITY,
  AP_SCAN_TASK_PRIORITY,
  AP_SIGN_MON_TASK_PRIORITY,
  AP_EPG_TASK_PRIORITY,
  AP_USB_UPG_TASK_PRIORITY,//add for usb upg
  AP_UPGRADE_TASK_PRIORITY,
  AP_OTA_TASK_PRIORITY,
  AP_TIME_TASK_PRIORITY, //
  //AP_REC_TASK_PRIORITY,
  //AP_TIMESHIFT_TASK_PRIORITY,
#ifndef TEMP_SUPPORT_DS_AD
  AP_REC_LED_TASK_PRIORITY,
  MUSIC_PLAYER_CHAIN_PRIORITY,
#endif
  MDL_CAS_TASK_PRIO_BEGIN,
  MDL_CAS_TASK_PRIO_END = MDL_CAS_TASK_PRIO_BEGIN + 8,

#ifndef TEMP_SUPPORT_DS_AD
  MDL_CAS_TASK_PRRO_QUEUE,
  MDL_VIDEO_SUBTITLE_PRIORITY,
  MDL_VBI_TASK_PRIORITY,
#endif
  MDL_DVB_TASK_PRIORITY,
  MDL_NIM_CTRL_TASK_PRIORITY,
  MDL_PNP_SVC_TASK_PRIORITY,


  NIM_NOTIFY_TASK_PRIORITY,
  DISP_HDMI_NOTIFY_TASK_PRIORITY,
  MDL_MONITOR_TASK_PRIORITY,
  MDL_DM_MONITOR_TASK_PRIORITY,  //50
#ifndef TEMP_SUPPORT_DS_AD
  BUS_MONITOR_TASK_PRIORITY,
  TRANSPORT_SHELL_TASK_PRIORITY,
  HTML_TASK_PRIO,
  USB_TASK_DUMP_PRIO,
#else
  MDL_ADS_TASK_PRIO_BEGIN,  //51
  MDL_ADS_TASK_PRIO_END = MDL_ADS_TASK_PRIO_BEGIN + 9,
  BUS_MONITOR_TASK_PRIORITY,
  TRANSPORT_SHELL_TASK_PRIORITY,
#endif
  LOWEST_TASK_PRIORITY = 128,
}all_task_priority_t;

#ifdef SHOW_MEM_SUPPORT
/*!
   Mem show flag
  */
 typedef enum
{
    AUDIO_FW_CFG_FLAG = 0,
    AV_POWER_UP_FLAG,
    VID_SD_WR_BACK_FLAG,
    VID_DI_CFG_FLAG,
    VIDEO_FW_CFG_FLAG,
    OSD0_VSCALER_FLAG,
    OSD1_VSCALER_FLAG,
    SUB_VSCALER_FLAG,
    OSD0_LAYER_FLAG,
    OSD1_LAYER_FLAG,
    SUB_LAYER_FLAG,
    REC_BUFFER_FLAG,
    EPG_BUFFER_FLAG,
    GUI_RESOURCE_BUFFER_FLAG,
    AV_STACK_FLAG,
    CODE_SIZE_FLAG,
    SYSTEM_PARTITION_FLAG = 16,

}all_mem_show_flat_t;
#endif
/*!
  Memory configuration
 */
#define SYSTEM_FLASH_END                 0x800000
#define OSD_BUFFER_ADDR                  0x1f0000

/*!
  Task stack size
 */
#define SYS_INIT_TASK_STK_SIZE           (32 * KBYTES)
#define SYS_TIMER_TASK_STK_SIZE          (4 * KBYTES)
#define SYS_CUNIT_TASK_STK_SIZE          (16 * KBYTES)

//#define VIDEO_PARTITION_SIZE             0



//#define SYS_PARTITION_SIZE       8 * 1024 * 1024
#define SYS_PARTITION_ATOM               64


#define GPIO_HW_MUTE               23
#define HW_MUTE_ON                 0
#define HW_MUTE_OFF                1
/*!
   Task stack size
  */
#define DEFAULT_STKSIZE              (16 * KBYTES)
#define SYS_INIT_TASK_STKSIZE        (32 * KBYTES)
#define SYS_TIMER_TASK_STKSIZE       (4 * KBYTES)

#define MDL_SUBT_TASK_STKSIZE        DEFAULT_STKSIZE
#define AP_FRM_TASK_STKSIZE          DEFAULT_STKSIZE
#define UI_FRM_TASK_STKSIZE          (32 * KBYTES)
#define MDL_CA_TASK_STKSIZE          DEFAULT_STKSIZE
#define AP_CA_TASK_STKSIZE           DEFAULT_STKSIZE
#define AP_TIME_TASK_STKSIZE         DEFAULT_STKSIZE
#define AP_UIO_TASK_STKSIZE          DEFAULT_STKSIZE
#define AP_PLAYBACK_TASK_STKSIZE     DEFAULT_STKSIZE
#define AP_TWIN_PORT_TASK_STKSIZE    DEFAULT_STKSIZE
#define AP_SCAN_TASK_STKSIZE         DEFAULT_STKSIZE
#define AP_MULTI_PIC_TASK_STKSIZE    DEFAULT_STKSIZE
#define AP_EPG_TASK_STKSIZE          (64 * KBYTES)
#define AP_SIGN_MON_TASK_STKSIZE     DEFAULT_STKSIZE
#define AP_USB_UPGRADE_TASK_STKSIZE  DEFAULT_STKSIZE
#define AP_UPGRADE_TASK_STKSIZE      DEFAULT_STKSIZE
#define AP_TKGS_TASK_STKSIZE  DEFAULT_STKSIZE
#define AP_SATCODX_TASK_STKSIZE      DEFAULT_STKSIZE
#define AP_OTA_TASK_STKSIZE          DEFAULT_STKSIZE
#define AP_IS_TASK_STKSIZE           DEFAULT_STKSIZE
#define AP_MONITOR_TASK_STKSIZE      DEFAULT_STKSIZE
#define AP_MUSIC_TASK_STKSIZE        DEFAULT_STKSIZE
#define AP_TS_TASK_STKSIZE           DEFAULT_STKSIZE
#define AP_JPEG_TASK_STKSIZE         DEFAULT_STKSIZE
#define AP_RECORD_TASK_STKSIZE       DEFAULT_STKSIZE
#define AP_NVOD_TASK_STKSIZE       DEFAULT_STKSIZE
#define RECORD_CHAIN_TASK_STK_SIZE   DEFAULT_STKSIZE
#define PLAY_CHAIN_TASK_STK_SIZE     DEFAULT_STKSIZE
#if defined DESAI_56_CA || defined DESAI_52_CA
#define AP_NIT_TASK_STKSIZE       DEFAULT_STKSIZE
#endif
#define MDL_VBI_TASK_STKSIZE         DEFAULT_STKSIZE
#define MDL_DVB_TASK_STKSIZE         (32 * KBYTES)
#define MDL_NIM_CTRL_TASK_STKSIZE    DEFAULT_STKSIZE
#define MDL_DM_MONITOR_TASK_STKSIZE  DEFAULT_STKSIZE
#define MDL_PNP_SVC_TASK_STKSIZE     DEFAULT_STKSIZE
#define MDL_MONITOR_TASK_STKSIZE  (10 * KBYTES)

#define DRV_HDMI_TASK_STKSIZE             (4 * KBYTES)
#define NIM_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)
#define DISP_HDMI_NOTIFY_TASK_STK_SIZE    (4 * KBYTES)
#define AUD_HDMI_NOTIFY_TASK_STK_SIZE     (4 * KBYTES)
#define DRV_SMC_TASK_STKSIZE              (4 * KBYTES)
#define DRV_USB_BUS_STKSIZE             (4 * KBYTES)
#define DRV_USB_MASSSTOR_STKSIZE             (2 * KBYTES)

#define USB_DUMP_LOG_STKSIZE         (32 * KBYTES)

#define FILE_PLAYBACK_STKSIZE       (32 * KBYTES)
/*!
   Datamanager configuration
  */
#define DM_MAX_BLOCK_NUM             (36)

#define DM_BOOTER_START_ADDR     0x60000//0xB0000
#define DM_HDR_START_ADDR_BAK  0x120000
#define DM_HDR_START_ADDR           0x120000

/*!
   Other configuration
  */
#define CHARSTO_SIZE (8 * MBYTES)
#define TELTEXT_MAX_PAGE_NUM_1    100
#define TELTEXT_MAX_PAGE_NUM_2    30

#define SLEEP_TIMER     YES


/*!
  The whole memory end, 64M.
  */
#define WHOLE_MEM_END           0xa7e00000
/*!
  av cpu init code size 20k
  */
#define AV_INIT_CODE_SIZE 0x5000

/*!
  av cpu stack size 1M-20k
  */
#define AV_STACK_SIZE (0x100000 - AV_INIT_CODE_SIZE)

/*!
  av cpu stack bottom addr, 63M
  */
#define AV_STACK_BOT_ADDR (WHOLE_MEM_END - AV_INIT_CODE_SIZE - AV_STACK_SIZE)

/*!
  av cpu init code addr, 63M + av cpu stack size
  */


/*!
  The video used size 36.1M, take ref to warriors_vdec_get_buf_requirement.
  */
//#define VIDEO_FW_CFG_SIZE           (0x2660000)//(44 * 1024 * 1024)//)0x2800000
#define VIDEO_FW_CFG_SIZE           0x2D20000   //VDEC_QAULITY_AD_128M
/*!
  The video mpeg or avs less than h.264 about 3.5M, mb info.
  */
#define VIDEO_FW_MPEG_LESS_H264     0x0000
/*!
  The video used addr
  */
#define VIDEO_FW_CFG_ADDR           (AV_STACK_BOT_ADDR - VIDEO_FW_CFG_SIZE)

/*!
  The DI used size, ~2.5M, hd require almost 2.3M
  3 luma fields:  720x288x3
  4 chroma fields(cb and cr , 4:2:0):  360x144x2x4
  2 motion fields: 360x288x2
  */
#define VID_DI_CFG_SIZE           0x2d6000

/*!
  The DI used addr.
  */
#define VID_DI_CFG_ADDR           (VIDEO_FW_CFG_ADDR - VID_DI_CFG_SIZE)

#define VBI_BUF_SIZE  0x10000
//#define VBI_BUF_SIZE  0x0000
#define VBI_BUF_ADDR  (VID_DI_CFG_ADDR - VBI_BUF_SIZE)

/*!
  The audio used size 2M, take ref to warriors_aud_get_buf_requirement.
  */
#define AUDIO_FW_CFG_SIZE           0x180000

/*!
  The audio used addr.
  */
#define AUDIO_FW_CFG_ADDR  (((VBI_BUF_ADDR & 0x1FFFFFFF) - AUDIO_FW_CFG_SIZE) | 0x80000000)


#define AP_AV_SHARE_MEM_SIZE (1024) 
#define AP_AV_SHARE_MEM_ADDR (VBI_BUF_ADDR - AUDIO_FW_CFG_SIZE - AP_AV_SHARE_MEM_SIZE)

/*!
  The video sd write back size, ~3.5M, 720*576*2*4. 422 frame mode
  */
#define VID_SD_WR_BACK_SIZE           0x4bf000
#define VID_SD_WR_BACK_FIELD_NO           0x6

/*!
  The video sd write back addr.
  */
#define VID_SD_WR_BACK_ADDR           (AP_AV_SHARE_MEM_ADDR - VID_SD_WR_BACK_SIZE)



#define EPG_BUFFER_SIZE   (0* KBYTES)// + 1M from sys partion, 2.5m from ui

//#define EPG_BUFFER_HW_ADDR   (AUDIO_FW_CFG_ADDR - EPG_BUFFER_SIZE)
//#define EPG_BUFFER_HW_ADDR   (0x84600000 - EPG_BUFFER_SIZE)
#define EPG_BUFFER_HW_ADDR   ((VID_SD_WR_BACK_ADDR & 0x8FFFFFFF) - EPG_BUFFER_SIZE)

#define EPG_BUFFER_ADDR   (EPG_BUFFER_HW_ADDR & (~0xa0000000))

/*!
  record size
  */
#define REC_BUFFER_SIZE (0* KBYTES) //+1m form ui resource, 400K to code size

/*!
  record addr
  */
//#define REC_BUFFER_HW_ADDR (AUDIO_FW_CFG_ADDR - REC_BUFFER_SIZE)
#define REC_BUFFER_HW_ADDR ((EPG_BUFFER_HW_ADDR & 0x8FFFFFFF) - REC_BUFFER_SIZE)

/*!
  record addr
  */
#define REC_BUFFER_ADDR (REC_BUFFER_HW_ADDR & (~0xa0000000))


/*!
  VIDEO_SHIFT_SIZE_WITH_FILEPALY
  */
#define VIDEO_SHIFT_SIZE_WITH_FILEPALY 0x0

/*!
  file play addr
  */
#define VIDEO_FILE_PLAY_ADDR  ((REC_BUFFER_ADDR | 0x80000000) - VIDEO_SHIFT_SIZE_WITH_FILEPALY)

/*!
  File seq module will use 30M
  */
#define VIDEO_FILE_PLAY_SIZE  (REC_BUFFER_SIZE + VIDEO_SHIFT_SIZE_WITH_FILEPALY)  // epg + rec


/*!
  malloc buffer end.
  */
#define SYSTEM_MEMORY_END                (VIDEO_FILE_PLAY_ADDR & 0x8FFFFFFF) //last buffer

/*!
   mem partition configuration
 */
#if defined(__LINUX__) || defined(__KERNEL__)

#else
extern unsigned long _end;
#define ALL_MEM_SIZE ROUNDDOWN( \
                                 (SYSTEM_MEMORY_END - ROUNDUP((u32)(&_end), 4)) \
                                 , 8)

#define ALL_MEM_HEADER (ALL_MEM_SIZE/MEM_DEFAULT_ALIGN_SIZE/8)
#endif

u32 get_flash_addr(void);
u32 get_bootload_off_addr(void);
u32 get_otaback_off_addr(void);
u32 get_maincode_off_addr(void);
#endif

