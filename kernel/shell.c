#include "../cmds/cmds.h"

void shell_run(void) {
    char line[256];
    char *argv[16];
    int argc;

    while (1) {
        shell_readline(line);
        cmd_parse(line, &argc, argv);

        if (argc == 0)
            continue;

        int found = 0;
        for (int i = 0; command_table[i].name; i++) {
            if (strcmp(argv[0], command_table[i].name) == 0) {
                command_table[i].func(argc, argv);
                found = 1;
                break;
            }
        }

        if (!found)
            puts("unknown command\n");
    }
}
