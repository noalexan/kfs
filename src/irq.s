.intel_syntax noprefix
.code32

.section .text
	.extern interrupt_handler

interrupt_routine:
	pusha

	xor eax, eax
	mov ax, ds
	push eax

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call interrupt_handler

	pop ebx
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx

	popa
	add esp, 0x08
	sti
	iret

.macro isq_stub num, vector
.global irq_\num
irq_\num:
	cli
	push 0
	push \vector
	jmp interrupt_routine
.endm

isq_stub  0, 32
isq_stub  1, 33
isq_stub  2, 34
isq_stub  3, 35
isq_stub  4, 36
isq_stub  5, 37
isq_stub  6, 38
isq_stub  7, 39
isq_stub  8, 40
isq_stub  9, 41
isq_stub 10, 42
isq_stub 11, 43
isq_stub 12, 44
isq_stub 13, 45
isq_stub 14, 46
isq_stub 15, 47
