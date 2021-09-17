/* Tests read and writes to a large bss. */
#include <lib.h>

#define ARRAY_SIZE (1024 * 1024)

uint32_t big_array[ARRAY_SIZE];

int main(int argc, char **argv)
{
	int i;

	printf("Making sure bss works right...\n");

	for (i = 0; i < ARRAY_SIZE; ++i) {
		if (big_array[i] != 0) {
			panic("big_array[%d] isn't cleared!\n", i);
		}
	}

	for (i = 0; i < ARRAY_SIZE; ++i) {
		big_array[i] = i;
	}

	for (i = 0; i < ARRAY_SIZE; ++i) {
		if (big_array[i] != i) {
			panic("big_array[%d] didn't hold its value!\n", i);
		}
	}

	printf("Yes, good. Now doing a wild write off the end...\n");
	big_array[ARRAY_SIZE + 1024] = 0;
	panic("Why are we still here?");
}
