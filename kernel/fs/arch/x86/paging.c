#include "paging.h"
#include <string.h>

static uint32_t *kernel_dir;

static inline void load_cr3(uint32_t phys) {
    __asm__ volatile ("mov %0, %%cr3" : : "r"(phys));
}

static inline void enable_paging(void) {
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));
}

void paging_map_page(uint32_t *dir, uint32_t vaddr, uint32_t paddr) {
    uint32_t dir_idx = vaddr >> 22;
    uint32_t tbl_idx = (vaddr >> 12) & 0x3FF;

    uint32_t *table;
    if (!(dir[dir_idx] & PAGE_PRESENT)) {
        table = (uint32_t*)0x400000; // simple static table base (adjust as needed)
        memset(table, 0, 0x1000);
        dir[dir_idx] = ((uint32_t)table) | PAGE_PRESENT | PAGE_RW;
    } else {
        table = (uint32_t*)(dir[dir_idx] & ~0xFFF);
    }

    table[tbl_idx] = (paddr & ~0xFFF) | PAGE_PRESENT | PAGE_RW;
}

void paging_init(void) {
    kernel_dir = (uint32_t*)0x300000; // static location for simplicity
    memset(kernel_dir, 0, 0x1000);

    // identity map first 4MB
    for (uint32_t addr = 0; addr < 0x400000; addr += PAGE_SIZE) {
        paging_map_page(kernel_dir, addr, addr);
    }

    load_cr3((uint32_t)kernel_dir);
    enable_paging();
}

uint32_t *paging_get_kernel_directory(void) {
    return kernel_dir;
}

uint32_t *paging_clone_directory(void) {
    uint32_t *new_dir = (uint32_t*)0x310000; // crude static clone
    memcpy(new_dir, kernel_dir, 0x1000);
    return new_dir;
}

void paging_switch_directory(uint32_t *dir) {
    load_cr3((uint32_t)dir);
}

void paging_unmap_page(uint32_t *dir, uint32_t vaddr) {
    uint32_t dir_idx = vaddr >> 22;
    uint32_t tbl_idx = (vaddr >> 12) & 0x3FF;

    if (!(dir[dir_idx] & PAGE_PRESENT)) return;
    uint32_t *table = (uint32_t*)(dir[dir_idx] & ~0xFFF);
    table[tbl_idx] = 0;
}

uint32_t paging_translate(uint32_t *dir, uint32_t vaddr) {
    uint32_t dir_idx = vaddr >> 22;
    uint32_t tbl_idx = (vaddr >> 12) & 0x3FF;

    if (!(dir[dir_idx] & PAGE_PRESENT)) return 0;
    uint32_t *table = (uint32_t*)(dir[dir_idx] & ~0xFFF);
    if (!(table[tbl_idx] & PAGE_PRESENT)) return 0;

    return (table[tbl_idx] & ~0xFFF) | (vaddr & 0xFFF);
}
