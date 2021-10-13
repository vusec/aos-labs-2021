#pragma once

#include <types.h>

#define PCI_CONF_ADDR 0xcf8
#define PCI_CONF_DATA 0xcfc
#define PCI_ANY_ID    0xffff

struct pci_regs {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t cmd;
	uint16_t status;
	uint8_t  rev_id;
	uint8_t  prog_if;
	uint8_t  subclass;
	uint8_t  class;
	uint8_t  cl_size;
	uint8_t  lat_timer;
	uint8_t  header_type : 7;
	uint8_t  multifunction : 1;
	uint8_t  bist;
	uint32_t bar[6];
} __attribute__ ((packed));

typedef union {
	uint32_t val;
	struct {
		uint32_t offset : 8;
		uint32_t func : 3;
		uint32_t slot : 5;
		uint32_t bus : 8;
		uint32_t _0 : 7;
		uint32_t enable : 1;
	} __attribute__ ((packed));
} pci_addr_io_t;

typedef union {
	uint32_t val;
	struct {
		uint32_t offset : 12;
		uint32_t func : 3;
		uint32_t slot : 5;
		uint32_t bus : 8;
		uint32_t _0 : 4;
	} __attribute__ ((packed));
} pci_addr_mmio_t;

