#include <lib.h>

int main(int argc, char **argv)
{
	/* Trigger a fault by executing data. */
	const char *s = "Hello, world!\n";
	void (* func)(void) = (void (*)(void))s;

	func();

	return 0;
}

