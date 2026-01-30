#include "block.h"
#include "../vga.h"
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

int disk_read_sector(uint32_t lba, uint8_t *buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    while (!(inb(0x1F7) & 0x08));

    for (int i = 0; i < 256; i++) {
        uint16_t data;
        __asm__ volatile ("inw %1, %0" : "=a"(data) : "Nd"(0x1F0));
        ((uint16_t*)buffer)[i] = data;
    }

    return 0;
}

int disk_write_sector(uint32_t lba, const uint8_t *buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x30);

    while (!(inb(0x1F7) & 0x08));

    for (int i = 0; i < 256; i++) {
        uint16_t data = ((uint16_t*)buffer)[i];
        __asm__ volatile ("outw %0, %1" : : "a"(data), "Nd"(0x1F0));
    }

    return 0;
}
