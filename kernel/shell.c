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
    else if (strcmp(cmd, "ls") == 0) cmd_ls("");
    else if (strncmp(cmd, "ls ", 3) == 0) cmd_ls(cmd + 3);

    else if (strncmp(cmd, "cat ", 4) == 0) cmd_cat(cmd + 4);

    else if (strncmp(cmd, "touch ", 6) == 0) cmd_touch(cmd + 6);

    else if (strncmp(cmd, "rm ", 3) == 0) cmd_rm(cmd + 3);

    else if (strncmp(cmd, "mkdir ", 6) == 0) cmd_mkdir(cmd + 6);

    else if (strncmp(cmd, "mv ", 3) == 0) cmd_mv(cmd + 3);

    else if (strncmp(cmd, "cd ", 3) == 0) cmd_cd(cmd + 3);

    else if (strcmp(cmd, "pwd") == 0) cmd_pwd("");

    else puts("Unknown command");
}
