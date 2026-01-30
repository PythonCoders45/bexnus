; boot.asm - Bexnus boot sector (BIOS, x86, MBR style)
; Assembled with: nasm -f bin boot.asm -o boot.bin

BITS 16
ORG 0x7C00

start:
    cli                     ; disable interrupts
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; simple stack just below us

    ; Print a tiny message using BIOS teletype (int 10h)
    mov si, msg
.print_char:
    lodsb
    or al, al
    jz .done_print
    mov ah, 0x0E            ; teletype output
    mov bh, 0x00
    mov bl, 0x07            ; light grey on black
    int 0x10
    jmp .print_char
.done_print:

    ; For now, just hang here.
    ; Later we’ll load the 32-bit kernel and jump to it.
.hang:
    hlt
    jmp .hang

msg: db "Bexnus booting...", 0

; Pad to 510 bytes, then add boot signature 0xAA55
times 510-($-$$) db 0
dw 0xAA55
