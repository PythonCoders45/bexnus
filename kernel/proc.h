#ifndef BEXNUS_PROC_H
#define BEXNUS_PROC_H

#include <stdint.h>

#define MAX_PROCS 32

typedef enum {
    PROC_UNUSED,
    PROC_RUNNABLE,
    PROC_RUNNING,
    PROC_ZOMBIE
} proc_state;

struct regs {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags, useresp, ss;
};

struct proc {
    int pid;
    proc_state state;
    struct regs ctx;
    uint32_t *page_dir;
};

extern struct proc procs[MAX_PROCS];
extern struct proc *current;

void sched_init(void);
void sched_tick(void);
int  proc_spawn(void (*entry)(void));
void proc_yield(void);

#endif
