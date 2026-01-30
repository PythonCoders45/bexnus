#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../../kernel/fs/fat12.h"
#include "../cmd.h"

void cmd_load(const char *args) {
    uint8_t buffer[8192];
    int size = fat12_read_file("BEXNUS  FS", buffer, sizeof(buffer));

    if (size < 0) {
        puts("No saved filesystem.\n");
        return;
    }

    vfs_init();

    int pos = 0;
    while (pos < size) {
        char name[32];
        memcpy(name, buffer + pos, 32);
        pos += 32;

        int type;
        memcpy(&type, buffer + pos, 4);
        pos += 4;

        int fsize;
        memcpy(&fsize, buffer + pos, 4);
        pos += 4;

        struct vfs_node *n = vfs_create(name, type);

        if (type == NODE_FILE) {
            n->data = kmalloc(fsize);
            memcpy(n->data, buffer + pos, fsize);
            n->size = fsize;
            pos += fsize;
        }
    }

    puts("Loaded.\n");
}
