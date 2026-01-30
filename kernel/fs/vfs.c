#ifndef BEXNUS_VFS_H
#define BEXNUS_VFS_H

#include <stdint.h>
#include "ext2.h"

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

static struct vfs_node *ext2_root = 0;

struct vfs_node *vfs_mount_ext2(const char *mountpoint, uint32_t lba_start) {
    if (ext2_mount(lba_start) < 0) return 0;

    struct vfs_node *m = vfs_create(mountpoint, NODE_DIR);
    ext2_root = m;
    return m;
}

int vfs_read_ext2(const char *path, char *buf, int max) {
    // path like "/ext2/..."
    if (strncmp(path, "/ext2/", 6) != 0) return -1;
    const char *sub = path + 5; // keep leading '/'

    return ext2_read_file(sub, (uint8_t*)buf, max);
}

#endif
