#pragma once

/* Define fixed-size integer types. */
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

#define INT8_C(x) (x)
#define UINT8_C(x) (x)
#define INT16_C(x) (x)
#define UINT16_C(x) (x)
#define INT32_C(x) (x)
#define UINT32_C(x) (x ## U)
#define INT64_C(x) (x ## LL)
#define UINT64_C(x) (x ## ULL)

/* Define size_t to represent memory object sizes. */
typedef uint32_t size_t;

/* Define ptrdiff_t to represent pointer differences. */
typedef int32_t ptrdiff_t;

/* Define intptr_t and uintptr_t for integral pointer arithmetic. */
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

/* Define physaddr_t to represent physical addresses. */
typedef uint32_t physaddr_t;

