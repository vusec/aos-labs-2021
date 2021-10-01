#include <lib.h>

int check_page(unsigned char *page, int c, size_t size)
{
	size_t i;

	for (i = 0; i < size; ++i) {
		if (page[i] != c) {
			return -1;
		}
	}

	return 0;
}

int main(void)
{
	pid_t child;
	void *page;

	page = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	memset(page, 0x42, 4096);

	if (check_page(page, 0x42, 4096) != 0) {
		panic("page is corrupt!");
	}

	child = fork();

	if (child > 0) {
		memset(page, 0x55, 4096);

		sched_yield();
		sched_yield();

		if (check_page(page, 0x55, 4096) != 0) {
			panic("parent page is corrupt!");
		}

		memset(page, 0x44, 4096);

		sched_yield();
		sched_yield();

		if (check_page(page, 0x44, 4096) != 0) {
			panic("parent page is corrupt!");
		}

		printf("[PID %5u] I am the parent!\n", getpid());
	} else {
		memset(page, 0xaa, 4096);

		sched_yield();
		sched_yield();

		if (check_page(page, 0xaa, 4096) != 0) {
			panic("child page is corrupt!");
		}

		memset(page, 0xbb, 4096);

		sched_yield();
		sched_yield();

		if (check_page(page, 0xbb, 4096) != 0) {
			panic("child page is corrupt!");
		}

		printf("[PID %5u] I am the child!\n", getpid());
	}

	return 0;
}

