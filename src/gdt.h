#pragma once

#include <types.h>

#define GDT_MAX_ENTRIES 256
#define GDT_ADDR        0x00000800
#define GDT_ENTRY(indx) ((segment_descriptor_t *)gdt_entries + (indx))
#define GDT_FLAGS       (SEGMENT_SIZE_BIT | GRANULARITY_BIT)

enum Gdt_Access_Byte {
	ACCESS_BIT      = 0b00000001, // Bit 0: Indique si le segment a été accédé par le CPU
	RW_BIT          = 0b00000010, // Bit 1: Lecture/écriture pour données, lecture pour code
	DC_BIT          = 0b00000100, // Bit 2: Direction pour données, Conforming pour code
	EXECUTABLE_BIT  = 0b00001000, // Bit 3: 1=segment de code, 0=segment de données
	DESCRIPTOR_TYPE = 0b00010000, // Bit 4: 1=segment code/données, 0=segment système
	DPL_RING0       = 0b00000000, // Bits 5-6: Niveau noyau, privilèges maximaux
	DPL_RING1       = 0b00100000, // Bits 5-6: Niveau intermédiaire, rarement utilisé
	DPL_RING2       = 0b01000000, // Bits 5-6: Niveau intermédiaire, rarement utilisé
	DPL_RING3       = 0b01100000, // Bits 5-6: Niveau utilisateur, privilèges minimaux
	PRESENT_BIT     = 0b10000000  // Bit 7: Indique si le segment est présent en mémoire
};

enum Gdt_Flags {
	RESERVED_BIT     = 0b0001, // Bit 0: Réservé, doit être à 0
	LONG_MODE_BIT    = 0b0010, // Bit 1: 1=segment 64-bit (mode long), 0=mode protégé standard
	SEGMENT_SIZE_BIT = 0b0100, // Bit 2: 0=16-bit segment, 1=32-bit segment
	GRANULARITY_BIT  = 0b1000  // Bit 3: 0=limite en octets, 1=limite en pages de 4Ko
};

typedef struct segment_descriptor {
	uint16_t limit_low;      // 0  -> 15
	uint16_t base_low;       // 16 -> 31
	uint8_t  base_middle;    // 32 -> 39
	uint8_t  access;         // 40 -> 47
	uint8_t  limit_high : 4; // 48 -> 51
	uint8_t  flags : 4;      // 51 -> 55
	uint8_t  base_high;      // 55 -> 63
} __attribute__((packed)) segment_descriptor_t;

typedef struct gdt_ptr {
	uint16_t              limit;
	segment_descriptor_t *base;
} __attribute__((packed)) gdt_ptr_t;

extern segment_descriptor_t *const gdt_entries;
extern gdt_ptr_t *const            gdt_ptr;

void gdt_init(void);
