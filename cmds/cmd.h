// cmd.h — shared interface for all Bexnus commands

#ifndef BEXNUS_CMD_H
#define BEXNUS_CMD_H

void cmd_cat(const char *args);
void cmd_mv(const char *args);
void cmd_echo(const char *args);
void cmd_clear(const char *args);
void cmd_help(const char *args);

#endif
