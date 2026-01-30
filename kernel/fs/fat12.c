#include "fat12.h"
#include "block.h"
#include "../vga.h"
#include <string.h>

static uint8_t fat[9 * 512];
static uint8_t rootdir[224 * 32];
static uint8_t data_area[2848 * 512];

void fat12_init(void) {
    disk_read_sector(1, fat);
    disk_read_sector(2, fat + 512);
    disk_read_sector(3, fat + 1024);

    for (int i = 0; i < 14; i++)
        disk_read_sector(19 + i, rootdir + i * 512);

    for (int i = 0; i < 2848; i++)
        disk_read_sector(33 + i, data_area + i * 512);
}

static uint16_t fat12_get_cluster(uint16_t cluster) {
    uint32_t index = cluster + cluster / 2;
    uint16_t value = *(uint16_t*)&fat[index];

    if (cluster & 1)
        return value >> 4;
    else
        return value & 0x0FFF;
}

int fat12_read_file(const char *name, uint8_t *buf, int max) {
    for (int i = 0; i < 224; i++) {
        uint8_t *entry = rootdir + i * 32;

        if (entry[0] == 0x00) break;
        if (entry[0] == 0xE5) continue;

        char filename[12];
        memcpy(filename, entry, 11);
        filename[11] = 0;

        if (strncmp(filename, name, 11) == 0) {
            uint16_t cluster = *(uint16_t*)&entry[26];
            int size = *(uint32_t*)&entry[28];

            int pos = 0;
            while (cluster < 0xFF8 && pos < max) {
                memcpy(buf + pos, data_area + (cluster - 2) * 512, 512);
                pos += 512;
                cluster = fat12_get_cluster(cluster);
            }

            return size;
        }
    }

    return -1;
}

int fat12_write_file(const char *name, const uint8_t *buf, int size) {
    puts("FAT12 write not implemented yet\n");
    return -1;
}
