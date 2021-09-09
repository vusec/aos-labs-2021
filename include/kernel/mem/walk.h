#pragma once

#include <types.h>
#include <paging.h>

struct page_walker;

typedef int (* map_pte_t)(physaddr_t *, uintptr_t, uintptr_t,
    struct page_walker *);

struct page_walker {
	map_pte_t pte_callback, pde_callback, pdpte_callback, pml4e_callback;
	map_pte_t pte_unmap, pde_unmap, pdpte_unmap, pml4e_unmap;
	int (* pt_hole_callback)(uintptr_t, uintptr_t, struct page_walker *);
	void *udata;
};

int walk_page_range(struct page_table *pml4, void *base, void *end,
	struct page_walker *walker);
int walk_all_pages(struct page_table *pml4, struct page_walker *walker);
int walk_user_pages(struct page_table *pml4, struct page_walker *walker);
int walk_kernel_pages(struct page_table *pml4, struct page_walker *walker);
