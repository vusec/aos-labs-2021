#pragma once

#define BOOT_SIG 0xAA55

#define SECT_SIZE 512

#ifndef __ASSEMBLER__
struct boot_info {
	uint32_t mmap_addr;
	uint32_t mmap_len;
	void *elf_hdr;
};

enum mmap_type {
	MMAP_FREE = 1,
	MMAP_RESERVED = 2,
	MMAP_ACPI_RECLAIMABLE = 3,
	MMAP_ACPI_NVS = 4,
	MMAP_BAD = 5,
};

struct mmap_entry {
	uint64_t addr;
	uint64_t len;
	uint32_t type;
	uint32_t flags;
};
#endif /* !defined(__ASSEMBLER__) */

