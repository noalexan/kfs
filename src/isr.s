.intel_syntax noprefix
.code32

.section .text

.macro isr_err_stub num
.global isr_stub_\num
isr_stub_\num:
	push \num
	call exception_handler
	iret
.endm

.macro isr_no_err_stub num
.global isr_stub_\num
isr_stub_\num:
	push \num
	call exception_handler
	iret
.endm

# ISRs for exceptions
isr_no_err_stub 0   # Division by Zero
isr_no_err_stub 1   # Debug Exception
isr_no_err_stub 2   # Non-Maskable Interrupt (NMI)
isr_no_err_stub 3   # Breakpoint Exception
isr_no_err_stub 4   # Overflow Exception
isr_no_err_stub 5   # Bounds Check Exception
isr_no_err_stub 6   # Invalid Opcode Exception
isr_no_err_stub 7   # Device Not Available Exception
isr_err_stub    8   # Double Fault
isr_no_err_stub 9   # Coprocessor Segment Overrun (386 or earlier only)
isr_err_stub    10  # Invalid Task State Segment
isr_err_stub    11  # Segment Not Present
isr_err_stub    12  # Stack Fault
isr_err_stub    13  # General Protection Fault
isr_err_stub    14  # Page Fault
isr_no_err_stub 15  # Reserved Exception
isr_no_err_stub 16  # x87 Floating-Point Exception
isr_err_stub    17  # Alignment Check Exception (486+)
isr_no_err_stub 18  # Machine Check Exception (Pentium+)
isr_no_err_stub 19  # SIMD Floating-Point Exception (SSE)
isr_no_err_stub 20  # Virtualization Exception (Intel VT-x)
isr_no_err_stub 21  # Control Protection Exception
isr_no_err_stub 22  # \
isr_no_err_stub 23  # |
isr_no_err_stub 24  # |
isr_no_err_stub 25  # |
isr_no_err_stub 26  # | reserved
isr_no_err_stub 27  # | exceptions
isr_no_err_stub 28  # |
isr_no_err_stub 29  # |
isr_err_stub    30  # |
isr_no_err_stub 31  # /
