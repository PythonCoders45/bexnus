// gfx.c
#include "gfx.h"

static uint8_t *fb = (uint8_t*)0xA0000;

void gfx_init_mode13(void) {
    __asm__ volatile (
        "mov $0x13, %ax\n"
        "int $0x10\n"
    );
}

void gfx_putpixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= 320 || y < 0 || y >= 200) return;
    fb[y * 320 + x] = color;
}

void gfx_clear(uint8_t color) {
    for (int i = 0; i < 320*200; i++) fb[i] = color;
}
