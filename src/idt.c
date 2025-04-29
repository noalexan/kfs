#include "idt.h"
#include "acpi.h"
#include "io.h"
#include "keyboard.h"
#include "panic.h"
#include "printk.h"
#include "vga.h"
#include <libft.h>

idtr_t     idtr;
idt_entry *idt_entries = (idt_entry *)IDT_BASE;
irqHandler irq_handlers[256];

const char *interrupt_names[] = {"Divide Error",
                                 "Debug Exception",
                                 "NMI Interrupt",
                                 "Breakpoint",
                                 "Overflow",
                                 "BOUND Range Exceeded",
                                 "Invalid Opcode (Undefined Opcode)",
                                 "Device Not Available (Math Coprocessor)",
                                 "Double Fault",
                                 "Coprocessor Segment Overrun (reserved)",
                                 "Invalid TSS",
                                 "Segment Not Present",
                                 "Stack-Segment Fault",
                                 "General Protection",
                                 "Page Fault",
                                 "reserved. Do not use.",
                                 "x87 FPU Floating-Point Error (Math Fault)",
                                 "Alignment Check",
                                 "Machine Check",
                                 "SIMD Floating-Point Exception",
                                 "Virtualization Exception",
                                 "Control Protection Exception"};

extern void isr_0(void);
extern void isr_1(void);
extern void isr_2(void);
extern void isr_3(void);
extern void isr_4(void);
extern void isr_5(void);
extern void isr_6(void);
extern void isr_7(void);
extern void isr_8(void);
extern void isr_9(void);
extern void isr_10(void);
extern void isr_11(void);
extern void isr_12(void);
extern void isr_13(void);
extern void isr_14(void);
extern void isr_15(void);
extern void isr_16(void);
extern void isr_17(void);
extern void isr_18(void);
extern void isr_19(void);
extern void isr_20(void);
extern void isr_21(void);
extern void isr_22(void);
extern void isr_23(void);
extern void isr_24(void);
extern void isr_25(void);
extern void isr_26(void);
extern void isr_27(void);
extern void isr_28(void);
extern void isr_29(void);
extern void isr_30(void);
extern void isr_31(void);

extern void irq_0(void);
extern void irq_1(void);
// extern void irq_2(void);
// extern void irq_3(void);
// extern void irq_4(void);
// extern void irq_5(void);
// extern void irq_6(void);
// extern void irq_7(void);
// extern void irq_8(void);
// extern void irq_9(void);
// extern void irq_10(void);
// extern void irq_11(void);
// extern void irq_12(void);
// extern void irq_13(void);
// extern void irq_14(void);
// extern void irq_15(void);

void exception_handler(REGISTERS regs)
{
	vga_set_mode(VGA_COLOR(VGA_COLOR_BLACK, VGA_COLOR_RED));
	vga_disable_cursor();
	kpanic(interrupt_names[regs.interrupt]);
}

void idt_register_interrupt_handler(uint8_t num, irqHandler handler)
{
	irq_handlers[num] = handler;
}

void interrupt_handler(REGISTERS regs)
{
	if (regs.interrupt >= 40)
		outb(0xA0, 0x20);
	outb(0x20, 0x20);

	if (irq_handlers[regs.interrupt] != NULL)
		irq_handlers[regs.interrupt](&regs);
}

