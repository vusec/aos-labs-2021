#include <lib.h>

int main(int argc, char **argv)
{
	/* Trigger a fault by writing to executable code. */
	*(char *)main = 0x42;

	return 0;
}

