#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"
#include <string.h>

void cmd_grep(const char *args) {
    char word[32], file[64];
    int i = 0;

    while (*args && *args != ' ') word[i++] = *args++;
    word[i] = 0;

    while (*args == ' ') args++;

    strcpy(file, args);

    char buf[2048];
    int len = vfs_read(file, buf, sizeof(buf)-1);

    if (len < 0) {
        puts("grep: cannot read file\n");
        return;
    }

    buf[len] = 0;

    if (strstr(buf, word)) {
        puts(buf);
    }
}
