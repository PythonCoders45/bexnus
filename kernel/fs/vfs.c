#ifndef BEXNUS_VFS_H
#define BEXNUS_VFS_H

#include <stdint.h>

typedef enum {
    NODE_FILE,
    NODE_DIR
} node_type;

struct vfs_node {
    char name[32];
    node_type type;
    struct vfs_node *parent;
    struct vfs_node *children[32];
    int child_count;

    char *data;
    uint32_t size;
};

extern struct vfs_node *vfs_root;
extern struct vfs_node *vfs_cwd;

void vfs_init(void);

struct vfs_node *vfs_find(const char *path);
struct vfs_node *vfs_create(const char *path, node_type type);
void vfs_delete(const char *path);

int vfs_read(const char *path, char *buf, int max);
int vfs_write(const char *path, const char *buf);

void vfs_list(const char *path);

#endif
