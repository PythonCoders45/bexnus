#ifndef BEXNUS_IDT_H
#define BEXNUS_IDT_H

#include <stdint.h>

void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base);

#endif