static void init_pic(void)
{
	/* Initialization of ICW1 */
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	/* Initialization of ICW2 */
	outb(0x21, 0x20); /* start vector = 32 */
	outb(0xA1, 0x28); /* start vector = 96 */

	/* Initialization of ICW3 */
	outb(0x21, 0x04);
	outb(0xA1, 0x02);

	/* Initialization of ICW4 */
	outb(0x21, 0x01);
	outb(0xA1, 0x01);

	/* mask interrupts */
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

static inline void idt_set_entry(idt_entry *ptr, uint16_t selector, uint8_t type, uint32_t offset)
{
	ptr->offset_1        = offset & 0xffff;
	ptr->offset_2        = (offset & 0xffff0000) >> 16;
	ptr->selector        = selector;
	ptr->type_attributes = type;
}

void idt_init(void)
{
	init_pic();

	ft_bzero(IDT_BASE, sizeof(idt_entry) * IDT_SIZE);

	idt_set_entry(IDT_ENTRY(0x00), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_0);
	idt_set_entry(IDT_ENTRY(0x01), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_1);
	idt_set_entry(IDT_ENTRY(0x02), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_2);
	idt_set_entry(IDT_ENTRY(0x03), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_3);
	idt_set_entry(IDT_ENTRY(0x04), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_4);
	idt_set_entry(IDT_ENTRY(0x05), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_5);
	idt_set_entry(IDT_ENTRY(0x06), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_6);
	idt_set_entry(IDT_ENTRY(0x07), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_7);
	idt_set_entry(IDT_ENTRY(0x08), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_8);
	idt_set_entry(IDT_ENTRY(0x09), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_9);
	idt_set_entry(IDT_ENTRY(0x0a), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_10);
	idt_set_entry(IDT_ENTRY(0x0b), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_11);
	idt_set_entry(IDT_ENTRY(0x0c), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_12);
	idt_set_entry(IDT_ENTRY(0x0d), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_13);
	idt_set_entry(IDT_ENTRY(0x0e), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_14);
	idt_set_entry(IDT_ENTRY(0x0f), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_15);
	idt_set_entry(IDT_ENTRY(0x10), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_16);
	idt_set_entry(IDT_ENTRY(0x11), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_17);
	idt_set_entry(IDT_ENTRY(0x12), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_18);
	idt_set_entry(IDT_ENTRY(0x13), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_19);
	idt_set_entry(IDT_ENTRY(0x14), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_20);
	idt_set_entry(IDT_ENTRY(0x15), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_21);
	idt_set_entry(IDT_ENTRY(0x16), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_22);
	idt_set_entry(IDT_ENTRY(0x17), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_23);
	idt_set_entry(IDT_ENTRY(0x18), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_24);
	idt_set_entry(IDT_ENTRY(0x19), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_25);
	idt_set_entry(IDT_ENTRY(0x1a), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_26);
	idt_set_entry(IDT_ENTRY(0x1b), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_27);
	idt_set_entry(IDT_ENTRY(0x1c), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_28);
	idt_set_entry(IDT_ENTRY(0x1d), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_29);
	idt_set_entry(IDT_ENTRY(0x1e), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_30);
	idt_set_entry(IDT_ENTRY(0x1f), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)isr_31);

	idt_set_entry(IDT_ENTRY(0x20), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_0);
	idt_set_entry(IDT_ENTRY(0x21), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_1);
	// idt_set_entry(IDT_ENTRY(0x20), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_2);
	// idt_set_entry(IDT_ENTRY(0x21), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_3);
	// idt_set_entry(IDT_ENTRY(0x20), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_4);
	// idt_set_entry(IDT_ENTRY(0x21), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_5);
	// idt_set_entry(IDT_ENTRY(0x20), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_6);
	// idt_set_entry(IDT_ENTRY(0x21), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_7);
	// idt_set_entry(IDT_ENTRY(0x20), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_8);
	// idt_set_entry(IDT_ENTRY(0x21), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_9);
	// idt_set_entry(IDT_ENTRY(0x20), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_10);
	// idt_set_entry(IDT_ENTRY(0x21), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_11);
	// idt_set_entry(IDT_ENTRY(0x20), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_12);
	// idt_set_entry(IDT_ENTRY(0x21), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_13);
	// idt_set_entry(IDT_ENTRY(0x20), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_14);
	// idt_set_entry(IDT_ENTRY(0x21), 0x08, PresentBit | IntGate_32 | CPU_Ring0, (uint32_t)irq_15);

	idtr.limit = sizeof(idt_entry) * (IDT_SIZE - 1);
	idtr.base  = IDT_BASE;

	__asm__ volatile("lidt %0" : : "m"(idtr));
	__asm__ volatile("sti");
}
