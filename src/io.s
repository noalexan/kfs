.intel_syntax noprefix

.text
.global outb
.global inb

outb:
	mov edx, [esp+4]
	mov eax, [esp+8]
	out dx, al
	ret

inb:
	mov edx, word [esp+4]
	in eax, edx
	ret
