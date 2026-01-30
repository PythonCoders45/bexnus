#include "keyboard.h"
#include "io.h"
#include "pic.h"
#include "../drivers/vga.h"

static char keymap[128] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',
};

static char buffer[128];
static int buf_pos = 0;

void keyboard_handler(void) {
    uint8_t sc = inb(0x60);

    if (sc & 0x80) {
        pic_send_eoi(1);
        return;
    }

    char c = keymap[sc];
    if (c) {
        buffer[buf_pos++] = c;
    }

    pic_send_eoi(1);
}

char keyboard_getchar(void) {
    while (buf_pos == 0);
    char c = buffer[0];
    for (int i = 1; i < buf_pos; i++)
        buffer[i-1] = buffer[i];
    buf_pos--;
    return c;
}

void keyboard_init(void) {
    // nothing needed here yet
}
