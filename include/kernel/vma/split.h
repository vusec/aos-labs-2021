#pragma once

#include <task.h>
#include <vma.h>

struct vma *split_vma(struct task *task, struct vma *lhs, void *addr);
struct vma *split_vmas(struct task *task, struct vma *vma, void *base,
	size_t size);

