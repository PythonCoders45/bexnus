#ifndef BEXNUS_SYSCALL_H
#define BEXNUS_SYSCALL_H

#include <stdint.h>

void syscall_init(void);

enum {
    SYS_write = 1,
    SYS_exit  = 2
};

int sys_write(const char *s);
void sys_exit(int code);

#endif
