#pragma once

#include <types.h>
#include <list.h>

#define SLAB_ALIGN 32

struct slab {
	struct list partial, full;
	uintptr_t info_off;
	size_t obj_size;
	size_t count;
};

struct slab_info {
	struct slab *slab;
	struct list node;
	struct list free_list;
	size_t free_count;
};

struct slab_obj {
	struct list node;
	struct slab_info *info;
};

int slab_alloc_chunk(struct slab *slab);
void slab_free_chunk(struct slab *slab, struct slab_info *info);
void slab_setup(struct slab *slab, size_t obj_size);
void *slab_alloc(struct slab *slab);
void slab_free(void *p);
