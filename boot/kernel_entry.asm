[bits 32]
[extern kernelMain] ; Define calling point. Must have same name as kernel.c 'main' function
call kernelMain ; Calls the C function. The linker will know where it is placed in memory
jmp $