#pragma once

#include <types.h>

#define GDT_DESCRIPTOR   ((gdt_descriptor *)0x00001000)
#define GDT_ENTRY(index) (gdt_base + index % 256)

enum GDT_ACCESS {
	GDT_ACCESS_CODE_READABLE         = 0x02,
	GDT_ACCESS_DATA_WRITABLE         = 0x02,

	GDT_ACCESS_CODE_CONFORMING       = 0x04,
	GDT_ACCESS_DATA_DIRECTION_NORMAL = 0x00,
	GDT_ACCESS_DATA_DIRECTION_DOWN   = 0x04,

	GDT_ACCESS_DATA_SEGMENT          = 0x10,
	GDT_ACCESS_CODE_SEGMENT          = 0x18,

	GDT_ACCESS_DESCRIPTOR_TSS        = 0x00,

	GDT_ACCESS_RING0                 = 0x00,
	GDT_ACCESS_RING1                 = 0x20,
	GDT_ACCESS_RING2                 = 0x40,
	GDT_ACCESS_RING3                 = 0x60,

	GDT_ACCESS_PRESENT               = 0x80
};

enum GDT_FLAGS {
	GDT_FLAGS_64BIT          = 0x20,
	GDT_FLAGS_32BIT          = 0x40,
	GDT_FLAGS_16BIT          = 0x00,

	GDT_FLAGS_GRANULARITY_1B = 0x00,
	GDT_FLAGS_GRANULARITY_4K = 0x80,
};

typedef struct {
	uint16_t llimit;
	uint16_t lbase;
	uint8_t  base;
	uint8_t  access;
	uint8_t  hlimit : 4;
	uint8_t  flags : 4;
	uint8_t  hbase;
} __attribute__((packed)) gdt_entry;

typedef struct {
	uint16_t   size;
	gdt_entry *ptr;
} __attribute__((packed)) gdt_descriptor;

gdt_entry                *gdt_base = (gdt_entry *)0x00000800;

void                      init_gdt();
