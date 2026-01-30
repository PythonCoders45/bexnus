// kernel.c — first C code for Bexnus
#include "vga.h" 
#include "idt.h" 
#include "pic.h" 
#include "timer.h" 
#include "keyboard.h" 
#include "paging.h" 
#include "proc.h" 
#include "syscall.h"
#include "drivers/pic.h" 
#include "drivers/timer.h" 
#include "drivers/keyboard.h" 
#include "drivers/vga.h" 
#include "drivers/pci.h" 
#include "drivers/net.h"

// Write a character to VGA text mode memory
static void putc(int row, int col, char c) {
    unsigned char *video = (unsigned char*)0xB8000;
    int index = (row * 80 + col) * 2;
    video[index] = c;
    video[index + 1] = 0x07; // light grey on black
}

void kmain(void) {
    clear_screen();

    paging_init();
    idt_init();
    pic_remap();
    tss_ini(0x9FFFF0);
    sched_init();
    syscall_init();

    timer_init();
    pic_unmask_irq(0);
    keybored_init();
    pic_unmask_irq(1);

    puts("Bexnus up. \n")

    extern void task_a(void);
    extern void task_b(void);
    proc_spawn(task_a);
    proc_spawn(task_b);
    
    net_init();

    for (;;)
        __asm__ volatile ("hlt");
}