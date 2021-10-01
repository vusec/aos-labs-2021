#include <error.h>
#include <list.h>

#include <kernel/console.h>
#include <kernel/mem.h>
#include <kernel/monitor.h>
#include <kernel/sched.h>
#include <kernel/vma.h>

extern struct list runq;

/* Allocates a task struct for the child process and copies the register state,
 * the VMAs and the page tables. Once the child task has been set up, it is
 * added to the run queue.
 */
struct task *task_clone(struct task *task)
{
	/* LAB 5: your code here. */
	return NULL;
}

pid_t sys_fork(void)
{
	/* LAB 5: your code here. */
	return -ENOSYS;
}

