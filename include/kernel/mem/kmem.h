#pragma once

#include <types.h>

#include <kernel/mem/slab.h>

#define slabs (this_cpu->kmem._slabs)
#define nslabs (this_cpu->kmem._nslabs)

int kmem_init(void);
int kmem_init_mp(void);
void *kmalloc(size_t size);
void kfree(void *p);

