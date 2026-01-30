#include "vga.h"
#include "io.h"
#include <stdint.h>

static uint16_t *vga = (uint16_t*)0xB8000;
static int row = 0, col = 0;

static void move_cursor(void) {
    uint16_t pos = row * 80 + col;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void clear_screen(void) {
    for (int i = 0; i < 80*25; i++)
        vga[i] = 0x0720;
    row = col = 0;
    move_cursor();
}

void putc(char c) {
    if (c == '\n') {
        row++;
        col = 0;
    } else {
        vga[row * 80 + col] = 0x0700 | c;
        col++;
        if (col >= 80) {
            col = 0;
            row++;
        }
    }

    if (row >= 25) {
        for (int i = 0; i < 24*80; i++)
            vga[i] = vga[i+80];
        for (int i = 24*80; i < 25*80; i++)
            vga[i] = 0x0720;
        row = 24;
    }

    move_cursor();
}

void puts(const char *s) {
    while (*s) putc(*s++);
}
