#include "../../kernel/vga.h"
#include "../cmd.h"

void cmd_echo(const char *args) {
    puts(args);
}
