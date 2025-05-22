BUILDDIR=build
BINDIR=$(BUILDDIR)/bin
ISODIR=$(BUILDDIR)/iso

AS=i686-linux-gnu-as
ASFLAGS=

CC=i686-linux-gnu-gcc
CFLAGS=-ffreestanding -fno-builtin -fno-exceptions -fno-stack-protector
CFLAGS+=-Wall -Wextra # -Werror
CFLAGS+=-I./include -I./lib/libft

AR=i686-linux-gnu-ar

LD=$(CC)
LDFLAGS=-z noexecstack -nostdlib -nodefaultlibs -static # -s
LDLIBS=-L./lib/libft -lft

QEMU=qemu-system-i386
QEMUFLAGS=-m 4G -smp 4 -cpu host -enable-kvm -net nic -net user -s -daemonize

DOCKERIMAGENAME=noalexan/cross-compiler
DOCKERIMAGETAG=ubuntu

OBJ=$(addprefix $(BINDIR)/, \
	boot.o                    \
	main.o                    \
	gdt.o                     \
	idt.o                     \
	isr.o                     \
	irq.o                     \
	keyboard.o                \
	panic.o                   \
	readline.o                \
	printk/printk.o           \
	test/test.o               \
	vga/tty.o                 \
	vga/vga.o                 \
	memory/boot_allocator.o   \
	memory/buddy.o            \
	memory/mb2_info.o         \
)

LIBFT_OBJ= \
	ft_bzero.o  \
	ft_memset.o \
	ft_memcpy.o \
	ft_memcmp.o \
	ft_strlen.o

$(BINDIR)/%.o: src/%.s
	@mkdir -pv $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

$(BINDIR)/%.o: src/%.c
	@mkdir -pv $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

ifneq ($(IN_DOCKER),1)

.PHONY: all
all:
	@docker run --rm -t -v .:/kfs -e IN_DOCKER=1 $(DOCKERIMAGENAME):$(DOCKERIMAGETAG) all

.PHONY: format
format:
	@docker run --rm -t -v .:/kfs -e IN_DOCKER=1 $(DOCKERIMAGENAME):$(DOCKERIMAGETAG) format

.PHONY: clean
clean:
	@docker run --rm -t -v .:/kfs -e IN_DOCKER=1 $(DOCKERIMAGENAME):$(DOCKERIMAGETAG) clean

else

.PHONY: all
all: $(BUILDDIR)/boot.iso

$(BUILDDIR)/boot.iso: $(ISODIR)/boot/kernel $(ISODIR)/boot/grub/grub.cfg
	@mkdir -pv $(@D)
	grub-mkrescue -o $@ $(ISODIR)

$(ISODIR)/boot/grub/grub.cfg: grub.cfg
	@mkdir -pv $(@D)
	cp grub.cfg $@

$(ISODIR)/boot/kernel: $(OBJ) linker.ld | libft
	@mkdir -pv $(@D)
	$(LD) -T linker.ld $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

.PHONY: libft
libft:
	@make -C lib/libft CC=$(CC) AR=$(AR) OBJ="$(LIBFT_OBJ)"

.PHONY: format
format:
	@clang-format --verbose --Werror -i $(shell find ./src ./include -regex '.*\.\(c\|h\)')

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)

endif

.PHONY: run
run: all
	$(QEMU) $(QEMUFLAGS) -cdrom $(BUILDDIR)/boot.iso

.PHONY: re
re: clean all

.NOTPARALLEL: all clean format
