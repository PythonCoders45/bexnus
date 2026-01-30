#include "../../kernel/vga.h"
#include "../cmd.h"

void cmd_help(const char *args) {
    puts("Available commands:");
    puts("  echo <text>");
    puts("  clear");
    puts("  help");
    puts("  uptime");
    puts("  mem");
    puts("  panic");
    puts("  test");
}
