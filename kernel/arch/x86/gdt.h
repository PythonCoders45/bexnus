#ifndef BEXNUS_GDT_H
#define BEXNUS_GDT_H

#include <stdint.h>

void gdt_init(void);

#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

#endif
