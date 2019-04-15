#include "../cpu/isr.h"
#include "../libc/function.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../drivers/ata.h"

void kernelMain() {
    clear_screen();

    isr_install();
    irq_install();

    u16 ata1 = 0x1F0;
    //u16 ata2 = 0x170;

    kprint("Primary master: ");
    identify(ata1, 1);
    kprint("\n");

    kprint("Primary slave: ");
    identify(ata1, 0);
    kprint("\n");

    kprint("Loaded the OS\n"
        "Type end to halt the CPU\n> ");
}

void user_input(char *input) {
    if (strcmp(input, "end") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp(input, "read") == 0) {
        read(0x1F0, 0, 0, 21);
    } else if (strcmp(input, "write") == 0) {
        write(0x1F0, 0, 0, (u8*)"http://www.google.com", 21);
        flush(0x1F0, 0);
    }
    kprint("\n> ");
}