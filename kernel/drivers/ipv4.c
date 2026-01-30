#include "ipv4.h"
#include "ethernet.h"
#include "arp.h"
#include "../drivers/vga.h"
#include <string.h>
#include "udp.h"


static uint32_t our_ip = 0;

static uint16_t ip_checksum(const void *vdata, size_t length) {
    const uint8_t *data = vdata;
    uint32_t acc = 0xffff;

    for (size_t i = 0; i + 1 < length; i += 2) {
        uint16_t word;
        memcpy(&word, data + i, 2);
        acc += word;
        if (acc > 0xffff)
            acc -= 0xffff;
    }

    if (length & 1) {
        uint16_t word = 0;
        memcpy(&word, data + length - 1, 1);
        acc += word;
        if (acc > 0xffff)
            acc -= 0xffff;
    }

    return ~acc;
}

void ipv4_init(uint32_t ip) {
    our_ip = ip;
}

uint32_t ipv4_get_ip(void) {
    return our_ip;
}

int ipv4_send(uint32_t dst_ip, uint8_t proto, const uint8_t *payload, int len) {
    uint8_t mac[6];
    if (arp_resolve(dst_ip, mac) != 0) {
        puts("ipv4: ARP failed\n");
        return -1;
    }

    uint8_t buf[1500];
    struct ipv4_header *ip = (struct ipv4_header*)buf;

    ip->ver_ihl     = 0x45;
    ip->tos         = 0;
    ip->total_length = (uint16_t)((sizeof(struct ipv4_header) + len) << 8) |
                       (uint16_t)((sizeof(struct ipv4_header) + len) >> 8);
    ip->id          = 0;
    ip->flags_frag  = 0;
    ip->ttl         = 64;
    ip->protocol    = proto;
    ip->checksum    = 0;
    ip->src         = our_ip;
    ip->dst         = dst_ip;

    ip->checksum = ip_checksum(ip, sizeof(struct ipv4_header));

    memcpy(buf + sizeof(struct ipv4_header), payload, len);

    return eth_send(mac, ETH_TYPE_IPv4, buf, sizeof(struct ipv4_header) + len);
}

void ipv4_handle(const uint8_t *frame, int len) {
    const struct eth_frame *eth = (const struct eth_frame*)frame;
    const struct ipv4_header *ip = (const struct ipv4_header*)eth->payload;

    if (ip->dst != our_ip) return;

    if (ip->protocol == IP_PROTO_ICMP) {
        extern void icmp_handle(const uint8_t *packet, int len);
        icmp_handle((const uint8_t*)ip, len - 14);
    }
}

void ipv4_handle(const uint8_t *frame, int len) {
    const struct eth_frame *eth = (const struct eth_frame*)frame;
    const struct ipv4_header *ip = (const struct ipv4_header*)eth->payload;

    if (ip->dst != our_ip) return;

    if (ip->protocol == IP_PROTO_ICMP) {
        extern void icmp_handle(const uint8_t *packet, int len);
        icmp_handle((const uint8_t*)ip, len - 14);
    } else if (ip->protocol == IP_PROTO_UDP) {
        udp_handle((const uint8_t*)ip, len - 14);
    }
}
