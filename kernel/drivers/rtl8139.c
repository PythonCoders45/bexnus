#include "rtl8139.h"
#include "pci.h"
#include "io.h"
#include "../drivers/vga.h"
#include "../drivers/pic.h"

#define RTL_VENDOR_ID 0x10EC
#define RTL_DEVICE_ID 0x8139

static uint16_t io_base;
static uint8_t  mac[6];

static uint8_t rx_buffer[8192 + 16 + 1500];
static uint32_t rx_buf_ptr = 0;
static int tx_cur = 0;

static inline uint8_t inb16(uint16_t port) { return inb(port); }
static inline void outb16(uint16_t port, uint8_t val) { outb(port, val); }
static inline void outw16(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}
static inline void outl16(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

void rtl8139_init(void) {
    pci_device_t dev;
    if (pci_find_device(RTL_VENDOR_ID, RTL_DEVICE_ID, &dev) != 0) {
        puts("rtl8139: not found\n");
        return;
    }

    uint32_t bar0 = pci_read_config_dword(dev.bus, dev.slot, dev.func, 0x10);
    io_base = (uint16_t)(bar0 & ~0x3);

    outb16(io_base + 0x52, 0x00); // config1: power on

    outb16(io_base + 0x37, 0x10); // reset
    while (inb16(io_base + 0x37) & 0x10);

    outl16(io_base + 0x30, (uint32_t)rx_buffer);
    outw16(io_base + 0x3C, 0x0005); // RX OK + TX OK IRQs

    outl16(io_base + 0x44, 0x0000000F); // RX config: accept all for now

    for (int i = 0; i < 6; i++)
        mac[i] = inb16(io_base + 0x00 + i);

    outb16(io_base + 0x37, 0x0C); // enable RX + TX

    puts("rtl8139: initialized, IO base=");
    // crude print
    // (you can add a hex print helper later)
    puts(" (MAC set)\n");

    pic_unmask_irq(11); // typical PCI IRQ, adjust if needed
}

int rtl8139_send(const uint8_t *data, int len) {
    if (len <= 0 || len > 1500) return -1;

    uint32_t tx_buf_addr = 0x100000 + tx_cur * 0x800; // crude static TX buffers
    uint16_t tx_status_reg = io_base + 0x10 + tx_cur * 4;

    uint8_t *dst = (uint8_t*)tx_buf_addr;
    for (int i = 0; i < len; i++) dst[i] = data[i];

    outl16(tx_status_reg, len & 0x1FFF);

    tx_cur = (tx_cur + 1) & 3;
    return 0;
}

int rtl8139_poll(uint8_t *buf, int max_len) {
    uint16_t status = inw(io_base + 0x3E);
    if (!(status & 0x01)) return 0; // no RX OK

    while ((inb16(io_base + 0x37) & 0x01) == 0) {
        uint32_t *hdr = (uint32_t*)(rx_buffer + rx_buf_ptr);
        uint16_t pkt_status = hdr[0] & 0xFFFF;
        uint16_t pkt_len    = hdr[0] >> 16;

        if (!(pkt_status & 0x01)) break;

        uint8_t *pkt = (uint8_t*)(rx_buffer + rx_buf_ptr + 4);
        int copy_len = (pkt_len - 4 < max_len) ? pkt_len - 4 : max_len;
        for (int i = 0; i < copy_len; i++) buf[i] = pkt[i];

        rx_buf_ptr = (rx_buf_ptr + pkt_len + 4 + 3) & ~3;
        outw16(io_base + 0x38, rx_buf_ptr - 0x10);
        outb16(io_base + 0x37, 0x01);

        return copy_len;
    }

    return 0;
}
