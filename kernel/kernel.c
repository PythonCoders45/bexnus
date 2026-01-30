// kernel/kernel.c — Bexnus main kernel entry

#include <stdint.h>

// arch
#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"
#include "arch/x86/tss.h"
#include "arch/x86/paging.h"

// drivers
#include "drivers/vga.h"
#include "drivers/pic.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"

// proc
#include "proc/proc.h"
#include "proc/syscall.h"

// fs
#include "fs/vfs.h"
#include "fs/tmpfs.h"
#include "fs/fat12.h"
#include "fs/ext2.h"

// net
#include "drivers/net.h"
#include "drivers/ethernet.h"

// shell
#include "shell.h"

static void init_arch(void) {
    gdt_init();
    idt_init();
    pic_remap();
    paging_init();

    // kernel stack top (adjust to your linker script)
    tss_init(0x009FF000);
}

static void init_drivers(void) {
    clear_screen();
    puts("Bexnus booting...\n");

    timer_init();
    keyboard_init();

    // unmask IRQ0 (timer) and IRQ1 (keyboard)
    pic_unmask_irq(0);
    pic_unmask_irq(1);
}

static void init_proc(void) {
    sched_init();
    syscall_init();
}

static void init_fs(void) {
    vfs_init();
    tmpfs_init();

    // if you have FAT12/ext2 ready, mount them here, e.g.:
    // fat12_init();
    // ext2_init();
    // vfs_mount_fat12("/fat", ...);
    // vfs_mount_ext2("/ext2", ...);
}

static void init_net(void) {
    net_init();
}

void kmain(void) {
    init_arch();
    init_drivers();
    init_proc();
    init_fs();
    init_net();

    puts("Bexnus ready.\n");

    // main loop: poll network + run shell
    while (1) {
        eth_poll();      // handle incoming packets
        shell_run();     // your interactive shell
        __asm__ volatile ("hlt");
    }
}
