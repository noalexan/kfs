#pragma once

#include <acpi.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>
#include <kernel/panic.h>
#include <memory/memory.h>
#include <stdint.h>
#include <types.h>

// Structure pour le pointeur GDT
typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

// Fonctions de test GDT
void test_gdt_loaded(void);
void test_gdt_access(void);
void test_gdt_print_segment_registers(void);

// Fonctions de test IDT / exceptions
void test_idt_divide_by_zero(void);
void test_idt_general_protection(void);
void test_idt_overflow(void);
void test_idt_invalid_opcode(void);
void test_idt_all_exceptions(void);
void test_idt_page_fault(void);

// debug
void register_scroll_lock_on_debug();

// memory
void test_boot_alocator(void);
