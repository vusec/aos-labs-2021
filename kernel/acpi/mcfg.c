#include <pci.h>
#include <stdio.h>

#include <kernel/acpi.h>
#include <kernel/dev/pci.h>
#include <kernel/mem.h>

int mcfg_init(struct rsdp *rsdp)
{
	struct mcfg *mcfg;
	struct acpi_hdr *hdr;
	struct mcfg_entry *entry;
	size_t i, len, count;

	mcfg = acpi_find_table(rsdp, "MCFG");

	if (!mcfg) {
		return -1;
	}

	hdr = &mcfg->hdr;
	len = hdr->len - offsetof(struct mcfg, entries);
	count = len / sizeof *entry;

	for (i = 0; i < count; ++i) {
		entry = mcfg->entries + i;
		pci_enum_devices(entry->base_addr, entry->bus_start, entry->bus_end);
	}

	return 0;
}

