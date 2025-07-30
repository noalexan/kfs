#include <acpi.h>
#include <drivers/vga.h>
#include <kernel/panic.h>
#include <libft.h>
#include <memory/memory.h>
#include <memory/paging.h>
#include <register.h>
#include <utils.h>
#include <x86.h>

/*
 * Virtual-to-physical address translation (32-bit paging):
 * - Bits 31-22: Page Directory index
 * - Bits 21-12: Page Table index
 * - Bits 11-0 : Offset within 4KB page
 *
 * See: https://wiki.osdev.org/Paging#32-bit_Paging_(Protected_Mode)
 */

/////////////////////////////////////////////////
// Defines

// Macro

extern page_t *page_descriptors;
extern uint8_t kernel_start[], kernel_end[];
extern void    enter_higher_half(uint32_t *page_dir_phys);

const uint32_t kernel_vstart = 0xC0000000;
const uint32_t default_flag  = (PDE_RW_BIT);

/////////////////////////////////////////////////
// Internal APIs

void bootstrap_map_page(uint32_t *page_dir, uintptr_t v_addr, uintptr_t p_addr, uint32_t flags)
{
	uint32_t pde_idx = GET_PDE_INDEX(v_addr);
	uint32_t pte_idx = GET_PTE_INDEX(v_addr);

	uint32_t *page_table;

	if (!(page_dir[pde_idx] & PDE_PRESENT_BIT)) {

		page_table = boot_alloc(PAGE_SIZE, LOWMEM_ZONE);
		if (!page_table) {
			kpanic("map_page: Failed to allocate page table!");
		}
		ft_bzero(page_table, PAGE_SIZE);

		page_dir[pde_idx] = (uint32_t)page_table | PDE_PRESENT_BIT | PDE_RW_BIT;
	}

	page_table          = GET_PT_FROM_PDE(page_dir[pde_idx]);
	page_table[pte_idx] = p_addr | flags;
}

/////////////////////////////////////////////////
// External APIs

void page_fault_handler(void)
{
	uint32_t faulting_address;
	uint32_t error_code;

	asm volatile("mov %%cr2, %0" : "=r"(faulting_address));
	asm volatile("mov 4(%%ebp), %0" : "=r"(error_code));

	vga_printf("Faulting address: 0x%x\n", faulting_address);
	vga_printf("Error code: 0x%x\n", error_code);

	if (error_code & 0x1) {
		vga_printf("Cause: Protection violation\n");
	} else {
		vga_printf("Cause: Page not present\n");
	}
	kpanic("Page fault");
}

void paging_init(void)
{
	uint32_t *page_dir = boot_alloc(PAGE_SIZE, LOWMEM_ZONE);
	if (!page_dir) {
		kpanic("Failed to allocate Page Directory!");
	}
	ft_bzero(page_dir, PAGE_SIZE);

	// Identity Mapping && Higher Half Mapping (Memory Alias)
	for (uintptr_t addr = 0; addr < 4 * MiB_SIZE; addr += PAGE_SIZE) {
		bootstrap_map_page(page_dir, addr, addr, PTE_PRESENT_BIT | PTE_RW_BIT);
		bootstrap_map_page(page_dir, addr + KERNEL_VADDR, addr, PTE_PRESENT_BIT | PTE_RW_BIT);
	}

	uintptr_t page_descriptors_ptr = (uintptr_t)page_descriptors;
	size_t    desc_size            = MAX_PAGES * sizeof(page_t);

	for (uintptr_t offset = 0; offset < desc_size; offset += PAGE_SIZE) {
		uintptr_t p_addr = page_descriptors_ptr + offset;
		uintptr_t v_addr = PAGE_DESCRIPTORS_VADDR + offset;
		bootstrap_map_page(page_dir, v_addr, p_addr, PTE_PRESENT_BIT | PTE_RW_BIT);
	}
	// recurif mapping, here we use page dir as page table to simplify page dir access in futur
	page_dir[PD_SLOT] = (uint32_t)page_dir | PDE_PRESENT_BIT | PDE_RW_BIT;
	enter_higher_half(page_dir);
}
