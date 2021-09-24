#include <lib.h>

int main(int argc, char **argv)
{
	/* Trigger a fault by executing NULL. */
	void (* func)(void) = NULL;
	asm("call *%0" : : "r"(func));

	return 0;
}

