.intel_syntax noprefix
.code32

.set MAGIC,         0xE85250D6
.set ARCHITECTURE,  0
.set HEADER_LENGTH, header_end - header_start
.set CHECKSUM,      -(MAGIC + ARCHITECTURE + HEADER_LENGTH)

#--------------------------
# --- Multiboot Section ---
.section .multiboot

.align 8
header_start:
.long MAGIC
.long ARCHITECTURE
.long HEADER_LENGTH
.long CHECKSUM
.short 7, 0
.long  8
.short 0, 0
.long  8
header_end:

#--------------------
# --- BSS Section ---
.section .bss

.align 4
page_directory_ptr:
    .long 0

#---------------------
# --- Text Section ---
.section .text

.global _start
.global enter_higher_half

.extern x86_init
.extern kernel_main
.extern kernel_stack_top

_start:
	lea esp, [kernel_stack_top]
	mov ebp, esp

	push ebx # ptr on mb2 info
	push eax # magic number set by grub for mb2 info

	call x86_init


# Security Loop
	cli
hang:
    hlt
    jmp hang


enter_higher_half:
	# Get page directory from C file
    mov eax, [esp + 4]
    mov [page_directory_ptr], eax

	# Load CR3
    mov eax, [page_directory_ptr]
    mov cr3, eax

	# Enable paging
    mov eax, cr0
    or  eax, 0x80000001
    mov cr0, eax

    push offset kernel_main
    ret
