#include <types.h>
#include <paging.h>

#include <kernel/mem.h>
#include <kernel/sched/task.h>

struct user_info {
	uintptr_t va;
	uint64_t flags;
};

static int check_user_hole(uintptr_t base, uintptr_t end,
	struct page_walker *walker)
{
	struct user_info *info = walker->udata;

	/* LAB 3: your code here. */
	return 0;
}

static int check_user_pte(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	struct user_info *info = walker->udata;

	/* LAB 3: your code here. */
	return 0;
}

static int check_user_pde(physaddr_t *entry, uintptr_t base, uintptr_t end,
    struct page_walker *walker)
{
	struct user_info *info = walker->udata;

	/* LAB 3: your code here. */
	return 0;
}

int check_user_mem(uintptr_t *fault_va, struct page_table *pml4, void *va,
	size_t size, uint64_t flags)
{
	struct user_info info = {
		.flags = flags | PAGE_PRESENT | PAGE_USER,
	};
	struct page_walker walker = {
		.pt_hole_callback = check_user_hole,
		.pte_callback = check_user_pte,
		.pde_callback = check_user_pde,
		.udata = &info,
	};
	int ret;

	ret = walk_page_range(pml4, va, (void *)((uintptr_t)va + size),
		&walker);

	*fault_va = info.va;

	return ret;
}

/*
 * Checks that the task is allowed to access the range of memory
 * [va, va + size) with the permissions flags | PAGE_PRESENT | PAGE_USER.
 * If it can, then the function simply returns.
 * If it cannot, the task gets killed and if the task is the current task,
 * this function will not return.
 */
void assert_user_mem(struct task *task, void *va, size_t size, int flags)
{
	uintptr_t fault_va;

	if (check_user_mem(&fault_va, task->task_pml4, va, size,
		flags | PAGE_USER) < 0) {
		cprintf("[PID %5u] Access violation for va %p\n",
			task->task_pid, fault_va);
		task_destroy(task);
	}
}
