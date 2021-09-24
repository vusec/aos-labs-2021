#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0xffff800000000000ull;

	munmap(addr, 0xffffffffffffffffull - 0xffff800000000000ull);

	print_vmas();

	return 0;
}

