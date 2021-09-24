OBJDIRS += lib

LIB_SRCFILES := \
	lib/console.c \
	lib/libmain.c \
	lib/exit.c \
	lib/panic.c \
	lib/printf.c \
	lib/printfmt.c \
	lib/readline.c \
	lib/string.c \
	lib/stubs.S \
	lib/syscall.c

# LAB 4 code
LIB_SRCFILES += \
	lib/vma.c

LIB_OBJFILES := $(patsubst lib/%.c, $(OBJDIR)/lib/%.o, $(LIB_SRCFILES))
LIB_OBJFILES := $(patsubst lib/%.S, $(OBJDIR)/lib/%.o, $(LIB_OBJFILES))

-include $(LIB_OBJFILES:.o=.d)

$(OBJDIR)/lib/%.o: lib/%.c $(OBJDIR)/.vars.USER_CFLAGS
	@echo + cc[USER] $<
	@mkdir -p $(@D)
	$(V)$(CC) -nostdinc $(USER_CFLAGS) -c -o $@ $< -MT $@ -MMD -MP -MF $(@:.o=.d)

$(OBJDIR)/lib/%.o: lib/%.S $(OBJDIR)/.vars.USER_CFLAGS
	@echo + as[USER] $<
	@mkdir -p $(@D)
	$(V)$(CC) -nostdinc $(USER_CFLAGS) -c -o $@ $< -MT $@ -MMD -MP -MF $(@:.o=.d)

$(OBJDIR)/lib/libjos.a: $(LIB_OBJFILES)
	@echo + ar $@
	$(V)$(AR) r $@ $(LIB_OBJFILES)
