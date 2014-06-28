/*
* UAE - The Un*x Amiga Emulator
*
* memory management
*
* Copyright 1995 Bernd Schmidt
*/

#ifndef MEMORY_H
#define MEMORY_H

#include "core_internal.h"


STATIC_INLINE uae_u32 get_long (uaecptr addr)
{
	return uaecore11::callbacks->get_long(addr);
}
STATIC_INLINE uae_u32 get_word (uaecptr addr)
{
	return uaecore11::callbacks->get_word(addr);
}
STATIC_INLINE uae_u32 get_byte (uaecptr addr)
{
	return uaecore11::callbacks->get_byte(addr);
}
STATIC_INLINE uae_u32 get_longi(uaecptr addr)
{
	return uaecore11::callbacks->get_longi(addr);
}
STATIC_INLINE uae_u32 get_wordi(uaecptr addr)
{
	return uaecore11::callbacks->get_wordi(addr);
}

STATIC_INLINE void put_long (uaecptr addr, uae_u32 l)
{
    uaecore11::callbacks->put_long(addr, l);
}
STATIC_INLINE void put_word (uaecptr addr, uae_u32 w)
{
    uaecore11::callbacks->put_word(addr, w);
}
STATIC_INLINE void put_byte (uaecptr addr, uae_u32 b)
{
    uaecore11::callbacks->put_byte(addr, b);
}

STATIC_INLINE uae_u8 *get_real_address (uaecptr addr)
{
	return 0;
}
#endif /* MEMORY_H */
