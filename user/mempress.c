/* Tests read and writes to a large bss. */
#include <lib.h>

#define ARRAY_SIZE (128 * 1024 * 1024)

char big_array[ARRAY_SIZE];

int main(int argc, char **argv)
{
	int i;

	/* Write to all of the available physical memory. */
	memset(big_array, 0xd0, sizeof big_array);
	assert(big_array[10] == (char)0xd0);
	printf("Memory of size %d bytes set to: %x\n", ARRAY_SIZE, big_array[10]);

	/* Read every page so that they get swapped back again. */
	for (i = 37; i < ARRAY_SIZE; i += PAGE_SIZE) {
		assert(big_array[i] == (char)0xd0);
	}

	printf("mempress successful.\n");

	return 0;
}

