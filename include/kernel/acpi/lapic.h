#pragma once

uint32_t lapic_read(uint16_t addr);
void lapic_write(uint16_t addr, uint32_t value);
void lapic_init(void);
int lapic_cpunum(void);
void lapic_eoi(void);
void lapic_ipi(int vector);
void lapic_startup(uint8_t apic_id, uint32_t addr);

