#ifndef BEXNUS_ARP_H
#define BEXNUS_ARP_H

#include <stdint.h>

void arp_init(uint32_t our_ip);
void arp_handle(const uint8_t *frame, int len);
int  arp_resolve(uint32_t ip, uint8_t mac_out[6]); // blocking-ish for now

#endif
