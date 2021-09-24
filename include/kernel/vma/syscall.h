#pragma once

struct vma_info;

int sys_mquery(struct vma_info *info, void *addr);
void *sys_mmap(void *addr, size_t len, int prot, int flags, int fd,
	uintptr_t offset);
void sys_munmap(void *addr, size_t len);
int sys_mprotect(void *addr, size_t len, int prot);
int sys_madvise(void *addr, size_t len, int advise);

