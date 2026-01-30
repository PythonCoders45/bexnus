#include "cmds.h"

// include each command’s .c file header if needed
// (depends on your build system)

struct command_entry command_table[] = {
    // filesystem
    { "ls",    cmd_ls },
    { "cat",   cmd_cat },
    { "mkdir", cmd_mkdir },
    { "rm",    cmd_rm },
    { "cp",    cmd_cp },
    { "tree",  cmd_tree },
    { "find",  cmd_find },
    { "pwd",   cmd_pwd },
    { "edit",  cmd_edit },

    // process / system
    { "run",   cmd_run },

    // networking
    { "ping",  cmd_ping },
    // { "netget", cmd_netget }, // soon
    { "netget", cmd_netget },

    { 0, 0 } // end of table
};
