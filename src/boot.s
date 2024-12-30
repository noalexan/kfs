.intel_syntax noprefix
.global _start

.equ MAGIC_NUMBER, 0x1BADB002
.equ FLAGS,        0x00000003  # Align modules on page boundaries and provide memory map
.equ CHECKSUM,    -(MAGIC_NUMBER + FLAGS)
.equ VGA_MODE,    0x0314       # 800x600 16-bit color

.section .multiboot
.align 4
    .long MAGIC_NUMBER
    .long FLAGS
    .long CHECKSUM
    .long 0, 0, 0, 0, 0        # Reserved fields
    .long 0                    # Graphics mode (0 = text mode)
    .long 800, 600, 16         # Width, height, depth

.section .text
_start:
    # Set VGA mode
    mov ax, 0x4F02
    mov bx, VGA_MODE
    int 0x10

    # Your code here

.loop:
    jmp .loop
