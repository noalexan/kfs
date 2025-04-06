.intel_syntax noprefix
.code32

.text
.global outw
.global outb
.global inb

outw:
	push edx
	mov edx, [esp+8]
	mov ax, [esp+12]
	outw dx, ax
	pop edx
	ret

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
