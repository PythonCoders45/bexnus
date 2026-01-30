// pic.c — PIC remapping for Bexnus

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void pic_remap(void) {
    // Command ports
    const uint8_t PIC1 = 0x20;
    const uint8_t PIC2 = 0xA0;

    // Data ports
    const uint8_t PIC1_DATA = 0x21;
    const uint8_t PIC2_DATA = 0xA1;

    // Initialization Control Word 1: start initialization
    outb(PIC1, 0x11);
    outb(PIC2, 0x11);

    // ICW2: vector offsets
    outb(PIC1_DATA, 0x20); // IRQ0–7 → interrupts 32–39
    outb(PIC2_DATA, 0x28); // IRQ8–15 → interrupts 40–47

    // ICW3: wiring between PIC1 and PIC2
    outb(PIC1_DATA, 4);    // PIC2 is on IRQ2
    outb(PIC2_DATA, 2);    // PIC2 identity

    // ICW4: 8086 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Mask all IRQs for now (we’ll unmask keyboard later)
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_unmask_irq(int irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        irq -= 8;
    }

    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
c