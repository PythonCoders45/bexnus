#ifndef BEXNUS_TSS_H
#define BEXNUS_TSS_H

#include <stdint.h>

void tss_init(uint32_t kernel_stack_top);

#endif
