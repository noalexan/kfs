.intel_syntax noprefix

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
	.long 8

	.short 0
	.short 0
	.long 8

header_end:

.section .bss
.align 16
.skip 16384

stack_top:

.section .text
.global _start
.extern kernel_main

gdt_start:
.long 0x00
.long 0x00

gdt_code:
.long 0xffff
.long 0x00
.short 0x00
.short 0b10011010
.short 0b11001111
.short 0x00

gdt_data:
.long 0xffff
.short 0x00
.short 0x00
.short 0b10010010
.short 0b11001111
.short 0x00

gdt_end:

gdt_descriptor:
.long gdt_end - gdt_start - 1
.long gdt_start

_start:
	lea esp, stack_top
	lgdt [gdt_descriptor]

	mov eax, 0b00000001
	mov cr0, eax
	cli

	call kernel_main

	hlt
