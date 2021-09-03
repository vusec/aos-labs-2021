#include <x86/asm.h>
#include <x86/gdt.h>

.macro GOTO_RMODE
	/* Clear interrupts. */
	cli

	/* Load 16-bit protected mode segments. */
	lgdt gdtr
	movw $0x20, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	ljmp $0x18, $1f

.code16
1:
	/* Disable protected mode. */
	movl %cr0, %eax
	andl $(~CR0_PM), %eax
	movl %eax, %cr0

	/* Zero out the segment registers. */
	ljmp $0, $2f

2:
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

	/* Load the BIOS IVT. */
	lidt idtr
.endm

.macro GOTO_PMODE
	/* Enable protected mode. */
	movl %cr0, %eax
	orl $CR0_PM, %eax
	movl %eax, %cr0

	/* Load 32-bit protected mode segments. */
	lgdt gdtr
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	ljmp $0x08, $3f

.code32
3:
.endm

.code32
.global puts32
puts32:
	push %ebp
	movl %esp, %ebp
	pushal

	GOTO_RMODE

	movl 8(%ebp), %esi
	call puts

	GOTO_PMODE

	popal
	movl %ebp, %esp
	pop %ebp
	ret

.global read_sector32
read_sector32:
	push %ebp
	movl %esp, %ebp
	pushal

	GOTO_RMODE

	movl 8(%ebp), %edi
	movl 12(%ebp), %esi
	call read_sector

	GOTO_PMODE

	popal
	movl %ebp, %esp
	pop %ebp
	ret

.section .data

idtr:
	.word 0x3ff
	.quad 0

.section .rodata

