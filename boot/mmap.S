.code16
.section .text

/* Attempts to get a memory map using INT 0x15, EAX=0xE820. This should be
 * supported by every PC BIOS since 2002. This is the most reliable and
 * accurate method of getting a memory map on x86(-64). Furthermore, this is
 * the only method that supports more than 4 GiB of memory.
 */
.global get_mmap_e820
get_mmap_e820:
	xorl %ebx, %ebx
	xorw %bp, %bp

	movl $1, %es:20(%di)
	movl $0xE820, %eax
	movl $24, %ecx
	movl $0x534D4150, %edx
	int $0x15
	
	/* Not supported. */
	jc 1f

	/* Restore %edx. */
	movl $0x0534D4150, %edx

	/* Check if %eax is valid. */
	cmpl %eax, %edx
	jne 1f

	/* Memory map contains only one entry. */
	testl %ebx, %ebx
	jnz 3f

1:
	/* Failure */
	stc
	ret

2:
	movl $1, %es:20(%di)
	movl $0xE820, %eax
	movl $24, %ecx
	int $0x15

	/* Check for final entry. */
	jc 3f

	/* Restore %edx. */
	movl $0x0534D4150, %edx

3:
	/* Check if entry size is not zero. */
	jcxz 5f

	/* Check if the entry is an ACPI 3.x entry. */
	cmpb $20, %cl
	jbe 4f

	/* Should the entry be ignored. */
	testb $1, %es:20(%di)
	je 5f

4:
	/* Check if length is not zero. */
	movl %es:8(%di), %ecx
	orl %es:12(%di), %ecx
	jz 5f

	/* Increment the counter and the destination register */
	incw %bp
	addw $24, %di

5:
	/* Check for final entry. */
	testl %ebx, %ebx
	jnz 2b

6:
	/* Success. */
	clc
	ret

