#pragma once

#include <types.h>
#include <paging.h>
#include <task.h>

int check_user_mem(uintptr_t *fault_va, struct page_table *pml4, void *va,
	size_t size, uint64_t flags);
void assert_user_mem(struct task *task, void *va, size_t size, int flags);

