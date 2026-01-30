#include <stdint.h>
#include "../kernel/drivers/vga.h"
#include "../kernel/drivers/dns.h"
#include "../kernel/drivers/icmp.h"
#include "../kernel/drivers/ethernet.h"
#include "../kernel/drivers/ipv4.h"

// simple atoi
static int to_int(const char *s) {
    int v = 0;
    while (*s >= '0' && *s <= '9') {
        v = v * 10 + (*s - '0');
        s++;
    }
    return v;
}

// parse dotted IPv4 string
static int parse_ip(const char *s, uint32_t *out) {
    int a = to_int(s);
    while (*s && *s != '.') s++;
    if (*s != '.') return -1;
    s++;

    int b = to_int(s);
    while (*s && *s != '.') s++;
    if (*s != '.') return -1;
    s++;

    int c = to_int(s);
    while (*s && *s != '.') s++;
    if (*s != '.') return -1;
    s++;

    int d = to_int(s);

    *out = (uint32_t)(a | (b << 8) | (c << 16) | (d << 24));
    return 0;
}

void cmd_ping(int argc, char **argv) {
    if (argc < 2) {
        puts("usage: ping <ip or hostname>\n");
        return;
    }

    const char *target = argv[1];
    uint32_t ip = 0;

    // Try parse as dotted IP
    if (parse_ip(target, &ip) == 0) {
        puts("ping: using IP directly\n");
    } else {
        puts("ping: resolving hostname...\n");
        if (dns_resolve(target, &ip) != 0) {
            puts("ping: DNS failed\n");
            return;
        }
        puts("ping: resolved OK\n");
    }

    puts("ping: sending 4 packets...\n");
    icmp_ping(ip, 4);
    puts("ping: done\n");
}
