#pragma once

#include <task.h>
#include <vma.h>

int insert_vma(struct task *task, struct vma *vma);
struct vma *add_executable_vma(struct task *task, char *name, void *addr,
	size_t size, int flags, void *src, size_t len);
struct vma *add_anonymous_vma(struct task *task, char *name, void *addr,
	size_t size, int flags);
struct vma *add_vma(struct task *task, char *name, void *addr, size_t size,
	int flags);

