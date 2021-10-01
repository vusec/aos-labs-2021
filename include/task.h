#pragma once

#include <types.h>
#include <list.h>
#include <rbtree.h>

#include <x86-64/idt.h>
#include <x86-64/memory.h>

typedef int32_t pid_t;

/* Values of task_status in struct task. */
enum {
	TASK_DYING = 0,
	TASK_RUNNABLE,
	TASK_RUNNING,
	TASK_NOT_RUNNABLE,
};

/* The method of interrupt used to switch to the kernel. */
enum {
	TASK_INT = 0,
	TASK_SYSCALL,
};

/* Special task types. */
enum task_type {
	TASK_TYPE_USER = 0,
};

struct task {
	/* The saved registers. */
	struct int_frame task_frame;

	/* The task this task is waiting on. */
	struct task *task_wait;

	/* The process ID of this task and its parent. */
	pid_t task_pid;
	pid_t task_ppid;

	/* The task type. */
	enum task_type task_type;

	/* The task status. */
	unsigned task_status;

	/* The number of times the task has been run. */
	unsigned task_runs;

	/* The CPU that the task is running on. */
	int task_cpunum;

	/* The virtual address space. */
	struct page_table *task_pml4;

	/* The VMAs */
	struct rb_tree task_rb;
	struct list task_mmap;

	/* The children */
	struct list task_children;
	struct list task_child;

	/* The zombies */
	struct list task_zombies;

	/* The anchor node (for zombies or the run queue) */
	struct list task_node;
};

