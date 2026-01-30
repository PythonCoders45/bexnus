#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"

void cmd_pwd(const char *args) {
    struct vfs_node *n = vfs_cwd;

    // reconstruct path
    char path[256] = "";
    while (n && n->parent) {
        char temp[256];
        strcpy(temp, "/");
        strcat(temp, n->name);
        strcat(temp, path);
        strcpy(path, temp);
        n = n->parent;
    }

    puts(path[0] ? path : "/");
    putc('\n');
}
