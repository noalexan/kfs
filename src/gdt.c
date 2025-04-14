#include "gdt.h"
#include <libft.h>

segment_descriptor_t *const gdt_entries = (segment_descriptor_t *)GDT_ADDR;
gdt_ptr_t *const            gdt_ptr     = (gdt_ptr_t *)GDT_ENTRY(GDT_MAX_ENTRIES);

static inline void gdt_set_entry(segment_descriptor_t *gdt_entry, uint32_t base, uint32_t limit,
                                 uint8_t access, uint8_t flags)
{
	gdt_entry->limit_low   = (limit & 0xFFFF);
	gdt_entry->base_low    = (base & 0xFFFF);
	gdt_entry->base_middle = (base >> 16) & 0xFF;
	gdt_entry->access      = access;
	gdt_entry->limit_high  = (limit >> 16) & 0x0F;
	gdt_entry->flags |= ((flags << 4) & 0xF0);
	gdt_entry->base_high = (base >> 24) & 0xFF;
}

// TODO : Implement TSS
void gdt_init(void)
{
	ft_bzero(gdt_entries, sizeof(segment_descriptor_t) * GDT_MAX_ENTRIES);

	// Kernel Segment
	// --------------------------------------------------------------------------------
	gdt_set_entry(
	    GDT_ENTRY(1), 0x00, 0xFFFFFF,
	    (ACCESS_BIT | RW_BIT | EXECUTABLE_BIT | DESCRIPTOR_TYPE | DPL_RING0 | PRESENT_BIT),
	    GDT_FLAGS);
	// Kernel Data Segment
	gdt_set_entry(GDT_ENTRY(2), 0x00, 0xFFFFFF,
	              (ACCESS_BIT | RW_BIT | DESCRIPTOR_TYPE | DPL_RING0 | PRESENT_BIT), GDT_FLAGS);
	// Kernel Stack
	gdt_set_entry(GDT_ENTRY(3), 0x00, 0xFFFFFF,
	              (ACCESS_BIT | DPL_RING0 | DC_BIT | RW_BIT | PRESENT_BIT), GDT_FLAGS);

	// User Code Segment
	// ------------------------------------------------------------------------------
	gdt_set_entry(
	    GDT_ENTRY(4), 0x00, 0xFFFFFF,
	    (ACCESS_BIT | RW_BIT | EXECUTABLE_BIT | DESCRIPTOR_TYPE | DPL_RING3 | PRESENT_BIT),
	    GDT_FLAGS);
	// User Data Segment
	gdt_set_entry(GDT_ENTRY(5), 0x00, 0xFFFFFF,
	              (ACCESS_BIT | RW_BIT | DESCRIPTOR_TYPE | DPL_RING0 | PRESENT_BIT), GDT_FLAGS);
	// User Stack
	gdt_set_entry(GDT_ENTRY(6), 0x00, 0xFFFFFF,
	              (ACCESS_BIT | DPL_RING3 | DC_BIT | RW_BIT | PRESENT_BIT), GDT_FLAGS);

	gdt_ptr->limit = (sizeof(segment_descriptor_t) * GDT_MAX_ENTRIES) - 1;
	gdt_ptr->base  = gdt_entries;

	// Register the gtd in the BIOS
	__asm__ volatile("lgdtl [0x00001000]");

	// initialize the segments
	// __asm__ volatile("movw $0x10, %ax	\n" // takes index in gdt bit 3-15 (2*8) = kernel
	//                                         // data segment
	//                  "movw %ax, %ds		\n" // data segment register
	//                  "movw %ax, %es		\n" // extra segment register
	//                  "movw %ax, %fs		\n" // additional F
	//                  "movw %ax, %gs		\n" // additional G
	//                  "ljmp $0x08, $next	\n" // long jump needed to reload the cs  (code
	//                                         // segment) register takes index in gdt bit 3 - 15
	//                                         // (1*8)
	//                                         // = kernel code segment + pointer
	//                  "next:				\n");
}
