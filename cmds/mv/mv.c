#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"
#include <string.h>

void cmd_mv(const char *args) {
    char src[64], dst[64];
    int i = 0;

    while (*args && *args != ' ') src[i++] = *args++;
    src[i] = 0;

    while (*args == ' ') args++;

    strcpy(dst, args);

    struct vfs_node *n = vfs_find(src);
    if (!n) {
        puts("mv: source not found\n");
        return;
    }

    strncpy(n->name, dst, 31);
}
