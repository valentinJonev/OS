#ifndef ATA_H
#define ATA_H

#include "../cpu/types.h"

/* Public kernel API */
void identify(u16 base_register, int master);
void read(u16 base_register, int master, u32 sector, int count);
void write(u16 base_register, int master, u32 sector, u8* data, int count);
void flush(u16 base_register, int master);

#endif