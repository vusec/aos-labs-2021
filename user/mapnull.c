/* Try to map NULL. */
#include <lib.h>
#include <string.h>

int main(int argc, char **argv)
{
	assert(mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
	     MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0) == MAP_FAILED);

	print_vmas();

	return 0;
}
