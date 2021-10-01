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

/* Values of status in struct cpuinfo */
enum {
	CPU_UNUSED = 0,
	CPU_STARTED,
	CPU_HALTED,
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
};

extern struct cpuinfo *this_cpu;
extern struct cpuinfo *boot_cpu;

#endif /* !defined(__ASSEMBLER__) */
