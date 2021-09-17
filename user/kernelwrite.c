#include <lib.h>

int main(int argc, char **argv)
{
	/* Trigger a fault by writing to the kernel. */
	*(volatile char *)0xffff800000100000 = 42;

	return 0;
}
