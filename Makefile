BUILDDIR=build

AS=/tools/bin/i386-linux-gnu-as
ASFLAGS=--32

CC=/tools/bin/i386-linux-gnu-gcc
CFLAGS=-fno-builtin -fno-exceptions -fno-stack-protector -O3 -Wall -Wextra -I./include -I./lib/libft -m32

CXX=/tools/bin/i386-linux-gnu-g++
CXXFLAGS=-fno-builtin -fno-exceptions -fno-stack-protector -fno-rtti -O3 -Wall -Wextra -I./include -I./lib/libft -m32

LD=$(CXX)
LDFLAGS=-z noexecstack -nostdlib -nodefaultlibs -m32
LDLIBS=-L./lib/libft -lft

QEMU=qemu-system-i386
QEMUFLAGS=-serial mon:stdio

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
	tty/tty.o \
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
	docker run --rm -t -v $(PWD):/kfs -e IN_DOCKER=1 $(DOCKERIMAGENAME):$(DOCKERIMAGETAG)
endif

$(BUILDDIR)/%/:
	@mkdir -pv $@

$(BUILDDIR)/%.o: src/%.s | $(@D)
	$(AS) $(ASFLAGS) -o $(BUILDDIR)/$(@F) $<

$(BUILDDIR)/%.o: src/%.c | $(@D)
	$(CC) $(CFLAGS) -c -o $(BUILDDIR)/$(@F) $<

$(BUILDDIR)/%.o: src/%.cpp | $(@D)
	$(CXX) $(CXXFLAGS) -c -o $(BUILDDIR)/$(@F) $<

$(BUILDDIR)/boot.iso: $(BUILDDIR)/iso/boot/kernel $(BUILDDIR)/iso/boot/grub/grub.cfg
	/tools/bin/grub-mkrescue -o $@ $(BUILDDIR)/iso

$(BUILDDIR)/iso/boot/grub/grub.cfg: grub.cfg $(BUILDDIR)/iso/boot/grub
	@cp -v grub.cfg $@

$(BUILDDIR)/iso/boot/kernel: $(OBJ) linker.ld | libft $(BUILDDIR)/iso/boot
	$(LD) -T linker.ld $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

.PHONY: libft
libft:
	@make -C ./lib/libft CC=$(CC) OBJ="$(LIBFT_OBJ)" static

.PHONY: format
format:
	@clang-format --verbose --Werror -i $(shell find ./src ./include -regex '.*\.\(c\|h\|cpp\|hpp\)')

.PHONY: run
run: all
	$(QEMU) $(QEMUFLAGS) -cdrom $(BUILDDIR)/boot.iso

.PHONY: clean
clean:
	@$(RM) -rv $(BUILDDIR)

.PHONY: re
re: clean all

.NOTPARALLEL: re
