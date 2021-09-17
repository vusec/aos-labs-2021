#include <lib.h>

int main(int argc, char **argv)
{
	/* Trigger a fault by reading from the kernel. */
	*(volatile char *)0xffff800000100000;

	return 0;
}
