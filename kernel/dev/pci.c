#include <pci.h>
#include <stdio.h>

#include <x86-64/asm.h>

#include <kernel/acpi.h>
#include <kernel/dev/pci.h>
#include <kernel/mem.h>

extern struct pci_driver ahci_driver;
extern struct pci_driver ata_driver;

struct pci_driver *pci_drivers[] = {
	&ahci_driver,
	&ata_driver,
	NULL,
};

uint8_t pci_io_read8(struct pci_dev *dev, size_t offset)
{
	outl(PCI_CONF_ADDR, dev->io_addr + offset);
	return inb(PCI_CONF_DATA);
}

uint16_t pci_io_read16(struct pci_dev *dev, size_t offset)
{
	offset &= ~0x1;
	outl(PCI_CONF_ADDR, dev->io_addr + offset);
	return inw(PCI_CONF_DATA);
}

uint32_t pci_io_read32(struct pci_dev *dev, size_t offset)
{
	offset &= ~0x3;
	outl(PCI_CONF_ADDR, dev->io_addr + offset);
	return inl(PCI_CONF_DATA);
}

void pci_io_write8(struct pci_dev *dev, size_t offset, uint8_t val)
{
	outl(PCI_CONF_ADDR, dev->io_addr + offset);
	outb(PCI_CONF_DATA, val);
}

void pci_io_write16(struct pci_dev *dev, size_t offset, uint16_t val)
{
	offset &= ~0x1;
	outl(PCI_CONF_ADDR, dev->io_addr + offset);
	outw(PCI_CONF_DATA, val);
}

void pci_io_write32(struct pci_dev *dev, size_t offset, uint32_t val)
{
	offset &= ~0x3;
	outl(PCI_CONF_ADDR, dev->io_addr + offset);
	outl(PCI_CONF_DATA, val);
}

struct pci_ops io_ops = {
	.read8   = pci_io_read8,
	.read16  = pci_io_read16,
	.read32  = pci_io_read32,
	.write8  = pci_io_write8,
	.write16 = pci_io_write16,
	.write32 = pci_io_write32,
};

uint8_t pci_mmio_read8(struct pci_dev *dev, size_t offset)
{
	return *(uint8_t *)((char *)dev->data + offset);
}

uint16_t pci_mmio_read16(struct pci_dev *dev, size_t offset)
{
	offset &= ~0x1;
	return *(uint16_t *)((char *)dev->data + offset);
}

uint32_t pci_mmio_read32(struct pci_dev *dev, size_t offset)
{
	offset &= ~0x3;
	return *(uint32_t *)((char *)dev->data + offset);
}

void pci_mmio_write8(struct pci_dev *dev, size_t offset, uint8_t val)
{
	*(uint8_t *)((char *)dev->data + offset) = val;
}

void pci_mmio_write16(struct pci_dev *dev, size_t offset, uint16_t val)
{
	offset &= ~0x1;
	*(uint16_t *)((char *)dev->data + offset) = val;
}

void pci_mmio_write32(struct pci_dev *dev, size_t offset, uint32_t val)
{
	offset &= ~0x3;
	*(uint32_t *)((char *)dev->data + offset) = val;
}

struct pci_ops mmio_ops = {
	.read8   = pci_mmio_read8,
	.read16  = pci_mmio_read16,
	.read32  = pci_mmio_read32,
	.write8  = pci_mmio_write8,
	.write16 = pci_mmio_write16,
	.write32 = pci_mmio_write32,
};

uint8_t pci_read8(struct pci_dev *dev, size_t offset)
{
	return dev->ops->read8(dev, offset);
}

uint16_t pci_read16(struct pci_dev *dev, size_t offset)
{
	return dev->ops->read16(dev, offset);
}

uint32_t pci_read32(struct pci_dev *dev, size_t offset)
{
	return dev->ops->read32(dev, offset);
}

void pci_write8(struct pci_dev *dev, size_t offset, uint8_t val)
{
	dev->ops->write8(dev, offset, val);
}

void pci_write16(struct pci_dev *dev, size_t offset, uint16_t val)
{
	dev->ops->write16(dev, offset, val);
}

void pci_write32(struct pci_dev *dev, size_t offset, uint32_t val)
{
	dev->ops->write32(dev, offset, val);
}

