#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *addr = (void *)0x1000000;

	assert(mmap(addr, PAGE_SIZE, PROT_READ | PROT_WRITE,
	     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0) == addr);
	memset(addr, 0xc3, PAGE_SIZE);	
	assert(mprotect(addr, PAGE_SIZE, PROT_READ | PROT_EXEC) == 0);
	((void (*)(void))addr)();

	print_vmas();

	return 0;
}
