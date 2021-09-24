#pragma once

#include <task.h>
#include <vma.h>

int task_page_fault_handler(struct task *task, void *va, int flags);

