#ifndef MEM_H
#define MEM_H

#include "../cpu/types.h"

void memory_copy(u8 *source, u8 *dest, int nbytes);
void memory_set(u8 *dest, u8 val, u32 len);
void memory_copy_32(u32 *source, u32 *dest, int nbytes);
void memory_set_32(u32 *dest, u32 val, u32 len);
u32 kmalloc(u32 size, int align, u32 *phys_addr);

#endif