#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"

void cmd_mkdir(const char *args) {
    if (!args || !*args) {
        puts("mkdir: missing directory name\n");
        return;
    }

    vfs_create(args, NODE_DIR);
}
