#include "idt.h"
#include <libft.h>

idt_entry idt_tab[IDT_MAX_ENTRY];

void      aa(void);
void      ab(void);
void      ac(void);
void      ad(void);
void      ae(void);
void      af(void);
void      ag(void);
void      ah(void);
void      ai(void);
void      aj(void);
void      ak(void);
void      al(void);
void      am(void);
void      an(void);
void      ao(void);
void      ap(void);
void      aq(void);
void      ar(void);
void      at(void);
void      au(void);
void      av(void);
void      aw(void);

#define set_idt_entry(index, selector, type_attr, offset)                      \
	idt_tab[index] = (idt_entry){                                              \
	    (uint16_t)((uintptr_t)offset & 0xFFFF), selector, 0, type_attr,        \
	    (uint16_t)(((uintptr_t)offset >> 16) & 0xFFFF)};

void init_idt(void)
{
	ft_bzero(idt_tab, sizeof(idt_tab));

	set_idt_entry(0x00, 0, 0, aa); // Divide Error
	set_idt_entry(0x01, 0, 0, ab); // Debug Exception
	set_idt_entry(0x02, 0, 0, ac); // NMI Interrupt
	set_idt_entry(0x03, 0, 0, ad); // Breakpoint
	set_idt_entry(0x04, 0, 0, ae); // Overflow
	set_idt_entry(0x05, 0, 0, af); // BOUND Range Exceeded
	set_idt_entry(0x06, 0, 0, ag); // Invalid Opcode (Undefined Opcode)
	set_idt_entry(0x07, 0, 0, ah); // Device Not Available (Math Coprocessor)
	set_idt_entry(0x08, 0, 0, ai); // Double Fault
	set_idt_entry(0x09, 0, 0, aj); // Coprocessor Segment Overrun (reserved)
	set_idt_entry(0x0A, 0, 0, ak); // Invalid TSS
	set_idt_entry(0x0B, 0, 0, al); // Segment Not Present
	set_idt_entry(0x0C, 0, 0, am); // Stack-Segment Fault
	set_idt_entry(0x0D, 0, 0, an); // General Protection
	set_idt_entry(0x0E, 0, 0, ao); // Page Fault
	set_idt_entry(0x0F, 0, 0, ap); // reserved. Do not use.
	set_idt_entry(0x10, 0, 0, aq); // x87 FPU Floating-Point Error (Math Fault)
	set_idt_entry(0x11, 0, 0, ar); // Alignment Check
	set_idt_entry(0x12, 0, 0, at); // Machine Check
	set_idt_entry(0x13, 0, 0, au); // SIMD Floating-Point Exception
	set_idt_entry(0x14, 0, 0, av); // Virtualization Exception
	set_idt_entry(0x15, 0, 0, aw); // Control Protection Exception
}
