#include <memory/memory.h>

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

#define GET_PDE_ADDR(pde) ((pde) & 0xFFFFF000)
#define IS_ALIGN(addr)    (((addr) & 0xFFF) == 0)

//  Page Table Entry (PTE)
enum Page_Table_Entry {
	PTE_PRESENT_BIT = 0b0000000000000001, // Bit 0  : Present         (page is in memory)
	PTE_RW_BIT      = 0b0000000000000010, // Bit 1  : Read/Write      (1 = writable, 0 = read-only)
	PTE_US_BIT =
	    0b0000000000000100, // Bit 2  : User/Supervisor (1 = user mode, 0 = supervisor/kernel)
	PTE_PWT_BIT =
	    0b0000000000001000, // Bit 3  : Write-Through   (1 = write-through caching, 0 = write-back)
	PTE_PCD_BIT      = 0b0000000000010000, // Bit 4  : Cache-Disable   (1 = no cache for this page)
	PTE_ACCESSED_BIT = 0b0000000000100000, // Bit 5  : Accessed        (set by CPU on access)
	PTE_DIRTY_BIT    = 0b0000000001000000, // Bit 6  : Dirty           (set by CPU on write)
	PTE_PAT_BIT = 0b0000000010000000, // Bit 7  : PAT             (Page Attribute Table extension)
	PTE_GLOBAL_BIT = 0b0000000100000000, // Bit 8  : Global          (1 = global page, not flushed
	                                     // from TLB on context switch)
	PTE_AVAIL1_BIT = 0b0000001000000000, // Bit 9  : Available       (for OS use)
	PTE_AVAIL2_BIT = 0b0000010000000000, // Bit 10 : Available       (for OS use)
	PTE_AVAIL3_BIT =
	    0b0000100000000000 // Bit 11 : Available       (for OS use)
	                       // Bits 12-31 : Physical address of the page (aligned on 4KiB)
};

// Page Directory Entry (PDE)
enum Page_Directory_Entry {
	PDE_PRESENT_BIT  = 0b0000000000000001, // Bit 0  : Present
	PDE_RW_BIT       = 0b0000000000000010, // Bit 1  : Read/Write
	PDE_US_BIT       = 0b0000000000000100, // Bit 2  : User/Supervisor
	PDE_PWT_BIT      = 0b0000000000001000, // Bit 3  : Write-Through (PWT)
	PDE_PCD_BIT      = 0b0000000000010000, // Bit 4  : Cache-Disable (PCD)
	PDE_ACCESSED_BIT = 0b0000000000100000, // Bit 5  : Accessed
	PDE_ALWAYS0_BIT  = 0b0000000001000000, // Bit 6  : Always 0 (must be zero for 4KiB pages)
	PDE_PS_BIT       = 0b0000000010000000, // Bit 7  : Page Size (0 = 4KiB, 1 = 4MiB)
	PDE_AVAIL1_BIT   = 0b0000000100000000, // Bit 8  : Available for OS
	PDE_AVAIL2_BIT   = 0b0000001000000000, // Bit 9  : Available for OS
	PDE_AVAIL3_BIT   = 0b0000010000000000, // Bit 10 : Available for OS
	PDE_AVAIL4_BIT =
	    0b0000100000000000 // Bit 11 : Available for OS
	                       // Bits 12-31 : Physical address of the page table (aligned on 4KiB)
};

const uint32_t kernel_vstart = 0xC0000000;
const uint32_t default_flag  = (PDE_RW_BIT);
uint32_t       kernel_page_directory[1024] __attribute__((aligned(PAGE_SIZE)));
uint32_t       first_page_table[1024] __attribute__((aligned(PAGE_SIZE)));

/////////////////////////////////////////////////
// External APIs

void pagination_init(void)
{
	for (size_t i = 0; i < 1024; i++) {
		kernel_page_directory[i] = default_flag;
	}
	for (size_t i = 0; i < 1024; i++) {
		first_page_table[i] = ((i * PAGE_SIZE) | PTE_PRESENT_BIT | PTE_RW_BIT);
	}
	kernel_page_directory[0] = ((unsigned int)first_page_table) | PTE_PRESENT_BIT | PTE_RW_BIT;
}
