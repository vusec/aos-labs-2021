#include <lib.h>

int main(int argc, char **argv)
{
	const char *s = "Hello, world!\n";

	puts(s, 0x800000000000);
}
