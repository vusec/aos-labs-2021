#pragma once

#include <task.h>
#include <vma.h>

int walk_vma_range(struct task *task, void *base, size_t size,
	int (* get_vma)(struct task *, void *, size_t, struct vma *, void *),
	void *udata);

