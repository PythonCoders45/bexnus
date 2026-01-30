#include "tcp.h"
#include "ipv4.h"
#include "../drivers/vga.h"
#include <string.h>

static uint8_t recv_buffer[4096];
static int recv_len = 0;

static uint16_t tcp_checksum(uint32_t src_ip, uint32_t dst_ip,
                             const uint8_t *tcp, int len) {
    uint32_t acc = 0;

    acc += (src_ip >> 16) & 0xFFFF;
    acc += (src_ip      ) & 0xFFFF;
    acc += (dst_ip >> 16) & 0xFFFF;
    acc += (dst_ip      ) & 0xFFFF;
    acc += 0x0006; // TCP protocol
    acc += len;

    for (int i = 0; i + 1 < len; i += 2) {
        uint16_t word = (tcp[i] << 8) | tcp[i+1];
        acc += word;
        if (acc > 0xFFFF) acc -= 0xFFFF;
    }

    if (len & 1) {
        uint16_t word = tcp[len - 1] << 8;
        acc += word;
        if (acc > 0xFFFF) acc -= 0xFFFF;
    }

    return ~acc;
}

void tcp_init(void) {
    recv_len = 0;
}

int tcp_recv(uint8_t *buf, int max) {
    int n = (recv_len < max) ? recv_len : max;
    memcpy(buf, recv_buffer, n);
    recv_len = 0;
    return n;
}

static int tcp_send_raw(uint32_t dst_ip,
                        uint16_t src_port, uint16_t dst_port,
                        uint32_t seq, uint32_t ack,
                        uint8_t flags,
                        const uint8_t *data, int len) {

    uint8_t buf[1500];
    struct tcp_header *tcp = (struct tcp_header*)buf;

    tcp->src_port = (src_port >> 8) | (src_port << 8);
    tcp->dst_port = (dst_port >> 8) | (dst_port << 8);
    tcp->seq      = __builtin_bswap32(seq);
    tcp->ack      = __builtin_bswap32(ack);
    tcp->offset_reserved = (5 << 4);
    tcp->flags = flags;
    tcp->window = __builtin_bswap16(1024);
    tcp->urgent = 0;
    tcp->checksum = 0;

    memcpy(buf + sizeof(struct tcp_header), data, len);

    int total_len = sizeof(struct tcp_header) + len;
    tcp->checksum = tcp_checksum(ipv4_get_ip(), dst_ip, buf, total_len);

    return ipv4_send(dst_ip, IP_PROTO_TCP, buf, total_len);
}

int tcp_connect(uint32_t dst_ip, uint16_t dst_port,
                uint16_t *local_port_out,
                uint32_t *seq_out, uint32_t *ack_out) {

    static uint16_t next_port = 40000;
    uint16_t src_port = next_port++;

    uint32_t seq = 0x1000;
    uint32_t ack = 0;

    // Send SYN
    tcp_send_raw(dst_ip, src_port, dst_port, seq, 0, TCP_FLAG_SYN, 0, 0);

    // Wait for SYN-ACK
    for (int i = 0; i < 2000000; i++) {
        extern void eth_poll(void);
        eth_poll();

        if (recv_len > 0) {
            struct tcp_header *tcp = (struct tcp_header*)recv_buffer;
            uint8_t flags = tcp->flags;

            if ((flags & TCP_FLAG_SYN) && (flags & TCP_FLAG_ACK)) {
                ack = __builtin_bswap32(tcp->seq) + 1;
                break;
            }
        }
    }

    if (ack == 0) {
        puts("tcp: SYN-ACK timeout\n");
        return -1;
    }

    // Send ACK
    tcp_send_raw(dst_ip, src_port, dst_port, seq + 1, ack, TCP_FLAG_ACK, 0, 0);

    *local_port_out = src_port;
    *seq_out = seq + 1;
    *ack_out = ack;

    return 0;
}

int tcp_send(uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
             uint32_t *seq_inout, uint32_t ack,
             const uint8_t *data, int len) {

    tcp_send_raw(dst_ip, src_port, dst_port,
                 *seq_inout, ack,
                 TCP_FLAG_ACK | TCP_FLAG_PSH,
                 data, len);

    *seq_inout += len;
    return 0;
}

void tcp_handle(const uint8_t *ip_packet, int len) {
    const struct ipv4_header *ip = (const struct ipv4_header*)ip_packet;
    const uint8_t *tcp_start = ip_packet + sizeof(struct ipv4_header);
    int tcp_len = len - sizeof(struct ipv4_header);

    if (tcp_len < (int)sizeof(struct tcp_header)) return;

    const struct tcp_header *tcp = (const struct tcp_header*)tcp_start;
    int header_len = (tcp->offset_reserved >> 4) * 4;

    const uint8_t *payload = tcp_start + header_len;
    int payload_len = tcp_len - header_len;

    if (payload_len > 0) {
        memcpy(recv_buffer, payload, payload_len);
        recv_len = payload_len;
    }
}
