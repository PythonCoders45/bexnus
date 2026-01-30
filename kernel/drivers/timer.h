#ifndef BEXNUS_TIMER_H
#define BEXNUS_TIMER_H

#include <stdint.h>

void timer_init(void);
void timer_handler(void);

extern volatile uint32_t ticks;

#endif
