; pm_switch.asm — switch to 32‑bit protected mode and jump to kmain
; Assembled with: nasm -f elf32 pm_switch.asm -o pm_switch.o

BITS 16

global pm_start
extern kmain

; -------------------------
; GDT definition (flat model)
; -------------------------

gdt_start:
    dq 0x0000000000000000     ; null descriptor

    ; code segment: base=0, limit=4GB, type=0x9A
    dq 0x00CF9A000000FFFF

    ; data segment: base=0, limit=4GB, type=0x92
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; -------------------------
; Entry point from boot.asm
; -------------------------

pm_start:
    cli
    lgdt [gdt_descriptor]

    ; enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; far jump to flush pipeline and enter 32-bit mode
    jmp 0x08:pm32_entry


; -------------------------
; 32-bit mode begins here
; -------------------------

BITS 32

pm32_entry:
    mov ax, 0x10        ; data segment selector
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x90000    ; temporary stack

    call kmain          ; jump into C kernel

.hang:
    hlt
    jmp .hang
