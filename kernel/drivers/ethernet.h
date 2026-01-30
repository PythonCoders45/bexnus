#ifndef BEXNUS_ETHERNET_H
#define BEXNUS_ETHERNET_H

#include <stdint.h>

#define ETH_TYPE_ARP  0x0806
#define ETH_TYPE_IPv4 0x0800

struct eth_frame {
    uint8_t  dst[6];
    uint8_t  src[6];
    uint16_t type;
    uint8_t  payload[];
} __attribute__((packed));

void eth_set_mac(const uint8_t mac[6]);
void eth_set_gateway_mac(const uint8_t mac[6]); // optional for later
void eth_set_ip(uint32_t ip);

int  eth_send(uint8_t dst_mac[6], uint16_t type, const uint8_t *payload, int len);
void eth_poll(void);

#endif
