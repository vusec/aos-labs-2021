#pragma once

#include <types.h>
#include <paging.h>

void unmap_page_range(struct page_table *pml4, void *va, size_t size);
void unmap_user_pages(struct page_table *pml4);
void page_remove(struct page_table *pml4, void *va);

