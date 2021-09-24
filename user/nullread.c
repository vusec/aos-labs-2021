#include <lib.h>

int main(int argc, char **argv)
{
	/* Trigger a fault by reading from NULL. */
	printf("I read %02x from location 0!\n", *(volatile char *)NULL);

	return 0;
}

