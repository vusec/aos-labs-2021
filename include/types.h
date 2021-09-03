#pragma once

#include <x86-64/types.h>

#ifndef NULL
#define NULL ((void*) 0)
#endif

/* Represents true-or-false values */
typedef _Bool bool;
enum { false, true };

/* Efficient min and max operations */
#define MIN(_a, _b)                                                            \
({                                                                             \
    typeof(_a) __a = (_a);                                                     \
    typeof(_b) __b = (_b);                                                     \
    __a <= __b ? __a : __b;                                                    \
})
#define MAX(_a, _b)                                                            \
({                                                                             \
    typeof(_a) __a = (_a);                                                     \
    typeof(_b) __b = (_b);                                                     \
    __a >= __b ? __a : __b;                                                    \
})

/* Rounding operations (efficient when n is a power of 2)
 * Round down to the nearest multiple of n */
#define ROUNDDOWN(a, n)                                                        \
({                                                                             \
    uintptr_t __a = (uintptr_t) (a);                                             \
    (typeof(a)) (__a - __a % (n));                                             \
})
/* Round up to the nearest multiple of n */
#define ROUNDUP(a, n)                                                          \
({                                                                             \
    uintptr_t __n = (uintptr_t) (n);                                             \
    (typeof(a)) (ROUNDDOWN((uintptr_t) (a) + __n - 1, __n));                    \
})

/* Return the offset of 'member' relative to the beginning of a struct type */
#define offsetof(type, member)  ((size_t) (&((type*)0)->member))

#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

#define length_of(a) (sizeof(a) / sizeof((a)[0]))

#define __always_inline         inline __attribute__((always_inline))

#define SIZE_MAX (~(size_t)0)
