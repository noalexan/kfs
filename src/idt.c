#include <idt.h>

void aa();
void ab();
void ac();
void ad();
void ae();
void af();
void ag();
void ah();
void ai(int);
void aj();
void ak();
void al();
void am();
void an();
void ao();
void ap();
void aq();
void ar();
void as();
void at();
void au();
void av();
void aw();
void ax();
void ay();
void az();
void ba();
void bb();
void bc();
void bd();
void be();
void bf();

static void set_idt_entry(idt_entry *entry, u32 offset, u8 flags)
{
	entry->loffset = (u32)offset & 0xffff;
	entry->segment = 0x08;
	entry->flags = 128 | flags;
	entry->hoffset = (u32)offset >> 16;
	entry->reserved = 0;
}

void init_idt()
{
	set_idt_entry(IDT_ENTRY(0), (u32)aa, 0x0E);
	set_idt_entry(IDT_ENTRY(1), (u32)ab, 0x0E);
	set_idt_entry(IDT_ENTRY(2), (u32)ac, 0x0E);
	set_idt_entry(IDT_ENTRY(3), (u32)ad, 0x0E);
	set_idt_entry(IDT_ENTRY(4), (u32)ae, 0x0E);
	set_idt_entry(IDT_ENTRY(5), (u32)af, 0x0E);
	set_idt_entry(IDT_ENTRY(6), (u32)ag, 0x0E);
	set_idt_entry(IDT_ENTRY(7), (u32)ah, 0x0E);
	set_idt_entry(IDT_ENTRY(8), (u32)ai, 0x0E);
	set_idt_entry(IDT_ENTRY(9), (u32)aj, 0x0E);
	set_idt_entry(IDT_ENTRY(10), (u32)ak, 0x0E);
	set_idt_entry(IDT_ENTRY(11), (u32)al, 0x0E);
	set_idt_entry(IDT_ENTRY(12), (u32)am, 0x0E);
	set_idt_entry(IDT_ENTRY(13), (u32)an, 0x0E);
	set_idt_entry(IDT_ENTRY(14), (u32)ao, 0x0E);
	set_idt_entry(IDT_ENTRY(15), (u32)ap, 0x0E);
	set_idt_entry(IDT_ENTRY(16), (u32)aq, 0x0E);
	set_idt_entry(IDT_ENTRY(17), (u32)ar, 0x0E);
	set_idt_entry(IDT_ENTRY(18), (u32)as, 0x0E);
	set_idt_entry(IDT_ENTRY(19), (u32)at, 0x0E);
	set_idt_entry(IDT_ENTRY(20), (u32)au, 0x0E);
	set_idt_entry(IDT_ENTRY(21), (u32)av, 0x0E);
	set_idt_entry(IDT_ENTRY(22), (u32)aw, 0x0E);
	set_idt_entry(IDT_ENTRY(23), (u32)ax, 0x0E);
	set_idt_entry(IDT_ENTRY(24), (u32)ay, 0x0E);
	set_idt_entry(IDT_ENTRY(25), (u32)az, 0x0E);
	set_idt_entry(IDT_ENTRY(26), (u32)ba, 0x0E);
	set_idt_entry(IDT_ENTRY(27), (u32)bb, 0x0E);
	set_idt_entry(IDT_ENTRY(28), (u32)bc, 0x0E);
	set_idt_entry(IDT_ENTRY(29), (u32)bd, 0x0E);
	set_idt_entry(IDT_ENTRY(30), (u32)be, 0x0E);
	set_idt_entry(IDT_ENTRY(31), (u32)bf, 0x0E);


	struct idt_descriptor *idt_descriptor = (struct idt_descriptor *)0x00002000;
	idt_descriptor->address = (u32)IDT_BASE;
	idt_descriptor->size = sizeof(idt_entry) * IDT_MAX_ENTRIES - 1;

	asm volatile ("lidt [0x00002000]");
	asm volatile ("sti");
}
