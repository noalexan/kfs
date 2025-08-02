#include <libft.h>
#include <memory/memory.h>
#include <types.h>
#include <x86.h>

/////////////////////////////
// Structs & Defines

#define GDT_MAX_ENTRIES 7
#define GDT_ADDR        0x00000800 + KERNEL_VADDR_BASE
#define GDT_ENTRY(indx) (gdt_entries + (indx))
#define GDT_FLAGS       (SEGMENT_SIZE_BIT | GRANULARITY_BIT)

enum Gdt_Access_Byte {
	ACCESS_BIT      = 0b00000001, // Bit 0: Indique si le segment a été accédé par le CPU
	RW_BIT          = 0b00000010, // Bit 1: Lecture/écriture pour données, lecture pour code
	DC_BIT          = 0b00000100, // Bit 2: Direction pour données, Conforming pour code
	EXECUTABLE_BIT  = 0b00001000, // Bit 3: 1=segment de code, 0=segment de données
	DESCRIPTOR_TYPE = 0b00010000, // Bit 4: 1=segment code/données, 0=segment système
	DPL_RING0       = 0b00000000, // Bits 5-6: Niveau noyau, privilèges maximaux
	DPL_RING1       = 0b00100000, // Bits 5-6: Niveau intermédiaire, rarement utilisé
	DPL_RING2       = 0b01000000, // Bits 5-6: Niveau intermédiaire, rarement utilisé
	DPL_RING3       = 0b01100000, // Bits 5-6: Niveau utilisateur, privilèges minimaux
	PRESENT_BIT     = 0b10000000  // Bit 7: Indique si le segment est présent en mémoire
};

enum Gdt_Flags {
	RESERVED_BIT     = 0b0001, // Bit 0: Réservé, doit être à 0
	LONG_MODE_BIT    = 0b0010, // Bit 1: 1=segment 64-bit (mode long), 0=mode protégé standard
	SEGMENT_SIZE_BIT = 0b0100, // Bit 2: 0=16-bit segment, 1=32-bit segment
	GRANULARITY_BIT  = 0b1000  // Bit 3: 0=limite en octets, 1=limite en pages de 4Ko
};

typedef struct segment_descriptor {
	uint16_t limit_low;      // 0  -> 15
	uint16_t base_low;       // 16 -> 31
	uint8_t  base_middle;    // 32 -> 39
	uint8_t  access;         // 40 -> 47
	uint8_t  limit_high : 4; // 48 -> 51
	uint8_t  flags : 4;      // 51 -> 55
	uint8_t  base_high;      // 55 -> 63
} __attribute__((packed)) segment_descriptor_t;

///////////////////////////////////
// Code

gdt_ptr_t             gdtr;
segment_descriptor_t *gdt_entries = (segment_descriptor_t *)GDT_ADDR;

static inline void gdt_set_entry(segment_descriptor_t *gdt_entry, uint32_t base, uint32_t limit,
                                 uint8_t access, uint8_t flags)
{
	gdt_entry->limit_low   = (limit & 0xFFFF);
	gdt_entry->base_low    = (base & 0xFFFF);
	gdt_entry->base_middle = (base >> 16) & 0xFF;
	gdt_entry->access      = access;
	gdt_entry->limit_high  = (limit >> 16) & 0x0F;
	gdt_entry->flags       = flags & 0x0F;
	gdt_entry->base_high   = (base >> 24) & 0xFF;
}

const gdt_ptr_t *gdtr_getter(void) { return &gdtr; }

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
