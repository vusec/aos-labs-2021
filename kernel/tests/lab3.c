#include <types.h>
#include <cpu.h>
#include <paging.h>

#include <x86-64/asm.h>

#include <kernel/acpi.h>
#include <kernel/mem.h>

void lab3_check_kmem_init(void)
{
	struct slab *slab;
	size_t obj_size;
	size_t i;

	for (i = 0; i < 32; ++i) {
		slab = slabs + i;
		obj_size = ROUNDUP((i + 1) * 32 + sizeof(struct slab_obj), 32);

		if (!list_is_empty(&slab->partial)) {
			panic("slab for object size %u has partial slabs",
				obj_size);
		}

		if (!list_is_empty(&slab->full)) {
			panic("slab for object size %u has full slabs",
				obj_size);
		}

		if (slab->obj_size != obj_size) {
			panic("slab for object size %u has unexpected object "
				"size of %u\n", obj_size, slab->obj_size);
		}
	}

	cprintf("[LAB 3] check_kmem_init() succeeded!\n");
}

void lab3_check_kmem_single_alloc(void)
{
	struct slab *slab;
	struct slab_info *info;
	struct slab_obj *obj;
	void *p;
	size_t obj_size, real_obj_size;
	size_t i;

	for (i = 0; i < 32; ++i) {
		slab = slabs + i;
		obj_size = (i + 1) * 32;
		real_obj_size = ROUNDUP(obj_size + sizeof(struct slab_obj), 32);

		if (!list_is_empty(&slab->partial)) {
			panic("slab for object size %u has partial slabs",
				obj_size);
		}

		if (!list_is_empty(&slab->full)) {
			panic("slab for object size %u has full slabs",
				obj_size);
		}

		if (slab->obj_size != real_obj_size) {
			panic("slab for object size %u has unexpected object "
				"size of %u\n", obj_size, slab->obj_size);
		}

		p = kmalloc(obj_size);

		if (!p) {
			panic("kmalloc(%u) returned NULL", obj_size);
		}

		if (list_is_empty(&slab->partial)) {
			panic("slab for object size %u has no partial slabs",
				obj_size);
		}

		if (!list_is_empty(&slab->full)) {
			panic("slab for object size %u has full slabs",
				obj_size);
		}

		info = container_of(slab->partial.next, struct slab_info, node);

		if (info->free_count + 1 != slab->count) {
			panic("slab for object size %u has inconsistent free "
				"counts");
		}
		
		obj = (struct slab_obj *)p - 1;

		if (obj->info != info) {
			panic("allocated object does not point to the slab it "
				"has been allocated from");
		}

		kfree(p);

		if (!list_is_empty(&slab->partial)) {
			panic("slab for object size %u has partial slabs",
				obj_size);
		}

		if (!list_is_empty(&slab->full)) {
			panic("slab for object size %u has full slabs",
				obj_size);
		}
	}

	cprintf("[LAB 3] check_kmem_single_alloc() succeeded!\n");
}

void lab3_check_kmem_full_alloc(void)
{
	struct slab *slab;
	struct slab_info *info;
	struct slab_obj *obj;
	void *p = NULL;
	size_t obj_size, real_obj_size;
	size_t i, k;

	for (i = 0; i < 32; ++i) {
		slab = slabs + i;
		obj_size = (i + 1) * 32;
		real_obj_size = ROUNDUP(obj_size + sizeof(struct slab_obj), 32);

		if (!list_is_empty(&slab->partial)) {
			panic("slab for object size %u has partial slabs",
				obj_size);
		}

		if (!list_is_empty(&slab->full)) {
			panic("slab for object size %u has full slabs",
				obj_size);
		}

		if (slab->obj_size != real_obj_size) {
			panic("slab for object size %u has unexpected object "
				"size of %u\n", obj_size, slab->obj_size);
		}

		for (k = 0; k < slab->count; ++k) {
			p = kmalloc(obj_size);

			if (!p) {
				panic("kmalloc(%u) returned NULL", obj_size);
			}
		}

		if (!list_is_empty(&slab->partial)) {
			panic("slab for object size %u has partial slabs",
				obj_size);
		}

		if (list_is_empty(&slab->full)) {
			panic("slab for object size %u has no full slabs",
				obj_size);
		}

		info = container_of(slab->full.next, struct slab_info, node);

		if (info->free_count != 0) {
			panic("slab for object size %u still has free "
				"objects", obj_size);
		}

		obj = (struct slab_obj *)p - 1;

		if (obj->info != info) {
			panic("allocated object does not point to the slab it "
				"has been allocated from");
		}

		kfree(p);

		if (list_is_empty(&slab->partial)) {
			panic("slab for object size %u has no partial slabs",
				obj_size);
		}

		if (!list_is_empty(&slab->full)) {
			panic("slab for object size %u has full slabs",
				obj_size);
		}

		slab_free_chunk(slab, info);
	}

	cprintf("[LAB 3] check_kmem_full_alloc() succeeded!\n");
}

