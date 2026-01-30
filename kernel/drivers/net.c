#include "net.h"
#include "rtl8139.h"
#include "../drivers/vga.h"
#include "ethernet.h" 
#include "arp.h"
#include "ipv4.h"
#include "udp.h"
#include "dns.h"

void net_init(void) { 
    puts("net: init\n"); 
    rtl8139_init(); 
    uint32_t ip = (10) | (0 << 8) | (2 << 16) | (15 << 24); // 10.0.2.15 
    eth_set_ip(ip); 
    arp_init(ip); 
    ipv4_init(ip); 
    udp_init();
}

int net_send_raw(const uint8_t *data, int len) {
    return rtl8139_send(data, len);
}

int net_poll_raw(uint8_t *buf, int max_len) {
    return rtl8139_poll(buf, max_len);
}
