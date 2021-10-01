#include <lib.h>

int main(void)
{
	pid_t child;
	size_t i;

	child = fork();

	if (child > 0) {
		assert(waitpid(child, NULL, 0) == child);
		printf("[PID %5u] I am the parent!\n", getpid());
	} else {
		printf("[PID %5u] I am the child!\n", getpid());

		for (i = 0; i < 5; ++i) {
			printf("[PID %5u] Back again!\n", getpid());
		}
	}

	return 0;
}

