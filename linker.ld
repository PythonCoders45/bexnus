/* linker.ld — Bexnus kernel linker script */

ENTRY(kmain)

SECTIONS
{
    /* Load the kernel at 1MB (0x00100000) */
    . = 0x00100000;

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(COMMON)
        *(.bss*)
    }
}
