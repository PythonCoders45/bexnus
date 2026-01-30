#include "arp.h"
#include "ethernet.h"
#include "../drivers/vga.h"
#include <string.h>

struct arp_packet {
    uint16_t htype;
    uint16_t ptype;
    uint8_t  hlen;
    uint8_t  plen;
    uint16_t oper;
    uint8_t  sha[6];
    uint32_t spa;
    uint8_t  tha[6];
    uint32_t tpa;
} __attribute__((packed));

struct arp_entry {
    uint32_t ip;
    uint8_t  mac[6];
};

#define ARP_CACHE_SIZE 16

static struct arp_entry cache[ARP_CACHE_SIZE];
static uint32_t our_ip_local = 0;

static void arp_cache_add(uint32_t ip, const uint8_t mac[6]) {
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (cache[i].ip == ip || cache[i].ip == 0) {
            cache[i].ip = ip;
            memcpy(cache[i].mac, mac, 6);
            return;
        }
    }
}

static int arp_cache_lookup(uint32_t ip, uint8_t mac_out[6]) {
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (cache[i].ip == ip) {
            memcpy(mac_out, cache[i].mac, 6);
            return 0;
        }
    }
    return -1;
}

void arp_init(uint32_t our_ip) {
    our_ip_local = our_ip;
    memset(cache, 0, sizeof(cache));
}

void arp_handle(const uint8_t *frame, int len) {
    const struct eth_frame *eth = (const struct eth_frame*)frame;
    const struct arp_packet *arp = (const struct arp_packet*)eth->payload;

    uint16_t oper = (arp->oper >> 8) | (arp->oper << 8);

    // learn sender
    arp_cache_add(arp->spa, arp->sha);

    // if it's a request for us, reply
    if (oper == 1 && arp->tpa == our_ip_local) {
        struct arp_packet reply;
        reply.htype = arp->htype;
        reply.ptype = arp->ptype;
        reply.hlen  = arp->hlen;
        reply.plen  = arp->plen;
        reply.oper  = (2 << 8) | (2 >> 8); // reply

        // our MAC/IP
        memcpy(reply.sha, eth->src, 6); // we should use our_mac, but eth_set_mac not exposed here
        reply.spa = our_ip_local;

        // target
        memcpy(reply.tha, arp->sha, 6);
        reply.tpa = arp->spa;

        uint8_t dst[6];
        memcpy(dst, arp->sha, 6);
        eth_send(dst, ETH_TYPE_ARP, (uint8_t*)&reply, sizeof(reply));
    }
}

int arp_resolve(uint32_t ip, uint8_t mac_out[6]) {
    if (arp_cache_lookup(ip, mac_out) == 0)
        return 0;

    struct arp_packet req;
    req.htype = (1 << 8) | (1 >> 8);      // Ethernet
    req.ptype = (0x0800 >> 8) | (0x0800 << 8); // IPv4
    req.hlen  = 6;
    req.plen  = 4;
    req.oper  = (1 << 8) | (1 >> 8);      // request

    // sender: our MAC/IP
    // we don't have direct access to our MAC here; for now just zero
    memset(req.sha, 0, 6);
    req.spa = our_ip_local;

    // target
    memset(req.tha, 0, 6);
    req.tpa = ip;

    uint8_t broadcast[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    eth_send(broadcast, ETH_TYPE_ARP, (uint8_t*)&req, sizeof(req));

    // crude wait loop: poll Ethernet a bit
    for (int i = 0; i < 100000; i++) {
        eth_poll();
        if (arp_cache_lookup(ip, mac_out) == 0)
            return 0;
    }

    return -1;
}
