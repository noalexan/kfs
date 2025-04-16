// https://samypesse.gitbook.io/how-to-create-an-operating-system/chapter-7

/*  Type Of Interrupts
 *--------------------
 *  Exceptions          -> Code Error       -> Trap Gate
 *  Hardware interrupt  -> Keyboard input   -> Interrupt Gate
 *  Software Interrupts -> Syscall          -> Trap Gate
 *  Interrup Gate Cannot handle another interrupt during the hanling
 *  Trap Gate Can handle an interrupt durring the first one
 */

#include "io.h"
#include "isr.h"
#include "lib.h"

#define GDT_KCODE_SEG    0x08
#define DEFAULT_INT_FLAG 0x8E

#define IDTBASE 0x00000000
#define IDTSIZE 0xFF

// #define FLAG_SET(x, flag) x |= (flag)
// #define FLAG_UNSET(x, flag) x &= ~(flag)

typedef struct {
	u16 limit;
	u32 base;
} __attribute__((packed)) idt_r_t;

typedef struct {
	u16 base_low;         // low bits from isr fun
	u16 segment_selector; // segment in the GDT
	u8  reserved;         // unused, always be zero
	u8  flags;            // types trap, interrupt gates
	u16 base_high;        // high bits from isr fun
} __attribute__((packed)) idt_entry_t;

idt_r_t     idt_r;        // IDTR
idt_entry_t idt[IDTSIZE]; // IDT

void idt_init_entry(u8 idx, u32 base, u16 segment_descriptor, u16 flags)
{
	idt[idx].base_low         = base & 0xFFFF;
	idt[idx].segment_selector = segment_descriptor;
	idt[idx].reserved         = 0;
	idt[idx].flags            = flags;
	idt[idx].base_high        = (base & 0xffff0000) >> 16;
}
void remap_irq_table(void)
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

void idt_init(void)
{
	memset(idt, 0, IDTSIZE * sizeof(idt_entry_t));
	remap_irq_table();

	idt_init_entry(0, (u32)isr_0, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(1, (u32)isr_1, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(2, (u32)isr_2, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(3, (u32)isr_3, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(4, (u32)isr_4, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(5, (u32)isr_5, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(6, (u32)isr_6, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(7, (u32)isr_7, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(8, (u32)isr_8, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(9, (u32)isr_9, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(10, (u32)isr_10, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(11, (u32)isr_11, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(12, (u32)isr_12, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(13, (u32)isr_13, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(14, (u32)isr_14, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(15, (u32)isr_15, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(16, (u32)isr_16, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(17, (u32)isr_17, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(18, (u32)isr_18, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(19, (u32)isr_19, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(20, (u32)isr_20, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(21, (u32)isr_21, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(22, (u32)isr_22, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(23, (u32)isr_23, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(24, (u32)isr_24, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(25, (u32)isr_25, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(26, (u32)isr_26, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(27, (u32)isr_27, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(28, (u32)isr_28, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(29, (u32)isr_29, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(30, (u32)isr_30, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(31, (u32)isr_31, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(32, (u32)irq_0, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(33, (u32)irq_1, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(34, (u32)irq_2, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(35, (u32)irq_3, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(36, (u32)irq_4, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(37, (u32)irq_5, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(38, (u32)irq_6, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(39, (u32)irq_7, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(40, (u32)irq_8, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(41, (u32)irq_9, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(42, (u32)irq_10, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(43, (u32)irq_11, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(44, (u32)irq_12, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(45, (u32)irq_13, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(46, (u32)irq_14, GDT_KCODE_SEG, DEFAULT_INT_FLAG);
	idt_init_entry(47, (u32)irq_15, GDT_KCODE_SEG, DEFAULT_INT_FLAG);

	idt_r.limit = ((IDTSIZE - 1) * sizeof(idt_entry_t));
	idt_r.base  = IDTBASE;

	/* Copy the IDT to the memory */
	memmove((void *)idt_r.base, idt, idt_r.limit);

	/* Load the IDTR registry */
	asm volatile("lidtl (idt_r)");

	// outb(0x21,0xfd);
	// outb(0xa1,0xff);

	asm("sti");
}