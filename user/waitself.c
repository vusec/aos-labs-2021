#include <lib.h>

int main(int argc, char **argv)
{
	assert(waitpid(getpid(), NULL, 0) == -ECHILD);

	return 0;
}

