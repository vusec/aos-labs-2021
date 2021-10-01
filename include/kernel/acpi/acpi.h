#pragma once

#include <acpi.h>

struct rsdp *rsdp_find(void);
void *acpi_find_table(struct rsdp *rsdp, const char *signature);
void acpi_dump_tables(struct rsdp *rsdp);

