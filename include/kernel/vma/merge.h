#pragma once

#include <task.h>
#include <vma.h>

struct vma *merge_vma(struct task *task, struct vma *lhs, struct vma *rhs);
struct vma *merge_vmas(struct task *task, struct vma *vma);

