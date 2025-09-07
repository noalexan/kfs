ifneq ($(MAKEBUILDTYPE),Release)
MAKEBUILDTYPE=Debug
endif

ifeq ($(MAKEBUILDTYPE),Release)
BUILDDIR=build/release
else
BUILDDIR=build/debug
endif

BINDIR=$(BUILDDIR)/bin
ISODIR=$(BUILDDIR)/iso

TOOLSDIR=tools

AS=i686-linux-gnu-as
ASFLAGS=

CC=i686-linux-gnu-gcc
CFLAGS=-ffreestanding -fno-builtin -fno-exceptions -fno-stack-protector
CFLAGS+=-Wall -Wextra

ifeq ($(MAKEBUILDTYPE),Release)
CFLAGS+=-Werror -DNDEBUG
endif

CFLAGS+=-I./include -I./lib/libft

AR=i686-linux-gnu-ar

LD=$(CC)
LDFLAGS=-z noexecstack -nostdlib -nodefaultlibs -static

ifeq ($(MAKEBUILDTYPE),Release)
LDFLAGS+=-s
endif

LDLIBS=-L./lib/libft -lft

QEMU=qemu-system-i386
QEMUFLAGS=-m 4G -smp 4 -cpu host -enable-kvm -net nic -net user -s -daemonize

DOCKERIMAGENAME=noalexan/cross-compiler
DOCKERIMAGETAG=ubuntu

OBJ=$(patsubst src/%,$(BINDIR)/%,$(shell find src -regex '.*\(\.c\|\.cpp\|\.s\)' | sed 's/\(\.c\|\.cpp\|\.s\)/.o/g'))

LIBFT_OBJ=    \
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

define docker_run
	docker run --rm -t -v .:/kfs -e IN_DOCKER=1 -e MAKEBUILDTYPE="$(MAKEBUILDTYPE)" $(DOCKERIMAGENAME):$(DOCKERIMAGETAG) $(1)
endef

ifeq ($(IN_DOCKER),1)

.PHONY: all
all: $(BUILDDIR)/boot.iso

.PHONY: format
format:
	@clang-format --verbose --Werror -i $(shell find ./src ./include -regex '.*\.\(c\|h\|cpp\|hpp\)')

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)

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

else

.PHONY: all
all:
	$(call docker_run, all)

.PHONY: format
format:
	$(call docker_run, format)

.PHONY: clean
clean:
	$(call docker_run, clean)

endif

# Todo: did not work. to revisit
.PHONY: setup-dev
setup-dev:
	@mkdir -vp $(TOOLSDIR)
	@pip install --target $(TOOLSDIR) pre-commit
	@PYTHONPATH="$(TOOLSDIR):$$PYTHONPATH" python -m pre_commit install

.PHONY: doxy
doxy:
	doxygen Doxyfile

.PHONY: run
run: all
	$(QEMU) $(QEMUFLAGS) -cdrom $(BUILDDIR)/boot.iso

.PHONY: re
re: clean all

.NOTPARALLEL: all clean format
