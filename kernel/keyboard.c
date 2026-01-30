// keyboard.c — basic keyboard driver for Bexnus

#include <stdint.h>

extern void putc_at(int row, int col, char c);  // from your VGA code
extern void idt_set_gate(int n, uint32_t handler);

static int cursor_col = 0;

static const char scancode_table[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0,' ',
    /* rest unused */
};

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

void keyboard_handler(void) {
    uint8_t sc = inb(0x60);

    if (sc & 0x80) {
        return; // key release, ignore for now
    }

    char c = scancode_table[sc];
    if (c) {
        putc_at(1, cursor_col++, c);
    }
}

void keyboard_init(void) {
    idt_set_gate(33, (uint32_t)keyboard_handler); // IRQ1 = interrupt 33
}
