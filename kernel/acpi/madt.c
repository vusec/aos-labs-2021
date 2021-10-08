#include <stdio.h>

#include <cpu.h>

#include <x86-64/asm.h>

#include <kernel/acpi.h>

extern physaddr_t lapic_addr;

void madt_parse_lapic(struct madt_lapic *lapic)
{
	if (lapic->apic_id < NCPUS) {
		cpus[lapic->apic_id].cpu_id = lapic->cpu_id;
		++ncpus;
		return;
	}

	cprintf("SMP: too many CPUs, ignoring CPU %d\n", lapic->apic_id);
}

int madt_init(struct rsdp *rsdp)
{
	struct acpi_hdr *hdr;
	struct madt *madt;
	struct madt_entry *entry;
	struct madt_lapic *lapic;
	struct madt_ioapic *ioapic;
	size_t i, len;
	char *p;

	madt = acpi_find_table(rsdp, "APIC");

	if (!madt) {
		return -1;
	}

	lapic_addr = madt->lapic_base;

	hdr = &madt->hdr;
	len = hdr->len - sizeof *madt;
	p = (char *)(madt + 1);
	ncpus = 0;

	for (i = 0; i < len; i += entry->len, p += entry->len) {
		entry = (struct madt_entry *)p;

		switch (entry->type) {
		case MADT_LAPIC:
			madt_parse_lapic(&entry->lapic);
			break;
		case MADT_IOAPIC:
			ioapic = &entry->ioapic;
			cprintf("I/O APIC with base=%p\n", ioapic->ioapic_base);
			break;
		case MADT_ISO:
		case MADT_NMI:
		case MADT_LAPIC64:
			break;
		default:
			cprintf("smp: unknown entry type in MADT\n");
			break;
		}
	}

	boot_cpu = cpus + lapic_cpunum();
	boot_cpu->cpu_status = CPU_STARTED;

	if (madt->flags) {
		/* If the hardware implements PIC mode, switch to getting
		 * interrupts from the local APIC.
		 */
		cprintf("SMP: setting IMCR to switch from PIC mode to "
		        "symmetric I/O mode\n");

		/* Select IMCR. */
		outb(0x22, 0x70);

		/* Mask external interrupts. */
		outb(0x23, inb(0x23) | 1);
	}

	return 0;
}

