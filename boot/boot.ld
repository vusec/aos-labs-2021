OUTPUT_FORMAT(elf32-i386)
ENTRY(_start);

SECTIONS {
	. = 0x7C00;

	.boot : AT(ADDR(.boot)) {
		*/boot/boot1.o (.text)
		*/boot/boot1.o (.rodata)
		*/boot/boot1.o (.data)
		*/boot/boot1.o (.bss)
	}

	. = 0x7DFE;

	.bootsig : AT(ADDR(.bootsig)) {
		SHORT(0xAA55);
	}

	stage2 = .;

	.text : AT(ADDR(.text)) {
		*(.text)
	}

	.rodata : AT(ADDR(.rodata)) {
		*(.rodata)
	}

	.data : AT(ADDR(.data)) {
		*(.data)
	}

	.bss : AT(ADDR(.bss)) {
		*(.bss)
	}

	. = ALIGN(512);
	stage2_end = .;

	/DISCARD/ : {
		*(COMMON)
		*(.eh_frame)
	}
}

