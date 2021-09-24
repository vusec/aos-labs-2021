#pragma once

#include <types.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <error.h>
#include <assert.h>
#include <task.h>
#include <syscall.h>
#include <paging.h>

#include <x86-64/memory.h>

enum {
	VMA_FREE = 0,
	VMA_ANONYMOUS,
	VMA_EXECUTABLE,
};

enum {
	VM_UNMAPPED = 0,
	VM_4K_PAGE,
	VM_2M_PAGE,
};

struct vma_info {
	char vm_name[64];
	void *vm_base, *vm_end;
	int vm_prot, vm_type, vm_mapped;
};

#define USED(x) (void)(x)

/* main user program */
void umain(int argc, char **argv);

/* libmain.c or entry.S */
extern const char *binary_name;

/* exit.c */
void exit(void);

/* readline.c */
char *readline(const char *buf);

/* syscall.c */
void puts(const char *string, size_t len);
int getc(void);
pid_t getpid(void);
int kill(pid_t);

int mquery(struct vma_info *info, void *addr);
void *mmap(void *addr, size_t len, int prot, int flags, int fd,
	uintptr_t offset);
void munmap(void *addr, size_t len);
int mprotect(void *addr, size_t len, int prot);
int madvise(void *addr, size_t len, int advise);

/* File open modes */
#define O_RDONLY    0x0000      /* open for reading only */
#define O_WRONLY    0x0001      /* open for writing only */
#define O_RDWR      0x0002      /* open for reading and writing */
#define O_ACCMODE   0x0003      /* mask for above modes */

#define O_CREAT     0x0100      /* create if nonexistent */
#define O_TRUNC     0x0200      /* truncate to zero length */
#define O_EXCL      0x0400      /* error if already exists */
#define O_MKDIR     0x0800      /* create directory, not regular file */

#define PROT_NONE     0
#define PROT_READ     (1 << 0)
#define PROT_WRITE    (1 << 1)
#define PROT_EXEC     (1 << 2)

#define MAP_PRIVATE   (1 << 0)
#define MAP_ANONYMOUS (1 << 1)
#define MAP_POPULATE  (1 << 4)
#define MAP_FIXED     (1 << 5)
#define MAP_FAILED    (void *)(0xffffffffffffffffull)

#define MADV_WILLNEED 1
#define MADV_DONTNEED 2

/* vma.c */
void print_vmas(void);

