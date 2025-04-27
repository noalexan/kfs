#pragma once

#include <types.h>

#define IDT_BASE        0x00000000
#define IDT_SIZE        0xFF
#define IDT_ENTRY(indx) (idt_entries + (indx))

typedef struct __attribute__((packed)) {
	uint16_t offset_1;
	uint16_t selector;
	uint8_t  zero;
	uint8_t  type_attributes;
	uint16_t offset_2;
} idt_entry;

typedef struct __attribute__((packed)) {
	uint16_t limit;
	uint32_t base;
} idtr_t;

enum IDTTypeAttributes {
	TaskGate    = 0x05,
	IntGate_16  = 0x06,
	TrapGate_16 = 0x07,
	IntGate_32  = 0x0E,
	TrapGate_32 = 0x0F,

	CPU_Ring0 = 0x00 << 5,
	CPU_Ring1 = 0x01 << 5,
	CPU_Ring2 = 0x02 << 5,
	CPU_Ring3 = 0x03 << 5,

	PresentBit = 0x01 << 7
};

typedef struct {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t interrupt, error;
	uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) REGISTERS;

// idt_entry         *idt_entries;
extern const char *interrupt_names[];

typedef void (*irqHandler)(void *);

void idt_init(void);
void idt_register_interrupt_handler(uint8_t num, irqHandler handler);
