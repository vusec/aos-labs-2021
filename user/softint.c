/* Buggy program - causes an illegal software interrupt. */

#include <lib.h>

int main(int argc, char **argv)
{
    asm volatile("int $14"); /* page fault */
}

