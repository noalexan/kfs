ENTRY(_start)

SECTIONS {
    . = 1M;
    PROVIDE(kernel_start = .);

    .multiboot2_header ALIGN(8) : {
        *(.multiboot2_header)
    }

    .text ALIGN(4K) : {
        *(.text)
    }

    .rodata ALIGN(4K) : {
        *(.rodata*)
    }

    .data ALIGN(4K) : {
        *(.data*)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }
    PROVIDE(kernel_end = .);
}
