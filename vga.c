// vga.c — VGA text mode driver with scrolling

#include <stdint.h>

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static uint16_t* const vga = (uint16_t*)VGA_MEMORY;
static int cursor_row = 0;
static int cursor_col = 0;

static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

void clear_screen(void) {
    for (int r = 0; r < VGA_HEIGHT; r++) {
        for (int c = 0; c < VGA_WIDTH; c++) {
            vga[r * VGA_WIDTH + c] = vga_entry(' ', 0x07);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}

static void scroll(void) {
    // Move each row up by one
    for (int r = 1; r < VGA_HEIGHT; r++) {
        for (int c = 0; c < VGA_WIDTH; c++) {
            vga[(r - 1) * VGA_WIDTH + c] = vga[r * VGA_WIDTH + c];
        }
    }

    // Clear the last row
    for (int c = 0; c < VGA_WIDTH; c++) {
        vga[(VGA_HEIGHT - 1) * VGA_WIDTH + c] = vga_entry(' ', 0x07);
    }

    cursor_row = VGA_HEIGHT - 1;
}

void putc_at(int row, int col, char c) {
    vga[row * VGA_WIDTH + col] = vga_entry(c, 0x07);
}

void putc(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else {
        putc_at(cursor_row, cursor_col, c);
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }

    // If we hit the bottom, scroll
    if (cursor_row >= VGA_HEIGHT) {
        scroll();
    }
}
