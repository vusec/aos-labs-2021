#include <types.h>
#include <assert.h>
#include <list.h>
#include <paging.h>

#include <kernel/mem.h>

extern struct list buddy_free_list[];

/* Checks the number of free pages available in both base memory and high
 * memory.
 */
void lab1_check_free_list_avail(void)
{
	struct page_info *page;
	struct list *node;
	size_t order;
	size_t nfree_basemem = 0;
	size_t nfree_extmem = 0;

	for (order = 0; order < BUDDY_MAX_ORDER; ++order) {
		list_foreach(buddy_free_list + order, node) {
			page = container_of(node, struct page_info, pp_node);

			if (page2pa(page) < EXT_PHYS_MEM) {
				++nfree_basemem;
			} else {
				++nfree_extmem;
			}
		}
	}

	assert(nfree_basemem > 0);
	assert(nfree_extmem > 0);

	cprintf("[LAB 1] check_free_list_avail() succeeded!\n");
}

/* Checks if the order of the free pages on the free list for the respective
 * order matches.
 */
void lab1_check_free_list_order(void)
{
	struct page_info *page;
	struct list *node;
	size_t order;
	size_t nviolations = 0;

	for (order = 0; order < BUDDY_MAX_ORDER; ++order) {
		list_foreach(buddy_free_list + order, node) {
			page = container_of(node, struct page_info, pp_node);

			if (page->pp_order != order)
				++nviolations;
		}
	}

	if (nviolations != 0) {
		panic("found %u order violations in free list", nviolations);
	}

	cprintf("[LAB 1] check_free_list_order() succeeded!\n");
}

void lab1_check_memory_layout(struct boot_info *boot_info)
{
	struct mmap_entry *entry;
	struct page_info *page;
	size_t order;
	physaddr_t pa, end;

	entry = (struct mmap_entry *)KADDR(boot_info->mmap_addr);
	end = PADDR(boot_alloc(0));

	for (order = 0; order < boot_info->mmap_len; ++order, ++entry) {
		for (pa = entry->addr; pa < entry->addr + entry->len;
		     pa += PAGE_SIZE) {
			if (pa >= BOOT_MAP_LIM)
				continue;

			page = pa2page(pa);

			if ((pa == 0 ||
			    pa == PAGE_ADDR(PADDR(boot_info)) ||
			    pa == (uintptr_t)boot_info->elf_hdr ||
			    (KERNEL_LMA <= pa && pa < end) ||
			    entry->type != MMAP_FREE) &&
			    page->pp_free) {
				panic("%p should not be free\n", page2pa(page));
			}
		}
	}

	cprintf("[LAB 1] check_memory_layout() succeeded!\n");
}

void check_buddy_consistency(physaddr_t addr, size_t order, struct page_info *parent)
{
	struct page_info *page;

	page = pa2page(addr);

	if (parent && parent != page) {
		if (page->pp_free || !list_is_empty(&page->pp_node) ||
			page->pp_order < parent->pp_order) {
			panic("page %p of order %u is free, while parent page "
				"%p of order %u is already free",
				page2pa(page), page->pp_order,
				page2pa(parent), parent->pp_order);
		}
	}

	if (page->pp_free && list_is_empty(&page->pp_node)) {
		panic("page %p of order %u is free, but not on the free list",
			page2pa(page), page->pp_order);
	}

	if (!page->pp_free && !list_is_empty(&page->pp_node)) {
		panic("page %p of order %u is in use, but on the free list",
			page2pa(page), page->pp_order);
	}

	if (page->pp_free && list_is_empty(&page->pp_node)) {
		parent = page;
	}

	if (order == 0) {
		return;
	}

	--order;
	check_buddy_consistency(addr, order, parent);
	check_buddy_consistency(addr | (1 << (order + 12)), order, parent);
}

void lab1_check_buddy_consistency(void)
{
	struct page_info *page;
	physaddr_t addr;

	for (addr = 0;
	     addr < BOOT_MAP_LIM;
	     addr += (1 << (BUDDY_MAX_ORDER + 12 - 1))) {
		check_buddy_consistency(addr, BUDDY_MAX_ORDER - 1, NULL);
	}

	cprintf("[LAB 1] check_buddy_consistency() succeeded!\n");
}

void lab1_check_split_and_merge(void)
{
	struct list stolen_free_list[10];
	struct page_info *page;
	size_t order;
	size_t nfree_pages;

	/* Count the number of huge pages. */
	nfree_pages = count_free_pages(BUDDY_2M_PAGE);

	/* Allocate a huge page. */
	page = page_alloc(ALLOC_HUGE);

	if (!page) {
		panic("can't allocate 2M page!");
	}

	/* Check against the count of huge pages. */
	assert(count_free_pages(BUDDY_2M_PAGE) + 1 == nfree_pages);

	/* Steal the lists of free pages. */
	for (order = 0; order < BUDDY_MAX_ORDER; ++order) {
		stolen_free_list[order] = buddy_free_list[order];
		list_init(buddy_free_list + order);
	}

	/* Return the huge page. */
	page_free(page);

	/* Check if we have a huge page. */
	for (order = 0; order < BUDDY_2M_PAGE; ++order) {
		assert(count_free_pages(order) == 0);
	}

	assert(count_free_pages(BUDDY_2M_PAGE) == 1);

	/* Allocate a normal page. */
	page = page_alloc(0);

	if (!page) {
		panic("can't allocate 4K page!");
	}

	/* Check if we have a page of every order. */
	for (order = 0; order < BUDDY_2M_PAGE; ++order) {
		assert(count_free_pages(order) == 1);
	}

	assert(count_free_pages(BUDDY_2M_PAGE) == 0);

	/* Return the normal page. */
	page_free(page);

	/* Check if we have a huge page. */
	for (order = 0; order < BUDDY_2M_PAGE; ++order) {
		assert(count_free_pages(order) == 0);
	}

	assert(count_free_pages(BUDDY_2M_PAGE) == 1);

	/* Allocate the huge page again. */
	page = page_alloc(ALLOC_HUGE);

	if (!page) {
		panic("can't allocate 2M page!");
	}

	/* Return the lists of free pages. */
	for (order = 0; order < BUDDY_MAX_ORDER; ++order) {
		buddy_free_list[order] = stolen_free_list[order];
	}

	/* Return the huge page. */
	page_free(page);

	cprintf("[LAB 1] check_split_and_merge() succeeded!\n");
}

void lab1_check_mem(struct boot_info *boot_info)
{
	lab1_check_free_list_avail();
	lab1_check_free_list_order();
	lab1_check_memory_layout(boot_info);
	lab1_check_buddy_consistency();
	lab1_check_split_and_merge();
}