void lab3_check_kmem_limit(void)
{
	void *p;
	size_t size = (nslabs + 1) * 32 + 1;

	p = kmalloc(size);

	if (p) {
		panic("kmalloc(%u) should not allocate memory", size);
	}

	cprintf("[LAB 3] check_kmem_limit() succeeded!\n");
}

void lab3_check_kmem(void)
{
	lab3_check_kmem_init();
	lab3_check_kmem_single_alloc();
	lab3_check_kmem_full_alloc();
	lab3_check_kmem_limit();
}

int lab3_check_pte_us(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	int is_user;

	(void)walker;

	if (!(*entry & PAGE_PRESENT)) {
		return 0;
	}

	is_user = *entry & PAGE_USER;

	if (base < KERNEL_VMA && !is_user) {
		panic("%p is mapped as supervisor!", base);
	} else if (base >= KERNEL_VMA && is_user) {
		panic("%p is mapped as user!", base);
	}

	return 0;
}

int lab3_check_pde_us(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	int is_user;

	(void)walker;

	if (!(*entry & PAGE_PRESENT) || !(*entry & PAGE_HUGE)) {
		return 0;
	}

	is_user = *entry & PAGE_USER;

	if (base < KERNEL_VMA && !is_user) {
		panic("%p is mapped as supervisor!", base);
	} else if (base >= KERNEL_VMA && is_user) {
		panic("%p is mapped as user!", base);
	}

	return 0;
}

int lab3_check_us(struct page_table *pml4)
{
	struct page_walker walker = {
		.pte_callback = lab3_check_pte_us,
		.pde_callback = lab3_check_pde_us,
	};

	walk_all_pages(pml4, &walker);

	cprintf("[LAB 3] check_us() succeeded!\n");

	return 0;
}

int lab3_check_pte_wx(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	uint64_t flags;

	(void)walker;

	flags = *entry & (PAGE_WRITE | PAGE_NO_EXEC);

	if (flags == PAGE_WRITE) {
		panic("%p is mapped as write executable!\n", base);
	}

	return 0;
}

int lab3_check_pde_wx(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	uint64_t flags;

	(void)walker;

	if (!(*entry & PAGE_HUGE)) {
		return 0;
	}

	flags = *entry & (PAGE_WRITE | PAGE_NO_EXEC);

	if (flags == PAGE_WRITE) {
		panic("%p is mapped as write executable!\n", base);
	}

	return 0;
}

int lab3_check_wx(struct page_table *pml4)
{
	struct page_walker walker = {
		.pte_callback = lab3_check_pte_wx,
		.pde_callback = lab3_check_pde_wx,
	};

	walk_all_pages(pml4, &walker);

	cprintf("[LAB 3] check_wx() succeeded!\n");

	return 0;
}

int lab3_check_null(struct page_table *pml4) {
	if (page_lookup(pml4, 0, NULL)) {
		panic("NULL should not be mapped!");
	}

	cprintf("[LAB 3] check_null() succeeded!\n");

	return 0;
}

void lab3_check_vas(struct page_table *pml4)
{
	lab3_check_us(pml4);
	lab3_check_wx(pml4);
	lab3_check_null(pml4);
}

