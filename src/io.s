.intel_syntax noprefix

.text
.global outb
.global inb

outb:
	push edx
	mov edx, [esp+8]
	mov al, [esp+12]
	out dx, al
	pop edx
	ret

inb:
	push edx
	mov dx, [esp+8]
	in al, dx
	pop edx
	ret
