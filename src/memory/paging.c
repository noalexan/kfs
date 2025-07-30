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

void bootstrap_map_page(uint32_t *page_dir, uintptr_t v_addr, uintptr_t p_addr, uint32_t flags,
                        bool freeable)
{
	uint32_t pde_idx = GET_PDE_INDEX(v_addr);
	uint32_t pte_idx = GET_PTE_INDEX(v_addr);

	uint32_t *page_table;

	if (!(page_dir[pde_idx] & PDE_PRESENT_BIT)) {

		page_table = boot_alloc(1024 * sizeof(uint32_t), LOWMEM_ZONE, freeable);
		if (!page_table) {
			kpanic("map_page: Failed to allocate page table!");
		}
		ft_bzero(page_table, 1024 * sizeof(uint32_t));

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

static inline void paging_reload_cr3(uint32_t *pd_phys_addr)
{
	__asm__ volatile("mov %0, %%cr3" ::"r"(pd_phys_addr));
}

static inline void paging_invalid_TLB_addr(uint32_t addr)
{
	__asm__ volatile("invlpg (%0)" ::"r"(addr));
}

static inline void paging_flush_TLB(void)
{
	uint32_t cr3;
	__asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
	__asm__ volatile("mov %0, %%cr3" ::"r"(cr3));
}

uintptr_t paging_virt_to_phy(uintptr_t v_addr)
{
	uint32_t  pde_idx  = GET_PDE_INDEX(v_addr);
	uint32_t  pte_idx  = GET_PTE_INDEX(v_addr);
	uint32_t *page_dir = (uint32_t *)PAGE_DIR_VADDR;

	if (!(page_dir[pde_idx] & PDE_PRESENT_BIT))
		return 0;

	uint32_t *page_table = (uint32_t *)GET_PT_WITH_INDEX(pte_idx);

	if (!(page_table[pte_idx] & PTE_PRESENT_BIT))
		return 0;

	uint32_t page_phys = page_table[pte_idx] & ~0xFFF;
	uint32_t offset    = v_addr & 0xFFF;

	return page_phys + offset;
}

void paging_cleanup(void)
{
	__asm__ volatile("cli");
	gdt_ptr_t *old_gdtr_ptr = gdtr_getter();
	idtr_t    *old_idtr_ptr = idtr_getter();

	gdt_ptr_t higher_half_gdt;
	idtr_t    higher_half_idt;

	higher_half_gdt.limit = old_gdtr_ptr->limit;
	higher_half_gdt.base  = old_gdtr_ptr->base + KERNEL_VADDR_BASE;

	higher_half_idt.limit = old_idtr_ptr->limit;
	higher_half_idt.base  = old_idtr_ptr->base + KERNEL_VADDR_BASE;

	asm volatile("lgdt %0" : : "m"(higher_half_gdt));
	asm volatile("lidt %0" : : "m"(higher_half_idt));

	page_descriptors   = (page_t *)(((uintptr_t)page_descriptors) + KERNEL_VADDR_BASE);
	uint32_t *page_dir = (uint32_t *)PAGE_DIR_VADDR;

	// if (page_dir[0] & PDE_PRESENT_BIT)
	// 	page_dir[0] = 0;

	paging_flush_TLB();

	__asm__ volatile("sti");

	boot_alloc_clean_up();
}

void paging_init(void)
{
	uint32_t *page_dir = boot_alloc(1024 * sizeof(uint32_t), LOWMEM_ZONE, TO_KEEP);
	if (!page_dir) {
		kpanic("Failed to allocate Page Directory!");
	}
	ft_bzero(page_dir, 1024 * sizeof(uint32_t));

	// Linear Mapping
	for (uintptr_t addr = 0; addr < LOWMEM_END; addr += PAGE_SIZE) {
		bootstrap_map_page(page_dir, addr + KERNEL_VADDR_BASE, addr, PTE_PRESENT_BIT | PTE_RW_BIT,
		                   TO_KEEP);
	}
	// Identity Mapping
	for (uintptr_t addr = 0; addr < 4 * MiB_SIZE; addr += PAGE_SIZE)
		bootstrap_map_page(page_dir, addr, addr, PTE_PRESENT_BIT | PTE_RW_BIT, TO_FREE);

	// recurif mapping, here we use page dir as page table to simplify page dir access in futur
	page_dir[PD_SLOT] = (uint32_t)page_dir | PDE_PRESENT_BIT | PDE_RW_BIT;
	enter_higher_half(page_dir);
}
