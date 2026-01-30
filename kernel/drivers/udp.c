#include "udp.h"
#include "ipv4.h"
#include "../drivers/vga.h"
#include <string.h>

#define MAX_UDP_HANDLERS 16

struct udp_binding {
    uint16_t port;
    udp_handler_t handler;
};

static struct udp_binding handlers[MAX_UDP_HANDLERS];

static uint16_t udp_checksum(uint32_t src_ip, uint32_t dst_ip,
                             const uint8_t *udp, int len) {
    uint32_t acc = 0;

    acc += (src_ip >> 16) & 0xFFFF;
    acc += (src_ip      ) & 0xFFFF;
    acc += (dst_ip >> 16) & 0xFFFF;
    acc += (dst_ip      ) & 0xFFFF;
    acc += 0x0011; // protocol UDP
    acc += len;

    for (int i = 0; i + 1 < len; i += 2) {
        uint16_t word;
        memcpy(&word, udp + i, 2);
        acc += word;
        if (acc > 0xFFFF) acc -= 0xFFFF;
    }

    if (len & 1) {
        uint16_t word = udp[len - 1] << 8;
        acc += word;
        if (acc > 0xFFFF) acc -= 0xFFFF;
    }

    return ~acc;
}

void udp_init(void) {
    memset(handlers, 0, sizeof(handlers));
}

void udp_set_handler(uint16_t port, udp_handler_t handler) {
    for (int i = 0; i < MAX_UDP_HANDLERS; i++) {
        if (handlers[i].port == 0 || handlers[i].port == port) {
            handlers[i].port = port;
            handlers[i].handler = handler;
            return;
        }
    }
}

int udp_send(uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
             const uint8_t *data, int len) {
    uint8_t buf[1500];
    struct udp_header *udp = (struct udp_header*)buf;

    udp->src_port = (src_port >> 8) | (src_port << 8);
    udp->dst_port = (dst_port >> 8) | (dst_port << 8);
    udp->length   = ((len + sizeof(struct udp_header)) >> 8) |
                    ((len + sizeof(struct udp_header)) << 8);
    udp->checksum = 0;

    memcpy(buf + sizeof(struct udp_header), data, len);

    int total_len = sizeof(struct udp_header) + len;
    udp->checksum = udp_checksum(ipv4_get_ip(), dst_ip, buf, total_len);

    return ipv4_send(dst_ip, IP_PROTO_UDP, buf, total_len);
}

void udp_handle(const uint8_t *ip_packet, int len) {
    const struct ipv4_header *ip = (const struct ipv4_header*)ip_packet;
    const uint8_t *udp_start = ip_packet + sizeof(struct ipv4_header);
    int udp_len = len - sizeof(struct ipv4_header);

    if (udp_len < (int)sizeof(struct udp_header)) return;

    const struct udp_header *udp = (const struct udp_header*)udp_start;
    uint16_t dst_port = (udp->dst_port >> 8) | (udp->dst_port << 8);
    uint16_t src_port = (udp->src_port >> 8) | (udp->src_port << 8);

    const uint8_t *payload = udp_start + sizeof(struct udp_header);
    int payload_len = udp_len - sizeof(struct udp_header);

    for (int i = 0; i < MAX_UDP_HANDLERS; i++) {
        if (handlers[i].port == dst_port && handlers[i].handler) {
            handlers[i].handler(ip->src, src_port, payload, payload_len);
            return;
        }
    }
}
