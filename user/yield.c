#include <lib.h>

int main(int argc, char **argv)
{
	pid_t pid = getpid();
	int i;

	printf("[PID %5u] Hello!\n", pid);

	for (i = 0; i < 5; ++i) {
		sched_yield();
		printf("[PID %5u] I am back! This is round %d\n", pid, i + 1);
	}

	printf("[PID %5u] I am done! Good bye!\n", pid);

	return 0;
}

