#pragma once

#include <types.h>
#include <paging.h>

void tlb_invalidate(struct page_table *pml4, void *va);

