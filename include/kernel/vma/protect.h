#pragma once

#include <task.h>
#include <vma.h>

int protect_vma_range(struct task *task, void *base, size_t size, int flags);

