#include "ext2.h"
#include "block.h"
#include "../vga.h"
#include <string.h>

static uint32_t ext2_lba = 0;
static struct ext2_superblock sb;
static struct ext2_group_desc gd;

static uint32_t block_size;

static void read_block(uint32_t block, uint8_t *buf) {
    uint32_t lba = ext2_lba + block * (block_size / 512);
    for (int i = 0; i < block_size / 512; i++)
        disk_read_sector(lba + i, buf + i * 512);
}

int ext2_mount(uint32_t lba_start) {
    ext2_lba = lba_start;

    uint8_t buf[1024];
    disk_read_sector(ext2_lba + EXT2_SUPERBLOCK_LBA, buf);
    memcpy(&sb, buf, sizeof(sb));

    if (sb.magic != EXT2_SUPER_MAGIC) {
        puts("ext2: bad magic\n");
        return -1;
    }

    block_size = 1024 << sb.log_block_size;

    // Read first group descriptor
    read_block(sb.first_data_block + 1, buf);
    memcpy(&gd, buf, sizeof(gd));

    puts("ext2: mounted\n");
    return 0;
}

static void read_inode(uint32_t inode_num, struct ext2_inode *inode) {
    uint32_t index = inode_num - 1;
    uint32_t table_block = gd.inode_table;

    uint32_t offset = index * sizeof(struct ext2_inode);
    uint32_t block = table_block + (offset / block_size);
    uint32_t block_offset = offset % block_size;

    uint8_t buf[4096];
    read_block(block, buf);

    memcpy(inode, buf + block_offset, sizeof(struct ext2_inode));
}

static uint32_t find_inode_in_dir(struct ext2_inode *dir, const char *name) {
    uint8_t buf[4096];

    for (int i = 0; i < 12; i++) {
        if (!dir->block[i]) continue;

        read_block(dir->block[i], buf);

        uint32_t offset = 0;
        while (offset < block_size) {
            struct ext2_dir_entry *e = (struct ext2_dir_entry*)(buf + offset);

            if (e->inode != 0) {
                char fname[256];
                memcpy(fname, e->name, e->name_len);
                fname[e->name_len] = 0;

                if (strcmp(fname, name) == 0)
                    return e->inode;
            }

            offset += e->rec_len;
        }
    }

    return 0;
}

static uint32_t resolve_path(const char *path) {
    if (path[0] != '/') return 0;

    struct ext2_inode inode;
    read_inode(2, &inode); // root inode

    char temp[256];
    strcpy(temp, path);

    char *token = strtok(temp, "/");
    while (token) {
        uint32_t ino = find_inode_in_dir(&inode, token);
        if (!ino) return 0;

        read_inode(ino, &inode);
        token = strtok(0, "/");
    }

    return inode.size ? inode.block[0] : 0;
}

int ext2_read_file(const char *path, uint8_t *buf, int max) {
    struct ext2_inode inode;
    uint32_t ino = resolve_path(path);
    if (!ino) return -1;

    read_inode(ino, &inode);

    int remaining = inode.size;
    int pos = 0;

    for (int i = 0; i < 12 && remaining > 0; i++) {
        if (!inode.block[i]) break;

        uint8_t blockbuf[4096];
        read_block(inode.block[i], blockbuf);

        int chunk = (remaining > block_size) ? block_size : remaining;
        memcpy(buf + pos, blockbuf, chunk);

        pos += chunk;
        remaining -= chunk;
    }

    return pos;
}

static void write_block(uint32_t block, const uint8_t *buf) {
    uint32_t lba = ext2_lba + block * (block_size / 512);
    for (int i = 0; i < block_size / 512; i++)
        disk_write_sector(lba + i, buf + i * 512);
}

int ext2_write_file_overwrite(uint32_t inode_num, const uint8_t *data, int size) {
    struct ext2_inode inode;
    read_inode(inode_num, &inode);

    int remaining = size;
    int pos = 0;

    for (int i = 0; i < 12 && remaining > 0; i++) {
        if (!inode.block[i]) break;

        uint8_t blockbuf[4096];
        int chunk = (remaining > block_size) ? block_size : remaining;

        memcpy(blockbuf, data + pos, chunk);
        if (chunk < (int)block_size)
            memset(blockbuf + chunk, 0, block_size - chunk);

        write_block(inode.block[i], blockbuf);

        pos += chunk;
        remaining -= chunk;
    }

    inode.size = size;
    // TODO: write inode back (you can add a write_inode() mirroring read_inode)
    return pos;
}
