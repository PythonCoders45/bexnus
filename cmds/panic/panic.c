#include "../../kernel/vga.h"
#include "../cmd.h"

void cmd_panic(const char *args) {
    puts("BEXNUS KERNEL PANIC: user invoked panic");
    for (;;) __asm__("hlt");
}
