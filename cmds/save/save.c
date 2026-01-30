#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../../kernel/fs/fat12.h"
#include "../cmd.h"

void cmd_save(const char *args) {
    uint8_t buffer[8192];
    int pos = 0;

    for (int i = 0; i < vfs_root->child_count; i++) {
        struct vfs_node *n = vfs_root->children[i];

        memcpy(buffer + pos, n->name, 32);
        pos += 32;

        memcpy(buffer + pos, &n->type, 4);
        pos += 4;

        memcpy(buffer + pos, &n->size, 4);
        pos += 4;

        if (n->type == NODE_FILE) {
            memcpy(buffer + pos, n->data, n->size);
            pos += n->size;
        }
    }

    fat12_write_file("BEXNUS  FS", buffer, pos);
    puts("Saved.\n");
}
