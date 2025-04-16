#include "isr.h"
#include "io.h"
#include "vga.h"

isrHandler int_handler[256];

void kpanic(char *msg)
{
	// REGISTERS reg;
	// save_stack(&reg);

	// print_stack();
	// vga_color_set_text(VGA_RED);
	// vga_put("Kernel panic at ");
	// vga_color_set_text(VGA_BRIGHT_CYAN);
	// vga_put(file);
	// vga_color_reset();
	// vga_put(":");
	// vga_color_set_text(VGA_YELLOW);
	// vga_printf("%u", line);
	// vga_color_reset();
	// vga_put(" - ");
	// vga_put(msg);
	// vga_put("\n");
	// clean_registers();
	printk(msg);
	while (1) {
		asm volatile("hlt");
	}
}

// Exception msg
char *exception_messages[32] = {"Division By Zero",
                                "Debug",
                                "Non Maskable Interrupt",
                                "Breakpoint",
                                "Overflow",
                                "BOUND Range Exceeded",
                                "Invalid Opcode",
                                "Device Not Available (No Math Coprocessor)",
                                "Double Fault", // yes
                                "Coprocessor Segment Overrun",
                                "Invalid TSS",         // yes
                                "Segment Not Present", // yes
                                "Stack-Segment Fault", // yes
                                "General Protection",  // yes
                                "Page Fault",          // yes
                                "Reserved",
                                "x87 FPU Floating-Point Error (Math Fault)",
                                "Alignment Check",
                                "Machine Check",
                                "SIMD Floating-Point Exception",
                                "Virtualization Exception",
                                "Control Protection Exception", // yes
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Hypervisor Injection Exception",
                                "VMM Communication Exception", // yes
                                "Security Exception",          // yes
                                "Reserved"};

void clean_registers()
{
	asm volatile("xor %eax, %eax\n"
	             "xor %ebx, %ebx\n"
	             "xor %ecx, %ecx\n"
	             "xor %edx, %edx\n"
	             "xor %esi, %esi\n"
	             "xor %edi, %edi\n"
	             "xor %ebp, %ebp\n"
	             // "xor %esp, %esp\n"
	);
}

// void save_stack(REGISTERS *reg) {
//     asm volatile (
//         "mov %%esp, %0\n"
//         "mov %%eax, %1\n"
//         "mov %%ebx, %2\n"
//         "mov %%ecx, %3\n"
//         "mov %%edx, %4\n"
//         "mov %%esi, %5\n"
//         "mov %%edi, %6\n"
//         "mov %%ebp, %7\n"
//         : "=r" (reg->esp),
//           "=r" (reg->eax),
//           "=r" (reg->ebx),
//           "=r" (reg->ecx),
//           "=r" (reg->edx),
//           "=r" (reg->esi),
//           "=r" (reg->edi),
//           "=r" (reg->ebp)
//     );
// }

void isr_init() { memset(int_handler, 0, sizeof(int_handler)); }

void isr_register_interrupt_handler(int num, isrHandler handler)
{
	if (num < 256)
		int_handler[num] = handler;
}

void isr_irq_handler(REGISTERS *reg)
{
	if (reg->interrupt >= 40) {
		/* reset signal to slave */
		outb(0xA0, 0x20);
	}
	/* reset signal to master */
	outb(0x20, 0x20);

	if (int_handler[reg->interrupt] != NULL) {
		// vga_printf("Hardware intterupt\n");
		isrHandler handler = int_handler[reg->interrupt];
		handler(reg);
	}

	// vga_printf("IRQ : %d\n", reg->interrupt);
	// pic_eoi(reg->interrupt);
}

void isr_except_handler(REGISTERS reg)
{
	if (int_handler[reg.interrupt] != NULL) {
		isrHandler handler = int_handler[reg.interrupt];
		handler(&reg);
	} else if (reg.interrupt <= 32) {
		// vga_printf("Software intterupt\n");
		// vga_printf("int=%u err=%u\n", reg.interrupt, reg.error);
		kpanic(exception_messages[reg.interrupt]);
		while (1)
			;
	} else {
		kpanic("Invalid Interrupt Number");
	}
}
