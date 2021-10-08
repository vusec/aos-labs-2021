#include <atomic.h>
#include <cpu.h>
#include <spinlock.h>

#include <x86-64/asm.h>

#include <kernel/acpi.h>

#ifdef USE_BIG_KERNEL_LOCK
/* The big kernel lock */
struct spinlock kernel_lock = {
#ifdef DEBUG_SPINLOCK
	.name = "kernel_lock"
#endif
};
#endif

static int holding(struct spinlock *lock)
{
	return lock->locked && lock->cpu == this_cpu;
}

void spin_init(struct spinlock *lock, const char *name)
{
	lock->locked = 0;

#ifdef DEBUG_SPINLOCK
	lock->name = name;
	lock->cpu = NULL;
#endif
}

void __spin_lock(struct spinlock *lock, const char *file, int line)
{
#ifdef DEBUG_SPINLOCK
	/* Check if this is the second time the lock is being acquired by the
	 * same CPU.
	 */
	if (holding(lock)) {
		panic("\n"
		      "%s:%d: cpu %2d: attempt to lock %s twice\n"
		      "%s:%d: cpu %2d: currently locked here\n",
		      file, line,
		      lapic_cpunum(),
		      lock->name ? lock->name : "anonymous",
		      lock->file, lock->line,
		      lock->cpu->cpu_id);
	}
#endif

	while (!atomic_cmpxchg(&lock->locked, 0, 1));

	atomic_barrier();

#ifdef DEBUG_SPINLOCK
	lock->cpu = this_cpu;
	lock->file = file;
	lock->line = line;
#endif
}

int __spin_trylock(struct spinlock *lock, const char *file, int line)
{
#ifdef DEBUG_SPINLOCK
	/* Check if this is the second time the lock is being acquired by the
	 * same CPU.
	 */
	if (holding(lock)) {
		panic("\n"
		      "%s:%d: cpu %2d: attempt to lock %s twice\n"
		      "%s:%d: cpu %2d: currently locked here\n",
		      file, line,
		      lapic_cpunum(),
		      lock->name ? lock->name : "anonymous",
		      lock->file, lock->line,
		      lock->cpu->cpu_id);
	}
#endif

	if (!atomic_cmpxchg(&lock->locked, 0, 1)) {
		return 0;
	}

	atomic_barrier();

#ifdef DEBUG_SPINLOCK
	lock->cpu = this_cpu;
	lock->file = file;
	lock->line = line;
#endif

	return 1;
}

void __spin_unlock(struct spinlock *lock, const char *file, int line)
{
#ifdef DEBUG_SPINLOCK
	/* Check if the lock is actually locked before unlocking. */
	if (!lock->locked) {
		panic("\n"
		      "%s:%d: cpu %2d:%s not locked\n",
		      file, line,
		      lapic_cpunum(),
		      lock->name ? lock->name : "anonymous");
	}

	/* Check if the lock that we are about to unlock is actually owned by
	 * another CPU.
	 */
	if (!holding(lock)) {
		panic("\n"
		      "%s:%d: cpu %2d: attempt to unlock %s\n"
		      "%s:%d: cpu %2d: currently locked here\n",
		      file, line,
		      lapic_cpunum(),
		      lock->name ? lock->name : "anonymous",
		      lock->file, lock->line, lock->cpu->cpu_id);
	}

	lock->cpu = NULL;
	lock->file = NULL;
	lock->line = 0;
#endif

	atomic_barrier();
	lock->locked = 0;
	atomic_barrier();
}

