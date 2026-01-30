#ifndef BEXNUS_NET_H
#define BEXNUS_NET_H

#include <stdint.h>

void net_init(void);
int  net_send_raw(const uint8_t *data, int len);
int  net_poll_raw(uint8_t *buf, int max_len);

#endif
