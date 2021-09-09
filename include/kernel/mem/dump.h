#pragma once

#include <types.h>
#include <paging.h>

int dump_page_tables(struct page_table *pml4, uint64_t mask);

