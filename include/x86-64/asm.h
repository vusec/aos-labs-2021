#pragma once

#define CR0_PM     (1 << 0)
#define CR0_PAGING (1 << 31)

#define CR4_PAE  (1 << 5)
#define CR4_SMEP (1 << 20)
#define CR4_SMAP (1 << 21)

#define FLAGS_CF      (1 << 0)
#define FLAGS_PF      (1 << 2)
#define FLAGS_AF      (1 << 4)
#define FLAGS_ZF      (1 << 6)
#define FLAGS_SF      (1 << 7)
#define FLAGS_TF      (1 << 8)
#define FLAGS_IF      (1 << 9)
#define FLAGS_DF      (1 << 10)
#define FLAGS_OF      (1 << 11)
#define FLAGS_IOPL(x) (((x & 0x3) << 12)
#define FLAGS_NT      (1 << 14)
#define FLAGS_RF      (1 << 16)
#define FLAGS_VM      (1 << 17)
#define FLAGS_AC      (1 << 18)
#define FLAGS_VIF     (1 << 19)
#define FLAGS_VIP     (1 << 20)
#define FLAGS_ID      (1 << 21)

#define MSR_APIC_BASE      0x0000001b

#define MSR_EFER           0xc0000080
#define MSR_STAR           0xc0000081
#define MSR_LSTAR          0xc0000082
#define MSR_CSTAR          0xc0000083
#define MSR_SFMASK         0xc0000084

#define MSR_FS_BASE        0xc0000100
#define MSR_GS_BASE        0xc0000101
#define MSR_KERNEL_GS_BASE 0xc0000102

#define MSR_EFER_SCE (1 << 0)
#define MSR_EFER_LME (1 << 8)
#define MSR_EFER_NXE (1 << 11)

#ifndef __ASSEMBLER__
#include <x86-64/types.h>

union star_reg {
	uint64_t reg;
	struct {
		uint32_t eip;
		uint16_t kern_sel;
		uint16_t user_sel;
	};
};

static inline uint64_t read_msr(unsigned int reg)
{
	uint32_t lo, hi;
	
	asm volatile(
		"rdmsr\n" :
		"=a" (lo),
		"=d" (hi) :
		"c" (reg));

	return (uint64_t)hi << 32 | (uint64_t)lo;
}

static inline void write_msr(unsigned reg, uint64_t val)
{
	asm volatile(
		"wrmsr" ::
		"c" (reg),
		"a" ((uint32_t)(val & 0xFFFFFFFF)),
		"d" ((uint32_t)((val >> 32) & 0xFFFFFFFF)));
}

static inline void *read_rbp(void)
{
	void *ret;
	
	asm volatile("movq %%rbp, %0" : "=r" (ret));
	
	return ret;
}

static inline void *read_rsp(void)
{
	void *ret;
	
	asm volatile("movq %%rsp, %0" : "=r" (ret));
	
	return ret;
}

static inline uint64_t read_rflags(void)
{
	uint64_t rflags;

	asm volatile(
		"pushfq\n"
		"popq %0\n"
		: "=q" (rflags));

	return rflags;
}

static inline uintptr_t read_cr2(void)
{
	uintptr_t ret;
	
	asm volatile("movq %%cr2, %0" : "=r" (ret));
	
	return ret;
}

static inline uintptr_t read_cr3(void)
{
	uintptr_t value;

	asm volatile("movq %%cr3, %0\n" : "=r" (value));

	return value;
}

static inline void write_cr3(uintptr_t value)
{
	asm volatile("movq %0, %%cr3\n" :: "r" (value));
}

static inline uintptr_t read_cr4(void)
{
	uintptr_t value;

	asm volatile("movq %%cr4, %0\n" : "=r" (value));

	return value;
}

static inline void write_cr4(uintptr_t value)
{
	asm volatile("movq %0, %%cr4\n" :: "r" (value));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t data;
	
	asm volatile("inb %w1, %0" : "=a" (data) : "d" (port));
	
	return data;
}

static inline void insb(uint16_t port, void *addr, size_t n)
{
	asm volatile(
		"cld\n"
		"repne\n"
		"insb\n" :
		"=D" (addr), "=c" (n) :
		"d" (port), "0" (addr), "1" (n) :
		"memory", "cc");
}

static inline uint16_t inw(uint16_t port)
{
	uint16_t data;
	
	asm volatile("inw %w1, %0" : "=a" (data) : "d" (port));
	
	return data;
}

static inline void insw(uint16_t port, void *addr, size_t n)
{
	asm volatile(
		"cld\n"
		"repne\n"
		"insw\n" :
		"=D" (addr), "=c" (n) :
		"d" (port), "0" (addr), "1" (n) :
		"memory", "cc");
}

static inline uint32_t inl(uint16_t port)
{
	uint32_t data;
	
	asm volatile("inl %w1, %0" : "=a" (data) : "d" (port));
	
	return data;
}

static inline void insl(uint16_t port, void *addr, size_t n)
{
	asm volatile(
		"cld\n"
		"repne\n"
		"insl\n" :
		"=D" (addr), "=c" (n) :
		"d" (port), "0" (addr), "1" (n) :
		"memory", "cc");
}

static inline void outb(uint16_t port, uint8_t data)
{
	asm volatile("outb %0, %w1" :: "a" (data), "d" (port));
}

static inline void outsb(uint16_t port, const void *addr, size_t n)
{
	asm volatile(
		"cld\n"
		"repne\n"
		"outsb\n" :
		"=S" (addr), "=c" (n) :
		"d" (port), "0" (addr), "1" (n) :
		"cc");
}

static inline void outw(uint16_t port, uint16_t data)
{
	asm volatile("outw %0, %w1" :: "a" (data), "d" (port));
}

static inline void outsw(uint16_t port, const void *addr, size_t n)
{
	asm volatile(
		"cld\n"
		"repne\n"
		"outsw\n" :
		"=S" (addr), "=c" (n) :
		"d" (port), "0" (addr), "1" (n) :
		"cc");
}

static inline void outl(uint16_t port, uint32_t data)
{
	asm volatile("outl %0, %w1" :: "a" (data), "d" (port));
}

static inline void outsl(uint16_t port, const void *addr, size_t n)
{
	asm volatile(
		"cld\n"
		"repne\n"
		"outsl\n" :
		"=S" (addr), "=c" (n) :
		"d" (port), "0" (addr), "1" (n) :
		"cc");
}

static inline uint64_t read_tsc(void)
{
	uint64_t ret;
	asm volatile("rdtsc" : "=A" (ret));
	return ret;
}

static inline uint32_t xchg(volatile uint32_t *addr, uint32_t newval)
{
	uint32_t ret;

	/* The + in "+m" denotes a read-modify-write operand. */
	asm volatile(
		"lock; xchgl %0, %1" :
		"+m" (*addr), "=a" (ret) :
		"1" (newval) :
		"cc");
	return ret;
}

static inline void cpuid(unsigned long fn, uint32_t *eaxp, uint32_t *ebxp,
	uint32_t *ecxp, uint32_t *edxp)
{
	uint32_t eax, ebx, ecx, edx;

	asm volatile("cpuid" :
		"=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) :
		"a" (fn));

	if (eaxp)
		*eaxp = eax;

	if (ebxp)
		*ebxp = ebx;

	if (ecxp)
		*ecxp = ecx;

	if (edxp)
		*edxp = edx;
}

#endif /* !defined(__ASSEMBLER__) */

