#
# This makefile system follows the structuring conventions
# recommended by Peter Miller in his excellent paper:
#
#	Recursive Make Considered Harmful
#	http://aegis.sourceforge.net/auug97.pdf
#
OBJDIR := obj
COMMIT := ""

# Run 'make V=1' to turn on verbose commands, or 'make V=0' to turn them off.
ifeq ($(V),1)
override V =
endif
ifeq ($(V),0)
override V = @
endif

-include conf/lab.mk

-include conf/env.mk

LABSETUP ?= ./

TOP = .

# Values for LEGACY are Yes or No
# 1. A value of Yes means use legacy JOS toolchain when compiling JOS kernel
# 2. A value of No means use llvm-13 toolchain when compiling JOS kernel
# default value is LEGACY=Yes
LEGACY=Yes

# Cross-compiler jos toolchain
#
# This Makefile will automatically use the cross-compiler toolchain
# installed as 'i386-jos-elf-*', if one exists.  If the host tools ('gcc',
# 'objdump', and so forth) compile for a 32-bit x86 ELF target, that will
# be detected as well.  If you have the right compiler toolchain installed
# using a different name, set CROSS_COMPILE explicitly in conf/env.mk

# try to infer the correct CROSS_COMPILE
ifndef CROSS_COMPILE
CROSS_COMPILE := $(shell if i386-jos-elf-objdump -i 2>&1 | grep '^elf32-i386$$' >/dev/null 2>&1; \
	then echo 'i386-jos-elf-'; \
	elif objdump -i 2>&1 | grep 'elf32-i386' >/dev/null 2>&1; \
	then echo ''; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find an i386-*-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with i386-jos-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your i386-*-elf toolchain is installed with a command" 1>&2; \
	echo "*** prefix other than 'i386-jos-elf-', set your CROSS_COMPILE" 1>&2; \
	echo "*** environment variable to that prefix and run 'make' again." 1>&2; \
	echo "*** To turn off this error, run 'gmake CROSS_COMPILE= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif


# try to generate a unique GDB port
GDBPORT	:= $(shell expr `id -u` % 5000 + 25000)

ifeq ($(LEGACY),Yes)
CC	:= $(CROSS_COMPILE)gcc -pipe
AS	:= $(CROSS_COMPILE)gcc
AR	:= $(CROSS_COMPILE)ar
LD	:= $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
NM	:= $(CROSS_COMPILE)nm
else
CC	:= clang -pipe
AS	:= clang
AR	:= llvm-ar
LD	:= ld.lld
OBJCOPY := llvm-objcopy
OBJDUMP := llvm-objdump
NM	:= llvm-nm
endif

# binutils-gold has problems with parsing -Ttext right
ifneq ($(shell which $(LD).bfd 2>/dev/null),)
LD := $(LD).bfd
endif

# Native commands
NCC	:= gcc $(CC_VER) -pipe
NATIVE_CFLAGS := $(CFLAGS) $(DEFS) $(LABDEFS) -I$(TOP) -MD -Wall
TAR	:= gtar
PERL	:= perl

# Compiler flags
# -fno-builtin is required to avoid refs to undefined functions in the kernel.
# -O1 and -fno-builtin to make backtraces work more nicely.
CFLAGS := $(CFLAGS) $(DEFS) $(LABDEFS) -O1 -fno-inline -fno-builtin -I$(TOP) -MD
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -Wall -Wno-format -Wno-unused -Werror
CFLAGS += -Iinclude

# Add -fno-stack-protector if the option exists.
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

ifneq ($(LEGACY),Yes)
CFLAGS += -Wno-unused-command-line-argument
endif

GCC_LIB := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

# Lists that the */Makefile makefile fragments will add to
OBJDIRS :=

# Make sure that 'all' is the first target
all:

# Eliminate default suffix rules
.SUFFIXES:

# Delete target files if there is an error (or make is interrupted)
.DELETE_ON_ERROR:

# make it so that no intermediate .o files are ever deleted
.PRECIOUS: %.o $(OBJDIR)/boot/%.o $(OBJDIR)/kernel/%.o \
	   $(OBJDIR)/lib/%.o $(OBJDIR)/fs/%.o $(OBJDIR)/net/%.o \
	   $(OBJDIR)/user/%.o

ifeq ($(LEGACY),Yes)
BOOT_CFLAGS := $(CFLAGS) -DJOS_KERNEL -fno-pie -Os -m32
BOOT_LDFLAGS := -N -nostdlib -T boot/boot.ld -m32 -static -fno-pie -Wl,-melf_i386
else
# For clang-13 -Os is not enough to make bootloader fit into 3 sectors
# so use Link-Time-Optimizations and -Oz flag (and system assembler).
BOOT_CFLAGS := $(CFLAGS) -DJOS_KERNEL -fno-pie -Oz -m32 -fno-integrated-as -flto -fno-stack-protector
BOOT_LDFLAGS := -N -nostdlib -T boot/boot.ld -m32 -static --no-pie -melf_i386
endif

KERNEL_CFLAGS := $(CFLAGS) -DOpenLSD_KERNEL -gdwarf-2 -mcmodel=large -fno-pie
KERNEL_CFLAGS += -DKERNEL_LMA=0x100000
KERNEL_CFLAGS += -DKERNEL_VMA=0xFFFF800000000000
KERNEL_CFLAGS += -mno-sse

ifneq ($(LEGACY),Yes)
# Works with the integrated LLVM assembler as well but lets keep some uniformity
# with the bootloader. Also, clang might generate some sse instructions for
# register variables which are not available at kernel boot time. Disable
# SSE extensions.
KERNEL_CFLAGS += -mno-mmx -mno-sse -mno-avx
# Note: all warnings should be fixed when clang is used. Else we might
# get into issues with uninitialized variables.
endif

ifeq ($(LEGACY),Yes)
KERNEL_LDFLAGS := -Tkernel/kernel.ld -nostdlib -n -fno-pie
KERNEL_LDFLAGS += -Wl,--defsym,KERNEL_LMA=0x100000
KERNEL_LDFLAGS += -Wl,--defsym,KERNEL_VMA=0xFFFF800000000000
KERNEL_LDFLAGS += -static
else
KERNEL_LDFLAGS := -Tkernel/kernel.ld -nostdlib -n --no-pie
KERNEL_LDFLAGS += --defsym=KERNEL_LMA=0x100000
KERNEL_LDFLAGS += --defsym=KERNEL_VMA=0xFFFF800000000000
KERNEL_LDFLAGS += -static
endif

USER_CFLAGS := $(CFLAGS) -DOpenLSD_USER -gdwarf-2 -fPIC
USER_LDFLAGS := -Tuser/user.ld -nostdlib -n

ifneq ($(LEGACY),Yes)
USER_CFLAGS += -mno-mmx -mno-sse -mno-avx -w -DUSE_CLANG
endif

# Update .vars.X if variable X has changed since the last make run.
#
# Rules that use variable X should depend on $(OBJDIR)/.vars.X.  If
# the variable's value has changed, this will update the vars file and
# force a rebuild of the rule that depends on it.
$(OBJDIR)/.vars.%: FORCE
	$(V)echo "$($*)" | cmp -s $@ || echo "$($*)" > $@
.PRECIOUS: $(OBJDIR)/.vars.%
.PHONY: FORCE


# Include Makefiles for subdirectories
include boot/Makefile
include kernel/Makefile
include user/Makefile
include lib/Makefile

CPUS ?= 1

QEMUOPTS = -drive format=raw,file=$(OBJDIR)/kernel/kernel.img -serial mon:stdio -gdb tcp::$(GDBPORT)
QEMUOPTS += $(shell if $(QEMU) -nographic -help | grep -q '^-D '; then echo '-D qemu.log'; fi)
QEMUOPTS += -no-reboot -D /dev/stdout
IMAGES = $(OBJDIR)/kernel/kernel.img
QEMUOPTS += -smp $(CPUS)
QEMUOPTS += $(QEMUEXTRA)

.gdbrc: .gdbrc.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

gdb: .gdbrc
	gdb -x .gdbrc obj/kernel/kernel

pre-qemu: .gdbrc

qemu: $(IMAGES) pre-qemu
	@$(QEMU) $(QEMUOPTS)

qemu-nox: $(IMAGES) pre-qemu
	@echo "***"
	@echo "*** Use Ctrl-a x to exit qemu"
	@echo "***"
	@$(QEMU) -nographic $(QEMUOPTS)

qemu-gdb: $(IMAGES) pre-qemu
	@sed "s/localhost:1234/localhost:$(GDBPORT)/" < .gdbrc.tmpl > .gdbrc
	@echo "***"
	@echo "*** Now run 'make gdb'." 1>&2
	@echo "***"
	@$(QEMU) $(QEMUOPTS) -S

qemu-nox-gdb: $(IMAGES) pre-qemu
	@sed "s/localhost:1234/localhost:$(GDBPORT)/" < .gdbrc.tmpl > .gdbrc
	@echo "***"
	@echo "*** Now run 'make gdb'." 1>&2
	@echo "***"
	@$(QEMU) -nographic $(QEMUOPTS) -S

print-qemu:
	@echo $(QEMU)

print-gdbport:
	@echo $(GDBPORT)

# For deleting the build
clean:
	rm -rf $(OBJDIR) .gdbrc jos.in qemu.log

realclean: clean
	rm -rf lab$(LAB).tar.gz \
		jos.out $(wildcard jos.out.*) \
		qemu.pcap $(wildcard qemu.pcap.*)

distclean: realclean
	rm -rf conf/gcc.mk

ifneq ($(V),@)
GRADEFLAGS += -v
endif

grade:
	@echo $(MAKE) clean
	@$(MAKE) clean || \
	  (echo "'make clean' failed.  HINT: Do you have another running instance of OpenLSD?" && exit 1)
	./grade-lab$(LAB) $(GRADEFLAGS)

handin: handin-check
ifeq (${COMMIT}, "")
		@git --no-pager log -3 --pretty=oneline;
		@echo "You did not specify a commit hash.";
		@echo "Please set COMMIT variable in the command line:";
		@echo "> make handin COMMIT=<your commit-hash>";
		@false;
endif
	@if git tag -l | grep "${TAG_LAB_SOLN}" >/dev/null; then \
		if ! git tag -d ${TAG_LAB_SOLN}; then \
			echo ;\
			echo "Tag: ${TAG_LAB_SOLN} exists already. Failed removing it."; \
			false; \
		fi; \
	fi;
	@echo "git tag ${TAG_LAB_SOLN} -m 'Solution for : Lab${LAB}' ${COMMIT} ";
	@if ! git tag ${TAG_LAB_SOLN} -m 'Solution for : Lab${LAB}' ${COMMIT}; then \
		echo ; \
		echo "*** Hand-in: Tagging your solution failed. Please retry."; \
		false; \
	fi;
	@echo "Your commit was tagged successfully: [ ${COMMIT} : ${TAG_LAB_SOLN} ]";

handin-bonus: handin-check
ifeq (${COMMIT}, "")
		@git --no-pager log -3 --pretty=oneline;
		@echo "You did not specify a commit hash for BONUS.";
		@echo "Please set COMMIT variable in the command line:";
		@echo "> make handin COMMIT=<your commit-hash>"
		@false;
endif
	@if git tag -l | grep "${TAG_LAB_BONUS}" >/dev/null; then \
		if ! git tag -d ${TAG_LAB_BONUS}; then \
			echo ;\
			echo "Tag: ${TAG_LAB_BONUS} exists already. Failed to remove it."; \
			false; \
		fi; \
	fi;
	@echo "git tag ${TAG_LAB_BONUS} -m 'Solution for BONUS of : Lab${LAB}' ${COMMIT} ";
	@if ! git tag ${TAG_LAB_BONUS} -m 'Solution for BONUS of : Lab${LAB}' ${COMMIT}; then \
		echo ; \
		echo "*** Hand-in BONUS: Tagging your solution failed. Please retry."; \
		false; \
	fi;
	@echo "Bonus commit tagged successfully: [ ${COMMIT} : ${TAG_LAB_BONUS} ]";

handin-check:
	@if test "$$(git symbolic-ref HEAD)" != refs/heads/lab$(LAB); then \
		git branch; \
		read -p "You are not on the lab$(LAB) branch.  Hand-in the current branch? [y/N] " r; \
		test "$$r" = y; \
	fi
	@if ! git diff-files --quiet || ! git diff-index --quiet --cached HEAD; then \
		git status; \
		echo; \
		echo "You have uncomitted changes.  Please commit or stash them."; \
		false; \
	fi
	@if test -n "`git ls-files -o --exclude-standard`"; then \
		git status; \
		read -p "Untracked files will not be handed in.  Continue? [y/N] " r; \
		test "$$r" = y; \
	fi

tarball: handin-check
	git archive --format=tar HEAD > ${PATCH_PREFIX}.lab$(LAB)-handin.tar
	tar -r --file=${PATCH_PREFIX}.lab$(LAB)-handin.tar ./.git
	gzip ${PATCH_PREFIX}.lab$(LAB)-handin.tar

apply-check: ${PATCH_PREFIX}.lab${LAB}.patch
	@if ! git apply --check ${PATCH_PREFIX}.lab${LAB}.patch; then \
		echo "** Patch does not apply: ${PATCH_PREFIX}.lab${LAB}.patch"; \
		false; \
	fi; \

apply: ${PATCH_PREFIX}.lab${LAB}.patch apply-check
	@if ! git am ${PATCH_PREFIX}.lab${LAB}.patch; then \
		echo "Failed to apply your soln patch: ${PATCH_PREFIX}.lab${LAB}.patch"; \
	fi; \

# For test runs

prep-%:
	$(V)$(MAKE) "INIT_CFLAGS=${INIT_CFLAGS} -DTEST=`case $* in *_*) echo $*;; *) echo user_$*;; esac`" $(IMAGES)

run-%-nox-gdb: prep-% pre-qemu
	@sed "s/localhost:1234/localhost:$(GDBPORT)/" < .gdbrc.tmpl > .gdbrc
	@echo "add-symbol-file obj/user/$*" >> .gdbrc
	@echo "***"
	@echo "*** Now run 'make gdb'." 1>&2
	@echo "***"
	@$(QEMU) -nographic $(QEMUOPTS) -S

run-%-gdb: prep-% pre-qemu
	@sed "s/localhost:1234/localhost:$(GDBPORT)/" < .gdbrc.tmpl > .gdbrc
	@echo "add-symbol-file obj/user/$*" >> .gdbrc
	@echo "***"
	@echo "*** Now run 'make gdb'." 1>&2
	@echo "***"
	@$(QEMU) $(QEMUOPTS) -S

run-%-nox: prep-% pre-qemu
	@$(QEMU) -nographic $(QEMUOPTS)

run-%: prep-% pre-qemu
	@$(QEMU) $(QEMUOPTS)

# This magic automatically generates makefile dependencies
# for header files included from C source files we compile,
# and keeps those dependencies up-to-date every time we recompile.
# See 'mergedep.pl' for more information.
$(OBJDIR)/.deps: $(foreach dir, $(OBJDIRS), $(wildcard $(OBJDIR)/$(dir)/*.d))
	@mkdir -p $(@D)
	@$(PERL) mergedep.pl $@ $^

-include $(OBJDIR)/.deps

always:
	@:

.PHONY: all always \
	handin tarball clean realclean distclean grade handin-prep handin-check gdb
