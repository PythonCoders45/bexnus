#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"

void cmd_touch(const char *args) {
    if (!args || !*args) {
        puts("touch: missing filename\n");
        return;
    }

    if (!vfs_find(args)) {
        vfs_create(args, NODE_FILE);
    }
}
