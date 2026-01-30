; idt_load.asm — loads the IDT pointer
; nasm -f elf32 idt_load.asm -o idt_load.o

global idt_load

idt_load:
    lidt [idtp]     ; idtp is provided by idt.c
    ret
