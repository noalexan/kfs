#pragma once

#include <types.h>

#include <register.h>

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
	uint16_t   limit;
	idt_entry *base;
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

extern idtr_t           idtr;
extern idt_entry *const idt_entries;
extern const char      *interrupt_names[];

typedef void (*irqHandler)(REGISTERS registers, int interrupt, int error);
typedef void (*syscallHandler)(REGISTERS registers);

void idt_init(void);
void idt_register_interrupt_handlers(uint8_t num, irqHandler handler);
