void user_test(void) {
    while (1) {
        __asm__ volatile (
            "mov $1, %eax\n"
            "mov $msg, %ebx\n"
            "int $0x80\n"
        );
    }

msg:
    __asm__(".string \"Hello from user mode!\\n\"");
}
