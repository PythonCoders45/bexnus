// shell.c — Bexnus shell

#include <stdint.h>
#include <string.h>

#include "../cmds/cmd.h"      // <-- THIS is where the include goes
#include "vga.h"
#include "timer.h"

static char input[128];
static int input_len = 0;

void shell_handle_char(char c) {
    if (c == '\n') {
        input[input_len] = 0;
        putc('\n');
        shell_execute(input);
        input_len = 0;
        puts("> ");
        return;
    }

    if (input_len < 127) {
        input[input_len++] = c;
        putc(c);
    }
}

void shell_execute(const char *cmd) {
    if (strncmp(cmd, "echo ", 5) == 0) cmd_echo(cmd + 5);
    else if (strcmp(cmd, "clear") == 0) cmd_clear("");
    else if (strcmp(cmd, "help") == 0) cmd_help("");
    else if (strcmp(cmd, "uptime") == 0) cmd_uptime("");
    else if (strcmp(cmd, "panic") == 0) cmd_panic("");
    else if (strcmp(cmd, "mem") == 0) cmd_mem("");
    else if (strcmp(cmd, "test") == 0) cmd_test("");
    else puts("Unknown command");
}
