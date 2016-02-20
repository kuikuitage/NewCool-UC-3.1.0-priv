/********************************************************************************************/
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "source_pin.h"
#include "transf_output_pin.h"
#include "mplayer_aud_lrc_output_pin_intra.h"
#include "err_check_def.h"

static mplayer_aud_lrc_out_pin_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((mplayer_aud_lrc_out_pin_t *)_this)->private_data;
}

static RET_CODE mplayer_aud_lrc_out_pin_decide_buffer_size(handle_t _this)
{
  mplayer_aud_lrc_out_pin_private_t *p_priv = get_priv(_this);
  imem_allocator_t *p_alloc = p_priv->p_alloc;
  allocator_properties_t properties;
  
  //config mem_alloc
  p_alloc->get_properties(p_alloc, &properties);
  properties.buffers = 4;
  properties.buffer_size = 4;
  p_alloc->set_properties(p_alloc, &properties, NULL);
  return SUCCESS;
}

mplayer_aud_lrc_out_pin_t * mplayer_aud_lrc_out_pin_create(mplayer_aud_lrc_out_pin_t *p_pin, 
                                                                interface_t *p_owner)
{
  mplayer_aud_lrc_out_pin_private_t *p_priv = NULL;
  ipin_t *p_ipin = NULL;
  base_output_pin_t *p_output_pin = NULL;
  media_format_t media_format = {MT_AUDIO, MF_LRC};
  transf_output_pin_para_t transf_pin_para = {0};

  //check input parameter
  CHECK_FAIL_RET_NULL(p_pin != NULL);
  p_ipin = (ipin_t *)p_pin;

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "mp_a_lrc_o_pin";
  transf_output_pin_create(&p_pin->m_pin, &transf_pin_para);

  //init private date
  p_priv = &(p_pin->private_data);
  memset(p_priv, 0x0, sizeof(mplayer_aud_lrc_out_pin_private_t));
  p_priv->p_this = p_pin;
  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);
  p_ipin->add_supported_media_format(p_ipin, &media_format);

  //over load virtual function
  p_output_pin = (base_output_pin_t *)p_pin;
  p_output_pin->decide_buffer_size = mplayer_aud_lrc_out_pin_decide_buffer_size;
  
  return p_pin;
}

