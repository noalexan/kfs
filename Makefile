BUILDDIR=build
BINDIR=$(BUILDDIR)/bin
ISODIR=$(BUILDDIR)/iso

AS=i686-linux-gnu-as
ASFLAGS=

CC=i686-linux-gnu-gcc
CFLAGS=-fno-builtin -fno-exceptions -fno-stack-protector
CFLAGS+=-Wall -Wextra # -Werror
CFLAGS+=-I./include -I./lib/libft

CXX=i686-linux-gnu-g++
CXXFLAGS=-fno-builtin -fno-exceptions -fno-stack-protector -fno-rtti
CXXFLAGS+=-Wall -Wextra # -Werror
CXXFLAGS+=-I./include -I./lib/libft

AR=i686-linux-gnu-ar

LD=$(CC)
LDFLAGS=-nostdlib -nodefaultlibs -static -s
LDLIBS=-L./lib/libft -lft

QEMU=qemu-system-i386
QEMUFLAGS=-m 4G -smp 4 -cpu host -enable-kvm -net nic -net user -daemonize

DOCKERIMAGENAME=noalexan/cross-compiler
DOCKERIMAGETAG=ubuntu

OBJ=$(patsubst src/%,$(BINDIR)/%,$(shell find src -regex '.*\(\.c\|\.cpp\|\.s\)' | sed 's/\(\.c\|\.cpp\|\.s\)/.o/g'))

LIBFT_OBJ=      \
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

$(BINDIR)/%.o: src/%.cpp
	@mkdir -pv $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: all
ifeq ($(IN_DOCKER),1)
all: $(BUILDDIR)/boot.iso
else
all:
	docker run --rm -t -v .:/kfs -e IN_DOCKER=1 $(DOCKERIMAGENAME):$(DOCKERIMAGETAG)
endif

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
	@clang-format --verbose --Werror -i $(shell find ./src ./include -regex '.*\.\(c\|h\|cpp\|hpp\)')

.PHONY: run
run: all
	$(QEMU) $(QEMUFLAGS) -cdrom $(BUILDDIR)/boot.iso

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)

.PHONY: re
re: clean all

.NOTPARALLEL: all clean
