.intel_syntax noprefix

# Multiboot2 header constants
.set MAGIC,         0xE85250D6
.set ARCHITECTURE,  0
.set HEADER_LENGTH, header_end - header_start
.set CHECKSUM,      -(MAGIC + ARCHITECTURE + HEADER_LENGTH)

# Multiboot2 header
.section .multiboot
.align 8
header_start:
	.long MAGIC
	.long ARCHITECTURE
	.long HEADER_LENGTH
	.long CHECKSUM

	# EFI boot services tag
	.short 7    # type
	.short 0    # flags
	.long 8     # size
	
	# End tag
	.short 0    # type
	.short 0    # flags
	.long 8     # size
header_end:

# The kernel entry point
.section .text
.global _start
.extern kernel_main

_start:
	call kernel_main
	hlt
