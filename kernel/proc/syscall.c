#include "syscall.h"
#include "proc.h"
#include "../drivers/vga.h"

extern void idt_set_gate(int n, uint32_t handler);

static void syscall_handler(void);

void syscall_init(void) {
    idt_set_gate(0x80, (uint32_t)syscall_handler);
}

int sys_write(const char *s) {
    puts(s);
    return 0;
}

void sys_exit(int code) {
    current->state = PROC_ZOMBIE;
    proc_yield();
}

__attribute__((naked))
static void syscall_handler(void) {
    __asm__ volatile (
        "pusha\n"
        "mov %esp, %eax\n"
        "mov 36(%eax), %ebx\n"
        "cmp $1, %ebx\n"
        "je .do_write\n"
        "cmp $2, %ebx\n"
        "je .do_exit\n"
        "jmp .done\n"
        ".do_write:\n"
        "mov 40(%eax), %ecx\n"
        "push %ecx\n"
        "call sys_write\n"
        "add $4, %esp\n"
        "jmp .done\n"
        ".do_exit:\n"
        "mov 40(%eax), %ecx\n"
        "push %ecx\n"
        "call sys_exit\n"
        "add $4, %esp\n"
        ".done:\n"
        "popa\n"
        "iret\n"
    );
}
