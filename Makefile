# Makefile — build Bexnus OS

AS = nasm
CC = i686-elf-gcc
LD = i686-elf-ld

CFLAGS = -ffreestanding -m32 -O2 -Wall -Wextra
LDFLAGS = -T linker.ld -nostdlib

all: bexnus.img

boot.bin: boot.asm
    $(AS) -f bin boot.asm -o boot.bin

pm_switch.o: pm_switch.asm
    $(AS) -f elf32 pm_switch.asm -o pm_switch.o

kernel.o: kernel.c
    $(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.bin: pm_switch.o kernel.o linker.ld
    $(LD) $(LDFLAGS) pm_switch.o kernel.o -o kernel.bin

bexnus.img: boot.bin kernel.bin
    cat boot.bin kernel.bin > bexnus.img

run: bexnus.img
    qemu-system-i386 -drive format=raw,file=bexnus.img

clean:
    rm -f *.o *.bin *.img
