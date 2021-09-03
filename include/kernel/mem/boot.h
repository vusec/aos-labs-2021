#pragma once

#include <boot.h>
#include <assert.h>
#include <paging.h>

#include <x86-64/memory.h>

extern char bootstacktop[], bootstack[];

void *boot_alloc(uint32_t n);
void align_boot_info(struct boot_info *boot_info);

