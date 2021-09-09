#pragma once

#include <types.h>
#include <task.h>
#include <vma.h>

struct vma *find_vma(struct rb_node **out_parent, int *out_dir,
	struct rb_tree *tree, void *addr);
struct vma *task_find_vma(struct task *task, void *addr);
struct vma *task_add_binary_vma(struct task *task, char *name, void *addr,
	size_t size, int flags, void *src, size_t len);
struct vma *task_add_anon_vma(struct task *task, char *name, void *addr,
	size_t size, int flags);
struct vma *task_new_vma(struct task *task, char *name, void *addr, size_t size,
	int flags);
void task_remove_vma(struct task *task, struct vma *vma);
void task_free_vmas(struct task *task);
int protect_vma_range(struct task *task, void *base, size_t size, int flags);
int unmap_vma_range(struct task *task, void *base, size_t size);
int task_page_fault_handler(struct task *task, void *va, int flags);
void show_vmas(struct task *task);

