#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0xffff800000000000ull;

	assert(mprotect(addr, 0xffffffffffffffffull - 0xffff800000000000ull,
	    PROT_NONE) != 0);

	print_vmas();

	return 0;
}
