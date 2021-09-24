#include <lib.h>

int main(int argc, char **argv)
{
	asm volatile("int $3");
}

