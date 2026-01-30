#include "tss.h"
#include "gdt.h"
#include <string.h>

struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap, iomap_base;
} __attribute__((packed));

static struct tss_entry tss;

extern void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

void tss_init(uint32_t kernel_stack_top) {
    uint32_t base  = (uint32_t)&tss;
    uint32_t limit = base + sizeof(tss);

    gdt_set_gate(5, base, limit, 0x89, 0x40); // TSS descriptor

    memset(&tss, 0, sizeof(tss));
    tss.ss0  = GDT_KERNEL_DATA;
    tss.esp0 = kernel_stack_top;

    __asm__ volatile ("ltr %%ax" : : "a"(5 << 3));
}
