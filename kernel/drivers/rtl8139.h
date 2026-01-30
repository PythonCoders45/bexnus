#ifndef BEXNUS_RTL8139_H
#define BEXNUS_RTL8139_H

#include <stdint.h>

void rtl8139_init(void);
int  rtl8139_send(const uint8_t *data, int len);
int  rtl8139_poll(uint8_t *buf, int max_len); // non-blocking receive

#endif
