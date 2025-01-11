#pragma once

#include <types.h>

#define IDT_BASE ((idt_entry *)0x00001800)
#define IDT_DESCRIPTOR ((idt_descriptor *)0x00002000)
#define IDT_ENTRY(index) (IDT_BASE + index % 256)
#define IDT_MAX_ENTRIES 256

typedef struct {
	u16 loffset;
	u16 segment;
	u8 reserved;
	u8 flags;
	u16 hoffset;
} __attribute__((packed)) idt_entry;

typedef struct {
	u16 size;
	idt_entry *ptr;
} __attribute__((packed)) idt_descriptor;

enum IDT_FLAGS {
	IDT_FLAGS_GATE_TASK = 0x05,
	IDT_FLAGS_GATE_16BIT_INT = 0x06,
	IDT_FLAGS_GATE_16BIT_TRAP = 0x07,
	IDT_FLAGS_GATE_32BIT_INT = 0x0E,
	IDT_FLAGS_GATE_32BIT_TRAP = 0xF,

	IDT_FLAGS_RING0 = (0 << 5),
	IDT_FLAGS_RING1 = (1 << 5),
	IDT_FLAGS_RING2 = (2 << 5),
	IDT_FLAGS_RING3 = (3 << 5),

	IDT_FLAGS_PRESENT = 0x80,
};

void init_idt();
