/*
 * Called from entry.S to get us going.
 */
#include <lib.h>

extern int main(int argc, char **argv);

const char *binary_name = "<unknown>";

void libmain(int argc, char **argv)
{
	/* Save the name of the program so that panic() can use it. */
	if (argc > 0) {
		binary_name = argv[0];
	}

	/* Call user main routine. */
	main(argc, argv);

	/* Exit gracefully. */
	exit();
}

