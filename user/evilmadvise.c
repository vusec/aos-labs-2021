#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0xffff800000000000ull;

	assert(madvise(addr, 0xffffffffffffffffull - 0xffff800000000000ull,
	    MADV_DONTNEED) != 0);

	print_vmas();

	return 0;
}
