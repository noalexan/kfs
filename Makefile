BUILDDIR=./build
TOOLSDIR=./tools

LD=$(TOOLSDIR)/bin/i386-linux-gnu-ld
LDFLAGS=-T linker.ld

AS=$(TOOLSDIR)/bin/i386-linux-gnu-as
ASFLAGS=-msyntax=intel -mnaked-reg

QEMU=qemu-system-i386
QEMUFLAGS=-drive if=pflash,format=raw,file=$(TOOLSDIR)/share/edk2/Build/OvmfIa32/DEBUG_GCC5/QEMU/bios.bin -vga virtio -full-screen

OBJ= \
	build/boot.o

.PHONY: all
all: $(BUILDDIR)/boot.iso

build/%.o: src/%.s
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILDDIR)/boot.iso: $(BUILDDIR) $(BUILDDIR)/iso/boot/grub/grubia32.efi $(BUILDDIR)/iso/boot/grub/grub.cfg $(BUILDDIR)/iso/boot/kernel
	$(TOOLSDIR)/bin/grub-mkrescue -o $@ $(BUILDDIR)/iso

$(BUILDDIR)/iso/boot/grub/grubia32.efi: $(BUILDDIR)/iso/boot/grub
	$(TOOLSDIR)/bin/grub-mkimage -O i386-efi -o $(BUILDDIR)/iso/boot/grub/grubia32.efi -p /boot/grub normal boot linux efinet

$(BUILDDIR)/iso/boot/grub/grub.cfg: grub.cfg
	@cp -v grub.cfg $@

$(BUILDDIR)/iso/boot/kernel: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

$(BUILDDIR) $(BUILDDIR)/iso/boot/grub:
	@mkdir -pv $@

.PHONY: run
run: $(BUILDDIR)/boot.iso
	$(QEMU) $(QEMUFLAGS) -cdrom $(BUILDDIR)/boot.iso

.PHONY: clean
clean:
	@$(RM) -rv $(BUILDDIR)

.PHONY: re
re:
