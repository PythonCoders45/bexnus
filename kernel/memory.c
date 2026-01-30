// memory.c — BIOS e820 memory map

#include <stdint.h>

struct e820_entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
} __attribute__((packed));

struct e820_entry memmap[32];
int memmap_count = 0;

void memory_detect(void) {
    uint32_t contID = 0;
    memmap_count = 0;

    while (1) {
        uint32_t eax = 0xE820;
        uint32_t ebx = contID;
        uint32_t ecx = 24;
        uint32_t edx = 0x534D4150; // "SMAP"

        __asm__ volatile (
            "int $0x15"
            : "+a"(eax), "+b"(ebx), "+c"(ecx), "+d"(edx),
              "=m"(memmap[memmap_count])
        );

        if (eax != 0x534D4150) break;

        memmap_count++;
        if (ebx == 0) break;

        contID = ebx;
    }
}
