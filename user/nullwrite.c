#include <lib.h>

int main(int argc, char **argv)
{
	/* Trigger a fault by writing to NULL. */
	*(volatile char *)NULL = 0x42;

	return 0;
}

