#include "idt.h"
#include "printk.h"
#include "vga.h"
#include <libft.h>

idt_entry idt_tab[IDT_MAX_ENTRY];
idtr_t    idtr;

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

void exception_handler(int interrupt_number)
{
	vga_set_mode(VGA_COLOR(VGA_COLOR_DARK_GREY, VGA_COLOR_YELLOW));

	const char *interrupt_name = interrupt_names[interrupt_number];

	if (interrupt_name) {
		int len = ft_strlen(interrupt_name);

		vga_set_cursor_position(VGA_WIDTH - len, 0);
		printk("%s", interrupt_name);
	}

	else {
		vga_set_cursor_position(VGA_WIDTH - 14, 0);
		printk("interrupt [%x]", interrupt_number);
	}

	vga_disable_cursor();

	__asm__ volatile("cli; hlt");
}

inline static void ax(void) { printk("<ax>\n"); }

#define set_idt_entry(index, selector, type_attr, offset)                                          \
 idt_tab[index] = (idt_entry){(uint16_t)((uintptr_t)offset & 0xFFFF), selector, 0, type_attr,      \
	                          (uint16_t)(((uintptr_t)offset >> 16) & 0xFFFF)};

void isr_stub_0(void);
void isr_stub_1(void);
void isr_stub_2(void);
void isr_stub_3(void);
void isr_stub_4(void);
void isr_stub_5(void);
void isr_stub_6(void);
void isr_stub_7(void);
void isr_stub_8(void);
void isr_stub_9(void);
void isr_stub_10(void);
void isr_stub_11(void);
void isr_stub_12(void);
void isr_stub_13(void);
void isr_stub_14(void);
void isr_stub_15(void);
void isr_stub_16(void);
void isr_stub_17(void);
void isr_stub_18(void);
void isr_stub_19(void);
void isr_stub_20(void);
void isr_stub_21(void);
void isr_stub_22(void);
void isr_stub_23(void);
void isr_stub_24(void);
void isr_stub_25(void);
void isr_stub_26(void);
void isr_stub_27(void);
void isr_stub_28(void);
void isr_stub_29(void);
void isr_stub_30(void);
void isr_stub_31(void);

void idt_init(void)
{
	ft_bzero(idt_tab, sizeof(idt_tab));

	idtr.base  = (uintptr_t)idt_tab;
	idtr.limit = (uint16_t)sizeof(idt_tab) - 1;

	set_idt_entry(0x00, 0x0008, PresentBit | IntGate_32,
	              isr_stub_0); // Divide Error
	set_idt_entry(0x01, 0x0008, PresentBit | IntGate_32,
	              isr_stub_1); // Debug Exception
	set_idt_entry(0x02, 0x0008, PresentBit | IntGate_32,
	              isr_stub_2); // NMI Interrupt
	set_idt_entry(0x03, 0x0008, PresentBit | IntGate_32,
	              isr_stub_3); // Breakpoint
	set_idt_entry(0x04, 0x0008, PresentBit | IntGate_32,
	              isr_stub_4); // Overflow
	set_idt_entry(0x05, 0x0008, PresentBit | IntGate_32,
	              isr_stub_5); // BOUND Range Exceeded
	set_idt_entry(0x06, 0x0008, PresentBit | IntGate_32,
	              isr_stub_6); // Invalid Opcode (Undefined Opcode)
	set_idt_entry(0x07, 0x0008, PresentBit | IntGate_32,
	              isr_stub_7); // Device Not Available (Math Coprocessor)
	set_idt_entry(0x08, 0x0008, PresentBit | IntGate_32,
	              isr_stub_8); // Double Fault
	set_idt_entry(0x09, 0x0008, PresentBit | IntGate_32,
	              isr_stub_9); // Coprocessor Segment Overrun (reserved)
	set_idt_entry(0x0A, 0x0008, PresentBit | IntGate_32,
	              isr_stub_10); // Invalid TSS
	set_idt_entry(0x0B, 0x0008, PresentBit | IntGate_32,
	              isr_stub_11); // Segment Not Present
	set_idt_entry(0x0C, 0x0008, PresentBit | IntGate_32,
	              isr_stub_12); // Stack-Segment Fault
	set_idt_entry(0x0D, 0x0008, PresentBit | IntGate_32,
	              isr_stub_13); // General Protection
	set_idt_entry(0x0E, 0x0008, PresentBit | IntGate_32,
	              isr_stub_14); // Page Fault
	set_idt_entry(0x0F, 0x0008, PresentBit | IntGate_32,
	              isr_stub_15); // reserved. Do not use.
	set_idt_entry(0x10, 0x0008, PresentBit | IntGate_32,
	              isr_stub_16); // x87 FPU Floating-Point Error (Math Fault)
	set_idt_entry(0x11, 0x0008, PresentBit | IntGate_32,
	              isr_stub_17); // Alignment Check
	set_idt_entry(0x12, 0x0008, PresentBit | IntGate_32,
	              isr_stub_18); // Machine Check
	set_idt_entry(0x13, 0x0008, PresentBit | IntGate_32,
	              isr_stub_19); // SIMD Floating-Point Exception
	set_idt_entry(0x14, 0x0008, PresentBit | IntGate_32,
	              isr_stub_20); // Virtualization Exception
	set_idt_entry(0x15, 0x0008, PresentBit | IntGate_32,
	              isr_stub_21); // Control Protection Exception
	set_idt_entry(0x16, 0x0008, PresentBit | IntGate_32, isr_stub_22); // |
	set_idt_entry(0x17, 0x0008, PresentBit | IntGate_32, isr_stub_23); // |
	set_idt_entry(0x18, 0x0008, PresentBit | IntGate_32, isr_stub_24); // |
	set_idt_entry(0x19, 0x0008, PresentBit | IntGate_32, isr_stub_25); // |
	set_idt_entry(0x1A, 0x0008, PresentBit | IntGate_32,
	              isr_stub_26); // | reserved
	set_idt_entry(0x1B, 0x0008, PresentBit | IntGate_32,
	              isr_stub_27);                                        // | exceptions
	set_idt_entry(0x1C, 0x0008, PresentBit | IntGate_32, isr_stub_28); // |
	set_idt_entry(0x1D, 0x0008, PresentBit | IntGate_32, isr_stub_29); // |
	set_idt_entry(0x1E, 0x0008, PresentBit | IntGate_32, isr_stub_30); // |
	set_idt_entry(0x1F, 0x0008, PresentBit | IntGate_32, isr_stub_31); // |

	set_idt_entry(0x21, 0x0008, PresentBit | IntGate_32, ax);

	__asm__ volatile("lidt %0" : : "m"(idtr));
	__asm__ volatile("sti");
}
