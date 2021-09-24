#include <task.h>
#include <vma.h>

#include <kernel/vma.h>

/* Walks over the address range [base, base + size) for the given task,
 * iterating over all the VMAs that fall within that address range and calling
 * the user-provided get_vma() function for each VMA.
 */
int walk_vma_range(struct task *task, void *base, size_t size,
	int (* get_vma)(struct task *, void *, size_t, struct vma *, void *),
	void *udata)
{
	struct vma *vma, *rhs;
	struct list *node, *next;
	void *end = (void *)((uintptr_t)base + size);
	int ret;

	/* Find the VMA with an end address greater than the base, but also the
	 * closest to the base.
	 */
	vma = find_vma(NULL, NULL, &task->task_rb, base);

	if (!vma || end <= vma->vm_base) {
		return -1;
	}

	next = list_next(&task->task_mmap, &vma->vm_mmap);

	/* Now call get_vma() on each VMA as long as there is a VMA and as long
	 * as each VMA has a base address less than the end address.
	 */
	while (vma && vma->vm_base < end) {
		ret = get_vma(task, base, size, vma, udata);

		if (ret < 0) {
			return ret;
		}

		node = next;
		next = list_next(&task->task_mmap, &vma->vm_mmap);
		vma = node ? container_of(node, struct vma, vm_mmap) : NULL;
	}

	return 0;
}

