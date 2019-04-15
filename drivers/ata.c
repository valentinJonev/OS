#include "ata.h"
#include "../cpu/ports.h"
#include "../cpu/types.h"
#include "screen.h"
#include "../libc/string.h"

void identify(u16 base_register, int master) {
    u8 drive_select = master ? 0xA0 : 0xB0;
    port_byte_out(base_register + 0x6, drive_select); // device port
    port_byte_out(base_register + 0x206, 0); // controll port

    port_byte_out(base_register + 0x6, 0xA0); // device port
    u8 status = port_byte_in(base_register + 0x7); // command
    if(status == 0xFF) {
        kprint("Error");
        return;
    }

    port_byte_out(base_register + 0x6, drive_select); // device port
    port_byte_out(base_register + 0x2, 0x00); // sector count
    port_byte_out(base_register + 0x3, 0x00); // lbaLow
    port_byte_out(base_register + 0x4, 0x00); // lbaMid
    port_byte_out(base_register + 0x5, 0x00); // lbaHi
    port_byte_out(base_register + 0x7, 0xEC); // command

    status = port_byte_in(base_register + 0x7); // command
    if(status == 0x00) {
        u8 error = port_byte_in(base_register + 0x1); // error
        kprint("ERROR: ");
        char error_str[16] = "";
        hex_to_ascii(error, error_str);
        kprint(error_str);
        kprint("\n");
        kprint("Drive does not exist");
        kprint("\n");
        return;
    }

    while(((status & 0x80) == 0x80)
        && ((status & 0x01) != 0x01))
            status = port_byte_in(base_register + 0x7); // command

    if(status & 0x01) {
        u8 error = port_byte_in(base_register + 0x1); // error
        kprint("ERROR: ");
        char error_str[16] = "";
        hex_to_ascii(error, error_str);
        kprint(error_str);
        return;
    }

    kprint("Drive is OK!");

    int i = 0;
    for(; i < 256; i++)
    {
        u16 data = port_byte_in(base_register); // data
        char *text = "  \0";
        text[1] = (data >> 8) & 0x00FF;
        text[0] = data & 0x00FF;
        kprint(text);
    }

    kprint("\n");
}

void read(u16 base_register, int master, u32 sector, int count) {
    if(sector > 0x0FFFFFFF)
        return;

    port_byte_out(base_register + 0x6, (master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24)); // Write to device port 0x1F6
    port_byte_out(base_register + 0x2, 1);
    
    port_byte_out(base_register + 0x3, sector & 0x000000FF); // 0x1F3
    port_byte_out(base_register + 0x4, (sector & 0x0000FF00) >> 8); // 0x1F4
    port_byte_out(base_register + 0x5, (sector & 0x00FF0000) >> 16); // 0x1F5
    port_byte_out(base_register + 0x7, 0x20); // 0x1F7

    u8 status = port_byte_in(base_register + 0x7); // 0x1F7
    while(((status & 0x80) == 0x80)
        && ((status & 0x01) != 0x01))
            status = port_byte_in(base_register + 0x7); // 0x1F7

    if(status & 0x01) {
        u8 error = port_byte_in(base_register + 0x1); // 0x1F1
        kprint("ERROR: ");
        char error_str[16] = "";
        hex_to_ascii(error, error_str);
        kprint(error_str);
        return;
    }

    kprint("Reading from ATA: ");

    int i = 0;
    for(; i < count; i += 2)
    {
        u16 wdata = port_word_in(base_register);

        char *text = "  \0";
        text[0] = wdata & 0xFF;
        if(i+1 < count)
            text[1] = (wdata >> 8) & 0xFF;
        else
            text[1] = '\0';
        kprint(text);
    }

    for(i = count + (count%2); i < 512; i += 2)
        port_word_in(base_register);

    kprint("\n");
}

void write(u16 base_register, int master, u32 sector, u8* data, int count) {
    if(sector > 0x0FFFFFFF)
        return;
    if(count > 512)
        return;

    port_byte_out(base_register + 0x6, (master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24)); // Write to device port 0x1F6
    port_byte_out(base_register + 0x2, 0x1);
    
    port_byte_out(base_register + 0x3, sector & 0x000000FF); // 0x1F3
    port_byte_out(base_register + 0x4, (sector & 0x0000FF00) >> 8); // 0x1F4
    port_byte_out(base_register + 0x5, (sector & 0x00FF0000) >> 16); // 0x1F5
    port_byte_out(base_register + 0x7, 0x30); // 0x1F7

    kprint("Writing to ATA: ");

    int i = 0;
    for(; i < count; i += 2)
    {
        u16 wdata = data[i];
        if(i+1 < count)
            wdata |= ((u16)data[i+1]) << 8;
        
        char *text = "  \0";
        text[1] = (wdata >> 8);
        text[0] = wdata;
        kprint(text);

        port_word_out(base_register, wdata);
    }

    for(i = count + (count%2); i < 512; i += 2){
        port_word_out(base_register, 0x0000);
    }

    kprint("\n");
}

void flush(u16 base_register, int master) {
    port_byte_out(base_register + 0x6, master ? 0xE0 : 0xF0); // Write to device port 0x1F6
    port_byte_out(base_register + 0x7, 0xE7);

    u8 status = port_byte_in(base_register + 0x7);
    while(((status & 0x80) == 0x80)
       && ((status & 0x01) != 0x01))
        status = port_byte_in(base_register + 0x7);
        
    if(status & 0x01)
    {
        kprint("ERROR");
        return;
    }

    u8 error = port_byte_in(base_register + 0x1); // 0x1F1
    if(error > 0x0) {
        kprint("ERROR: ");
        char error_str[16] = "";
        hex_to_ascii(error, error_str);
        kprint(error_str);

        kprint("\n");
    }

    kprint("Flushed! \n");
}
