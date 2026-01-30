#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"

void cmd_cd(const char *args) {
    struct vfs_node *n = vfs_find(args);

    if (!n || n->type != NODE_DIR) {
        puts("cd: not a directory\n");
        return;
    }

    vfs_cwd = n;
}
