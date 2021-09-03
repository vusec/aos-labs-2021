OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

PHDRS {
	.boot PT_LOAD;
	.text PT_LOAD;
	.rodata PT_LOAD;
	.data PT_LOAD;
}

SECTIONS {
	. = KERNEL_LMA;
	entry = KERNEL_VMA + .;

	.boot ALIGN(4K) : {
		*/kernel/boot.o (.text)
		*/kernel/boot.o (.data)
		*/kernel/boot.o (.bss)
		boot_end = .;
	} :.boot

	. += KERNEL_VMA;

	.text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA) ALIGN(4K) {
		*(.text)
	} :.text

	etext = .;

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA) ALIGN(4K) {
		*(.rodata)
	} :.rodata

	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA) ALIGN(4K) {
		*(.data)
	} :.data

	edata = .;

	.bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA) ALIGN(4K) {
		*(.bss)
		*(COMMON)
	} :.data

	end = .;
}

