#
# Makefile fragment for the OpenLSD kernel.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory
# where the GNUmakefile is located.
#

OBJDIRS += boot

BOOT_OBJS := \
    $(OBJDIR)/boot/boot1.o \
    $(OBJDIR)/boot/boot2.o \
    $(OBJDIR)/boot/a20.o \
    $(OBJDIR)/boot/compat.o \
    $(OBJDIR)/boot/mmap.o \
    $(OBJDIR)/boot/main.o

-include $(BOOT_OBJS:.o=.d)

$(OBJDIR)/boot/%.o: boot/%.c
	@echo + cc -Os $<
	@mkdir -p $(@D)
	$(V)$(CC) -nostdinc $(BOOT_CFLAGS) -c -o $@ $< -MT $@ -MMD -MP -MF $(@:.o=.d)

$(OBJDIR)/boot/%.o: boot/%.S
	@echo + as $<
	@mkdir -p $(@D)
	$(V)$(CC) -nostdinc $(BOOT_CFLAGS) -c -o $@ $< -MT $@ -MMD -MP -MF $(@:.o=.d)

$(OBJDIR)/boot/boot: $(BOOT_OBJS)
	@echo + ld boot/boot
	$(V)$(LD) $(BOOT_LDFLAGS) -o $@.elf $^
	$(V)$(OBJCOPY) -O binary $@.elf $@
#	$(V)$(OBJDUMP) -S $@.out >$@.asm
#	$(V)$(OBJCOPY) -S -O binary -j .text $@.out $@
#	$(V)perl boot/sign.pl $(OBJDIR)/boot/boot

