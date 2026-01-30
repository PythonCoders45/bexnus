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

int fat12_find_free_cluster(void) {
    for (int i = 2; i < 2848; i++) {
        uint16_t val = fat12_get_cluster(i);
        if (val == 0x000) return i;
    }
    return -1;
}

void fat12_set_cluster(uint16_t cluster, uint16_t value) {
    uint32_t index = cluster + cluster / 2;

    if (cluster & 1) {
        uint16_t old = *(uint16_t*)&fat[index];
        old &= 0x000F;
        old |= (value << 4);
        *(uint16_t*)&fat[index] = old;
    } else {
        uint16_t old = *(uint16_t*)&fat[index];
        old &= 0xF000;
        old |= (value & 0x0FFF);
        *(uint16_t*)&fat[index] = old;
    }
}

uint8_t *fat12_find_dir_entry(const char *name) {
    char fatname[11];
    memset(fatname, ' ', 11);

    int i = 0, j = 0;
    while (name[i] && name[i] != '.' && j < 8) fatname[j++] = toupper(name[i++]);

    if (name[i] == '.') {
        i++;
        j = 8;
        while (name[i] && j < 11) fatname[j++] = toupper(name[i++]);
    }

    for (int e = 0; e < 224; e++) {
        uint8_t *entry = rootdir + e * 32;
        if (entry[0] == 0x00) return entry; // free entry
        if (entry[0] == 0xE5) return entry; // deleted entry
        if (memcmp(entry, fatname, 11) == 0) return entry;
    }

    return 0;
}

int fat12_write_file(const char *name, const uint8_t *buf, int size) {
    uint8_t *entry = fat12_find_dir_entry(name);
    if (!entry) {
        puts("FAT12: no directory entry available\n");
        return -1;
    }

    // Build FAT-style filename
    char fatname[11];
    memset(fatname, ' ', 11);

    int i = 0, j = 0;
    while (name[i] && name[i] != '.' && j < 8) fatname[j++] = toupper(name[i++]);

    if (name[i] == '.') {
        i++;
        j = 8;
        while (name[i] && j < 11) fatname[j++] = toupper(name[i++]);
    }

    memcpy(entry, fatname, 11);

    // Allocate first cluster
    int cluster = fat12_find_free_cluster();
    if (cluster < 0) {
        puts("FAT12: no free clusters\n");
        return -1;
    }

    *(uint16_t*)&entry[26] = cluster;
    *(uint32_t*)&entry[28] = size;

    int remaining = size;
    const uint8_t *src = buf;

    while (remaining > 0) {
        memcpy(data_area + (cluster - 2) * 512, src, 512);

        src += 512;
        remaining -= 512;

        int next = (remaining > 0) ? fat12_find_free_cluster() : 0xFFF;

        fat12_set_cluster(cluster, next);
        cluster = next;
    }

    // Write FAT back to disk
    for (int i = 0; i < 9; i++)
        disk_write_sector(1 + i, fat + i * 512);

    // Write root directory back
    for (int i = 0; i < 14; i++)
        disk_write_sector(19 + i, rootdir + i * 512);

    // Write data area back
    for (int i = 0; i < 2848; i++)
        disk_write_sector(33 + i, data_area + i * 512);

    return 0;
}

