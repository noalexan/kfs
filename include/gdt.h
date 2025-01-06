#pragma once

#include <types.h>

#define GDT_BASE (gdt_entry *)0x00000800
#define GDT_ENTRY(index) (GDT_BASE + index % 256)

enum GDT_ACCESS {
	DPL_KERNEL = 0,
	DPL_USER = 3 << 5,

	SEGMENT_SYSTEM = 0,
	SEGMENT_DATA = 2 << 3,
	SEGMENT_CODE = 3 << 3,

	DATA_DIRECTION_UP = 0,
	DATA_DIRECTION_DOWN = 1 << 2,

	EXEC_DPL = 0,
	EXEC_LOW_OR_EQ_DPL = 1 << 2,

	CODE_READABLE = 1 << 1,
	DATA_WRITABLE = 1 << 1,

	ACCESS = 1
};

enum GDT_FLAGS {
	GRANULARITY = 1 << 3,
	SIZE = 1 << 2,
	LONG_MODE = 1 << 1,
};

typedef struct __attribute__((packed)) s_gdt_entry {
	u16 llimit;
	u16 lbase;
	u8 base;
	u8 access;
	u8 hlimit : 4;
	u8 flags : 4;
	u8 hbase;
} gdt_entry;

struct __attribute__((packed)) gdt_descriptor {
	u16 size;
	u32 address;
};

void init_gdt();
