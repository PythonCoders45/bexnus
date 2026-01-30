#include "../../kernel/vga.h"
#include "../../kernel/timer.h"
#include "../cmd.h"

void cmd_uptime(const char *args) {
    uint32_t sec = timer_ticks / 100;
    print_number(sec);
    puts(" seconds");
}
