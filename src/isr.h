#pragma once

#include "lib.h"
typedef struct {
	u32 ds;                                     // data segment pushed by us
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
	u32 interrupt, error;                       // interrupt number and error code
	u32 eip, cs, eflags, useresp, ss;           // pushed by the processor automatically
} __attribute__((packed)) REGISTERS;

typedef void (*isrHandler)(REGISTERS *);
void _kpanic(char *file, int line, char *msg);

// macro from irq.s
extern void irq_0(void);
extern void irq_1(void);
extern void irq_2(void);
extern void irq_3(void);
extern void irq_4(void);
extern void irq_5(void);
extern void irq_6(void);
extern void irq_7(void);
extern void irq_8(void);
extern void irq_9(void);
extern void irq_10(void);
extern void irq_11(void);
extern void irq_12(void);
extern void irq_13(void);
extern void irq_14(void);
extern void irq_15(void);

// macro from exception.s

extern void isr_0(void);
extern void isr_1(void);
extern void isr_2(void);
extern void isr_3(void);
extern void isr_4(void);
extern void isr_5(void);
extern void isr_6(void);
extern void isr_7(void);
extern void isr_8(void);
extern void isr_9(void);
extern void isr_10(void);
extern void isr_11(void);
extern void isr_12(void);
extern void isr_13(void);
extern void isr_14(void);
extern void isr_15(void);
extern void isr_16(void);
extern void isr_17(void);
extern void isr_18(void);
extern void isr_19(void);
extern void isr_20(void);
extern void isr_21(void);
extern void isr_22(void);
extern void isr_23(void);
extern void isr_24(void);
extern void isr_25(void);
extern void isr_26(void);
extern void isr_27(void);
extern void isr_28(void);
extern void isr_29(void);
extern void isr_30(void);
extern void isr_31(void);

void isr_init();

void isr_irq_handler(REGISTERS *reg);
// void    isr_except_handler(REGISTERS reg);
void clean_registers();
void isr_register_interrupt_handler(int num, isrHandler handler);
void kpanic(char *msg);
// void    save_stack(REGISTERS *reg);

// IRQ
// #define IRQ_BASE            0x20
// #define IRQ0_TIMER          0x00
// #define IRQ1_KEYBOARD       0x01
// #define IRQ2_CASCADE        0x02
// #define IRQ3_SERIAL_PORT2   0x03
// #define IRQ4_SERIAL_PORT1   0x04
// #define IRQ5_RESERVED       0x05
// #define IRQ6_DISKETTE_DRIVE 0x06
// #define IRQ7_PARALLEL_PORT  0x07
// #define IRQ8_CMOS_CLOCK     0x08
// #define IRQ9_CGA            0x09
// #define IRQ10_RESERVED      0x0A
// #define IRQ11_RESERVED      0x0B
// #define IRQ12_AUXILIARY     0x0C
// #define IRQ13_FPU           0x0D
// #define IRQ14_HARD_DISK     0x0E
// #define IRQ15_RESERVED      0x0F
