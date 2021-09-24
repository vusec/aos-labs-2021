#include <x86-64/memory.h>

/* Entry point - this is where the kernel (or our parent task) starts the
 * new task.
 */
.text
.global _start
_start:
	/* Check if we have any arguments on the stack. */
	movabsq $USTACK_TOP, %rax
	cmpq %rax, %rsp
	jne args_exist

	/* If there are no arguments on the stack, we have to push dummy
	 * argc/argv arguments. This happens when the program gets loaded
	 * by the kernel, because the kernel does not care about passing
	 * initial arguments. */
	xorl %edi, %edi
	xorl %esi, %esi

args_exist:
	call libmain
1:
	jmp 1b

