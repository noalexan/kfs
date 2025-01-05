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

.section .text
.global _start
.extern kernel_main

_start:
	lea esp, [0x00090000]

	mov eax, 0b00000001
	mov cr0, eax
	cli

	call kernel_main

	hlt
