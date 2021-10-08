#include <x86-64/asm.h>
#include <x86-64/gdt.h>
#include <x86-64/memory.h>
#include <x86-64/paging.h>

#define BOOT_AP_PHYS(x) ((x) - boot_ap16 + MPENTRY_PADDR)

.code16
.global boot_ap16
boot_ap16:
	cli

	/* Enable protected mode. */
	movl %cr0, %eax
	orl $CR0_PM, %eax
	movl %eax, %cr0

	/* Load the global descriptor table and set up the segment registers. */
	lgdt BOOT_AP_PHYS(gdtr32)

	movw $GDT_KDATA, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	ljmp $GDT_KCODE, $(BOOT_AP_PHYS(boot_ap32))

.code32
boot_ap32:
	/* Load the root of the page table hierarchy. */
	movl $pml4, %eax
	movl %eax, %cr3

	/* Enable physical address extensions to be able to access all physical
	 * memory. This is mandatory to set up x86-64.
	 */
	movl %cr4, %eax
	orl $CR4_PAE, %eax
	movl %eax, %cr4

	/* Enter compatibility mode (i.e. 32-bit long mode). */
	movl $MSR_EFER, %ecx
	rdmsr
	orl $MSR_EFER_LME, %eax
	wrmsr

	/* Enable paging to actually use the mapping we have set up. This mapping
	 * is temporary as we will set up full paging in lab 2.
	 */
	movl %cr0, %eax
	orl $CR0_PAGING, %eax
	movl %eax, %cr0

	/* On x86 both segmentation and paging are supported as models of memory
	 * protection. For compatibility reasons we still have to set up a global
	 * descriptor table with segment descriptors for both our code and data.
	 * With the introduction of x86-64, a long mode flag has been added to the
	 * segment descriptors to support both 32-bit (compatibility mode) and
	 * 64-bit applications (long mode). Load a basic global descriptor table
	 * and update the segment registers accordingly.
	 */
	lgdt gdtr64

	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss

	ljmp $0x08, $boot_ap64

.code64
boot_ap64:
	/* Temporarily use the boot stack. */
	movabsq $bootstack, %rax
	addq $4096, %rax
	movq %rax, %rsp
	xorl %ebp, %ebp

	movabsq $mp_main, %rax
	callq *%rax

	/* We are not supposed to get here, but if we do halt the system. */
	hlt

.section .data

.balign 8
gdt32:
	/* Null descriptor. */
	.word 0
	.word 0
	.byte 0
	.word 0
	.byte 0

	/* Kernel code descriptor. */
	.word 0xFFFF
	.word 0
	.byte 0
	.word GDT_KCODE_FLAGS | GDT_SIZE | GDT_GRANULARITY | GDT_LIMIT(0xF)
	.byte 0

	/* Kernel data descriptor. */
	.word 0xFFFF
	.word 0
	.byte 0
	.word GDT_KDATA_FLAGS | GDT_SIZE | GDT_GRANULARITY | GDT_LIMIT(0xF)
	.byte 0

gdtr32:
	.word . - gdt32 - 1
	.long gdt32

.global boot_ap_end
boot_ap_end:

