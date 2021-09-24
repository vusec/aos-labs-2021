#include <task.h>
#include <vma.h>
#include <stdio.h>

#include <kernel/vma.h>

void show_vmas(struct task *task)
{
	struct vma *vma;
	struct list *node;

	cprintf("[PID %5u] VMAs:\n", task->task_pid);

	list_foreach(&task->task_mmap, node) {
		vma = container_of(node, struct vma, vm_mmap);

		cprintf("  %016p - %016p [%c%c%c] ",
			vma->vm_base, vma->vm_end,
			(vma->vm_flags & VM_READ) ? 'r' : '-',
			(vma->vm_flags & VM_WRITE) ? 'w' : '-',
			(vma->vm_flags & VM_EXEC) ? 'x' : '-');

		if (vma->vm_src) {
			cprintf("%016p - %016p ",
				vma->vm_src, vma->vm_len);
		}

		cprintf("\"%s\"\n", vma->vm_name);
	}
}

