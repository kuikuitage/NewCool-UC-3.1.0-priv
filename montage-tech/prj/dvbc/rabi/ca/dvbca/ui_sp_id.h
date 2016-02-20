#ifndef __UI_DVBCA_SP_ID_H__
#define __UI_DVBCA_SP_ID_H__

/*cont*/
#define DVB_CA_CONT_X          0
#define DVB_CA_CONT_Y          ((SCREEN_HEIGHT - CA_CONT_H) / 2)
#define DVB_CA_CONT_W          SCREEN_WIDTH
#define DVB_CA_CONT_H          SCREEN_HEIGHT

//TILTLE
#define DVB_SP_ID_TITLE_X       WHOLE_SCR_TITLE_X
#define DVB_SP_ID_TITLE_Y       WHOLE_SCR_TITLE_Y
#define DVB_SP_ID_TITLE_W      WHOLE_SCR_TITLE_W
#define DVB_SP_ID_TITLE_H       35

//plist
#define DVB_CA_PLIST_BG_X  DVB_SP_ID_TITLE_X
#define DVB_CA_PLIST_BG_Y  (RIGHT_BG_Y + DVB_CA_MBOX_H)
#define DVB_CA_PLIST_BG_W  DVB_SP_ID_TITLE_W
#define DVB_CA_PLIST_BG_H  (RIGHT_BG_H - DVB_CA_MBOX_H)

//mbox
#define DVB_CA_MBOX_X           DVB_SP_ID_TITLE_X
#define DVB_CA_MBOX_Y           RIGHT_BG_Y
#define DVB_CA_MBOX_W          DVB_SP_ID_TITLE_W
#define DVB_CA_MBOX_H           40
#define DVB_CA_MBOX_MID_X   20
#define DVB_CA_MBOX_MID_W  (DVB_CA_MBOX_W - 100)
#define DVB_CA_MBOX_MID_H   DVB_CA_MBOX_H

//plist rect
#define DVB_CA_LIST_BG_MIDL         4
#define DVB_CA_LIST_BG_MIDT         4//10
#define DVB_CA_LIST_BG_MIDW        (DVB_CA_PLIST_BG_W-DVB_CA_LIST_BG_MIDL*2)//(CA_PLIST_BG_W-20)
#define DVB_CA_LIST_BG_MIDH        (DVB_CA_PLIST_BG_H-DVB_CA_LIST_BG_MIDT*2)//(CA_PLIST_BG_H-20)
#define DVB_CA_LIST_BG_VGAP         3///2
//other
#define DVB_CA_INFO_LIST_PAGE         11
#define DVB_CA_INFO_ENTITLE_FIELD        2

#define DVB_CA_ITME_COUNT           2
#define OPERATOR_NAME_MAX_LENGTH 	80


u16 my_sp_id;


/* rect style */
RET_CODE open_dvb_ca_sp_id_list(u32 para1,u32 para2);
RET_CODE open_dvb_ca_sp_id_list2(u32 para1,u32 para2);
void sp_id_name_get_unistr(u16 * uni_str, u8 * str_addr);
void set_update_sp_flag(BOOL flag);
BOOL get_update_sp_flag();

#endif

