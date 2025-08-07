#include "test.h"

void test_gdt_loaded(void)
{
	gdt_ptr_t gdt_ptr;
	__asm__ volatile("sgdt %0" : "=m"(gdt_ptr));
	vga_printf("GDT loaded: base=0x%x, limit=0x%x\n", gdt_ptr.base, gdt_ptr.limit);
}

void test_gdt_access(void)
{
	volatile int test = 42;
	test += 1;
	vga_printf("GDT access test: OK (no fault)\n");
}

static inline uint16_t get_cs(void)
{
	uint16_t v;
	__asm__ volatile("mov %%cs,%0" : "=r"(v));
	return v;
}
static inline uint16_t get_ds(void)
{
	uint16_t v;
	__asm__ volatile("mov %%ds,%0" : "=r"(v));
	return v;
}
static inline uint16_t get_ss(void)
{
	uint16_t v;
	__asm__ volatile("mov %%ss,%0" : "=r"(v));
	return v;
}

void test_gdt_print_segment_registers(void)
{
	// CS : Code Segment
	vga_printf("CS = 0x%x (attendu: 0x8)   // Code Segment\n", get_cs());
	// DS : Data Segment
	vga_printf("DS = 0x%x (attendu: 0x10)  // Data Segment\n", get_ds());
	// SS : Stack Segment
	vga_printf("SS = 0x%x (attendu: 0x10)  // Stack Segment\n", get_ss());
}

void test_idt_divide_by_zero(void)
{
	volatile int a = 1, b = 0, c;
	c = a / b;
	(void)c;
}

void test_idt_general_protection(void) { __asm__ volatile("int $13"); }

void test_idt_page_fault(void)
{
	volatile int *p = (int *)0xDEADBEEF;
	int           x = *p; // Provoque #PF si la page n'est pas mappée
	(void)x;
}

void test_idt_overflow(void)
{
	__asm__ volatile("add $0x7F, %%al\n\t"
	                 "add $0x7F, %%al\n\t"
	                 "into\n\t"
	                 :
	                 :
	                 : "al");
}

void test_idt_invalid_opcode(void) { __asm__ volatile(".byte 0x0F, 0x0B"); }

void test_idt_all_exceptions(void)
{
	// test_idt_divide_by_zero();
	// test_idt_general_protection();
	test_idt_page_fault();
	// test_idt_invalid_opcode();
	// test_idt_overflow();
}

void debugkey(keyboard_key_t key)
{
	(void)key;
	kpanic("test");
}

void register_scroll_lock_on_debug()
{
	keyboard_key_t key = {0, 0, 0x46, 0, 0, NULL}; // Scroll lock
	keyboard_bind_key(debugkey, key);
}

void test_boot_alocator(void)
{

	vga_printf("---------------------Testing Boot Allocator---------------------\n");
	boot_allocator_print_inital_layout();
	vga_printf("-----------------------------------------------------------------\n");
	void *a = boot_alloc(64, HIGHMEM_ZONE, TO_FREE);
	void *b = boot_alloc(128, HIGHMEM_ZONE, TO_FREE);
	void *c = boot_alloc(256, HIGHMEM_ZONE, TO_FREE);

	vga_printf("boot_alloc(64) , start = %p | end %p\n", a, a + 64);
	vga_printf("boot_alloc(128) , start = %p | end %p\n", b, b + 128);
	vga_printf("boot_alloc(256) , start = %p | end %p\n", c, c + 256);

	vga_printf("-----------------------------------------------------------------\n");
	boot_allocator_print_inital_layout();
	vga_printf("-----------------------------------------------------------------\n");
}
