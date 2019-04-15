[bits 16]
[org 0x7c00]
KERNEL_OFFSET equ 0x1000

    mov [BOOT_DRIVE], dl
    mov bp, 0x9000
    mov sp, bp

    mov bx, MSG_REAL_MODE
    call print
    call print_nl

    call load_kernel
    call switch_to_pm
    jmp $

jmp $

%include 'boot/print.asm'
%include 'boot/print_hex.asm'
%include 'boot/disk.asm'
%include 'boot/switch_pm.asm'
%include 'boot/gdt.asm'
%include 'boot/32bit_print.asm'

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print
    call print_nl

    mov bx, KERNEL_OFFSET
    mov dh, 16
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_PM: ; after the switch we will get here
    mov ebx, MSG_PROT_MODE
    call print_string_pm ; Note that this will be written at the top left corner
    call KERNEL_OFFSET
    jmp $

BOOT_DRIVE: db 0
MSG_REAL_MODE: db 'Started in 16 bit real mode', 0
MSG_PROT_MODE: db 'Switched to 32 bit protected mode', 0
MSG_LOAD_KERNEL: db "Loading kernel into memory", 0

times 510 - ($-$$) db 0
dw 0xaa55 
