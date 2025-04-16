.text
.extern isr_except_handler

.macro ISR_NOERRCODE num
.globl isr_\num
isr_\num:
    cli
    pushl $0                              # push 0 as error code
    pushl $\num                           # push interrupt number
    jmp isr_handler
.endm

.macro ISR_ERRCODE num
.globl isr_\num
isr_\num:
    cli
    pushl $\num                           # push interrupt number
    jmp isr_handler
.endm

isr_handler:
    pusha                                 # pushes in order: eax, ecx, edx, ebx, esp, ebp, esi, edi

    # xor %eax, %eax                      # clear eax
    movw %ds, %ax                         # copy ds in ax
    pushl %eax                            # push ds to save it

    movw $0x10, %ax                       # load kernel data segment
    movw %ax, %ds                         # load segment selector into ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    #pushl %esp                           # pass pointer to stack to C, so we can access all the pushed information
    call isr_except_handler
    #addl $4, %esp                        # clean up the stack

    popl %eax                             # restore old segment
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    popa                                  # restore all registers
    addl $8, %esp                         # remove error code and interrupt number
    sti
    iret                                  # return from interrupt

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_NOERRCODE 9
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 31

ISR_ERRCODE 8
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_ERRCODE 17
ISR_ERRCODE 21
ISR_ERRCODE 29
ISR_ERRCODE 30
