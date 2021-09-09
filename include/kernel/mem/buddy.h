#pragma once

#include <boot.h>
#include <assert.h>
#include <paging.h>

#include <x86-64/memory.h>

#define BUDDY_MAX_ORDER (BUDDY_2M_PAGE + 1)

extern struct page_info *pages;
extern size_t npages;

/*
 * This macro takes a kernel virtual address -- an address that points above
 * KERNBASE, where the machine's maximum 256MB of physical memory is mapped --
 * and returns the corresponding physical address.  It panics if you pass it a
 * non-kernel virtual address.
 */
#define PADDR(kva) _paddr(__FILE__, __LINE__, kva)

static inline physaddr_t _paddr(const char *file, int line, void *kva)
{
	if ((uintptr_t)kva < KERNEL_VMA)
		_panic(file, line, "PADDR called with invalid kva %08lx", kva);
	return (physaddr_t)kva - KERNEL_VMA;
}

/* This macro takes a physical address and returns the corresponding kernel
 * virtual address.  It panics if you pass an invalid physical address. */
#define KADDR(pa) _kaddr(__FILE__, __LINE__, pa)

static inline void *_kaddr(const char *file, int line, physaddr_t pa)
{
	if (PAGE_INDEX(pa) >= npages)
		_panic(file, line, "KADDR called with invalid pa %08lx", pa);
	return (void *)(pa + KERNEL_VMA);
}


enum {
	/* For page_alloc, zero the returned physical page. */
	ALLOC_ZERO = 1 << 0,
	ALLOC_HUGE = 1 << 1,
	ALLOC_PREMAPPED = 1 << 2,
};

/* The buddy allocator order for known page sizes. */
enum {
	BUDDY_4K_PAGE = 0,
	BUDDY_2M_PAGE = 9,
	BUDDY_1G_PAGE = 18,
};

size_t count_free_pages(size_t order);
void show_buddy_info(void);
size_t count_total_free_pages(void);
struct page_info *page_alloc(int alloc_flags);
struct page_info *buddy_find(size_t req_order);
void page_free(struct page_info *pp);
void page_decref(struct page_info *pp);

static inline physaddr_t page2pa(struct page_info *pp)
{
	return (pp - pages) << PAGE_TABLE_SHIFT;
}

static inline struct page_info *pa2page(physaddr_t pa)
{
	if (PAGE_INDEX(pa) >= npages)
		panic("pa2page called with invalid pa");

	return pages + PAGE_INDEX(pa);
}

static inline void *page2kva(struct page_info *pp)
{
	return KADDR(page2pa(pp));
}

