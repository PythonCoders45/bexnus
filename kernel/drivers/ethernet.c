#include "ethernet.h"
#include "net.h"
#include "../drivers/vga.h"
#include <string.h>

static uint8_t our_mac[6] = {0x02,0x00,0x00,0x00,0x00,0x01}; // temp
static uint32_t our_ip = 0;

void eth_set_mac(const uint8_t mac[6]) {
    memcpy(our_mac, mac, 6);
}

void eth_set_ip(uint32_t ip) {
    our_ip = ip;
}

int eth_send(uint8_t dst_mac[6], uint16_t type, const uint8_t *payload, int len) {
    uint8_t buf[1514];
    struct eth_frame *f = (struct eth_frame*)buf;

    memcpy(f->dst, dst_mac, 6);
    memcpy(f->src, our_mac, 6);
    f->type = (type >> 8) | (type << 8); // htons

    memcpy(f->payload, payload, len);

    return net_send_raw(buf, 14 + len);
}

void eth_poll(void) {
    uint8_t buf[1600];
    int len = net_poll_raw(buf, sizeof(buf));
    if (len <= 0) return;

    struct eth_frame *f = (struct eth_frame*)buf;
    uint16_t type = (f->type >> 8) | (f->type << 8); // ntohs

    if (type == ETH_TYPE_ARP) {
        extern void arp_handle(const uint8_t *frame, int len);
        arp_handle(buf, len);
    }
    // later: IPv4 handler
}
