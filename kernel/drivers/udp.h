#ifndef BEXNUS_UDP_H
#define BEXNUS_UDP_H

#include <stdint.h>

struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed));

typedef void (*udp_handler_t)(uint32_t src_ip, uint16_t src_port,
                              const uint8_t *data, int len);

void udp_init(void);
int  udp_send(uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
              const uint8_t *data, int len);
void udp_handle(const uint8_t *ip_packet, int len);

void udp_set_handler(uint16_t port, udp_handler_t handler);

#endif
