#pragma once

#define atomic_add(ptr, val) \
	__sync_fetch_and_add((ptr), (val))
#define atomic_sub(ptr, val) \
	__sync_fetch_and_sub((ptr), (val))
#define atomic_inc(ptr) \
	__sync_fetch_and_add((ptr), 1)
#define atomic_dec(ptr) \
	__sync_fetch_and_sub((ptr), 1)

#define atomic_cmpxchg(ptr, old, val) \
	__sync_bool_compare_and_swap((ptr), (old), (val))

#define atomic_barrier() \
	__sync_synchronize()

