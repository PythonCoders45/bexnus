#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../../kernel/keyboard.h"
#include "../cmd.h"
#include <string.h>

static char buffer[2048];
static int pos = 0;

void cmd_edit(const char *args) {
    struct vfs_node *n = vfs_find(args);
    if (!n) n = vfs_create(args, NODE_FILE);

    pos = vfs_read(args, buffer, sizeof(buffer)-1);
    if (pos < 0) pos = 0;

    clear_screen();
    puts("Editing: ");
    puts(args);
    putc('\n');
    puts("Press ESC to save and exit\n\n");

    puts(buffer);

    while (1) {
        char c = keyboard_getchar();

        if (c == 27) break; // ESC

        buffer[pos++] = c;
        putc(c);
    }

    buffer[pos] = 0;
    vfs_write(args, buffer);
}
