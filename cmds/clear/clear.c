#include "../../kernel/vga.h"
#include "../cmd.h"

void cmd_clear(const char *args) {
    clear_screen();
}
