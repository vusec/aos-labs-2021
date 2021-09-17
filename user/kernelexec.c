#include <lib.h>

int main(int argc, char **argv)
{
	/* Trigger a fault by executing the kernel. */
	void (* func)(void) = (void (*)(void))0xffff800000100000;

	func();

	return 0;
}
