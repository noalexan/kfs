#pragma once

// ============================================================================
//                                Memory System Tests
// ============================================================================
//
//      This file contains the comprehensive test suite for the kernel's
//      dynamic memory management system, including the slab allocator,
//      the buddy allocator, and the kmalloc/kfree/ksize API.
//
// ============================================================================

/**
 * @brief Runs the complete suite of memory system tests.
 *
 * This function executes a series of validation tests on the kernel's memory
 * allocators. It will cause a kernel panic via the ASSERT macro if any test fails.
 * If all tests complete successfully, it will print a confirmation message.
 */
void run_memory_tests(void);
void test_minimal_buddy(void);
