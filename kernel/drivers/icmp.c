#include "icmp.h"
#include "ipv4.h"
#include "../drivers/vga.h"
#include <string.h>

struct icmp_header {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;
    uint8_t  data[];
} __attribute__((packed));

static uint16_t icmp_checksum(const void *vdata, size_t length) {
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

void icmp_handle(const uint8_t *ip_packet, int len) {
    const struct ipv4_header *ip = (const struct ipv4_header*)ip_packet;
    const struct icmp_header *icmp = (const struct icmp_header*)(ip_packet + sizeof(struct ipv4_header));

    if (icmp->type == 8) { // echo request
        uint8_t buf[1500];
        struct icmp_header *reply = (struct icmp_header*)buf;

        int icmp_len = len - sizeof(struct ipv4_header);
        memcpy(reply, icmp, icmp_len);

        reply->type = 0;
        reply->checksum = 0;
        reply->checksum = icmp_checksum(reply, icmp_len);

        ipv4_send(ip->src, IP_PROTO_ICMP, (uint8_t*)reply, icmp_len);
    }
}

int icmp_ping(uint32_t dst_ip, int count) {
    uint8_t buf[64];
    struct icmp_header *icmp = (struct icmp_header*)buf;

    for (int i = 0; i < count; i++) {
        memset(icmp, 0, sizeof(*icmp) + 32);
        icmp->type = 8;
        icmp->code = 0;
        icmp->id   = 0x1234;
        icmp->seq  = i;

        for (int j = 0; j < 32; j++)
            icmp->data[j] = (uint8_t)j;

        int len = sizeof(*icmp) + 32;
        icmp->checksum = 0;
        icmp->checksum = icmp_checksum(icmp, len);

        ipv4_send(dst_ip, IP_PROTO_ICMP, (uint8_t*)icmp, len);
    }

    return 0;
}
