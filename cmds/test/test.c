#include "../../kernel/vga.h"
#include "../../kernel/timer.h"
#include "../cmd.h"

void cmd_test(const char *args) {
    for (int i = 1; i <= 10; i++) {
        print_number(i);
        putc(' ');
        sleep(200);
    }
    putc('\n');
}
