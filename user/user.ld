/* Simple linker script for OpenLSD user-level programs.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

PHDRS {
	.text PT_LOAD;
	.rodata PT_LOAD;
	.data PT_LOAD;
	.bss PT_LOAD;
	.stab PT_LOAD;
}

SECTIONS
{
	/* Load programs at this address: "." means the current address */
	. = 0x800020;

	.text : {
		*(.text .stub .text.* .gnu.linkonce.t.*)
	} :.text

	PROVIDE(etext = .);	/* Define the 'etext' symbol to this value */

	.rodata ALIGN(4K) : ALIGN(4K) {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	} :.rodata

	/* Adjust the address for the data segment to the next page */
	. = ALIGN(0x1000);

	.data ALIGN(4K) : ALIGN(4K) {
		*(.data)
	} :.data

	PROVIDE(edata = .);

	.bss ALIGN(4K) : ALIGN(4K) {
		*(.bss)
	} :.bss

	PROVIDE(end = .);


	/* Place debugging symbols so that they can be found by
	 * the kernel debugger.
	 * Specifically, the four words at 0x200000 mark the beginning of
	 * the stabs, the end of the stabs, the beginning of the stabs
	 * string table, and the end of the stabs string table, respectively.
	 */

	.stab_info 0x200000 : {
		LONG(__STAB_BEGIN__);
		LONG(__STAB_END__);
		LONG(__STABSTR_BEGIN__);
		LONG(__STABSTR_END__);
	} :.stab

	.stab : {
		__STAB_BEGIN__ = DEFINED(__STAB_BEGIN__) ? __STAB_BEGIN__ : .;
		*(.stab);
		__STAB_END__ = DEFINED(__STAB_END__) ? __STAB_END__ : .;
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	} :.stab

	.stabstr : {
		__STABSTR_BEGIN__ = DEFINED(__STABSTR_BEGIN__) ? __STABSTR_BEGIN__ : .;
		*(.stabstr);
		__STABSTR_END__ = DEFINED(__STABSTR_END__) ? __STABSTR_END__ : .;
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	} :.stab

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack .comment)
	}
}
