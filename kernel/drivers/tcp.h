#ifndef BEXNUS_TCP_H
#define BEXNUS_TCP_H

#include <stdint.h>

#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10

struct tcp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t  offset_reserved;
    uint8_t  flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent;
} __attribute__((packed));

void tcp_init(void);

// Minimal TCP connect
int tcp_connect(uint32_t dst_ip, uint16_t dst_port,
                uint16_t *local_port_out,
                uint32_t *seq_out, uint32_t *ack_out);

// Send data after handshake
int tcp_send(uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
             uint32_t *seq_inout, uint32_t ack,
             const uint8_t *data, int len);

// Poll for incoming TCP packets
void tcp_handle(const uint8_t *ip_packet, int len);

// Receive buffer (simple)
int tcp_recv(uint8_t *buf, int max);

#endif
