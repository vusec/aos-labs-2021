#include <lib.h>

int main(int argc, char **argv)
{
	assert(wait(NULL) == -ECHILD);

	return 0;
}

