// idt.c — Interrupt Descriptor Table for Bexnus

#include <stdint.h>

#define IDT_SIZE 256

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtp;

extern void idt_load(void); // implemented in assembly

void idt_set_gate(int n, uint32_t handler) {
    idt[n].base_low  = handler & 0xFFFF;
    idt[n].sel       = 0x08;        // kernel code segment
    idt[n].always0   = 0;
    idt[n].flags     = 0x8E;        // present, ring 0, 32-bit interrupt gate
    idt[n].base_high = (handler >> 16) & 0xFFFF;
}

void idt_init(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    // Clear IDT
    for (int i = 0; i < IDT_SIZE; i++) {
        idt[i].base_low  = 0;
        idt[i].sel       = 0;
        idt[i].always0   = 0;
        idt[i].flags     = 0;
        idt[i].base_high = 0;
    }

    idt_load(); // load IDT with lidt
}
