#include "gdt.h"
#include <libft.h>

gdt_ptr_t             gdtr;
segment_descriptor_t *gdt_entries = (segment_descriptor_t *)GDT_ADDR;

static inline void gdt_set_entry(segment_descriptor_t *gdt_entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	gdt_entry->limit_low   = (limit & 0xFFFF);
	gdt_entry->base_low    = (base & 0xFFFF);
	gdt_entry->base_middle = (base >> 16) & 0xFF;
	gdt_entry->access      = access;
	gdt_entry->limit_high  = (limit >> 16) & 0x0F;
	gdt_entry->flags       = flags & 0x0F;
	gdt_entry->base_high   = (base >> 24) & 0xFF;
}

// TODO : Implement TSS
void gdt_init(void)
{
	gdt_set_entry(GDT_ENTRY(0), 0x00, 0x00, 0x00, 0x00);
	// -------------- Kernel descriptors --------------
	// Kernel Segment
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

	// -------------- User descriptors --------------
	// User Code Segment
	gdt_set_entry(
	    GDT_ENTRY(4), 0x00, 0xFFFFFF,
	    (ACCESS_BIT | RW_BIT | EXECUTABLE_BIT | DESCRIPTOR_TYPE | DPL_RING3 | PRESENT_BIT),
	    GDT_FLAGS);
	// User Data Segment
	gdt_set_entry(GDT_ENTRY(5), 0x00, 0xFFFFFF,
	              (ACCESS_BIT | RW_BIT | DESCRIPTOR_TYPE | DPL_RING3 | PRESENT_BIT), GDT_FLAGS);
	// User Stack
	gdt_set_entry(GDT_ENTRY(6), 0x00, 0xFFFFFF,
	              (ACCESS_BIT | DPL_RING3 | DC_BIT | RW_BIT | PRESENT_BIT), GDT_FLAGS);

	gdtr.limit = (sizeof(segment_descriptor_t) * GDT_MAX_ENTRIES) - 1;
	gdtr.base  = (uint32_t)gdt_entries;

	// Register the GDT in the CPU
	__asm__ volatile("lgdtl (gdtr)");

	// Reload segment registers
	__asm__ volatile("movw $0x10, %ax \n" // Kernel data segment
	                 "movw %ax, %ds \n"
	                 "movw %ax, %es \n"
	                 "movw %ax, %fs \n"
	                 "movw %ax, %gs \n"
	                 "movw %ax, %ss \n"
	                 "ljmp $0x08, $next \n" // Long jump to reload CS
	                 "next:");
}
