#pragma once

#include <boot.h>
#include <assert.h>
#include <paging.h>

#include <x86-64/memory.h>

extern struct page_table *kernel_pml4;

void mem_init(struct boot_info *boot_info);
void page_init(struct boot_info *boot_info);
void page_init_ext(struct boot_info *boot_info);

