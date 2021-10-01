#include <lib.h>

int main(void)
{
	pid_t child;

	child = fork();

	if (child > 0) {
		sched_yield();
		printf("[PID %5u] I am the parent!\n", getpid());
		assert(waitpid(child, NULL, 0) == child);
	} else {
		printf("[PID %5u] I am the child!\n", getpid());
	}

	return 0;
}

