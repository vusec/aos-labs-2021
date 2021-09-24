#include <lib.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0x1000000;

	assert(mmap(addr, PAGE_SIZE, PROT_READ | PROT_WRITE,
	     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0) == addr);
	memset(addr, 0, PAGE_SIZE);
	assert(mprotect(addr, PAGE_SIZE, PROT_WRITE) != 0);
	print_vmas();
	*(volatile char *)addr;

	return 0;
}
