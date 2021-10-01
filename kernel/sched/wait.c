#include <types.h>
#include <error.h>

#include <kernel/mem.h>
#include <kernel/sched.h>

pid_t sys_wait(int *rstatus)
{
	/* LAB 5: your code here. */
	return -ENOSYS;
}

pid_t sys_waitpid(pid_t pid, int *rstatus, int opts)
{
	/* LAB 5: your code here. */
	return -ENOSYS;
}

