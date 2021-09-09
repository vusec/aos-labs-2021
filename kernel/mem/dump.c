#include <types.h>
#include <paging.h>

#include <kernel/mem.h>

struct dump_info {
	uintptr_t base;
	uintptr_t end;
	uint64_t flags;
	uint64_t mask;
};

/* Print the region before the hole if there was any and reset the info struct.
 */
static int dump_hole(uintptr_t base, uintptr_t end, struct page_walker *walker)
{
	struct dump_info *info = walker->udata;

	if (info->flags & PAGE_PRESENT) {
		cprintf("  %016p - %016p [%c%c%c%c",
			info->base,
			info->end,
			(info->flags & PAGE_PRESENT) ? 'r' : '-',
			(info->flags & PAGE_WRITE) ? 'w' : '-',
			(info->flags & PAGE_NO_EXEC) ? '-' : 'x',
			(info->flags & PAGE_USER) ? 'u' : '-'
		);

		if (info->mask & PAGE_HUGE) {
			cprintf(" %s",
				(info->flags & PAGE_HUGE) ? "2M" : "4K"
			);
		}

		cprintf("]\n");
	}

	info->flags = 0;

	return 0;
}

/* Update the end pointer if the flags are the same. Otherwise print the region
 * and keep track of the new region.
 */
static int dump_pte(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	struct dump_info *info = walker->udata;
	uint64_t flags;

	flags = *entry & info->mask;

	if (flags == info->flags) {
		info->end = end;

		return 0;
	}

	dump_hole(base, end, walker);

	info->base = base;
	info->end = end;
	info->flags = flags;

	return 0;
}

/* Update the end pointer if the flags are the same and the PDE points to a
 * huge page. Otherwise print the region and keep track of the new region.
 */
static int dump_pde(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	struct dump_info *info = walker->udata;
	uint64_t flags;

	if (!(*entry & PAGE_HUGE))
		return 0;

	flags = *entry & info->mask;

	if (flags == info->flags) {
		info->end = end;

		return 0;
	}

	dump_hole(base, end, walker);

	info->base = base;
	info->end = end;
	info->flags = flags;

	return 0;
}

/* Given the root pml4 to the page table hierarchy, dumps the mapped regions
 * with the same flags. mask can be PAGE_HUGE to differentiate regions mapped
 * with normal pages from those mapped with huge pages.
 */
int dump_page_tables(struct page_table *pml4, uint64_t mask)
{
	struct dump_info info = {
		.base = 0,
		.flags = 0,
		.mask = mask | PAGE_PRESENT | PAGE_WRITE | PAGE_NO_EXEC |
		        PAGE_USER,
	};
	struct page_walker walker = {
		.pte_callback = dump_pte,
		.pde_callback = dump_pde,
		.pt_hole_callback = dump_hole,
		.udata = &info,
	};

	if (walk_all_pages(pml4, &walker) < 0)
		return -1;

	dump_hole(0, 0, &walker);

	return 0;
}

