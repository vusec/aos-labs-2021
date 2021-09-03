#pragma once

#define PAGE_SIZE 4096

#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITE (1 << 1)
#define PAGE_USER (1 << 2)
#define PAGE_WRITE_THROUGH (1 << 3)
#define PAGE_NO_CACHE (1 << 4)
#define PAGE_ACCESSED (1 << 5)
#define PAGE_DIRTY (1 << 6)
#define PAGE_HUGE (1 << 7)
#define PAGE_GLOBAL (1 << 8)

#ifdef __ASSEMBLER__
#define PAGE_NO_EXEC (1 << 63)
#else
#define PAGE_NO_EXEC (1ULL << 63)
#endif

#define PAGE_TABLE_SHIFT (12)
#define PAGE_DIR_SHIFT   (PAGE_TABLE_SHIFT + 9)
#define PDPT_SHIFT       (PAGE_DIR_SHIFT + 9)
#define PML4_SHIFT       (PDPT_SHIFT + 9)

#define HPAGE_SIZE       (1 << PAGE_DIR_SHIFT)

#ifdef __ASSEMBLER__
#define PAGE_TABLE_SPAN (1 << PAGE_DIR_SHIFT)
#define PAGE_DIR_SPAN   (1 << PDPT_SHIFT)
#define PDPT_SPAN       (1 << PML4_SHIFT)
#else
#define PAGE_TABLE_SPAN (UINT64_C(1) << PAGE_DIR_SHIFT)
#define PAGE_DIR_SPAN   (UINT64_C(1) << PDPT_SHIFT)
#define PDPT_SPAN       (UINT64_C(1) << PML4_SHIFT)
#endif

#define PAGE_TABLE_MASK ((1 << 9) - 1)
#define PAGE_DIR_MASK   ((1 << 9) - 1)
#define PDPT_MASK       ((1 << 9) - 1)
#define PML4_MASK       ((1 << 9) - 1)

#define PAGE_INDEX(x)       ((x) >> PAGE_TABLE_SHIFT)
#define PAGE_TABLE_INDEX(x) (((x) >> PAGE_TABLE_SHIFT) & PAGE_TABLE_MASK)
#define PAGE_DIR_INDEX(x)   (((x) >> PAGE_DIR_SHIFT) & PAGE_DIR_MASK)
#define PDPT_INDEX(x)       (((x) >> PDPT_SHIFT) & PDPT_MASK)
#define PML4_INDEX(x)       (((x) >> PML4_SHIFT) & PML4_MASK)

#define PAGE_MASK (0xFFFF000000000FFF)
#define PAGE_UMASK (PAGE_PRESENT | PAGE_WRITE | PAGE_NO_EXEC | PAGE_USER)
#define PAGE_ADDR(x) ((x) & ~PAGE_MASK)

#define PF_PRESENT  (1 << 0)
#define PF_WRITE    (1 << 1)
#define PF_USER     (1 << 2)
#define PF_RESERVED (1 << 3)
#define PF_IFETCH   (1 << 4)

#ifndef __ASSEMBLER__
#include <x86-64/types.h>
#include <x86-64/asm.h>

#define PAGE_TABLE_ENTRIES (PAGE_SIZE / sizeof(physaddr_t))

struct page_table {
	physaddr_t entries[PAGE_TABLE_ENTRIES];
} __attribute__((aligned(PAGE_SIZE)));

static inline void load_pml4(struct page_table *pml4)
{
	write_cr3((uintptr_t)pml4);
}

static inline void flush_page(void *addr)
{
	asm volatile("invlpg (%0)" :: "r" (addr) : "memory");
}

static inline int page_aligned(uintptr_t p)
{
	return !(p & (PAGE_SIZE - 1));
}

static inline int hpage_aligned(uintptr_t p)
{
	return !(p & (HPAGE_SIZE - 1));
}
#endif /* !defined(__ASSEMBLER__) */

