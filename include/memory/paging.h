#pragma once

// ============================================================================
// IMCLUDES
// ============================================================================

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

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

#define KERNEL_VADDR             0xC0000000
#define PAGE_DESCRIPTORS_VADDR   0xC1000000
#define PD_SLOT                  1022
#define KERNEL_PAGE_TABLES_VADDR (PD_SLOT << 22)
#define KERNEL_PAGE_DIR_VADDR    (KERNEL_PAGE_TABLES_VADDR | (PD_SLOT << 12))

// Macros

#define GET_PDE_INDEX(vaddr) (vaddr >> 22)
#define GET_PTE_INDEX(vaddr) ((vaddr >> 12) & 0x3FF)
#define GET_PT_FROM_PDE(pde) ((uint32_t *)(pde & ~0xFFF))

// ============================================================================
// STRUCT
// ============================================================================

// Enums

// Structures

// Typedefs

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// ============================================================================
// EXTERNAL APIs
// ============================================================================
