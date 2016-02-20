#ifndef __STRING_POOL_H__
#define __STRING_POOL_H__


RET_CODE string_pool_init(u32* p_hdl);

RET_CODE string_pool_destroy(u32 hdl);

u8* str_dup(u8* p_url, u32 hdl);

#endif
