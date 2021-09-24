#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0xffffc00000000000;

	assert(mmap(addr, PAGE_SIZE, PROT_READ | PROT_WRITE,
	    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0) == MAP_FAILED);

	addr = (void *)USER_LIM;
	assert(mmap(addr, PAGE_SIZE, PROT_READ | PROT_WRITE,
	    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0) == MAP_FAILED);

	print_vmas();

	return 0;
}
