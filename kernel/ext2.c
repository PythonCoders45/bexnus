// ext2.c
#include "ext2.h"
#include "block.h"
#include "vga.h"

int ext2_mount(uint32_t lba_start) {
    uint8_t sb[1024];
    disk_read_sector(lba_start + 2, sb); // superblock
    puts("ext2: mount stub (superblock read)\n");
    return 0;
}

int ext2_read_file(const char *path, uint8_t *buf, int max) {
    puts("ext2: read_file stub\n");
    return -1;
}
