#include <stdint.h>
#include "../kernel/drivers/vga.h"
#include "../kernel/drivers/ethernet.h"
#include "../kernel/drivers/ipv4.h"
#include "../kernel/drivers/arp.h"

// forward declaration from arp.c
extern int arp_dump_entry(int index, uint32_t *ip, uint8_t mac[6]);

static void print_ip(uint32_t ip) {
    uint8_t a = (ip      ) & 0xFF;
    uint8_t b = (ip >> 8 ) & 0xFF;
    uint8_t c = (ip >> 16) & 0xFF;
    uint8_t d = (ip >> 24) & 0xFF;

    putc('0' + (a/100)%10); putc('0' + (a/10)%10); putc('0' + (a%10)); putc('.');
    putc('0' + (b/100)%10); putc('0' + (b/10)%10); putc('0' + (b%10)); putc('.');
    putc('0' + (c/100)%10); putc('0' + (c/10)%10); putc('0' + (c%10)); putc('.');
    putc('0' + (d/100)%10); putc('0' + (d/10)%10); putc('0' + (d%10));
}

static void print_mac(const uint8_t mac[6]) {
    const char *hex = "0123456789ABCDEF";
    for (int i = 0; i < 6; i++) {
        putc(hex[(mac[i] >> 4) & 0xF]);
        putc(hex[(mac[i]     ) & 0xF]);
        if (i != 5) putc(':');
    }
}

void cmd_netinfo(int argc, char **argv) {
    puts("=== Network Information ===\n");

    // IP
    uint32_t ip = ipv4_get_ip();
    puts("IP Address: ");
    print_ip(ip);
    puts("\n");

    // MAC
    extern uint8_t rtl8139_mac[6]; // expose from rtl8139.c
    puts("MAC Address: ");
    print_mac(rtl8139_mac);
    puts("\n");

    // DNS (QEMU default)
    uint32_t dns = (10) | (0 << 8) | (2 << 16) | (3 << 24);
    puts("DNS Server: ");
    print_ip(dns);
    puts("\n");

    // Gateway (QEMU default)
    uint32_t gw = (10) | (0 << 8) | (2 << 16) | (2 << 24);
    puts("Gateway:   ");
    print_ip(gw);
    puts("\n");

    // ARP cache
    puts("\nARP Cache:\n");

    for (int i = 0; i < 16; i++) {
        uint32_t entry_ip;
        uint8_t entry_mac[6];

        if (arp_dump_entry(i, &entry_ip, entry_mac) == 0) {
            print_ip(entry_ip);
            puts("  ");
            print_mac(entry_mac);
            puts("\n");
        }
    }

    puts("===========================\n");
}
