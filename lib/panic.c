#include <lib.h>

/* Panic is called on unresolvable fatal errors. It prints "panic: <message>"
 * then causes a breakpoint exception, which triggers the kernel monitor.
 */
void _panic(const char *file, int line, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	/* Print the panic message. */
	printf("[PID %5u] user panic in %s at %s:%d: ",
		getpid(), binary_name, file, line);
	vprintf(fmt, ap);
	printf("\n");

	/* Cause a breakpoint exception. */
	while (1) {
		asm volatile("int3");
	}
}

