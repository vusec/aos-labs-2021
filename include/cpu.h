#pragma once

#define CPU_ID       0
#define CPU_STATUS   4
#define CPU_TASK     8
#define CPU_TSS_RSP0 20
#define CPU_TSS_RSP1 28
#define CPU_TSS_RSP2 36
#define CPU_TSS_RSP3 44

#ifndef __ASSEMBLER__
#include <types.h>
#include <task.h>

#include <x86-64/gdt.h>
#include <x86-64/memory.h>

#include <kernel/mem/slab.h>
#include <kernel/acpi.h>

/* Values of status in struct cpuinfo */
enum {
	CPU_UNUSED = 0,
	CPU_STARTED,
	CPU_HALTED,
};

struct kmem_cache {
	struct slab _slabs[32];
	size_t _nslabs;
};

/* Per-CPU state */
struct cpuinfo {
	/* The local APIC ID. */
	uint8_t cpu_id;

	/* The status of the CPU. */
	volatile unsigned cpu_status;

	/* Task currently running on the CPU. */
	struct task *cpu_task;

	/* Used by x86 to find the stack for the interrupt. */
	struct tss cpu_tss;

	/* Per-CPU slab allocator */
	struct kmem_cache kmem;

	/* Per-CPU run queue */
	struct list runq, nextq;
	size_t runq_len;
};

#define NCPUS 64

extern struct cpuinfo cpus[NCPUS];
extern struct cpuinfo *boot_cpu;
#define this_cpu (cpus + lapic_cpunum())
extern size_t ncpus;

#endif /* !defined(__ASSEMBLER__) */
