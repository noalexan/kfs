.section .text
    .extern isr_irq_handler

irq_handler:
    pusha                   # push edi, esi, ebp, esp, ebx, edx, ecx, eax 
    
    xorl %eax, %eax        # clear eax
    movw %ds, %ax          # copy ds in ax
    pushl %eax             # push ds to save it

    movw $0x10, %ax        # load kernel data segment
    movw %ax, %ds          # load segment selector into ds
    movw %ax, %es          
    movw %ax, %fs
    movw %ax, %gs

    pushl %esp             # pass pointer to C; we can access all the information in the C Code
    call isr_irq_handler   # call the isr, irq routine
    popl %esp              # restore esp

    popl %ebx              # restore kernel data segment
    movw %bx, %ds
    movw %bx, %es
    movw %bx, %fs
    movw %bx, %gs

    popa                   # restore all registers
    addl $0x8, %esp        # restore stack for error code not pushed
    sti                    # enable interrupts 
    iret                   # return from interrupt

.macro IRQ num, vector     # macro to handle hardware interrupts
    .global irq_\num       # make the IRQ handler global
irq_\num:
    cli                    # disable interrupts
    pushl $0               # push dummy error code
    pushl $\vector         # push interrupt number
    jmp irq_handler        # jump to the IRQ handler
.endm

# Remapping of hardware interrupts
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47
