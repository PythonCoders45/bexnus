#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"
#include <string.h>

static void find_rec(struct vfs_node *n, const char *name) {
    if (strstr(n->name, name)) {
        puts(n->name);
        putc('\n');
    }

    if (n->type == NODE_DIR) {
        for (int i = 0; i < n->child_count; i++)
            find_rec(n->children[i], name);
    }
}

void cmd_find(const char *args) {
    if (!args || !*args) {
        puts("find: missing name\n");
        return;
    }
    find_rec(vfs_cwd, args);
}
