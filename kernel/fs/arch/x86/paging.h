#ifndef BEXNUS_PAGING_H
#define BEXNUS_PAGING_H

#include <stdint.h>

#define PAGE_SIZE      0x1000
#define PAGE_PRESENT   0x001
#define PAGE_RW        0x002
#define PAGE_USER      0x004

void paging_init(void);
uint32_t *paging_get_kernel_directory(void);
uint32_t *paging_clone_directory(void);
void paging_switch_directory(uint32_t *dir);
void paging_map_page(uint32_t *dir, uint32_t vaddr, uint32_t paddr);
void paging_unmap_page(uint32_t *dir, uint32_t vaddr);
uint32_t paging_translate(uint32_t *dir, uint32_t vaddr);

#endif
