override PROGRAM := loop.elf

CC := /usr/bin/cc
LD := /usr/bin/ld

PROGRAM_ENTRY := main

CFLAGS ?= -O2 -g -Wall -Wextra -Wpedantic -pipe -std=c11
LDFLAGS ?=

ABSDIR := $(shell pwd)
SRCDIR := src
BUILDHOME := build
BUILDDIR := build/bin
OBJDIR := build/lib

DIRS := $(wildcard $(SRCDIR)/*)

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Check that given variables are set and all have non-empty values,
# die with an error otherwise.
#
# Params:
#   1. Variable name(s) to test.
#   2. (optional) Error message to print.
check_defined = \
    $(strip $(foreach 1,$1, \
        $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
    $(if $(value $1),, \
      $(error Undefined $1$(if $2, ($2))))

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
    -mno-red-zone        \
    -mcmodel=kernel      \
    -MMD
override CFLAGS += $(CEXTRA)

override LDFLAGS +=         \
    -nostdlib               \
    -static                 \
    -z max-page-size=0x1000 \
    -T linker.ld

override CFILES :=$(call rwildcard,$(SRCDIR),*.c)        
override OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CFILES))
override HEADER_DEPS := $(CFILES:.c=.d)

# You may comment @make run in this target if you intend
# to use virtualbox or real hardware.
all:
	@echo "Building program..."
	@make program
program: $(OBJS) link


$(OBJDIR)/%.o: $(SRCDIR)/%.c
#	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $^ -o $@

link: 
#	@ echo !==== LINKING $^
	@ mkdir -p $(BUILDDIR)
	@$(LD) $(LDFLAGS) -o $(BUILDDIR)/$(PROGRAM) $(OBJS)

clean:
	@rm -rf $(OBJDIR)/*
	@rm -rf $(OBJS)
	@rm -rf $(BUILDDIR)/*