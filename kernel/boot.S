#include <x86-64/asm.h>
#include <x86-64/gdt.h>
#include <x86-64/memory.h>
#include <x86-64/paging.h>

.section .text
.code32

.global _start
_start:
	cli

	/* Set up a small boot stack to use. */
	movl $bootstack_top, %esp

	/* Push the arguments. */
	pushl $0
	pushl %ebx

	/* At this point the kernel will have been loaded at 0x100000 by the boot
	 * loader, but since we want to keep the lower part of the virtual address
	 * space available for our applications in userspace, we linked the kernel
	 * at 0xFFFF800000000000. Consequently, all addresses in kernel space will
	 * be sign-extended on x86-64.
	 *
	 * Set up paging to map both 0x0000000000000000 and 0xFFFF800000000000 to
	 * the first 8 MiB. Since huge page is guaranteed to be available on
	 * x86-64, we will be using 2 MiB pages (huge pages) to simplify the
	 * mapping.
	 */
	movl $(PAGE_PRESENT | PAGE_WRITE | PAGE_HUGE), %eax
	movl $page_dir, %edi
	movl $4, %ecx

.map_pages:
	movl %eax, (%edi)
	addl $0x200000, %eax
	addl $8, %edi
	dec %ecx
	jnz .map_pages

	movl $page_dir, %eax
	orl $(PAGE_PRESENT | PAGE_WRITE), %eax
	movl %eax, pdpt

	movl $pdpt, %eax
	orl $(PAGE_PRESENT | PAGE_WRITE), %eax
	movl %eax, pml4
	movl %eax, pml4 + 256 * 8

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

	ljmp $0x08, $_start64

.code64
_start64:
	/* At this point the code will be running in 64-bit long mode. However, the
	 * instruction pointer and the stack pointer are still pointing to the
	 * physical addresses we have been using up to this point. Let's relocate
	 * the stack pointer by adding the virtual memory address base.
	 */
	movabs $KERNEL_VMA, %rax
	addq %rax, %rsp

	/* Let's relocate the instruction pointer by jumping to the actual kernel
	 * code using absolute addressing.
	 */
	popq %rdi
	addq %rax, %rdi

	movabs $kmain, %rax
	call *%rax

	/* We are not supposed to get here, but if we do halt the system. */
	hlt

.section .data

.balign 8
gdt64:
	/* Null descriptor. */
	.word 0
	.word 0
	.byte 0
	.word 0
	.byte 0

	/* Kernel code descriptor. */
	.word 0
	.word 0
	.byte 0
	.word GDT_KCODE_FLAGS | GDT_LONG_MODE
	.byte 0

	/* Kernel data descriptor. */
	.word 0
	.word 0
	.byte 0
	.word GDT_KDATA_FLAGS
	.byte 0

.global gdtr64
gdtr64:
	.word . - gdt64 - 1
	.long gdt64

.section .bss

.balign 16
.global bootstack
bootstack:
	.skip KSTACK_SIZE
bootstack_top:

.balign PAGE_SIZE
.global pml4
pml4:
	.skip PAGE_SIZE

pdpt:
	.skip PAGE_SIZE

page_dir:
	.skip PAGE_SIZE

