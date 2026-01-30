#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"

void cmd_rm(const char *args) {
    if (!args || !*args) {
        puts("rm: missing file\n");
        return;
    }

    vfs_delete(args);
}
