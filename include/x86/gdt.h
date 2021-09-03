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
#define GDT_SIZE (1 << 14)
#define GDT_GRANULARITY (1 << 15)

/* Bitmasks for common segments. */
#define GDT_KCODE_FLAGS \
	(GDT_RW | GDT_EXEC | GDT_DEFAULT | GDT_PRESENT)
#define GDT_KDATA_FLAGS \
	(GDT_RW | GDT_DEFAULT | GDT_PRESENT)
#define GDT_UCODE_FLAGS \
	(GDT_KCODE_FLAGS | GDT_PRIVL(3))
#define GDT_UDATA_FLAGS \
	(GDT_KDATA_FLAGS | GDT_PRIVL(3))

/* Common segment descriptor numbers. */
#define GDT_KCODE 0x08
#define GDT_KDATA 0x10
#define GDT_UCODE 0x18
#define GDT_UDATA 0x20
#define GDT_TSS0 0x28

#ifndef __ASSEMBLER__
#include <inc/x86/types.h>

struct gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint16_t flags;
	uint8_t base_high;
} __attribute__((packed));

struct gdtr {
	uint16_t limit;
	struct gdt_entry *entries;
} __attribute__((packed));

struct tss_esp {
	uint32_t esp;
	uint32_t ss;
} __attribute__((packed));

struct tss {
	uint32_t link;
	struct tss_esp esp[3];
	uint32_t cr3;
	uint32_t eip, eflags;
	uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint32_t es, cs, ss, ds, fs, gs;
	uint32_t ldt_sel;
	uint16_t always0;
	uint16_t iomap_base;
} __attribute__((packed));

static inline void load_gdt(struct gdtr *gdtr)
{
	asm volatile("lgdt (%0)" :: "r" (gdtr));
}

static inline void load_task_reg(uint16_t sel)
{
	asm volatile("ltr %0" :: "r" (sel));
}
#endif

