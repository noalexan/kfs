# Some links

-   [Intel System Architecture](https://cdrdv2-public.intel.com/671447/325384-sdm-vol-3abcd.pdf)
-   [Os-dev](https://wiki.osdev.org/Expanded_Main_Page)
-   [Guideline From Navi](https://segmentfault.com/a/1190000040187304/en)

src/
├── arch/
│   └── x86/
│       ├── boot.s
│       ├── gdt.c
│       ├── gdt.h
│       ├── idt.c
│       ├── idt.h
│       ├── io.h
│       ├── irq.s
│       ├── isr.s
│       └── acpi.h
├── kernel/
│   ├── main.c
│   ├── panic.c
│   ├── mb2_info.c
│   └── assert.c
├── memory/
│   ├── boot_allocator.c
│   ├── boot_allocator.h
│   ├── buddy.c
│   ├── buddy.h
│   ├── page.c
│   └── page.h
├── drivers/
|   └──keyboard/
│   |   ├── keyboard.c
│   |   └── keyboard.h
|   └── vga/
│   |   ├── vga_printf.c
│   |   └── vga_printf.h
│   |   ├── vga.c
│   |   └── vga.h
|   └── tty/
│       ├── tty.c
│       ├── tty.h
├── debug/
    ├── test.c
    └── test.h
