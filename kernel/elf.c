#include "elf.h"
#include "paging.h"
#include <string.h>

struct elf_hdr {
    uint8_t  magic[4];
    uint8_t  cls, data, ver, osabi, abiver;
    uint8_t  pad[7];
    uint16_t type, machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
};

struct elf_phdr {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
};

int elf_load(const uint8_t *data, struct proc *p) {
    const struct elf_hdr *h = (const struct elf_hdr*)data;
    if (h->magic[0] != 0x7F || h->magic[1] != 'E' || h->magic[2] != 'L' || h->magic[3] != 'F')
        return -1;

    const struct elf_phdr *ph = (const struct elf_phdr*)(data + h->phoff);

    for (int i = 0; i < h->phnum; i++) {
        if (ph[i].type != 1) continue; // PT_LOAD

        for (uint32_t off = 0; off < ph[i].memsz; off += 0x1000) {
            paging_map_page(p->page_dir, ph[i].vaddr + off, ph[i].vaddr + off);
        }

        memcpy((void*)ph[i].vaddr, data + ph[i].offset, ph[i].filesz);
        if (ph[i].memsz > ph[i].filesz) {
            memset((void*)(ph[i].vaddr + ph[i].filesz), 0, ph[i].memsz - ph[i].filesz);
        }
    }

    p->ctx.eip = h->entry;
    return 0;
}
