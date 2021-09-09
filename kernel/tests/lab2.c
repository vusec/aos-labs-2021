#include <string.h>
#include <types.h>
#include <paging.h>

#include <x86-64/asm.h>

#include <kernel/mem.h>

extern struct list buddy_free_list[];
extern struct page_table *kernel_pml4;

int lab2_do_check_ptbl_flags(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	uint64_t flags;

	(void)walker;

	if (!(*entry & PAGE_PRESENT) || (*entry & PAGE_HUGE)) {
		return 0;
	}

	flags = *entry & PAGE_MASK;

	if (flags != (PAGE_PRESENT | PAGE_WRITE | PAGE_USER)) {
		panic("%p points to a page table with the wrong "
		    "permissions!\n", base);
	}

	return 0;
}

void lab2_check_ptbl_flags(void)
{
	struct page_walker walker = {
		.pde_callback = lab2_do_check_ptbl_flags,
		.pdpte_callback = lab2_do_check_ptbl_flags,
		.pml4e_callback = lab2_do_check_ptbl_flags,
	};

	walk_all_pages(kernel_pml4, &walker);

	cprintf("[LAB 2] check_ptbl_flags() succeeded!\n");
}

int lab2_check_pte_wx(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	uint64_t flags;

	(void)walker;

	if (!(*entry & PAGE_PRESENT)) {
		return 0;
	}

	flags = *entry & (PAGE_WRITE | PAGE_NO_EXEC);

	if (flags == PAGE_WRITE) {
		panic("%p is mapped as write executable!\n", base);
	}

	return 0;
}

int lab2_check_pde_wx(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	uint64_t flags;

	(void)walker;

	if (!(*entry & PAGE_PRESENT) || !(*entry & PAGE_HUGE)) {
		return 0;
	}

	flags = *entry & (PAGE_WRITE | PAGE_NO_EXEC);

	if (flags == PAGE_WRITE) {
		panic("%p is mapped as write executable!\n", base);
	}

	return 0;
}

void lab2_check_wx(void)
{
	struct page_walker walker = {
		.pte_callback = lab2_check_pte_wx,
		.pde_callback = lab2_check_pde_wx,
	};

	walk_all_pages(kernel_pml4, &walker);

	cprintf("[LAB 2] check_wx() succeeded!\n");
}

void lab2_check_nx(void)
{
	if (!(read_msr(MSR_EFER) & MSR_EFER_NXE)) {
		panic("No eXecute bit is disabled!");
	}

	cprintf("[LAB 2] check_nx() succeeded!\n");
}

int lab2_check_pte_vas(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	(void)walker;

	if (!(*entry & PAGE_PRESENT)) {
		return 0;
	}

	if (KERNEL_VMA <= base && end < KERNEL_VMA + 64ULL * 1024 * 1024 * 1024) {
		return 0;
	}

	if (KPAGES <= base) {
		return 0;
	}

	if (KSTACK_TOP - KSTACK_SIZE <= base && end < KSTACK_TOP) {
		return 0;
	}

	panic("region mapped at %p - %p is not the identity map, page info "
		"structs or the kernel stack!", base, end);

	return 0;
}

int lab2_check_pde_vas(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	if (!(*entry & PAGE_PRESENT) || !(*entry & PAGE_HUGE)) {
		return 0;
	}

	return lab2_check_pte_vas(entry, base, end, walker);
}

void lab2_check_vas(void)
{
	struct page_walker walker = {
		.pte_callback = lab2_check_pte_vas,
		.pde_callback = lab2_check_pde_vas,
	};

	walk_all_pages(kernel_pml4, &walker);

	cprintf("[LAB 2] check_vas() succeeded!\n");
}

void lab2_check_4k_paging(void)
{
	struct page_info *page, *ret;
	physaddr_t *entry;
	size_t nfree;

	/* Remember the amount of free pages. */
	nfree = count_total_free_pages();

	/* Allocate a 4K page. */
	page = page_alloc(0);

	if (!page) {
		panic("cannot allocate 4K page!");
	}

	/* Insert the page. */
	assert(page_insert(kernel_pml4, page, 0, PAGE_PRESENT) == 0);
	assert(page->pp_ref == 1);
	assert(!page->pp_free);
	assert(kernel_pml4->entries[0] != 0);

	/* Look up the page. */
	ret = page_lookup(kernel_pml4, 0, &entry);
	assert((*entry & PAGE_MASK) == PAGE_PRESENT);
	assert(PAGE_ADDR(*entry) == page2pa(page));
	assert(ret == page);

	/* Remove the page. */
	page_remove(kernel_pml4, 0);
	assert(page->pp_free);
	assert(!page_lookup(kernel_pml4, 0, NULL));

	/* Check if the page tables have been cleaned up. */
	assert(kernel_pml4->entries[0] == 0);

	/* Check if we leaked memory. */
	assert(nfree == count_total_free_pages());

	cprintf("[LAB 2] check_4k_paging() succeeded!\n");
}

