#include "timer.h"
#include "io.h"
#include "pic.h"
#include "../proc/proc.h"

volatile uint32_t ticks = 0;

void timer_handler(void) {
    ticks++;

    if (ticks % 10 == 0)
        sched_tick();

    pic_send_eoi(0);
}

void timer_init(void) {
    uint32_t divisor = 1193180 / 100; // 100 Hz

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}
