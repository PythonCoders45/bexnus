#ifndef BEXNUS_IPV4_H
#define BEXNUS_IPV4_H

#include <stdint.h>

struct ipv4_header {
    uint8_t  ver_ihl;
    uint8_t  tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_frag;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src;
    uint32_t dst;
} __attribute__((packed));

#define IP_PROTO_ICMP 1
#define IP_PROTO_UDP  17
#define IP_PROTO_TCP  6

void ipv4_init(uint32_t our_ip);
int  ipv4_send(uint32_t dst_ip, uint8_t proto, const uint8_t *payload, int len);
void ipv4_handle(const uint8_t *frame, int len);

uint32_t ipv4_get_ip(void);

#endif