void lab2_check_2m_paging(void)
{
	struct page_info *page, *ret;
	physaddr_t *entry;
	size_t nfree;

	/* Remember the amount of free pages. */
	nfree = count_total_free_pages();

	/* Allocate a 2M page. */
	page = page_alloc(ALLOC_HUGE);

	if (!page) {
		panic("cannot allocate 2M page!");
	}

	/* Misaligned insert should fail. */
	assert(page_insert(kernel_pml4, page, (void *)PAGE_SIZE, PAGE_PRESENT) != 0);

	/* Insert the page. */
	assert(page_insert(kernel_pml4, page, 0, PAGE_PRESENT) == 0);
	assert(page->pp_ref == 1);
	assert(!page->pp_free);
	assert(kernel_pml4->entries[0] != 0);

	/* Look up the page. */
	ret = page_lookup(kernel_pml4, 0, &entry);
	assert((*entry & PAGE_MASK) == (PAGE_PRESENT | PAGE_HUGE));
	assert(PAGE_ADDR(*entry) == page2pa(page));
	assert(ret == page);

	/* Remove the page. */
	page_remove(kernel_pml4, 0);
	assert(page->pp_free);
	assert(!page_lookup(kernel_pml4, 0, NULL));

	/* Check if the page tables have been cleaned up. */
	assert(kernel_pml4->entries[0] == 0);

	/* Check if we leaked memory. */
	assert(nfree == count_total_free_pages());

	cprintf("[LAB 2] check_2m_paging() succeeded!\n");
}

int ismemset(void *s, int c, size_t n)
{
	unsigned char *p = s;
	size_t i;

	for (i = 0; i < n; ++i, ++p) {
		if (*p != c) {
			return 0;
		}
	}

	return 1;
}

void lab2_check_transparent_2m_paging(void)
{
	struct page_info *page, *ret;
	physaddr_t *entry;
	char *addr, *data;
	size_t i, k;
	size_t nfree;

	/* Remember the amount of free pages. */
	nfree = count_total_free_pages();

	/* Allocate a 2M page. */
	page = page_alloc(ALLOC_HUGE);

	if (!page) {
		panic("cannot allocate 2M page!");
	}

	/* Fill up page. */
	data = page2kva(page);

	for (i = 0; i < 512; ++i) {
		memset(data, i & 0xff, PAGE_SIZE);
		data += PAGE_SIZE;
	}

	/* Misaligned insert should fail. */
	assert(page_insert(kernel_pml4, page, (void *)PAGE_SIZE, PAGE_PRESENT) != 0);

	/* Insert the page. */
	assert(page_insert(kernel_pml4, page, 0, PAGE_PRESENT) == 0);
	assert(page->pp_ref == 1);
	assert(!page->pp_free);
	assert(kernel_pml4->entries[0] != 0);

	/* Look up the page. */
	ret = page_lookup(kernel_pml4, 0, &entry);
	assert((*entry & PAGE_MASK) == (PAGE_PRESENT | PAGE_HUGE));
	assert(PAGE_ADDR(*entry) == page2pa(page));
	assert(ret == page);

	/* Remove the 4K page at 0x1000. */
	unmap_page_range(kernel_pml4, (void *)PAGE_SIZE, PAGE_SIZE);

	/* Look up the other 4K pages. */
	addr = NULL;

	for (i = 0; i < PAGE_TABLE_ENTRIES; ++i, addr += PAGE_SIZE) {
		entry = NULL;
		ret = page_lookup(kernel_pml4, addr, &entry);

		if (addr == (void *)PAGE_SIZE) {
			assert(!ret);
			assert(!entry);
		} else {
			assert(ret);
			assert(entry);
			assert((*entry & PAGE_MASK) == PAGE_PRESENT);
			page = pa2page(PAGE_ADDR(*entry));
			assert(page->pp_order == 0);
			assert(!page->pp_free);
			assert(list_is_empty(&page->pp_node));

			if (!ismemset(addr, (i & 0xff), PAGE_SIZE)) {
				panic("page %p is corrupt", addr);
			}
		}
	}

	/* Allocate a 4K page. */
	page = page_alloc(0);

	if (!page) {
		panic("cannot allocate 4K page!");
	}

	/* Fill up page. */
	data = page2kva(page);
	memset(data, 1, PAGE_SIZE);

	/* Insert the page at 0x1000. */
	assert(page_insert(kernel_pml4, page, (void *)PAGE_SIZE, PAGE_PRESENT) == 0);

	/* Look up the page. */
	page_lookup(kernel_pml4, 0, &entry);
	assert((*entry & PAGE_MASK) == (PAGE_PRESENT | PAGE_HUGE));

	/* Check for corruption. */
	data = NULL;

	for (i = 0; i < 512; ++i) {
		if (!ismemset(data, i & 0xff, PAGE_SIZE)) {
			panic("page %p is corrupt\n", NULL);
		}

		data += PAGE_SIZE;
	}

	/* Remove the page. */
	unmap_page_range(kernel_pml4, 0, HPAGE_SIZE);
	assert(!page_lookup(kernel_pml4, 0, NULL));

	/* Check if the page tables have been cleaned up. */
	assert(kernel_pml4->entries[0] == 0);

	/* Check if we leaked memory. */
	assert(nfree == count_total_free_pages());

	cprintf("[LAB 2] check_transparent_2m_paging() succeeded!\n");
}

