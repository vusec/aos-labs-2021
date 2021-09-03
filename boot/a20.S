.code16
.section .text

/* Tests whether the A20 line has been enabled by writing two different values
 * to 0000:0500 and FFFF:0510 and by comparing them. Since FFFF:0510 is the
 * same as 0x100500, having the A20 line disabled caused the address to wrap
 * around to 0x500. Thus, if the two values are the same, the A20 line has been
 * disabled. Otherwise, if the values are different, the A20 line has been
 * enabled.
 */
test_a20:
	pushw %ds
	pushw %es
	pushw %di
	pushw %si

	/* Set es:di to 0x0000:0x0500 and ds:si to 0xFFFF:0x0510. */
	xorw %ax, %ax
	movw %ax, %es
	notw %ax
	movw %ax, %es
	movw $0x500, %di
	movw $0x510, %si

	/* Set es:di to 0x00. */
	xorw %ax, %ax
	movb %al, %es:(%di)

	/* Set ds:si to 0xFF. */
	notw %ax
	movb %al, %ds:(%si)

	/* Check if es:si is equal to 0xFF. */
	cmpb %al, %es:(%di)
	
	popw %si
	popw %di
	popw %es
	popw %ds
	
	je 1f
	
	/* A20 is enabled. */
	clc
	ret

1:
	/* A20 is disabled. */
	stc
	ret

/* Attempts to enable the A20 line using INT 0x15, AX=0x2401. */
set_a20_bios:
	/* Check if the PC BIOS supports enabling the A20. */
	movw $0x2403, %ax
	int $0x15
	jc 1f
	test %ah, %ah
	jnz 1f

	/* Enable the A20 gate using the PC BIOS. */
	movw $0x2401, %ax
	int $0x15
	jc 1f
	test %ah, %ah
	jnz 1f
	ret

1:
	stc
	ret

/* Attempts to enable the A20 line using the keyboard controller. */
set_a20_kbd:
	/* Disable the keyboard. */
	call wait_kbd_cmd
	movb $0xAD, %al
	outb %al, $0x64

	/* Read from input. */
	call wait_kbd_cmd
	movb $0xD0, %al
	outb %al, $0x64

	call wait_kbd_data
	inb $0x60, %al
	pushw %ax

	/* Write to output. */
	call wait_kbd_cmd
	movb $0xD1, %al
	outb %al, $0x64

	call wait_kbd_data
	popw %ax
	orb $2, %al
	outb %al, $0x60

	/* Enable the keyboard. */
	call wait_kbd_cmd
	movb $0xAE, %al
	outb %al, $0x64

	/* Wait for completion. */
	call wait_kbd_cmd
	ret

/* Waits for the write to the keyboard command register to complete. */
wait_kbd_cmd:
	inb $0x64, %al
	testb $2, %al
	jnz wait_kbd_cmd
	ret

/* Waits for the write to the keyboard data register to complete. */
wait_kbd_data:
	inb $0x64, %al
	testb $1, %al
	jnz wait_kbd_data
	ret

/* Attempts to enable the A20 line using the A20 fast gate. This method may
 * cause weird behaviour on systems where the A20 fast gate is not supported.
 * Furthermore, we already test if the bit has been set before setting it and
 * writing the value to avoid unnecessary writes.
 */
set_a20_fast:
	inb $0x92, %al
	testb $2, %al
	jnz 1f
	orb $2, %al
	and $~2, %al
	outb %al, $0x92

1:
	ret

/* Attempt to enable the A20 line using various approaches. */
.global set_a20
set_a20:
	/* Check if the A20 line has already been enabled. */
	call test_a20
	jnc 2f

	/* Attempt to enable the A20 line using the PC BIOS. */
	call set_a20_bios
	call test_a20
	jnc 2f

	/* Attempt to enable the A20 line using the keyboard controller. */
	call set_a20_kbd
	call test_a20
	jnc 2f

	/* Attempt to enable the A20 line using the A20 fast gate. */
	call set_a20_fast
	call test_a20
	jnc 2f

1:
	stc
	ret

2:
	ret

