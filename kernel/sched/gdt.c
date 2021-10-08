#include <x86-64/gdt.h>
#include <x86-64/memory.h>

#include <cpu.h>
#include <kernel/acpi.h>
#include <kernel/sched/gdt.h>

struct gdt_entry gdt_entries[5 + 2] = {
	[GDT_KCODE >> 3] = { .flags = GDT_KCODE_FLAGS | GDT_LONG_MODE },
	[GDT_KDATA >> 3] = { .flags = GDT_KDATA_FLAGS },
	[GDT_UCODE >> 3] = { .flags = GDT_UCODE_FLAGS | GDT_LONG_MODE },
	[GDT_UDATA >> 3] = { .flags = GDT_UDATA_FLAGS },
};

struct gdtr gdtr = {
	.limit = sizeof(gdt_entries) - 1,
	.entries = gdt_entries,
};

void gdt_init(void)
{
	/* Set up the kernel stack pointer in the TSS. Add the TSS to the GDT.
	 * Load the GDT and the task selector.
	 */
	/* LAB 6: your code here. */
	this_cpu->cpu_tss.rsp[0] = KSTACK_TOP;

	set_tss_entry((struct tss_entry *)(gdt_entries + (GDT_TSS0 >> 3)),
	    &this_cpu->cpu_tss);
	load_gdt(&gdtr, GDT_KCODE, GDT_KDATA);
	load_task_sel(GDT_TSS0);
}

void gdt_init_mp(void)
{
	/* LAB 6: your code here. */
}

