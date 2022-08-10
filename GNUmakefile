# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := kernel.elf
override ISO := limine-cd.iso
override LIMINECFG := limine.cfg
# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
CC := /usr/bin/cc
LMN := ./limine/limine-deploy
QEMU := "/mnt/c/Program Files/qemu/qemu-system-x86_64.exe"

QFLAGS := -m 512 -boot d -cdrom
# Same thing for "ld" (the linker).
LD := /usr/bin/ld
# User controllable CFLAGS.
CFLAGS ?= -O2 -g -Wall -Wextra -Wpedantic -pipe -std=c11

# User controllable nasm flags.
NASMFLAGS ?= -F dwarf -g

# User controllable linker flags. We set none by default.
LDFLAGS ?=

# User controllable preprocessor flags. We set none by default.
CPPFLAGS ?=

LMNDIR := limine
SRCDIR := src
BUILDDIR := build/bin
OBJDIR := build/lib
ISOBUILDDIR := build/iso_root
ISODIR := build/image

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Internal C flags that should not be changed by the user.
override CFLAGS +=       \
    -I.                  \
    -std=c11             \
    -ffreestanding       \
    -fno-stack-protector \
    -fno-stack-check     \
    -fno-pie             \
    -fno-pic             \
    -m64                 \
    -march=x86-64        \
    -mabi=sysv           \
    -mno-80387           \
    -mno-mmx             \
    -mno-sse             \
    -mno-sse2            \
    -mno-red-zone        \
    -mcmodel=kernel      \
    -MMD

# Internal linker flags that should not be changed by the user.
override LDFLAGS +=         \
    -nostdlib               \
    -static                 \
    -z max-page-size=0x1000 \
    -T linker.ld

# Internal nasm flags that should not be changed by the user.
override NASMFLAGS += \
    -f elf64

# Use find to glob all *.c, *.S, and *.asm files in the directory and extract the object names.
override CFILES :=$(call rwildcard,$(SRCDIR),*.c)        
override ASFILES := $(call rwildcard,$(SRCDIR),*.S)
override NASMFILES := $(call rwildcard,$(SRCDIR),*.asm)

override OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CFILES))
override OBJS += $(patsubst $(SRCDIR)/%.S, $(OBJDIR)/%_s.o, $(ASFILES))
override OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(NASMFILES))
DIRS := $(wildcard $(SRCDIR)/*)
override HEADER_DEPS := $(CFILES:.c=.d) $(ASFILES:.S=.d)

all:
	@echo "Building kernel..."
	@make kernel
	@echo "Building ISO..."
	@make buildimg
	@echo "Running QEMU..."
	@make run
	@echo "Done!"

# Link rules for the final kernel executable.
kernel: $(OBJS) link

# Compilation rules for *.c files.
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(ASMC) $(CFLAGS) $^ -f elf64 -o $@

$(OBJDIR)/%_s.o: $(SRCDIR)/%.S
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(ASMC) $(NASMFLAGS) $^ -f elf64 -o $@

link: 
	@ echo !==== LINKING $^
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)

clean:
	@rm -rf $(OBJDIR)/*
	@rm -rf $(OBJS)
	@rm -f $(ISOBUILDDIR)/$(KERNEL)
	@rm -f $(ISOBUILDDIR)/$(LIMINECFG)
	@rm -f $(BUILDDIR)/$(KERNEL)
	@rm -f $(ISODIR)/$(ISO)

setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(ISOBUILDDIR)
	@mkdir -p $(ISODIR)
	@cp -v $(LMNDIR)/limine.sys $(LMNDIR)/limine-cd.bin $(LMNDIR)/limine-cd-efi.bin $(ISOBUILDDIR)
buildimg:
	@cp -v limine.cfg $(BUILDDIR)/$(KERNEL) $(ISOBUILDDIR)
	@xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-cd-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        $(ISOBUILDDIR) -o $(ISODIR)/$(ISO)
	$(LMN) $(ISODIR)/$(ISO)

run:
	$(QEMU) $(QFLAGS) $(ISODIR)/$(ISO)