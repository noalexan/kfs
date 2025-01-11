#include <gdt.h>
#include <libft.h>

static void set_gdt_entry(gdt_entry *entry, u32 base, u32 limit, u8 flags, u8 access)
{
	entry->lbase = base;
	entry->base = base >> 16;
	entry->hbase = base >> 24;
	entry->llimit = limit;
	entry->hlimit = limit >> 16;
	entry->flags = flags;
	entry->access = access;
}

void init_gdt()
{
	int index = 0;

	ft_bzero(GDT_ENTRY(index++), sizeof(gdt_entry));

	set_gdt_entry(GDT_ENTRY(index++), 0x00000000, 0xfffff, GDT_FLAGS_32BIT | GDT_FLAGS_GRANULARITY_4K, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE);
	set_gdt_entry(GDT_ENTRY(index++), 0x00000000, 0xfffff, GDT_FLAGS_32BIT | GDT_FLAGS_GRANULARITY_4K, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE);

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

	GDT_DESCRIPTOR->ptr = GDT_BASE;
	GDT_DESCRIPTOR->size = index * sizeof(gdt_entry) - 1;

	asm volatile("lgdt [0x00001000]");
}
