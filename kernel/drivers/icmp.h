#ifndef BEXNUS_ICMP_H
#define BEXNUS_ICMP_H

#include <stdint.h>

void icmp_handle(const uint8_t *ip_packet, int len);
int  icmp_ping(uint32_t dst_ip, int count);

#endif
