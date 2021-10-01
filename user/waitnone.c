#include <lib.h>

int main(int argc, char **argv)
{
	assert(wait(NULL) == -ECHILD);
	assert(waitpid(-1, NULL, 0) == -ECHILD);
	assert(waitpid(0, NULL, 0) == -ECHILD);

	return 0;
}

