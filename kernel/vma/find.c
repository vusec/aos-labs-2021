#include <task.h>
#include <vma.h>

#include <kernel/vma.h>

/* Finds the VMA in the red-black tree with an end address greater than the
 * requested address. If the base address of any found VMA is less than the
 * requested address, then the VMA is an exact match and that VMA is returned
 * directly.
 *
 * The arguments out_parent and out_dir are used to insert new VMAs into the
 * red-black tree. If these arguments are provided, then out_parent is set to
 * the parent of the found VMA and out_dir is set to the direction in which
 * you would find the found VMA as child from the parent.
 */
struct vma *find_vma(struct rb_node **out_parent, int *out_dir,
	struct rb_tree *tree, void *addr)
{
	struct rb_node *node, *parent = NULL;
	struct vma *vma = NULL;
	struct vma *vma_tmp = NULL;
	int cmp, dir = 0;

	node = tree->root;

	while (node) {
		vma_tmp = container_of(node, struct vma, vm_rb);
		parent = node;
		dir = (addr >= vma_tmp->vm_end);

		if (!dir) {
			vma = vma_tmp;

			if (vma_tmp->vm_base <= addr) {
				break;
			}
		}

		node = node->child[dir];
	}

	if (out_parent) {
		*out_parent = parent;
	}

	if (out_dir) {
		*out_dir = dir;
	}

	return vma;
}

/* Given a task and an address, this function finds the VMA that the address
 * belongs to. Otherwise this function returns NULL if no VMA is found.
 */
struct vma *task_find_vma(struct task *task, void *addr)
{
	struct vma *vma;

	vma = find_vma(NULL, NULL, &task->task_rb, addr);

	if (!vma || addr < vma->vm_base) {
		return NULL;
	}

	return vma;
}

