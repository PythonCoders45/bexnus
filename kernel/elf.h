#ifndef BEXNUS_ELF_H
#define BEXNUS_ELF_H

#include <stdint.h>
#include "proc.h"

int elf_load(const uint8_t *data, struct proc *p);

#endif
