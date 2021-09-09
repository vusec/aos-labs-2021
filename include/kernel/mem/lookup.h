#pragma once

#include <types.h>
#include <paging.h>

struct page_info *page_lookup(struct page_table *pml4, void *va,
	physaddr_t **entry_store);

