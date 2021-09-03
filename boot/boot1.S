#include <boot.h>

.code16
.section .text

.global _start
_start:
	cli

	/* The BIOS may have set up the segment registers and the stack in any kind
	 * of state. We will use the memory area below 0x0000:0x7C00 as stack
	 * space and have all the segment registers set to zero.
	 */
	movw $0x7c00, %sp

	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss

	ljmp $0x0, $1f

1:
	/* The BIOS stores the number of the drive we have booted from in dl. Let's
	 * preserve it.
	 */
	movb %dl, boot_drive

	/* Reset the disk. */
	xorb %ah, %ah
	int $0x13

	/* Determine the size of the second stage in sectors to load from LBA 1 to
	 * 0x0000:0x7E00 and load them.
	 */
	movl $0x1, %esi
	movw $0x7e00, %di
	movw $stage2_end, %cx
	subw $stage2, %cx
	shrw $9, %cx
	movw $32, %cx
	call read_sectors
	jc 2f

	/* The second stage has been loaded. Jump to its entry point. */
	jmp boot2

2:
	/* The BIOS failed to read the sectors from the disk. Display an error
	 * message and bail out.
	 */
	movw $error, %si
	call puts
	hlt

.global puts
puts:
	/* Grab the byte from SI and print if it is not zero. Otherwise simply
	 * return.
	 */
	lodsb
	movb $0xe, %ah
	orb %al, %al
	jz .done
	int $0x10
	jmp puts

.done:
	movb $'\r', %al
	int $0x10
	movb $'\n', %al
	int $0x10
	ret

.global read_sector
read_sector:
	/* Set up the disk packet. */
	movw $disk_packet, %bp
	movl %edi, 4(%bp)
	movl %esi, 8(%bp)
	movw $disk_packet, %si
	movb $0x42, %ah
	movw $6, %cx
	movb boot_drive, %dl

1:
	/* Check if another attempt should be tried. */
	decw %cx
	jnz 2f
	stc
	ret

2:
	/* Issue the read. */
	int $0x13
	jc 1b
	test %ah, %ah
	jnz 1b
	ret

read_sectors:
	/* Read the amount of sectors specified by cx from the disk beginning at
	 * LBA esi to 0x0000:di.
	 */
	pushal
	call read_sector
	popal
	jc 1f
	addl $512, %edi
	incl %esi
	loop read_sectors
	ret

1:
	stc
	ret

.section .data

boot_drive:
	.byte 0

.balign 16
disk_packet:
	.word 16
	.word 1
	.word 0
	.word 0
	.quad 0

error: .ascii "Disk error!\0"

