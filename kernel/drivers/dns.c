#include "dns.h"
#include "udp.h"
#include "ipv4.h"
#include "../drivers/vga.h"
#include <string.h>

#define DNS_PORT 53
#define DNS_MAX_NAME 255

struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} __attribute__((packed));

static volatile int dns_done = 0;
static uint32_t dns_result_ip = 0;

static void dns_udp_handler(uint32_t src_ip, uint16_t src_port,
                            const uint8_t *data, int len) {
    if (src_port != DNS_PORT) return;
    if (len < (int)sizeof(struct dns_header)) return;

    const struct dns_header *hdr = (const struct dns_header*)data;
    uint16_t qd = (hdr->qdcount >> 8) | (hdr->qdcount << 8);
    uint16_t an = (hdr->ancount >> 8) | (hdr->ancount << 8);
    if (qd == 0 || an == 0) return;

    const uint8_t *p = data + sizeof(struct dns_header);

    // skip question
    for (int i = 0; i < qd; i++) {
        while (*p && p < data + len) p++;
        p++; // zero
        p += 4; // QTYPE + QCLASS
    }

    // first answer
    if (p + 12 > data + len) return;
    p += 2; // name (compressed)
    uint16_t type = (p[0] << 8) | p[1];
    uint16_t class = (p[2] << 8) | p[3];
    uint16_t rdlen = (p[8] << 8) | p[9];
    p += 10;

    if (type == 1 && class == 1 && rdlen == 4 && p + 4 <= data + len) {
        memcpy(&dns_result_ip, p, 4);
        dns_done = 1;
    }
}

int dns_resolve(const char *name, uint32_t *out_ip) {
    dns_done = 0;
    dns_result_ip = 0;

    udp_set_handler(12345, dns_udp_handler);

    uint8_t buf[512];
    struct dns_header *hdr = (struct dns_header*)buf;
    memset(hdr, 0, sizeof(*hdr));

    hdr->id = 0x1234;
    hdr->flags = 0x0100; // recursion desired
    hdr->qdcount = (1 << 8) | 1;

    uint8_t *p = buf + sizeof(struct dns_header);

    const char *s = name;
    while (*s) {
        const char *label = s;
        int len = 0;
        while (*s && *s != '.') {
            len++;
            s++;
        }
        *p++ = (uint8_t)len;
        for (int i = 0; i < len; i++)
            *p++ = label[i];
        if (*s == '.') s++;
    }
    *p++ = 0;

    *p++ = 0x00; *p++ = 0x01; // QTYPE A
    *p++ = 0x00; *p++ = 0x01; // QCLASS IN

    int msg_len = p - buf;

    // QEMU user-mode DNS server: 10.0.2.3
    uint32_t dns_ip = (10) | (0 << 8) | (2 << 16) | (3 << 24);

    udp_send(dns_ip, 12345, DNS_PORT, buf, msg_len);

    for (int i = 0; i < 1000000 && !dns_done; i++) {
        extern void eth_poll(void);
        eth_poll();
    }

    if (!dns_done) return -1;
    *out_ip = dns_result_ip;
    return 0;
}
