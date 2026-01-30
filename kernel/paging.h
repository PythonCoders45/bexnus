// paging.h — Bexnus paging interface

#ifndef BEXNUS_PAGING_H
#define BEXNUS_PAGING_H

#include <stdint.h>

#define PAGE_SIZE      0x1000
#define PAGE_PRESENT   0x001
#define PAGE_RW        0x002
#define PAGE_USER      0x004

// Initialize paging with an identity-mapped kernel
void paging_init(void);

// Return the current page directory (kernel's)
uint32_t *paging_get_kernel_directory(void);

// Clone the current page directory for a new process
uint32_t *paging_clone_directory(void);

// Switch to a different page directory (CR3)
void paging_switch_directory(uint32_t *dir);

// Map a single page: virtual -> physical with flags
void paging_map_page(uint32_t *dir, uint32_t vaddr, uint32_t paddr);

// Unmap a single page
void paging_unmap_page(uint32_t *dir, uint32_t vaddr);

// Translate virtual address to physical (if mapped), or 0
uint32_t paging_translate(uint32_t *dir, uint32_t vaddr);

#endif
