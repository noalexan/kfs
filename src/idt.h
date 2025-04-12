#pragma once

#include <types.h>

#define IDT_MAX_ENTRY 256

typedef struct __attribute__((__packed__)) {
	uint16_t offset_1;        // offset bits 0..15
	uint16_t selector;        // a code segment selector in GDT or LDT
	uint8_t  zero;            // unused, set to 0
	uint8_t  type_attributes; // gate type, dpl, and p fields
	uint16_t offset_2;        // offset bits 16..31
} idt_entry;

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

extern idt_entry idt_tab[];
