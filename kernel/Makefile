#
# Makefile fragment for OpenLSD kernel.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory
# where the GNUmakefile is located.
#

OBJDIRS += kernel

# LAB 1 code
KERNEL_SRCFILES := \
	kernel/boot.S \
	kernel/console.c \
	kernel/main.c \
	kernel/monitor.c \
	kernel/pic.c \
	kernel/printf.c \
	kernel/mem/boot.c \
	kernel/mem/buddy.c \
	kernel/mem/init.c \
	kernel/tests/lab1.c \
	lib/list.c \
	lib/printfmt.c \
	lib/rbtree.c \
	lib/readline.c \
	lib/string.c

# Only build files if they exist.
KERNEL_SRCFILES := $(wildcard $(KERNEL_SRCFILES))

KERNEL_OBJFILES := $(patsubst %.c, $(OBJDIR)/%.o, $(KERNEL_SRCFILES))
KERNEL_OBJFILES := $(patsubst %.S, $(OBJDIR)/%.o, $(KERNEL_OBJFILES))
KERNEL_OBJFILES := $(patsubst $(OBJDIR)/lib/%, $(OBJDIR)/kernel/%, $(KERNEL_OBJFILES))

-include $(KERNEL_OBJFILES:.o=.d)

KERNEL_BINFILES := $(patsubst %, $(OBJDIR)/%, $(KERNEL_BINFILES))

# How to build kernel object files
$(OBJDIR)/kernel/%.o: kernel/%.c $(OBJDIR)/.vars.KERNEL_CFLAGS
	@echo + cc $<
	@mkdir -p $(@D)
	$(V)$(CC) -nostdinc $(KERNEL_CFLAGS) -c -o $@ $< -MT $@ -MMD -MP -MF $(@:.o=.d)

$(OBJDIR)/kernel/%.o: kernel/%.S $(OBJDIR)/.vars.KERNEL_CFLAGS
	@echo + as $<
	@mkdir -p $(@D)
	$(V)$(AS) -nostdinc $(KERNEL_CFLAGS) -c -o $@ $<

$(OBJDIR)/kernel/%.o: lib/%.c $(OBJDIR)/.vars.KERNEL_CFLAGS
	@echo + cc $<
	@mkdir -p $(@D)
	$(V)$(CC) -nostdinc $(KERNEL_CFLAGS) -c -o $@ $< -MT $@ -MMD -MP -MF $(@:.o=.d)

# Special flags for kernel/init
$(OBJDIR)/kernel/main.o: override KERNEL_CFLAGS+=$(INIT_CFLAGS)
$(OBJDIR)/kernel/main.o: $(OBJDIR)/.vars.INIT_CFLAGS

# How to build the kernel itself
$(OBJDIR)/kernel/kernel: $(KERNEL_OBJFILES) $(KERNEL_BINFILES) kernel/kernel.ld \
	  $(OBJDIR)/.vars.KERNEL_LDFLAGS
	@echo + ld $@
	$(V)$(LD) -o $@ $(KERNEL_LDFLAGS) $(KERNEL_OBJFILES) $(GCC_LIB) $(KERNEL_BINFILES)
	$(V)$(OBJDUMP) -S $@ > $@.asm
	$(V)$(NM) -n $@ > $@.sym

# How to build the kernel disk image
$(OBJDIR)/kernel/kernel.img: $(OBJDIR)/kernel/kernel $(OBJDIR)/boot/boot
	@echo + mk $@
	$(V)truncate -s %512 $(OBJDIR)/boot/boot
	$(V)dd if=$(OBJDIR)/boot/boot of=$(OBJDIR)/kernel/kernel.img~ 2>/dev/null
	$(V)dd if=$(OBJDIR)/kernel/kernel >>$(OBJDIR)/kernel/kernel.img~ 2>/dev/null
	$(V)truncate -s 5120000 $(OBJDIR)/kernel/kernel.img~
	$(V)mv $(OBJDIR)/kernel/kernel.img~ $(OBJDIR)/kernel/kernel.img

all: $(OBJDIR)/kernel/kernel.img

grub: $(OBJDIR)/jos-grub

$(OBJDIR)/jos-grub: $(OBJDIR)/kernel/kernel
	@echo + oc $@
	$(V)$(OBJCOPY) --adjust-vma=0x10000000 $^ $@
