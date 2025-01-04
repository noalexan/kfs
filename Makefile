BUILDDIR=./build

AS=/tools/bin/i386-linux-gnu-as
ASFLAGS=

CC=/tools/bin/i386-linux-gnu-gcc
CFLAGS=-fno-builtin -fno-exceptions -fno-stack-protector -O3 -Wall -Wextra -Werror -I./include

LD=/tools/bin/i386-linux-gnu-ld
LDFLAGS=-z noexecstack -T linker.ld
LDLIBS=

QEMU=qemu-system-i386
QEMUFLAGS=# -monitor stdio -vga virtio -full-screen

OBJ= \
	build/boot.o \
	build/main.o \
	build/vga.o \
	build/printk.o \
	build/io.o

.PHONY: all
ifeq ($(IN_DOCKER),1)
all: $(BUILDDIR)/boot.iso
else
all:
	docker run --rm -v .:/kfs $(shell docker build -q .)
endif

build/%.o: src/%.s
	$(AS) $(ASFLAGS) -o $@ $<

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/boot.iso: $(BUILDDIR) $(BUILDDIR)/iso $(BUILDDIR)/iso/boot/grub/grub.cfg $(BUILDDIR)/iso/boot/kernel
	/tools/bin/grub-mkrescue -o $@ $(BUILDDIR)/iso

$(BUILDDIR)/iso/boot/grub/grub.cfg: grub.cfg $(BUILDDIR)/iso/boot/grub
	@cp -v grub.cfg $@

$(BUILDDIR)/iso/boot/kernel: linker.ld $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

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
