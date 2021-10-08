#include <lib.h>

int main(int argc, char **argv)
{
	pid_t pid;
	unsigned cpuid;
	size_t i;

	/* Fork a bunch of processes. */
	for (i = 0; i < 16; ++i) {
		if (fork() == 0) {
			break;
		}
	}

	pid = getpid();
	cpuid = getcpuid();

	printf("[PID %5u] Running on CPU %u\n", pid, cpuid);

	return 0;
}

