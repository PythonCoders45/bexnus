#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"
#include <string.h>

void cmd_cp(const char *args) {
    char src[64], dst[64];
    int i = 0;

    while (*args && *args != ' ') src[i++] = *args++;
    src[i] = 0;

    while (*args == ' ') args++;

    strcpy(dst, args);

    char buf[2048];
    int len = vfs_read(src, buf, sizeof(buf)-1);

    if (len < 0) {
        puts("cp: cannot read source\n");
        return;
    }

    if (!vfs_find(dst)) vfs_create(dst, NODE_FILE);

    vfs_write(dst, buf);
}
