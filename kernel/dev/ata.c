#include <error.h>
#include <stdio.h>
#include <ata.h>
#include <pci.h>

#include <x86-64/asm.h>

#include <kernel/dev/disk.h>
#include <kernel/dev/pci.h>
#include <kernel/mem.h>

static struct pci_device_id compat[] = {
	PCI_MATCH_CLASS(1, 1),
	{},
};

static struct ata_chan channels[2];

void ata_wait_400ns(struct ata_chan *chan)
{
	inb(chan->alt_status);
	inb(chan->alt_status);
	inb(chan->alt_status);
	inb(chan->alt_status);
}

void ata_reset(struct ata_chan *chan)
{
	outb(chan->ctrl, 4);
	ata_wait_400ns(chan);
	outb(chan->ctrl, 0);
}

void ata_disable_irq(void)
{
	outb(channels[ATA_PRIMARY].ctrl, (1 << 1));
	outb(channels[ATA_SECONDARY].ctrl, (1 << 1));
}

enum ata_state {
	ATA_IDLE,
	ATA_READ,
	ATA_WRITE,
};

struct ata_disk {
	struct disk disk;
	struct ata_chan *chan;
	size_t drive;
	enum ata_state state;
	uint64_t nsectors;
	size_t sect_size;
};

int ata_poll(struct disk *disk)
{
	struct ata_disk *ata_disk = container_of(disk, struct ata_disk, disk);
	struct ata_chan *chan = ata_disk->chan;
	uint8_t status;

	status = inb(chan->status);

	return (status & (ATA_STAT_BUSY | ATA_STAT_READY)) == ATA_STAT_READY;
}

int ata_stat(struct disk *disk, struct disk_stat *stat)
{
	struct ata_disk *ata_disk = container_of(disk, struct ata_disk, disk);

	if (!stat) {
		return -1;
	}

	stat->nsectors = ata_disk->nsectors;
	stat->sect_size = ata_disk->sect_size;

	return 0;
}

static int ata_wait_ready(struct ata_chan *chan, int check_err)
{
	uint8_t status;

	do {
		status = inb(chan->status);
	} while ((status & (ATA_STAT_BUSY | ATA_STAT_READY)) != ATA_STAT_READY);

	if (check_err && (status & (ATA_STAT_DF | ATA_STAT_ERR)) != 0) {
		return -1;
	}

	return 0;
}

int64_t ata_read(struct disk *disk, void *buf, size_t count, uint64_t addr)
{
	struct ata_disk *ata_disk = container_of(disk, struct ata_disk, disk);
	struct ata_chan *chan = ata_disk->chan;

	/* Select the right drive. */
	outb(chan->drive, 0xa0 | (ata_disk->drive << 4));

	if (ata_disk->state == ATA_IDLE) {
		/* Wait for it to become ready. */
		ata_wait_ready(chan, 0);

		/* The disk is currently idle, issue a read request. */
		outb(chan->sector_count, count);
		outb(chan->lba_lo, addr & 0xff);
		outb(chan->lba_mid, (addr >> 8) & 0xff);
		outb(chan->lba_hi, (addr >> 16) & 0xff);
		outb(chan->drive, 0xe0 | ((ata_disk->drive) << 4) |
			((addr >> 24) & 0xf));	
		outb(chan->cmd, ATA_CMD_READ);

		ata_disk->state = ATA_READ;

		return -EAGAIN;
	}

	if (ata_disk->state != ATA_READ) {
		/* The disk is currently not available. */
		return -EAGAIN;
	}

	if (!ata_poll(disk)) {
		/* The disk is not ready yet. */
		return -EAGAIN;
	}

	insl(chan->data, buf, ata_disk->sect_size / 4);
	ata_disk->state = ATA_IDLE;

	return count * ata_disk->sect_size;
}

int64_t ata_write(struct disk *disk, const void *buf, size_t count, uint64_t addr)
{
	struct ata_disk *ata_disk = container_of(disk, struct ata_disk, disk);
	struct ata_chan *chan = ata_disk->chan;

	/* Select the right drive. */
	outb(chan->drive, 0xa0 | (ata_disk->drive << 4));

	if (ata_disk->state == ATA_IDLE) {
		/* Wait for it to become ready. */
		ata_wait_ready(chan, 0);

		/* The disk is currently idle, issue a write request. */
		outb(chan->sector_count, count);
		outb(chan->lba_lo, addr & 0xff);
		outb(chan->lba_mid, (addr >> 8) & 0xff);
		outb(chan->lba_hi, (addr >> 16) & 0xff);
		outb(chan->drive, 0xe0 | ((ata_disk->drive) << 4) |
			((addr >> 24) & 0xf));
		outb(chan->cmd, ATA_CMD_WRITE);

		ata_disk->state = ATA_WRITE;

		return -EAGAIN;
	}

	if (ata_disk->state != ATA_WRITE) {
		/* The disk is currently not available. */
		return -EAGAIN;
	}

	if (!ata_poll(disk)) {
		/* The disk is not ready yet. */
		return -EAGAIN;
	}
		/* Select the right drive. */
		outb(chan->drive, 0xa0 | (ata_disk->drive << 4));

		/* Wait for it to become ready. */
		ata_wait_ready(chan, 0);


	outsl(chan->data, buf, count * ata_disk->sect_size / 4);
	ata_disk->state = ATA_IDLE;

	/* Write completed, now do a cache flush to prevent temporary bad
	 * sectors.
	 */
	ata_wait_ready(chan, 0);
	outb(chan->cmd, ATA_CMD_FLUSH);
	ata_wait_ready(chan, 0);

	return count * ata_disk->sect_size;
}

