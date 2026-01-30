#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"

void cmd_ls(const char *args) {
    const char *path = (*args) ? args : ".";
    vfs_list(path);
}
