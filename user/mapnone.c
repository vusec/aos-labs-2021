/* Attempt to merge from the middle. */
#include <lib.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0x1000000;
	char *ret;

	ret = mmap(addr, PAGE_SIZE, PROT_NONE,
	     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(ret == addr);

	print_vmas();

	*(volatile char *)addr;

	return 0;
}
