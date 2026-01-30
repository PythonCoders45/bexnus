#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"

void cmd_cat(const char *args) {
    char buf[1024];
    int len = vfs_read(args, buf, sizeof(buf)-1);

    if (len < 0) {
        puts("cat: cannot read file\n");
        return;
    }

    buf[len] = 0;
    puts(buf);
}
