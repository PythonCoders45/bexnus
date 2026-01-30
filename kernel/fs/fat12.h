#ifndef BEXNUS_FAT12_H
#define BEXNUS_FAT12_H

#include <stdint.h>

void fat12_init(void);
int fat12_read_file(const char *name, uint8_t *buf, int max);
int fat12_write_file(const char *name, const uint8_t *buf, int size);

#endif
