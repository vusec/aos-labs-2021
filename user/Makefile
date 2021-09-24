OBJDIRS += user

USERLIBS = jos

-include $(OBJDIR)/user/%.d

$(OBJDIR)/user/%.o: user/%.c $(OBJDIR)/.vars.USER_CFLAGS
	@echo + cc[USER] $<
	@mkdir -p $(@D)
	$(V)$(CC) -nostdinc $(USER_CFLAGS) -c -o $@ $< -MT $@ -MMD -MP -MF $(@:.o=.d)

$(OBJDIR)/user/%: $(OBJDIR)/user/%.o $(OBJDIR)/lib/entry.o $(USERLIBS:%=$(OBJDIR)/lib/lib%.a) user/user.ld
	@echo + ld $@
	$(V)$(LD) -o $@ $(USER_LDFLAGS) $(OBJDIR)/lib/entry.o $@.o -L$(OBJDIR)/lib $(USERLIBS:%=-l%) $(GCC_LIB)
	$(V)$(OBJDUMP) -S $@ > $@.asm
	$(V)$(NM) -n $@ > $@.sym
	$(V)$(OBJCOPY) --only-keep-debug "$@" "$@.debug"
	$(V)$(OBJCOPY) --add-gnu-debuglink="$@.debug" --input-target binary --output-target elf64-x86-64 -B i386:x86-64 $@ $@

