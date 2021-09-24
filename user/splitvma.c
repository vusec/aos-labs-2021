/* Attempt to merge from the middle. */
#include <lib.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0x1000000;

	mmap(addr, 3 * PAGE_SIZE, PROT_READ,
	     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	munmap(addr + 1 * PAGE_SIZE, PAGE_SIZE);

	print_vmas();

	return 0;
}
