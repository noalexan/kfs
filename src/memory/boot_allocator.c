#include "boot_allocator.h"
#include "mb2_info.h"
#include "panic.h"
#include "gdt.h"
#include "idt.h"

/*
 *  The following APIs are used to store precoce allocations and to keep a track of evry used memory 
 *  before the memory system is available
 */

// Globals

extern uint8_t kernel_start[];
extern uint8_t kernel_end[];
boot_allocator_t allocator;

// APIs

/*
 * Adds a memory region (reserved or free) to the boot allocator's region tables
 * Panics if the region is invalid or if the maximum number of regions is exceeded
 */

static void boot_allocato_add_region(boot_allocator_t *alloc, uintptr_t start, uintptr_t end, enum mem_type type) {
    char *str_type = (type == RESERVED_MEMORY ? "reserved" : "free");
    if (start > end) {
        kpanic("Error: boot allocator - start address is greater than end address\n");
    }
    if (type == RESERVED_MEMORY) {
        if (alloc->reserved_count >= MAX_REGIONS)
            kpanic("Error: boot allocator too many %s regions registered\n", str_type);
        alloc->reserved_regions[alloc->reserved_count++] = (region_t){start, end};
        return ;
    }
    else {
        if (alloc->free_count >= MAX_REGIONS)
            kpanic("Error: boot allocator too many %s regions registered\n", str_type);
        alloc->free_regions[alloc->free_count++] = (region_t){start, end};
        return ;
    }
    kpanic("Error: boot_alloc: add_region fun unknown memory type\n");
}

/*
 * Wrapper function for adding a reserved region to the boot allocator
 * Used as a callback/handler for memory map iteration
 */

void boot_allocator_reserved_wrapper(uintptr_t start, uintptr_t end)
{
    boot_allocator_add_region(&allocator, start, end, RESERVED_MEMORY);
}

static bool overlaps(uintptr_t start, uintptr_t end, enum mem_type type)
{
    uint32_t count = (type == RESERVED_MEMORY ? allocator.reserved_count : allocator.free_count);
    region_t *reg = (type == RESERVED_MEMORY ? allocator.reserved_regions : allocator.free_regions);
	for (size_t i = 0; i < count; i++) {
		if (!(end <= reg[i].start || start >= reg[i].end)) {
			return true;
		}
	}
	return false;
}

void boot_allocator_free_handler(uintptr_t start, uintptr_t end)
{

}

/*
 * Initializes the boot allocator by marking all critical reserved regions 
 * (kernel, GDT, IDT, Multiboot info) before memory management is fully available
 */

void boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end)
{
    add_region(&allocator, kernel_start, kernel_end, RESERVED_MEMORY);
    add_region(&allocator, gdtr.base, (gdtr.limit + 1), RESERVED_MEMORY);
    add_region(&allocator, idtr.base, (idtr.limit + 1), RESERVED_MEMORY);
    add_region(&allocator, mb2info, (mb2info + mb2info->total_size), RESERVED_MEMORY);
    mb2_mmap_iter(mmap, mmap_end, boot_allocator_reserved_wrapper, false);
    mb2_mmap_iter(mmap, mmap_end, boot_allocator_reserved_wrapper, true);
}