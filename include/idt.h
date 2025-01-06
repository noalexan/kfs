#pragma once

#include <types.h>

#define IDT_BASE (idt_entry *)0x00001800
#define IDT_ENTRY(index) (IDT_BASE + index % 256)
#define IDT_MAX_ENTRIES 256

typedef struct __attribute__((packed)) s_idt_entry {
	u16 loffset;
	u16 segment;
	u8 reserved;
	u8 flags;
	u16 hoffset;
} idt_entry;

struct __attribute__((packed)) idt_descriptor {
	u16 size;
	u32 address;
};

void init_idt();
