#include <types.h>
#include <paging.h>

#include <kernel/mem.h>

struct lookup_info {
	physaddr_t *entry;
};

/* If the PTE points to a present page, store the pointer to the PTE into the
 * info struct of the walker.
 */
static int lookup_pte(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	struct lookup_info *info = walker->udata;

	/* LAB 2: your code here. */
	return 0;
}

/* If the PDE points to a present huge page, store the pointer to the PDE into
 * the info struct of the walker. */
static int lookup_pde(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	struct lookup_info *info = walker->udata;

	/* LAB 2: your code here. */
	return 0;
}

/* Return the page mapped at virtual address 'va'.
 * If entry_store is not zero, then we store the address of the PTE for this
 * page into entry_store.
 * This is function can be used to verify page permissions for system call
 * arguments, but should generally not be used by most callers.
 *
 * Return NULL if there is no page mapped at va.
 *
 * Hint: this function calls walk_page_range() and pa2page().
 */
struct page_info *page_lookup(struct page_table *pml4, void *va,
    physaddr_t **entry_store)
{
	struct lookup_info info = {
		.entry = NULL,
	};

	struct page_walker walker = {
		.pte_callback = lookup_pte,
		.pde_callback = lookup_pde,
		.udata = &info,
	};

	if (walk_page_range(pml4, va, (void *)((uintptr_t)va + PAGE_SIZE),
			    &walker) < 0)
		return NULL;

	/* LAB 2: your code here. */

	return NULL;
}
