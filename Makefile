BUILDDIR=./build

AS=/tools/bin/i386-linux-gnu-as
ASFLAGS=

CC=/tools/bin/i386-linux-gnu-gcc
CFLAGS=-fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -O3 -Wall -Wextra -I./include -I./lib/libft

LD=/tools/bin/i386-linux-gnu-ld
LDFLAGS=-z noexecstack -T linker.ld
LDLIBS=-L./lib/libft -lft

QEMU=qemu-system-i386
QEMUFLAGS=# -monitor stdio -vga virtio -full-screen

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
)

.PHONY: all
ifeq ($(IN_DOCKER),1)
all: $(BUILDDIR)/boot.iso
else
all:
	docker run --rm -t -v .:/kfs $(DOCKERIMAGENAME):$(DOCKERIMAGETAG)
endif

$(BUILDDIR)/%.o: src/%.s $(BUILDDIR)
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILDDIR)/%.o: src/%.c $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/boot.iso: $(BUILDDIR)/iso/boot/kernel $(BUILDDIR)/iso/boot/grub/grub.cfg
	/tools/bin/grub-mkrescue -o $@ $(BUILDDIR)/iso

$(BUILDDIR)/iso/boot/grub/grub.cfg: grub.cfg $(BUILDDIR)/iso/boot/grub
	@cp -v grub.cfg $@

$(BUILDDIR)/iso/boot/kernel: ./lib/libft/libft.a $(OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

./lib/libft/libft.a:
	make -C ./lib/libft CC=$(CC) OBJ="ft_bzero.o ft_memset.o ft_memcpy.o" static

$(BUILDDIR) $(BUILDDIR)/iso $(BUILDDIR)/iso/boot $(BUILDDIR)/iso/boot/grub:
	@mkdir -pv $@

.PHONY: run
run: all
	$(QEMU) $(QEMUFLAGS) -cdrom $(BUILDDIR)/boot.iso

.PHONY: clean
clean:
	@$(RM) -rv $(BUILDDIR)

.PHONY: re
re: clean all
