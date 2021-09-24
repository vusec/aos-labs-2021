/*
 * Evil hello world -- kernel pointer passed to kernel kernel should destroy
 * user environment in response.
 */

#include <lib.h>

int main(int argc, char **argv)
{
	/* Try to print the kernel entry point as a string!  Mua ha ha! */
	puts((char*)0xffff800000100000, 100);
}

