.intel_syntax noprefix
.code32

.set MAGIC,         0xE85250D6
.set ARCHITECTURE,  0
.set HEADER_LENGTH, header_end - header_start
.set CHECKSUM,      -(MAGIC + ARCHITECTURE + HEADER_LENGTH)

.section .multiboot

.align 8

header_start:

.long MAGIC
.long ARCHITECTURE
.long HEADER_LENGTH
.long CHECKSUM

.short 7
.short 0
.long  8

.short 0
.short 0
.long  8

header_end:

.section .text

.global _start
.extern gdt_init
.extern idt_init
.extern kernel_main

_start:
	lea esp, [0x00090000]
	mov ebp, esp

    push ebx	# ptr on mb2 info
    push eax	# magic number set by grub for mb2 info

	call kernel_main

	hlt
	jmp . - 1
