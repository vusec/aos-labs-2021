/* Attempt to merge from the middle. */
#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0x1000000;
	char *ret;

	ret = mmap(addr, PAGE_SIZE, PROT_NONE,
	     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(ret == addr);

	ret = mmap(addr, PAGE_SIZE, PROT_READ | PROT_WRITE,
	     MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
	assert(ret == addr);

	memset(addr, 0, PAGE_SIZE);

	print_vmas();

	return 0;
}
