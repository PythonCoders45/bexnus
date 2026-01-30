// kernel.c — first C code for Bexnus

// Write a character to VGA text mode memory
static void putc(int row, int col, char c) {
    unsigned char *video = (unsigned char*)0xB8000;
    int index = (row * 80 + col) * 2;
    video[index] = c;
    video[index + 1] = 0x07; // light grey on black
}

void kmain(void) {
    const char *msg = "Bexnus kernel running";
    int col = 0;

    while (*msg) {
        putc(0, col++, *msg++);
    }

    for (;;) {
        __asm__("hlt");
    }
}
