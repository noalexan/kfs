#include <drivers/vga.h>
#include <kernel/panic.h>
#include <libft.h>
#include <memory/memory.h>
#include <memory/vmm.h>

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

#define PD_SLOT                1022
#define FIRST_PAGE_TABLE_VADDR (PD_SLOT << 22)
#define PAGE_DIR_VADDR         ((PD_SLOT << 22) | (PD_SLOT << 12))
#define GET_PT_WITH_INDEX(idx) (FIRST_PAGE_TABLE_VADDR + (idx * PAGE_SIZE))

/////////////////////////////////////////////////
// Internal APIs

/////////////////////////////////////////////////
// External APIs

void page_fault_handler(REGISTERS reg, int interrupt, int error)
{
	uint32_t faulting_address;
	uint32_t error_code;

	(void)interrupt;
	(void)error;
	(void)reg;
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

void vmm_finalize(void)
{
	uintptr_t page_dir_phys =
	    (uintptr_t)boot_alloc_at(PAGE_SIZE, DMA_ZONE, TO_KEEP, 0x0, MiB_SIZE * 4, PAGE_SIZE);
	if (!page_dir_phys)
		kpanic("Failed PD alloc");

	uint32_t *page_dir_ptr = (uint32_t *)page_dir_phys;
	ft_bzero(page_dir_ptr, PAGE_SIZE);

	const uint32_t pt_count = 224;
	uintptr_t pt_pool_phys  = (uintptr_t)boot_alloc_at(pt_count * PAGE_SIZE, DMA_ZONE, TO_KEEP, 0x0,
	                                                   MiB_SIZE * 4, PAGE_SIZE);
	if (!pt_pool_phys)
		kpanic("Failed PT pool alloc");

	/*-----------------------------------------------------------
	    -   In pure higher half kernel this part is not needed but actually i dont know
	        why commnt the following code create an triple fault
	    -   Maybe timer interrupt or something else
	    - ??>????ZD?SD?S?gfdvasd?fbasd?fb fUCK
	*/
	uintptr_t identity_pt_phys =
	    (uintptr_t)boot_alloc_at(PAGE_SIZE, DMA_ZONE, TO_KEEP, 0x0, MiB_SIZE * 4, PAGE_SIZE);
	if (!identity_pt_phys)
		kpanic("Failed identity PT alloc");

	page_dir_ptr[0]           = identity_pt_phys | PDE_PRESENT_BIT | PDE_RW_BIT;
	uint32_t *identity_pt_ptr = (uint32_t *)identity_pt_phys;
	ft_bzero(identity_pt_ptr, PAGE_SIZE);
	for (int k = 0; k < 1024; k++) {
		identity_pt_ptr[k] = (k * PAGE_SIZE) | PTE_PRESENT_BIT | PTE_RW_BIT;
	}

	//-----------------------------------------------------------

	for (uint32_t i = 0; i < pt_count; i++) {
		uint32_t pde_idx = 768 + i;

		uintptr_t current_pt_phys_addr = pt_pool_phys + (i * PAGE_SIZE);

		page_dir_ptr[pde_idx] = current_pt_phys_addr | PDE_PRESENT_BIT | PDE_RW_BIT;

		uint32_t *current_pt_ptr = (uint32_t *)current_pt_phys_addr;
		ft_bzero(current_pt_ptr, PAGE_SIZE);

		for (uint32_t j = 0; j < 1024; j++) {
			uint32_t p_addr   = (i * 4 * MiB_SIZE) + (j * PAGE_SIZE);
			current_pt_ptr[j] = p_addr | PTE_PRESENT_BIT | PTE_RW_BIT;
		}
	}
	paging_reload_cr3(page_dir_phys);
}

// uintptr_t paging_virt_to_phy(uintptr_t v_addr)
// {
// 	uint32_t  pde_idx  = GET_PDE_INDEX(v_addr);
// 	uint32_t  pte_idx  = GET_PTE_INDEX(v_addr);
// 	uint32_t *page_dir = (uint32_t *)PAGE_DIR_VADDR;

// 	if (!(page_dir[pde_idx] & PDE_PRESENT_BIT))
// 		return 0;

// 	uint32_t *page_table = (uint32_t *)GET_PT_WITH_INDEX(pte_idx);

// 	if (!(page_table[pte_idx] & PTE_PRESENT_BIT))
// 		return 0;

// 	uint32_t page_phys = page_table[pte_idx] & ~0xFFF;
// 	uint32_t offset    = v_addr & 0xFFF;

// 	return page_phys + offset;
// }

// void paging_cleanup(void)
// {
// 	page_descriptors   = (page_t *)(((uintptr_t)page_descriptors) + KERNEL_VADDR_BASE);
// 	uint32_t *page_dir = (uint32_t *)PAGE_DIR_VADDR;

// 	if (!(page_dir[0] & PDE_PRESENT_BIT)) {
// 		vga_printf("PDE[0] already not present\n");
// 		return;
// 	}

// 	uint32_t *page_table = (uint32_t *)GET_PT_WITH_INDEX(0);

// 	for (int pte_idx = 0; pte_idx < 1024; pte_idx++) {
// 		if (page_table[pte_idx] & PTE_PRESENT_BIT) {
// 			uint32_t vaddr = pte_idx * PAGE_SIZE;
// 			// vga_printf("Unmapping 0x%x\n", vaddr);

// 			page_table[pte_idx] = 0;
// 			asm volatile("invlpg (%0)" ::"r"(vaddr));
// 		}
// 	}
// 	page_dir[0] = 0;
// 	paging_flush_TLB();

// 	boot_alloc_clean_up();
// }
