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
	entry->loffset = offset;
	entry->segment = (0x01 << 3) | 0b000;
	entry->flags = flags;
	entry->hoffset = offset >> 16;
	entry->reserved = 0;
}

void init_idt()
{
	int index = 0;

	set_idt_entry(IDT_ENTRY(index++), (u32)aa, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ab, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ac, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ad, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ae, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)af, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ag, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ah, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ai, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)aj, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ak, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)al, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)am, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)an, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ao, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ap, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)aq, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ar, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)as, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)at, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)au, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)av, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)aw, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ax, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ay, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)az, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)ba, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)bb, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)bc, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)bd, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)be, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);
	set_idt_entry(IDT_ENTRY(index++), (u32)bf, IDT_FLAGS_RING0 | IDT_FLAGS_GATE_32BIT_INT | IDT_FLAGS_PRESENT);

	IDT_DESCRIPTOR->ptr = IDT_BASE;
	IDT_DESCRIPTOR->size = sizeof(idt_entry) * index - 1;

	asm volatile ("lidt [0x00002000]");
	asm volatile ("sti");
}
