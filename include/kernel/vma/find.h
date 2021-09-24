#pragma once

#include <task.h>
#include <vma.h>

struct vma *find_vma(struct rb_node **out_parent, int *out_dir,
	struct rb_tree *tree, void *addr);
struct vma *task_find_vma(struct task *task, void *addr);

