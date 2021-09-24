#include <string.h>
#include <lib.h>

void putchar(int ch)
{
	char c = ch;

	/* Unlike standard Unix's putchar, the cputchar function _always_
	 * outputs to the system console.
	 */
	puts(&c, 1);
}

