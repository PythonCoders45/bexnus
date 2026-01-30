#include "vfs.h"
#include "../vga.h"
#include "../memory.h"
#include <string.h>

struct vfs_node *vfs_root = 0;
struct vfs_node *vfs_cwd = 0;

static struct vfs_node *new_node(const char *name, node_type type) {
    struct vfs_node *n = kmalloc(sizeof(struct vfs_node));
    memset(n, 0, sizeof(struct vfs_node));

    strncpy(n->name, name, 31);
    n->type = type;
    return n;
}

void vfs_init(void) {
    vfs_root = new_node("/", NODE_DIR);
    vfs_cwd = vfs_root;
}

static struct vfs_node *find_in_dir(struct vfs_node *dir, const char *name) {
    for (int i = 0; i < dir->child_count; i++) {
        if (strcmp(dir->children[i]->name, name) == 0)
            return dir->children[i];
    }
    return 0;
}

struct vfs_node *vfs_find(const char *path) {
    if (strcmp(path, "/") == 0) return vfs_root;

    struct vfs_node *cur = (path[0] == '/') ? vfs_root : vfs_cwd;

    char temp[128];
    strncpy(temp, path, 127);

    char *token = strtok(temp, "/");
    while (token) {
        cur = find_in_dir(cur, token);
        if (!cur) return 0;
        token = strtok(0, "/");
    }
    return cur;
}

struct vfs_node *vfs_create(const char *path, node_type type) {
    char temp[128];
    strncpy(temp, path, 127);

    char *name = strrchr(temp, '/');
    struct vfs_node *parent;

    if (name) {
        *name = 0;
        name++;
        parent = vfs_find(temp[0] ? temp : "/");
    } else {
        parent = vfs_cwd;
        name = temp;
    }

    if (!parent || parent->type != NODE_DIR) return 0;

    struct vfs_node *n = new_node(name, type);
    n->parent = parent;
    parent->children[parent->child_count++] = n;

    return n;
}

void vfs_delete(const char *path) {
    struct vfs_node *n = vfs_find(path);
    if (!n || n == vfs_root) return;

    struct vfs_node *p = n->parent;

    for (int i = 0; i < p->child_count; i++) {
        if (p->children[i] == n) {
            for (int j = i; j < p->child_count - 1; j++)
                p->children[j] = p->children[j + 1];
            p->child_count--;
            break;
        }
    }

    if (n->data) kfree(n->data);
    kfree(n);
}

int vfs_read(const char *path, char *buf, int max) {
    struct vfs_node *n = vfs_find(path);
    if (!n || n->type != NODE_FILE) return -1;

    int len = (n->size < max) ? n->size : max;
    memcpy(buf, n->data, len);
    return len;
}

int vfs_write(const char *path, const char *buf) {
    struct vfs_node *n = vfs_find(path);
    if (!n || n->type != NODE_FILE) return -1;

    int len = strlen(buf);
    n->data = kmalloc(len + 1);
    memcpy(n->data, buf, len + 1);
    n->size = len;
    return len;
}

void vfs_list(const char *path) {
    struct vfs_node *dir = vfs_find(path);
    if (!dir || dir->type != NODE_DIR) return;

    for (int i = 0; i < dir->child_count; i++) {
        puts(dir->children[i]->name);
        putc('\n');
    }
}
