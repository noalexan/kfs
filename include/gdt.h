#pragma once

#include <types.h>

#define GDT_BASE (gdt_entry *)0x00000800
#define GDT_ENTRY(index) (GDT_BASE + index)

typedef struct __attribute__((packed)) s_gdt_entry {
	u16 llimit;
	u16 lbase;
	u8 base;
	u8 access;
	u8 hlimit : 4;
	u8 flags : 4;
	u8 hbase;
} gdt_entry;

void init_gdt();
