#pragma once

#include <syscall.h>

#include <x86-64/idt.h>

void sysret64(struct int_frame *frame);

void syscall_init(void);
void syscall_init_mp(void);
int64_t syscall(uint64_t num, uint64_t a1, uint64_t a2, uint64_t a3,
	uint64_t a4, uint64_t a5, uint64_t a6);
