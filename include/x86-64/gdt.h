#pragma once

/* Flags and bit masks to set the flags of an entry in the global descriptor
 * table.
 */
#define GDT_ACCESSED (1 << 0)
#define GDT_RW (1 << 1)
#define GDT_DIR (1 << 2)
#define GDT_EXEC (1 << 3)
#define GDT_DEFAULT (1 << 4)
#define GDT_PRIVL(x) (((x) & 0x3) << 5)
#define GDT_PRESENT (1 << 7)
#define GDT_LIMIT(x) (((x) & 0xF) << 8)
#define GDT_LONG_MODE (1 << 13)
#define GDT_SIZE (1 << 14)
#define GDT_GRANULARITY (1 << 15)

/* System descriptor types. */
#define GDT_TSS64 9

/* Bitmasks for common segments. */
#define GDT_KCODE_FLAGS \
	(GDT_RW | GDT_EXEC | GDT_DEFAULT | GDT_PRESENT)
#define GDT_KDATA_FLAGS \
	(GDT_RW | GDT_DEFAULT | GDT_PRESENT)
#define GDT_UCODE_FLAGS \
	(GDT_KCODE_FLAGS | GDT_PRIVL(3))
#define GDT_UDATA_FLAGS \
	(GDT_KDATA_FLAGS | GDT_PRIVL(3))
#define GDT_TSS_FLAGS \
	(GDT_PRESENT | GDT_TSS64)

/* Common segment descriptor numbers. */
#define GDT_KCODE 0x08
#define GDT_KDATA 0x10
#define GDT_UCODE 0x20
#define GDT_UDATA 0x18
#define GDT_TSS0 0x28

#ifndef __ASSEMBLER__
#include <x86-64/types.h>

struct gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint16_t flags;
	uint8_t base_high;
} __attribute__((packed));

struct tss_entry {
	struct gdt_entry entry;
	uint32_t base_high;
	uint32_t always0;
} __attribute__((packed));

struct gdtr {
	uint16_t limit;
	struct gdt_entry *entries;
} __attribute__((packed));

struct tss {
	uint32_t always0_0;
	uint64_t rsp[3];
	uint64_t always0_1;
	uint64_t ist[7];
	uint64_t always0_2;
	uint16_t always0_3;
	uint16_t iomap_base;
} __attribute__((packed));

static inline void set_gdt_entry(struct gdt_entry *entry, void *base,
	size_t limit, unsigned flags)
{
	entry->base_low = (uintptr_t)base & 0xFFFF;
	entry->base_middle = ((uintptr_t)base >> 16) & 0xFF;
	entry->base_high = ((uintptr_t)base >> 24) & 0xFF;
	entry->limit_low = limit & 0xFFFF;
	entry->flags = flags | GDT_LIMIT(limit >> 16);
}

static inline void set_tss_entry(struct tss_entry *entry, struct tss *tss)
{
	set_gdt_entry(&entry->entry, tss, sizeof *tss, GDT_TSS_FLAGS);
	entry->base_high = ((uintptr_t)tss >> 32) & 0xFFFFFFFF;
}

static inline void load_gdt(struct gdtr *gdtr, uint16_t code_sel,
	uint16_t data_sel)
{
	asm volatile(
		"movq %%rsp, %%rbp\n"
		"lgdt (%0)\n"
		"movw %w2, %%ds\n"
		"movw %w2, %%es\n"
		"pushq %q2\n"
		"pushq %%rbp\n"
		"pushf\n"
		"pushq %q1\n"
		"movabs $1f, %%rax\n"
		"pushq %%rax\n"
		"iretq\n"
		"1:\n" ::
		"r" (gdtr), "r" (code_sel), "r" (data_sel));
}

static inline void load_task_sel(uint16_t sel)
{
	asm volatile("ltr %0" :: "r" (sel));
}
#endif

