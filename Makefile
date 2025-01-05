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

DOCKERIMAGENAME=kfs-builder
DOCKERIMAGETAG=24.04

OBJ= \
	build/boot.o \
	build/main.o \
	build/vga.o \
	build/printk.o \
	build/io.o \
	build/gdt.o

.PHONY: all
ifeq ($(IN_DOCKER),1)
all: $(BUILDDIR)/boot.iso
else
all: .dockerimageid
	docker run --rm -t -v .:/kfs $(DOCKERIMAGENAME):$(DOCKERIMAGETAG)

.dockerimageid:
	docker build -t $(DOCKERIMAGENAME):$(DOCKERIMAGETAG) . \
		&& docker images -q $(DOCKERIMAGENAME):$(DOCKERIMAGETAG) > $@
endif

build/%.o: src/%.s
	$(AS) $(ASFLAGS) -o $@ $<

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/boot.iso: $(BUILDDIR) $(BUILDDIR)/iso $(BUILDDIR)/iso/boot/grub/grub.cfg $(BUILDDIR)/iso/boot/kernel
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
