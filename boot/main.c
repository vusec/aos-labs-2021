#include <x86/asm.h>

#include <boot.h>
#include <elf.h>

/**********************************************************************
 * This a dirt simple boot loader, whose sole job is to boot
 * an ELF kernel image from the first IDE hard disk.
 *
 * DISK LAYOUT
 *  * This program(boot.S and main.c) is the bootloader.  It should
 *    be stored in the first sector of the disk.
 *
 *  * The 2nd sector onward holds the kernel image.
 *
 *  * The kernel image must be in ELF format.
 *
 * BOOT UP STEPS
 *  * when the CPU boots it loads the BIOS into memory and executes it
 *
 *  * the BIOS intializes devices, sets of the interrupt routines, and
 *    reads the first sector of the boot device(e.g., hard-drive)
 *    into memory and jumps to it.
 *
 *  * Assuming this boot loader is stored in the first sector of the
 *    hard-drive, this code takes over...
 *
 *  * control starts in boot.S -- which sets up protected mode,
 *    and a stack so C code then run, then calls bootmain()
 *
 *  * bootmain() in this file takes over, reads in the kernel and jumps to it.
 **********************************************************************/

#define SECTSIZE	512
#define ELFHDR	  ((struct elf *) 0x10000) /* scratch space */

void readsect(void*, uint32_t);
void readseg(uint32_t, uint32_t, uint32_t);

extern void puts32(const char *s);
extern void read_sector32(void *, uint32_t);

void bootmain(struct boot_info *boot_info)
{
	struct elf_proghdr *ph, *eph;

	boot_info->elf_hdr = ELFHDR;

	/* read 1st page off disk */
	readseg((uint32_t) ELFHDR, SECTSIZE * 8, 0);

	/* is this a valid ELF? */
	if (ELFHDR->e_magic != ELF_MAGIC) {
		puts32("bad ELF");
		goto bad;
	}

	/* load each program segment (ignores ph flags) */
	ph = (struct elf_proghdr *) ((uint8_t *) ELFHDR + ELFHDR->e_phoff);
	eph = ph + ELFHDR->e_phnum;
	for (; ph < eph; ph++)
		/* p_pa is the load address of this segment (as well as the physical
		 * address) */
		readseg(ph->p_pa, ph->p_memsz, ph->p_offset);

	/* call the entry point from the ELF header
	 * note: does not return! */
	asm volatile("jmp *%%edi" ::
		"b" (boot_info), "D" ((uint32_t)ELFHDR->e_entry));

bad:
	outw(0x8A00, 0x8A00);
	outw(0x8A00, 0x8E00);
	while (1)
		/* do nothing */;
}

void *memcpy(void *dst, const void *src, size_t n)
{
	const char *s = src;
	char *d = dst;

	while (n--) {
		*d++ = *s++;
	}

	return dst;
}

/*
 * Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
 * Might copy more than asked.
 */
void readseg(uint32_t pa, uint32_t count, uint32_t offset)
{
	extern char stage2[], stage2_end[];
	uint32_t end_pa;

	end_pa = pa + count;

	/* round down to sector boundary */
	pa &= ~(SECTSIZE - 1);

	/* translate from bytes to sectors, and kernel starts at sector 1 */
	offset = (offset / SECTSIZE);
	offset += 1;
	offset += (stage2_end - stage2 + SECTSIZE - 1) / SECTSIZE;

	/* If this is too slow, we could read lots of sectors at a time. We'd write
	 * more to memory than asked, but it doesn't matter -- we load in increasing
	 * order. */
	while (pa < end_pa) {
		/* Since we haven't enabled paging yet and we're using an identity
		 * segment mapping (see boot.S), we can use physical addresses directly.
		 * This won't be the case once OpenLSD enables the MMU. */
		readsect((uint8_t *) pa, offset);
		pa += SECTSIZE;
		offset++;
	}
}

void readsect(void *dst, uint32_t offset)
{
	read_sector32((void *)0x1000, offset);
	memcpy(dst, (void *)0x1000, SECTSIZE);
}

