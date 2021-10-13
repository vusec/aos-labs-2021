#pragma once

#include <acpi.h>
#include <pci.h>

struct pci_device_id {
	uint16_t vendor_id;
	uint16_t device_id;
	uint8_t class;
	uint8_t subclass;
};

#define PCI_MATCH_DEVICE(vendor_id, device_id) \
	{ vendor_id, device_id }
#define PCI_MATCH_CLASS(class, subclass) \
	{ PCI_ANY_ID, PCI_ANY_ID, class, subclass }

struct pci_dev;

struct pci_ops {
	uint8_t (* read8)(struct pci_dev *, size_t);
	uint16_t (* read16)(struct pci_dev *, size_t);
	uint32_t (* read32)(struct pci_dev *, size_t);
	void (* write8)(struct pci_dev *, size_t, uint8_t);
	void (* write16)(struct pci_dev *, size_t, uint16_t);
	void (* write32)(struct pci_dev *, size_t, uint32_t);
};

struct pci_dev {
	union {
		void *data;
		uint32_t io_addr;
	};
	struct pci_ops *ops;
};

struct pci_driver {
	struct pci_device_id *compat;
	int (* probe)(struct pci_dev *);
	void (* remove)(struct pci_dev *);
};

void pci_enum_devices(uint32_t base_addr, size_t bus_start, size_t bus_end);
int pci_init(struct rsdp *rsdp);

uint8_t pci_read8(struct pci_dev *dev, size_t offset);
uint16_t pci_read16(struct pci_dev *dev, size_t offset);
uint32_t pci_read32(struct pci_dev *dev, size_t offset);

void pci_write8(struct pci_dev *dev, size_t offset, uint8_t val);
void pci_write16(struct pci_dev *dev, size_t offset, uint16_t val);
void pci_write32(struct pci_dev *dev, size_t offset, uint32_t val);

int pci_read_bar(uint32_t *base, uint32_t *size, struct pci_dev *dev,
	size_t index);

