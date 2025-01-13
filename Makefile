BUILDDIR=./build

AS=/tools/bin/i386-linux-gnu-as
ASFLAGS=

CC=/tools/bin/i386-linux-gnu-gcc
CFLAGS=-O3 -Wall -Wextra -I./include -I./lib/libft

CXX=/tools/bin/i386-linux-gnu-g++
CXXFLAGS=-O3 -Wall -Wextra -I./include -I./lib/libft

LD=/tools/bin/i386-linux-gnu-ld
LDFLAGS=-T linker.ld -z noexecstack -fno-builtin -fno-exceptions -fno-stack-protector -fno-rtti -nostdlib -nodefaultlibs
LDLIBS=-L./lib/libft -lft

QEMU=qemu-system-i386
QEMUFLAGS=

DOCKERIMAGENAME=noalexan/kfs-builder
DOCKERIMAGETAG=24.04

OBJ= $(addprefix $(BUILDDIR)/, \
	boot.o \
	main.o \
	vga.o \
	printk.o \
	io.o \
	gdt.o \
	idt.o \
	exceptions_handlers.o \
	tty.o \
)

LIBFT_OBJ= \
	ft_bzero.o \
	ft_memset.o \
	ft_memcpy.o

.PHONY: all
ifeq ($(IN_DOCKER),1)
all: $(BUILDDIR)/boot.iso
else
all:
	docker run --rm -t -v .:/kfs $(DOCKERIMAGENAME):$(DOCKERIMAGETAG)
endif

$(BUILDDIR)/%.o: src/%.s | $(BUILDDIR)
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILDDIR)/%.o: src/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o: src/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/boot.iso: $(BUILDDIR)/iso/boot/kernel $(BUILDDIR)/iso/boot/grub/grub.cfg
	/tools/bin/grub-mkrescue -o $@ $(BUILDDIR)/iso

$(BUILDDIR)/iso/boot/grub/grub.cfg: grub.cfg $(BUILDDIR)/iso/boot/grub
	@cp -v grub.cfg $@

$(BUILDDIR)/iso/boot/kernel: $(OBJ) linker.ld | libft $(BUILDDIR)/iso/boot
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

.PHONY: libft
libft:
	@make -C ./lib/libft CC=$(CC) OBJ="$(LIBFT_OBJ)" static

$(BUILDDIR) $(BUILDDIR)/iso $(BUILDDIR)/iso/boot $(BUILDDIR)/iso/boot/grub:
	@mkdir -pv $@

.PHONY: format
format:
	@clang-format --verbose --Werror -i $(shell find ./{src,include} -regex '.*\.\(c\|h\|cpp\|hpp\)')

.PHONY: run
run: all
	$(QEMU) $(QEMUFLAGS) -cdrom $(BUILDDIR)/boot.iso

.PHONY: clean
clean:
	@$(RM) -rv $(BUILDDIR)

.PHONY: re
re: clean all

.NOTPARALLEL: re fclean
