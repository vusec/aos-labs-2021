#pragma once

#include <x86-64/paging.h>

/* At IOPHYSMEM (640K) there is a 384K hole for I/O.  From the kernel,
 * IOPHYSMEM can be addressed at KERNBASE + IOPHYSMEM.  The hole ends
 * at physical address EXTPHYSMEM. */
#define IO_PHYS_MEM   0x0A0000
#define EXT_PHYS_MEM  0x100000

/* Physical address of startup code for non-boot CPUs (APs) */
#define MPENTRY_PADDR   0x7000

/* Kernel address space limit. */
#ifdef __ASSEMBLER__
#define KERNEL_LIM 0xffffffffffffffff
#else
#define KERNEL_LIM UINT64_C(0xffffffffffffffff)
#endif

/* A fixed location for the array of page_info structs. */
#ifdef __ASSEMBLER__
#define KPAGES 0xFFFFFF8000000000
#else
#define KPAGES UINT64_C(0xFFFFFF8000000000)
#endif

/* Memory mapped I/O. */
#define MMIO_LIM KPAGES
#define MMIO_BASE (MMIO_LIM - PDPT_SPAN)

/* Map of PIDs */
#define PIDMAP_LIM MMIO_BASE 
#define PIDMAP_BASE (PIDMAP_LIM - PDPT_SPAN)

/* Kernel stack. */
#define KSTACK_TOP PIDMAP_BASE
#define KSTACK_SIZE (8 * PAGE_SIZE)
#define KSTACK_GAP (8 * PAGE_SIZE)

/* User address space limit. */
#ifdef __ASSEMBLER__
#define USER_LIM 0x800000000000
#else
#define USER_LIM UINT64_C(0x800000000000)
#endif

/* User stacks. */
#define USER_TOP USER_LIM
#define UXSTACK_TOP USER_TOP
#define USTACK_TOP (UXSTACK_TOP - 2 * PAGE_SIZE)

/* The amount of memory mapped in the boot stub. */
#define BOOT_MAP_LIM (8 * 1024 * 1024)

