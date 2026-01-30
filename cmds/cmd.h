#ifndef BEXNUS_CMDS_H
#define BEXNUS_CMDS_H

// Every command uses this signature
typedef void (*command_func_t)(int argc, char **argv);

// Command table entry
struct command_entry {
    const char       *name;
    command_func_t    func;
};

// === Command declarations ===
// Add every command here

// filesystem
void cmd_ls(int argc, char **argv);
void cmd_cat(int argc, char **argv);
void cmd_mkdir(int argc, char **argv);
void cmd_rm(int argc, char **argv);
void cmd_cp(int argc, char **argv);
void cmd_tree(int argc, char **argv);
void cmd_find(int argc, char **argv);
void cmd_pwd(int argc, char **argv);
void cmd_edit(int argc, char **argv);

// process / system
void cmd_run(int argc, char **argv);

// networking
void cmd_ping(int argc, char **argv);
// later: void cmd_netget(int argc, char **argv);
void cmd_netget(int argc, char **argv);

// === Command table (defined in cmds.c or shell.c) ===
extern struct command_entry command_table[];

#endif
