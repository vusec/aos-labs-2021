#pragma once

#include <task.h>
#include <vma.h>

void remove_vma(struct task *task, struct vma *vma);
void free_vmas(struct task *task);
int do_remove_vma(struct task *task, void *base, size_t size, struct vma *vma,
	void *udata);
int remove_vma_range(struct task *task, void *base, size_t size);
int unmap_vma_range(struct task *task, void *base, size_t size);

