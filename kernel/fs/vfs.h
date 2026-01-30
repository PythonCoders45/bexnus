// vfs.h — Bexnus Virtual Filesystem Layer
// Unified interface for tmpfs, FAT12, ext2, and future filesystems.

#ifndef BEXNUS_VFS_H
#define BEXNUS_VFS_H

#include <stdint.h>

//
// Node types
//
typedef enum {
    NODE_FILE,
    NODE_DIR,
    NODE_MOUNTPOINT
} node_type;

//
// Forward declaration
//
struct vfs_node;

//
// Filesystem driver interface
//
typedef struct vfs_fs {
    const char *name;

    // Required operations
    struct vfs_node *(*lookup)(struct vfs_node *dir, const char *name);
    int   (*read)(struct vfs_node *node, char *buf, int max);
    int   (*write)(struct vfs_node *node, const char *buf, int size);
    struct vfs_node *(*create)(struct vfs_node *dir, const char *name, node_type type);
    int   (*list)(struct vfs_node *dir);

    // Optional (for real FS like ext2)
    void *fs_private;
} vfs_fs_t;


//
// VFS node structure
//
typedef struct vfs_node {
    char name[64];
    node_type type;

    struct vfs_node *parent;
    struct vfs_node *children[64];
    int child_count;

    // For tmpfs
    char *data;
    uint32_t size;

    // Filesystem driver
    vfs_fs_t *fs;

    // Filesystem-specific data (inode number, cluster, etc.)
    uint32_t fs_inode;
    uint32_t fs_block;
} vfs_node_t;


//
// Global VFS state
//
extern vfs_node_t *vfs_root;
extern vfs_node_t *vfs_cwd;


//
// Initialization
//
void vfs_init(void);


//
// Path resolution
//
vfs_node_t *vfs_resolve(const char *path);
vfs_node_t *vfs_walk(vfs_node_t *start, const char *path);


//
// File operations
//
int vfs_read(const char *path, char *buf, int max);
int vfs_write(const char *path, const char *buf);
vfs_node_t *vfs_create(const char *path, node_type type);
void vfs_delete(const char *path);
void vfs_list(const char *path);


//
// Mounting filesystems
//
vfs_node_t *vfs_mount(const char *mountpoint, vfs_fs_t *fs);
vfs_node_t *vfs_mount_ext2(const char *mountpoint, uint32_t lba_start);
vfs_node_t *vfs_mount_fat12(const char *mountpoint, uint32_t lba_start);


//
// Helpers
//
int vfs_is_absolute(const char *path);
void vfs_set_cwd(vfs_node_t *node);

#endif