struct disk_ops ata_ops = {
	.poll = ata_poll,
	.stat = ata_stat,
	.read = ata_read,
	.write = ata_write,
};

void ata_enum_device(struct ata_chan *chan, size_t chan_id, size_t drive)
{
	uint32_t buf[128];
	struct ata_ident_dev *ident;
	struct ata_disk *ata_disk;
	size_t i;
	int err = 0, drq = 0;
	int type = ATA_DEV_ATA;
	uint16_t lba;
	uint8_t status;

	/* Reset the controller. */
	ata_reset(chan);
	ata_wait_400ns(chan);

	/* Select the drive. */
	outb(chan->drive, 0xa0 | (drive << 4));

	/* Reset LBA and sector count. */
	outb(chan->sector_count, 0);
	outb(chan->lba_lo, 0);
	outb(chan->lba_mid, 0);
	outb(chan->lba_hi, 0);

	/* Send the identify command. */
	outb(chan->cmd, ATA_CMD_IDENT);

	/* Device does not exist. */
	if (inb(chan->status) == 0) {
		return;
	}

	lba  = inb(chan->lba_lo);
	lba |= inb(chan->lba_hi) << 8; 

	/* Poll until DRQ or ERR is set. */
	while (!drq && !err) {
		drq = inb(chan->status) & ATA_STAT_DRQ;
		err = inb(chan->status) & ATA_STAT_ERR;
	}

	if (err) {
		/* Check if it is an ATAPI device. */
		if (lba == 0xeb14 || lba == 0x9669) {
			/* Send the identify command. */
			type = ATA_DEV_ATAPI;
			outb(chan->cmd, ATA_CMD_IDENT_PCKT);
			ata_wait_400ns(chan);
		} else {
			return;
		}
	}

	/* Read the identification space of the device. */
	insl(chan->data, buf, 128);

	ident = (struct ata_ident_dev *)buf;

	for (i = 0; i < 40; i += 2) {
		uint8_t c;

		c = ident->model[i];
		ident->model[i] = ident->model[i + 1];
		ident->model[i + 1] = c;
	}

	cprintf("ATA: channel %u drive %u size %u MiB\n",
		chan_id, drive,
		ident->max_lba * 512 / (1024 * 1024));

	if (ndisks >= MAX_DISKS) {
		return;
	}

	ata_disk = kmalloc(sizeof *ata_disk);

	if (!ata_disk) {
		return;
	}

	ata_disk->chan = chan;
	ata_disk->drive = drive;
	ata_disk->state = ATA_IDLE;
	ata_disk->nsectors = ident->max_lba;
	ata_disk->sect_size = 512;
	ata_disk->disk.ops = &ata_ops;

	disks[ndisks++] = &ata_disk->disk;
}

void ata_enum_devices(void)
{
	size_t chan;
	size_t drive;

	for (chan = 0; chan < 2; ++chan) {
		for (drive = 0; drive < 2; ++drive) {
			ata_enum_device(channels + chan, chan, drive);
		}
	}
}

void ata_setup_chan(struct ata_chan *chan, uint16_t base, uint16_t ctrl)
{
	chan->data = base;
	chan->err = base + 1;
	chan->sector_count = base + 2;
	chan->lba_lo = base + 3;
	chan->lba_mid = base + 4;
	chan->lba_hi = base + 5;
	chan->drive = base + 6;
	chan->cmd = base + 7;
	chan->alt_status = ctrl;
}

int ata_probe(struct pci_dev *dev)
{
	uint32_t bar, base, ctrl;

	/* Set up I/O bases for the primary channel. */
	pci_read_bar(&bar, NULL, dev, 0);
	base = (bar & 0xfffffffc) + 0x1f0 * !bar;
	pci_read_bar(&bar, NULL, dev, 1);
	ctrl = (bar & 0xfffffffc) + 0x3f6 * !bar;
	ata_setup_chan(channels + ATA_PRIMARY, base, ctrl);

	/* Set up I/O bases for the secondary channel. */
	pci_read_bar(&bar, NULL, dev, 2);
	base = (bar & 0xfffffffc) + 0x170 * !bar;
	pci_read_bar(&bar, NULL, dev, 3);
	ctrl = (bar & 0xfffffffc) + 0x376 * !bar;
	ata_setup_chan(channels + ATA_SECONDARY, base, ctrl);

	ata_disable_irq();
	ata_enum_devices();

	return 0;
}

struct pci_driver ata_driver = {
	.compat = compat,
	.probe = ata_probe,
};

