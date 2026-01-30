#include "../../kernel/vga.h"
#include "../../kernel/fs/vfs.h"
#include "../cmd.h"

static void print_tree(struct vfs_node *n, int depth) {
    for (int i = 0; i < depth; i++) puts("  ");
    puts(n->name);
    putc('\n');

    if (n->type == NODE_DIR) {
        for (int i = 0; i < n->child_count; i++)
            print_tree(n->children[i], depth + 1);
    }
}

void cmd_tree(const char *args) {
    struct vfs_node *start = (*args) ? vfs_find(args) : vfs_cwd;
    if (!start) {
        puts("tree: path not found\n");
        return;
    }
    print_tree(start, 0);
}
