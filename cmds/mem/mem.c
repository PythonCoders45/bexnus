#include "../../kernel/vga.h"
#include "../../kernel/memory.h"
#include "../cmd.h"

void cmd_mem(const char *args) {
    for (int i = 0; i < memmap_count; i++) {
        puts("Region: ");
        print_hex(memmap[i].base);
        puts(" - ");
        print_hex(memmap[i].length);
        puts(" type ");
        print_number(memmap[i].type);
        putc('\n');
    }
}
