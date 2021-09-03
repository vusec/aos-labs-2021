#pragma once

#define CR0_PM (1 << 0)
#define CR0_PAGING (1 << 31)

#define CR4_PAE (1 << 5)

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

#define MSR_EFER (0xC0000080)
#define MSR_EFER_LME (1 << 8)

#ifndef __ASSEMBLER__
#include <x86/types.h>

static inline unsigned long read_msr(unsigned int reg)
{
	unsigned long val;
	asm volatile("rdmsr" : "=A" (val) : "c" (reg));
	return val;
}

static inline void write_msr(unsigned reg, unsigned long val)
{
	asm volatile("wrmsr" :: "c" (reg), "A" (val));	
}

static inline void *read_rbp(void)
{
	void *ret;
	asm volatile("movq %%rbp, %0" : "=r" (ret));
	return ret;
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

#endif /* !defined(__ASSEMBLER__) */

