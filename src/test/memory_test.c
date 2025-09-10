#include <drivers/vga.h>
#include <kernel/panic.h>
#include <libft.h>
#include <memory/buddy.h> // Pour buddy_print_summary (optionnel)
#include <memory/kmalloc.h>
#include <memory/memory.h>
#include <memory/page.h>
#include <memory/slab.h> // Pour slab_print_summary (optionnel)
#include <test/mm_test.h>

// ============================================================================
//                                Mini Test Framework
// ============================================================================

#define ASSERT(condition, message)                                                                 \
	do {                                                                                           \
		if (!(condition)) {                                                                        \
			vga_printf("\nTEST FAILED in %s:\n   -> %s\n", __func__, message);                     \
			kpanic("Halting due to test failure.");                                                \
		}                                                                                          \
	} while (0)

#define TEST_PASSED(message) vga_printf("[OK] %s\n", message)

// ============================================================================
//                                Test Suite Functions
// ============================================================================

static void test_slab_basic_allocations(void)
{
	void *p1 = kmalloc(8, GFP_KERNEL);
	ASSERT(p1 != NULL, "Slab: Minimal allocation (8 bytes) failed.");

	void *p2 = kmalloc(100, GFP_KERNEL);
	ASSERT(p2 != NULL, "Slab: Medium allocation (100 bytes) failed.");

	ASSERT(p1 != p2, "Slab: Consecutive allocations returned the same pointer.");

	kfree(p1);
	kfree(p2);
}

static void test_buddy_basic_allocations(void)
{
	void *p1 = kmalloc(MAX_SLAB_SIZE + 1, GFP_KERNEL);
	ASSERT(p1 != NULL, "Buddy: Smallest buddy allocation failed.");

	void *p2 = kmalloc(PAGE_SIZE * 2, GFP_KERNEL);
	ASSERT(p2 != NULL, "Buddy: Multi-page allocation failed.");

	ASSERT(p1 != p2, "Buddy: Consecutive allocations returned the same pointer.");

	kfree(p1);
	kfree(p2);
}

static void test_ksize_and_kfree_integration(void)
{
	void *p_slab = kmalloc(30, GFP_KERNEL);
	ASSERT(p_slab != NULL, "ksize/slab: Allocation failed.");
	ASSERT(ksize(p_slab) == 32, "ksize/slab: Incorrect size for 30-byte request.");

	void *p_buddy = kmalloc(3000, GFP_KERNEL);
	ASSERT(p_buddy != NULL, "ksize/buddy: Allocation failed.");
	ASSERT(ksize(p_buddy) == 4096, "ksize/buddy: Incorrect size for 3000-byte request.");

	kfree(p_slab);
	kfree(p_buddy);

	void *p3 = kmalloc(128, GFP_KERNEL);
	ASSERT(p3 != NULL, "kfree integration: Re-allocation failed after freeing.");
	kfree(p3);
}

static void test_gfp_flags(void)
{
	size_t         test_size = 128;
	unsigned char *p_zero    = kmalloc(test_size, __GFP_ZERO);
	ASSERT(p_zero != NULL, "__GFP_ZERO: Allocation failed.");
	for (size_t i = 0; i < test_size; i++) {
		ASSERT(p_zero[i] == 0, "__GFP_ZERO: Memory was not zeroed.");
	}
	kfree(p_zero);

	void *p_dma = kmalloc(256, __GFP_DMA);
	ASSERT(p_dma != NULL, "__GFP_DMA: Allocation failed.");
	uintptr_t phys_addr = VIRT_TO_PHYS_LINEAR(p_dma);
	ASSERT(phys_addr < (16 * MiB_SIZE), "__GFP_DMA: Allocated memory is not in DMA zone.");
	kfree(p_dma);
}

static void test_edge_cases(void)
{
	void *p1 = kmalloc(0, GFP_KERNEL);
	ASSERT(p1 == NULL, "kmalloc(0) should return NULL.");

	void *p2 = kmalloc(MAX_KMALLOC_SIZE, GFP_KERNEL);
	ASSERT(p2 == NULL, "Allocation of MAX_KMALLOC_SIZE should fail.");

	kfree(NULL); // Should not panic.
}

static void test_stress_and_fragmentation(void)
{
#define STRESS_COUNT 500
	void *pointers[STRESS_COUNT];
	int   allocated_count = 0;

	for (int i = 0; i < STRESS_COUNT; i++) {
		pointers[i] = kmalloc(PAGE_SIZE, GFP_KERNEL);
		if (pointers[i] == NULL)
			break;
		allocated_count++;
	}
	ASSERT(allocated_count > 0, "Could not allocate even one page.");

	for (int i = 0; i < allocated_count; i++) {
		kfree(pointers[i]);
	}

	void *p_big = kmalloc(PAGE_SIZE * allocated_count / 2, GFP_KERNEL);
	ASSERT(p_big != NULL, "Large allocation failed after freeing all memory.");
	kfree(p_big);
}

// ============================================================================
//                                Main Test Runner
// ============================================================================

void test_minimal_buddy(void)
{
	void *p_slab = kmalloc(30, GFP_KERNEL);

	// --- DEBUG ---
	uintptr_t debug_phys = VIRT_TO_PHYS_LINEAR(p_slab);
	page_t   *debug_page = page_addr_to_page(debug_phys);
	vga_printf("DEBUG: After kmalloc(30), page state is %u\n", PAGE_GET_STATE(debug_page));
	// --- FIN DEBUG ---

	ASSERT(p_slab != NULL, "ksize/slab: Allocation failed.");
	ASSERT(ksize(p_slab) == 32, "ksize/slab: Incorrect size returned for 30-byte request.");
	// vga_printf("--- Testing Minimal Buddy Allocation ---\n");

	// // FAITES UNE SEULE ALLOCATION BUDDY
	// void* ptr = kmalloc(PAGE_SIZE, GFP_KERNEL); // Force une alloc buddy
	// ASSERT(ptr != NULL, "Minimal buddy test failed to allocate.");

	// uintptr_t phys_addr = VIRT_TO_PHYS_LINEAR(ptr);
	// page_t *page = page_addr_to_page(phys_addr);
	// size_t state = PAGE_GET_STATE(page);

	// vga_printf("Minimal buddy test: ptr virt %p, phys %p\n", ptr, (void*)phys_addr);
	// vga_printf("Minimal buddy test: page state is %u\n", state);

	// ASSERT(state == PAGE_STATE_ALLOCATED, "State is not ALLOCATED after buddy alloc!");

	// kfree(ptr);
	// TEST_PASSED("Minimal buddy allocation state");
}

void run_memory_tests(void)
{
	vga_printf("\nRunning Memory System Tests...\n");

	test_slab_basic_allocations();
	TEST_PASSED("Slab basic allocations");

	test_buddy_basic_allocations();
	TEST_PASSED("Buddy basic allocations");

	test_ksize_and_kfree_integration();
	TEST_PASSED("ksize and kfree integration");

	test_gfp_flags();
	TEST_PASSED("gfp_t flags (__GFP_ZERO, __GFP_DMA)");

	test_edge_cases();
	TEST_PASSED("Edge cases (0-size, oversized, NULL free)");

	test_stress_and_fragmentation();
	TEST_PASSED("Stress test (exhaustion and recovery)");

	vga_printf("\n All Memory System Tests Passed! 🎉🎉🎉\n");
}
