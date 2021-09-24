#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0x1000000;
	char *ret;

	ret = mmap(addr, PAGE_SIZE, PROT_WRITE,
	     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(ret == MAP_FAILED);

	print_vmas();

	return 0;
}
