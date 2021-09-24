/* Attempt to merge from the middle. */
#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0x1000000;

	mmap(addr, PAGE_SIZE, PROT_READ | PROT_WRITE,
	     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	memset(addr, 0, PAGE_SIZE);
	munmap(addr, PAGE_SIZE);

	print_vmas();

	return 0;
}
