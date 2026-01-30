// gfx.h
#ifndef BEXNUS_GFX_H
#define BEXNUS_GFX_H

#include <stdint.h>

void gfx_init_mode13(void);
void gfx_putpixel(int x, int y, uint8_t color);
void gfx_clear(uint8_t color);

#endif
