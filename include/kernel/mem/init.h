#pragma once

#include <boot.h>
#include <assert.h>
#include <paging.h>

#include <x86-64/memory.h>

void mem_init(struct boot_info *boot_info);
void page_init(struct boot_info *boot_info);

