#pragma once

#include <register.h>
#include <types.h>

typedef struct gdt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

typedef struct __attribute__((packed)) {
	uint16_t limit;
	uint32_t base;
} idtr_t;

typedef void (*irqHandler)(REGISTERS registers, int interrupt, int error);

void             x86_init(uint32_t magic, uint32_t mbi_addr);
const gdt_ptr_t *gdtr_getter(void);
const idtr_t    *idtr_getter(void);
void             idt_register_interrupt_handlers(uint8_t num, irqHandler handler);
