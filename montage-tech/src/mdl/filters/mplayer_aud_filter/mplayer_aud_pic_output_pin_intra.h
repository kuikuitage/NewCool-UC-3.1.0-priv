/********************************************************************************************/
/********************************************************************************************/
#ifndef __MPLAYER_AUD_PIC_OUT_PIN_INTRA_H_
#define __MPLAYER_AUD_PIC_OUT_PIN_INTRA_H_

/*!
  private data length
  */
typedef struct tag_mplayer_aud_pic_out_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    mem alloc interface
    */
  imem_allocator_t *p_alloc;
}mplayer_aud_pic_out_pin_private_t;

/*!
  the mp3_trans output pin define
  */
typedef struct tag_mplayer_aud_pic_out_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_output_pin_t m_pin;
  /*!
    private data
    */
  mplayer_aud_pic_out_pin_private_t private_data;
}mplayer_aud_pic_out_pin_t;

/*!
  create a pin

  \return return the instance of usb_pin_t
  */
mplayer_aud_pic_out_pin_t * mplayer_aud_pic_out_pin_create(
                          mplayer_aud_pic_out_pin_t *p_pin, interface_t *p_owner);



#endif // End for __MPLAYER_AUD_PIC_OUT_PIN_INTRA_H_

