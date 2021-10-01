#include <acpi.h>
#include <stdio.h>
#include <string.h>

uint8_t memsum(void *s, size_t len)
{
	char *p = s;
	uint8_t sum = 0;

	while (len) {
		sum += *p;
		++p;
		--len;
	}

	return sum;
}

int acpi_check(struct acpi_hdr *hdr, const char *signature)
{
	if (memcmp(hdr->signature, signature, 4) != 0) {
		return 0;
	}

	return memsum(hdr, hdr->len) == 0;
}

static void *rsdp_find_range(void *start, void *end)
{
	void *p;

	for (p = start; p < end; p += 16) {
		if (memcmp(p, "RSD PTR ", 8) == 0) {
			return p;
		}
	}

	return NULL;
}

struct rsdp *rsdp_find(void)
{
	struct rsdp *rsdp;

	return rsdp_find_range((void *)(KERNEL_VMA + 0xe0000),
		(void *)(KERNEL_VMA + 0xfffff));
}

static void *xsdt_find_table(struct acpi_hdr *hdr, const char *signature)
{
	uint64_t *sdtp = (uint64_t *)(hdr + 1);
	size_t i, count;

	count = hdr->len / sizeof *sdtp;

	for (i = 0; i < count; ++i, ++sdtp) {
		hdr = (struct acpi_hdr *)(KERNEL_VMA + *sdtp);

		if (!acpi_check(hdr, signature)) {
			continue;
		}

		return hdr;
	}

	return NULL;
}

static void *rsdt_find_table(struct acpi_hdr *hdr, const char *signature)
{
	uint32_t *sdtp = (uint32_t *)(hdr + 1);
	size_t i, count;

	count = hdr->len / sizeof *sdtp;

	for (i = 0; i < count; ++i, ++sdtp) {
		hdr = (struct acpi_hdr *)(KERNEL_VMA + *sdtp);

		if (!acpi_check(hdr, signature)) {
			continue;
		}

		return hdr;
	}

	return NULL;
}

void *acpi_find_table(struct rsdp *rsdp, const char *signature)
{
	struct rsdp20 *rsdp20;
	struct acpi_hdr *hdr;

	if (rsdp->revision > 0) {
		rsdp20 = (struct rsdp20 *)rsdp;
		hdr = (struct acpi_hdr *)(KERNEL_VMA + rsdp20->xsdt_base);

		if (acpi_check(hdr, "XSDT")) {
			return xsdt_find_table(hdr, signature);
		}
	}

	hdr = (struct acpi_hdr *)(KERNEL_VMA + rsdp->rsdt_base);

	if (acpi_check(hdr, "RSDT")) {
		return rsdt_find_table(hdr, signature);
	}

	return NULL;
}

static void xsdt_dump_tables(struct acpi_hdr *hdr)
{
	uint64_t *sdtp = (uint64_t *)(hdr + 1);
	size_t i, count;

	count = hdr->len / sizeof *sdtp;

	for (i = 0; i < count; ++i, ++sdtp) {
		if (!*sdtp) {
			continue;
		}

		hdr = (struct acpi_hdr *)(KERNEL_VMA + *sdtp);

		cprintf("  ACPI table with signature %.4s\n",
			hdr->signature);
	}
}

static void rsdt_dump_tables(struct acpi_hdr *hdr)
{
	uint32_t *sdtp = (uint32_t *)(hdr + 1);
	size_t i, count;

	count = hdr->len / sizeof *sdtp;

	for (i = 0; i < count; ++i, ++sdtp) {
		if (!*sdtp) {
			continue;
		}

		hdr = (struct acpi_hdr *)(KERNEL_VMA + *sdtp);

		cprintf("  ACPI table with signature %.4s\n",
			hdr->signature);
	}
}

void acpi_dump_tables(struct rsdp *rsdp)
{
	struct rsdp20 *rsdp20;
	struct acpi_hdr *hdr;

	if (rsdp->revision > 0) {
		rsdp20 = (struct rsdp20 *)rsdp;
		hdr = (struct acpi_hdr *)(KERNEL_VMA + rsdp20->xsdt_base);

		if (acpi_check(hdr, "XSDT")) {
			xsdt_dump_tables(hdr);
		}
	}

	hdr = (struct acpi_hdr *)(KERNEL_VMA + rsdp->rsdt_base);

	if (acpi_check(hdr, "RSDT")) {
		rsdt_dump_tables(hdr);
	}
}

