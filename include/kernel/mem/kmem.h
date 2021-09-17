#pragma once

#include <types.h>

int kmem_init(void);
void *kmalloc(size_t size);
void kfree(void *p);
