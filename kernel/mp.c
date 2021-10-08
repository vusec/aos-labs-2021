#include <x86-64/asm.h>

#include <cpu.h>

#include <kernel/acpi.h>
#include <kernel/mem.h>
#include <kernel/sched.h>

/* While boot_cpus() is booting a given CPU, it communicates the per-core stack
 * pointer that should be loaded by boot_ap().
 */
void *mpentry_kstack;

void boot_cpus(void)
{
	extern unsigned char boot_ap16[], boot_ap_end[];
	void *code;
	struct cpuinfo *cpu;

	/* Write entry code at the reserved page at MPENTRY_PADDR. */
	code = KADDR(MPENTRY_PADDR);
	memmove(code, KADDR((physaddr_t)boot_ap16), boot_ap_end - boot_ap16);

	/* Boot each CPU one at a time. */
	for (cpu = cpus; cpu < cpus + ncpus; ++cpu) {
		/* Skip the boot CPU */
		if (cpu == boot_cpu) {
			continue;
		}

		/* Set up the kernel stack. */
		mpentry_kstack = (void *)cpu->cpu_tss.rsp[0];

		/* Start the CPU at boot_ap16(). */
		lapic_startup(cpu->cpu_id, PADDR(code));

		/* Wait until the CPU becomes ready. */
		while (cpu->cpu_status != CPU_STARTED);
	}
}

void mp_main(void)
{
	/* Eable the NX-bit. */
	write_msr(MSR_EFER, read_msr(MSR_EFER) | MSR_EFER_NXE);

	/* Load the kernel PML4. */
	asm volatile("movq %0, %%cr3\n" :: "r" (PADDR(kernel_pml4)));

	/* Load the per-CPU kernel stack. */
	asm volatile("movq %0, %%rsp\n" :: "r" (mpentry_kstack));

	cprintf("SMP: CPU %d starting\n", lapic_cpunum());

	/* LAB 6: your code here. */
	/* Initialize the local APIC. */
	/* Set up segmentation, interrupts, system call support. */
	/* Set up the per-CPU slab allocator. */
	/* Set up the per-CPU scheduler. */

	/* Notify the main CPU that we started up. */
	xchg(&this_cpu->cpu_status, CPU_STARTED);

	/* Schedule tasks. */
	/* LAB 6: remove this code when you are ready */
	asm volatile(
		"cli\n"
		"hlt\n");
	sched_yield();
}

