#ifndef BEXNUS_PIC_H
#define BEXNUS_PIC_H

#include <stdint.h>

void pic_remap(void);
void pic_send_eoi(int irq);
void pic_unmask_irq(int irq);

#endif
