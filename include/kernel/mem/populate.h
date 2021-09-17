#pragma once

#include <types.h>
#include <paging.h>

void populate_region(struct page_table *pml4, void *va, size_t size,
	uint64_t flags);
