#pragma once

#include <acpi.h>
#include <lib.h>

struct rsdp *rsdp_find(void);
void *acpi_find_table(struct rsdp *rsdp, const char *signature);
void acpi_dump_tables(struct rsdp *rsdp);

int madt_init(struct rsdp *rsdp);

void lapic_init(void);
void lapic_eoi(void);

int hpet_init(struct rsdp *rsdp);
void hpet_get_time(struct timespec *time);

