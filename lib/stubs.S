#include <x86-64/idt.h>

.section .text

.global do_syscall
do_syscall:
	pushq %rbp
	movq 0x10(%rsp), %rbp

#ifdef LAB3_SYSCALL
	movq %rcx, %r10
	syscall
#else
	int $INT_SYSCALL
#endif

	popq %rbp

	retq
