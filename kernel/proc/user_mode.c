#include "../arch/x86/gdt.h"

void enter_user_mode(uint32_t entry, uint32_t user_stack_top) {
    __asm__ volatile (
        "cli\n"
        "mov %0, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        :
        : "i"(GDT_USER_DATA | 0x3)
        : "ax"
    );

    __asm__ volatile (
        "pushl %0\n"
        "pushl %1\n"
        "pushl $0x202\n"
        "pushl %2\n"
        "pushl %3\n"
        "iret\n"
        :
        : "i"(GDT_USER_DATA | 0x3),
          "r"(user_stack_top),
          "i"(GDT_USER_CODE | 0x3),
          "r"(entry)
    );
}
