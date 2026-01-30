#include "pci.h"
#include "io.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static uint32_t pci_config_addr(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    return (uint32_t)(
        (1U << 31) |
        ((uint32_t)bus  << 16) |
        ((uint32_t)slot << 11) |
        ((uint32_t)func << 8)  |
        (offset & 0xFC)
    );
}

uint32_t pci_read_config_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    outb(PCI_CONFIG_ADDRESS, 0);
    uint32_t addr = pci_config_addr(bus, slot, func, offset);
    __asm__ volatile ("outl %0, %1" : : "a"(addr), "Nd"(PCI_CONFIG_ADDRESS));
    uint32_t val;
    __asm__ volatile ("inl %1, %0" : "=a"(val) : "Nd"(PCI_CONFIG_DATA));
    return val;
}

uint16_t pci_read_config_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t v = pci_read_config_dword(bus, slot, func, offset & ~3);
    return (v >> ((offset & 2) * 8)) & 0xFFFF;
}

uint8_t pci_read_config_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t v = pci_read_config_dword(bus, slot, func, offset & ~3);
    return (v >> ((offset & 3) * 8)) & 0xFF;
}

int pci_find_device(uint16_t vendor, uint16_t device, pci_device_t *out) {
    for (uint8_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            uint16_t v = pci_read_config_word(bus, slot, 0, 0x00);
            if (v == 0xFFFF) continue;

            uint16_t d = pci_read_config_word(bus, slot, 0, 0x02);
            if (v == vendor && d == device) {
                if (out) {
                    out->bus = bus;
                    out->slot = slot;
                    out->func = 0;
                    out->vendor_id = v;
                    out->device_id = d;
                }
                return 0;
            }
        }
    }
    return -1;
}