/* Checks if the order of the free pages on the free list for the respective
 * order matches.
 */
void lab2_check_free_list_order(void)
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

	cprintf("[LAB 2] check_free_list_order() succeeded!\n");
}

void lab2_check_memory_layout(struct boot_info *boot_info)
{
	struct mmap_entry *entry;
	struct page_info *page;
	size_t i;
	physaddr_t pa, end;
	void *addr;

	entry = (struct mmap_entry *)KADDR(boot_info->mmap_addr);
	end = PADDR(boot_alloc(0));

	for (i = 0; i < boot_info->mmap_len; ++i, ++entry) {
		for (pa = entry->addr; pa < entry->addr + entry->len;
		     pa += PAGE_SIZE) {
			if (npages <= PAGE_INDEX(pa)) {
				continue;
			}

			page = pa2page(pa);
			addr = (void *)ROUNDDOWN((uintptr_t)page, PAGE_SIZE);

			if (!page_lookup(kernel_pml4, addr, NULL)) {
				continue;
			}

			if ((pa == 0 ||
			    pa == (uintptr_t)boot_info->elf_hdr ||
			    (KERNEL_LMA <= pa && pa < end) ||
			    entry->type != MMAP_FREE) &&
			    page->pp_free) {
				panic("%p should not be free\n", page2pa(page));
			}
		}
	}

	cprintf("[LAB 2] check_memory_layout() succeeded!\n");
}

void check_buddy_consistency(physaddr_t addr, size_t order,
	struct page_info *parent);

void lab2_check_buddy_consistency(void)
{
	struct page_info *page;
	physaddr_t addr;

	for (addr = 0;
	     addr < npages * PAGE_SIZE;
	     addr += (1 << (BUDDY_MAX_ORDER + 12 - 1))) {
		if (!page_lookup(kernel_pml4, pa2page(addr), NULL)) {
			continue;
		}

		check_buddy_consistency(addr, BUDDY_MAX_ORDER - 1, NULL);
	}

	cprintf("[LAB 2] check_buddy_consistency() succeeded!\n");
}

void lab2_check_pml4(void)
{
	lab2_check_ptbl_flags();
	lab2_check_wx();
	lab2_check_nx();
}

void lab2_check_paging(void)
{
	lab2_check_4k_paging();
        /** BONUS
	lab2_check_2m_paging();
	lab2_check_transparent_2m_paging();
        **/
}

void lab2_check_buddy(struct boot_info *boot_info)
{
	lab2_check_free_list_order();
	lab2_check_memory_layout(boot_info);
	lab2_check_buddy_consistency();
	lab2_check_vas();
}

