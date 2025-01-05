#include <gdt.h>
#include <libft.h>

static void set_gdt_entry(gdt_entry *entry, u32 base, u32 limit, u8 flags, u8 access)  //, flag, limit, access)
{
	entry->lbase = base;
	entry->base = base >> 16;
	entry->hbase = base >> 24;
	entry->llimit = limit;
	entry->hlimit = limit >> 16;
	entry->flags = flags;
	entry->access = 0x80 | access;
}

void init_gdt()
{
	int index = 0;

	ft_bzero(GDT_ENTRY(index++), sizeof(gdt_entry));

	set_gdt_entry(GDT_ENTRY(index++), 0x00000000, 0xfffff, GRANULARITY | SIZE, SEGMENT_CODE | CODE_READABLE | ACCESS);
	set_gdt_entry(GDT_ENTRY(index++), 0x00000000, 0xfffff, 0b1100, SEGMENT_DATA | DATA_WRITABLE | ACCESS);

	// gdt_code:
	// .short 0xffff
	// .short 0x00
	// .byte 0x00
	// .byte 0b10011010
	// .byte 0b1100
	// .byte 0x00

	// gdt_data:
	// .short 0xffff
	// .short 0x00
	// .byte 0x00
	// .byte 0b10010010
	// .byte 0b11001111
	// .byte 0x00

	// gdt_stack:
	// .short 0xffff
	// .short 0x00
	// .byte 0x00
	// .byte 0b10010110
	// .byte 0b11001111
	// .byte 0x00

	// gdt_user_code:
	// .short 0xffff
	// .short 0x00
	// .byte 0x00
	// .byte 0b11111010
	// .byte 0b11001111
	// .byte 0x00

	// gdt_user_data:
	// .short 0xffff
	// .short 0x00
	// .byte 0x00
	// .byte 0b11110010
	// .byte 0b11001111
	// .byte 0x00

	// gdt_user_stack:
	// .short 0xffff
	// .short 0x00
	// .byte 0x00
	// .byte 0b11110110
	// .byte 0b11001111
	// .byte 0x00

	// gdt_end:

	// gdt_descriptor:
	// .short gdt_end - gdt_start - 1
	// .long gdt_start

	struct gdt_descriptor *gdt_descriptor = (struct gdt_descriptor *)GDT_ENTRY(index);
	gdt_descriptor->address = (u32)GDT_BASE;
	gdt_descriptor->size = (u32)gdt_descriptor - (u32)GDT_BASE;

	asm volatile("lgdt %0" : : "m"(gdt_descriptor));
}
