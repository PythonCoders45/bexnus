#include "proc.h"
#include "../arch/x86/paging.h"
#include "../drivers/vga.h"
#include <string.h>

struct proc procs[MAX_PROCS];
struct proc *current = 0;
static int next_pid = 1;

extern void ctx_switch(struct regs *old, struct regs *new);

void sched_init(void) {
    memset(procs, 0, sizeof(procs));
    current = &procs[0];
    current->pid = 0;
    current->state = PROC_RUNNING;
}

int proc_spawn(void (*entry)(void)) {
    for (int i = 1; i < MAX_PROCS; i++) {
        if (procs[i].state == PROC_UNUSED) {
            struct proc *p = &procs[i];
            memset(p, 0, sizeof(*p));

            p->pid = next_pid++;
            p->state = PROC_RUNNABLE;
            p->page_dir = paging_clone_directory();

            p->ctx.eip = (uint32_t)entry;
            p->ctx.cs  = 0x08;
            p->ctx.eflags = 0x202;
            p->ctx.useresp = 0x800000;
            p->ctx.ss = 0x10;

            return p->pid;
        }
    }
    return -1;
}

void sched_tick(void) {
    int cur_idx = current - procs;
    int next = (cur_idx + 1) % MAX_PROCS;

    for (int i = 0; i < MAX_PROCS; i++) {
        int idx = (next + i) % MAX_PROCS;
        if (procs[idx].state == PROC_RUNNABLE) {
            struct proc *old = current;
            struct proc *new = &procs[idx];

            old->state = PROC_RUNNABLE;
            new->state = PROC_RUNNING;
            current = new;

            paging_switch_directory(new->page_dir);
            ctx_switch(&old->ctx, &new->ctx);
            return;
        }
    }
}

void proc_yield(void) {
    sched_tick();
}