int pci_read_bar(uint32_t *base, uint32_t *size, struct pci_dev *dev,
	size_t index)
{
	size_t offset = offsetof(struct pci_regs, bar) + index * 4;
	uint32_t bar;

	bar = pci_read32(dev, offset);

	if (base) {
		*base = bar;
	}

	if (size) {
		pci_write32(dev, offset, 0xffffffff);
		*size = (~pci_read32(dev, offset) | 0xf) + 1;
		pci_write32(dev, offset, bar);
	}

	return 0;
}

struct pci_dev *alloc_mmio_dev(uint32_t base_addr, size_t bus, size_t slot,
	size_t func)
{
	struct pci_dev *dev;
	pci_addr_mmio_t addr;

	addr.val = base_addr;
	addr.func = func;
	addr.slot = slot;
	addr.bus = bus;

	dev = kmalloc(sizeof *dev);

	if (!dev) {
		return NULL;
	}

	dev->data = mmio_map_region(addr.val, PAGE_SIZE);

	if (!dev->data) {
		kfree(dev);
		return NULL;
	}

	dev->ops = &mmio_ops;

	return dev;
}

struct pci_dev *alloc_io_dev(size_t bus, size_t slot, size_t func)
{
	struct pci_dev *dev;
	pci_addr_io_t addr;

	addr.func = func;
	addr.slot = slot;
	addr.bus = bus;
	addr.enable = 1;

	dev = kmalloc(sizeof *dev);

	if (!dev) {
		return NULL;
	}

	dev->io_addr = addr.val;
	dev->ops = &io_ops;

	return dev;
}

struct pci_driver *pci_find_driver(uint16_t device_id, uint16_t vendor_id,
	uint8_t class, uint8_t subclass)
{
	struct pci_driver **driver;
	struct pci_device_id *compat;

	for (driver = pci_drivers; *driver; ++driver) {
		for (compat = (*driver)->compat; compat->vendor_id; ++compat) {
			if (compat->device_id == device_id &&
			    compat->vendor_id == vendor_id) {
				return *driver;
			}

			if (compat->device_id != PCI_ANY_ID ||
			    compat->vendor_id != PCI_ANY_ID) {
				continue;
			}

			if (compat->class == class &&
			    compat->subclass == subclass) {
				return *driver;
			}
		}
	}

	return NULL;
}

int pci_enum_device(uint32_t base_addr, size_t bus, size_t slot, size_t func)
{
	struct pci_driver *driver;
	struct pci_dev *dev;
	uint16_t device_id, vendor_id;
	uint8_t class, subclass;

	if (base_addr) {
		dev = alloc_mmio_dev(base_addr, bus, slot, func);
	} else {
		dev = alloc_io_dev(bus, slot, func);
	}

	if (!dev) {
		return -1;
	}

	device_id = pci_read16(dev, offsetof(struct pci_regs, device_id));
	vendor_id = pci_read16(dev, offsetof(struct pci_regs, vendor_id));

	if (device_id == PCI_ANY_ID && vendor_id == PCI_ANY_ID) {
		kfree(dev);
		return -1;
	}

	class = pci_read8(dev, offsetof(struct pci_regs, class));
	subclass = pci_read8(dev, offsetof(struct pci_regs, subclass));

	cprintf("PCI: %02x:%02x.%u %04x:%04x %02x:%02x\n",
		bus, slot, func,
		device_id, vendor_id,
		class, subclass);

	driver = pci_find_driver(device_id, vendor_id, class, subclass);

	if (!driver) {
		kfree(dev);
		return 0;
	}

	if (driver->probe(dev) < 0) {
		kfree(dev);
		return 0;
	}

	return 0;
}

void pci_enum_devices(uint32_t base_addr, size_t bus_start, size_t bus_end)
{
	volatile struct pci_regs *regs;
	pci_addr_mmio_t addr;
	size_t bus, slot, func;

	for (bus = bus_start; bus < bus_end; ++bus) {
		for (slot = 0; slot < 32; ++slot) {
			for (func = 0; func < 8; ++func) {
				if (pci_enum_device(base_addr, bus, slot,
					func) != 0) {
					break;
				}
			}
		}
	}
}

int pci_init(struct rsdp *rsdp)
{
	if (mcfg_init(rsdp) == 0) {
		return 0;
	}

	pci_enum_devices(0, 0, 256);

	return 0;
}

