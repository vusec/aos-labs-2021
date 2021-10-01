#include <stdio.h>

#include <x86-64/asm.h>

#include <hpet.h>

#include <kernel/acpi.h>
#include <kernel/mem.h>

static volatile struct hpet_regs *hpet_regs;
static uint64_t hpet_freq;

int hpet_init(struct rsdp *rsdp)
{
	struct acpi_hdr *hdr;
	struct hpet *hpet;

	hpet = acpi_find_table(rsdp, "HPET");

	if (!hpet) {
		return -1;
	}

	if (!hpet_regs) {
		hpet_regs = mmio_map_region(hpet->addr.addr, PAGE_SIZE);
	}

	if (!hpet_regs) {
		return -1;
	}

	hpet_regs->cfg.enable_cnf = 0;
	hpet_regs->main_cnt_val = 0;
	hpet_regs->cfg.enable_cnf = 1;

	hpet_freq = 1000000000000000ULL / hpet_regs->caps.cnt_clk_period;

	return 0;
}

void hpet_get_time(struct timespec *time)
{
	uint64_t cnt = hpet_regs->main_cnt_val;

	time->tv_sec = cnt / hpet_freq;
	time->tv_nsec = cnt % hpet_freq;
}

