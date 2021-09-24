/* System call stubs. */

#include <syscall.h>
#include <lib.h>

extern int64_t do_syscall(uint64_t num, uint64_t a1, uint64_t a2,
	uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6);

static inline unsigned long syscall(int num, int check,
	unsigned long a1, unsigned long a2, unsigned long a3, unsigned long a4,
	unsigned long a5, unsigned long a6)
{
	unsigned long ret;

	/*
	 * Generic system call: pass system call number in AX,
	 * up to five parameters in DX, CX, BX, DI, SI.
	 * Interrupt kernel with T_SYSCALL.
	 *
	 * The "volatile" tells the assembler not to optimize
	 * this instruction away just because we don't use the
	 * return value.
	 *
	 * The last clause tells the assembler that this can
	 * potentially change the condition codes and arbitrary
	 * memory locations.
	 */
	ret = do_syscall(num, a1, a2, a3, a4, a5, a6);

	if(check && ret < 0) {
		panic("syscall %d returned %d (> 0)", num, ret);
	}

	return ret;
}

void puts(const char *s, size_t len)
{
	syscall(SYS_cputs, 0, (uintptr_t)s, len, 0, 0, 0, 0);
}

int getc(void)
{
	return syscall(SYS_cgetc, 0, 0, 0, 0, 0, 0, 0);
}

int kill(pid_t pid)
{
	return syscall(SYS_kill, 1, pid, 0, 0, 0, 0, 0);
}

pid_t getpid(void)
{
	 return syscall(SYS_getpid, 0, 0, 0, 0, 0, 0, 0);
}

int mquery(struct vma_info *info, void *addr)
{
	return syscall(SYS_mquery, 0, (uint64_t)info, (uint64_t)addr, 0, 0, 0, 0);
}

void *mmap(void *addr, size_t len, int prot, int flags, int fd, uintptr_t offset)
{
	return (void *)syscall(SYS_mmap, 0, (uint64_t)addr, len, prot, flags, fd, offset);
}

void munmap(void *addr, size_t len)
{
	syscall(SYS_munmap, 0, (uint64_t)addr, len, 0, 0, 0, 0);
}

int mprotect(void *addr, size_t len, int prot)
{
	return syscall(SYS_mprotect, 0, (uint64_t)addr, len, prot, 0, 0, 0);
}

int madvise(void *addr, size_t len, int advice)
{
	return syscall(SYS_madvise, 0, (uint64_t)addr, len, advice, 0, 0, 0);
}

