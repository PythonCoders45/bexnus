// timer.c — PIT timer driver for Bexnus

#include <stdint.h>
#include "proc.h"

extern void idt_set_gate(int n, uint32_t handler);
extern void putc(char c);

static volatile uint32_t ticks = 0;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void timer_handler(void) { 
    ticks++; 

    if (ticks % 10 == 0) { 
        sched_tick(); 
    } 
}
void timer_init(void) {
    // PIT runs at 1,193,180 Hz
    uint32_t divisor = 1193180 / 100; // 100 Hz

    // Command byte: channel 0, low+high byte, mode 3 (square wave)
    outb(0x43, 0x36);

    // Send divisor
    outb(0x40, divisor & 0xFF);        // low byte
    outb(0x40, (divisor >> 8) & 0xFF); // high byte

    // Install IRQ0 handler (interrupt 32)
    idt_set_gate(32, (uint32_t)timer_handler);
}

void sleep(uint32_t ms) {
    uint32_t target = ticks + (ms / 10);
    while (ticks < target) {
        __asm__("hlt");
    }
}
