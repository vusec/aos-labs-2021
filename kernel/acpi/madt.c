#include <stdio.h>

#include <x86-64/asm.h>

#include <kernel/acpi.h>

extern physaddr_t lapic_addr;

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

	return 0;
}

