// task.c — cooperative multitasking

#include <stdint.h>
#include "vga.h" 
#include "proc.h"

#define MAX_TASKS 8

struct task {
    uint32_t esp;
    uint8_t active;
};

static struct task tasks[MAX_TASKS];
static int current = 0;

extern void switch_task(uint32_t *old_esp, uint32_t new_esp);

void task_create(void (*entry)(void)) {
    static uint8_t stacks[MAX_TASKS][4096];

    for (int i = 0; i < MAX_TASKS; i++) {
        if (!tasks[i].active) {
            tasks[i].active = 1;
            uint32_t *stack = (uint32_t*)(stacks[i] + 4096);
            *(--stack) = (uint32_t)entry;
            tasks[i].esp = (uint32_t)stack;
            return;
        }
    }
}

void task_yield(void) {
    int next = (current + 1) % MAX_TASKS;
    while (!tasks[next].active) next = (next + 1) % MAX_TASKS;

    int old = current;
    current = next;

    switch_task(&tasks[old].esp, tasks[next].esp);
}


void task_a(void) {
    while (1) {
        puts("A")
        for (volatile int i = 0; i < 1000000; i++)
        proc_yield();
    }
}
void task_b(void) {
    while (1) {
        puts("B")
        for (volatile int i = 0; i < 1000000; i++)
        proc_yield();
    }
}