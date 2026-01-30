// ext2.h
#ifndef BEXNUS_EXT2_H
#define BEXNUS_EXT2_H

#include <stdint.h>

int ext2_mount(uint32_t lba_start);
int ext2_read_file(const char *path, uint8_t *buf, int max);

#endif
