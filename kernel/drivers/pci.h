#ifndef BEXNUS_PCI_H
#define BEXNUS_PCI_H

#include <stdint.h>

typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
    uint16_t vendor_id;
    uint16_t device_id;
} pci_device_t;

uint32_t pci_read_config_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_read_config_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint8_t  pci_read_config_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

int pci_find_device(uint16_t vendor, uint16_t device, pci_device_t *out);

#endif
