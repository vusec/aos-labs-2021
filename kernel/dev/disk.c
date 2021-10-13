#include <types.h>

#include <kernel/dev/disk.h>

struct disk *disks[MAX_DISKS];
size_t ndisks = 0;

int disk_poll(struct disk *disk)
{
	return disk->ops->poll(disk);
}

int disk_stat(struct disk *disk, struct disk_stat *stat)
{
	return disk->ops->stat(disk, stat);
}

int64_t disk_read(struct disk *disk, void *buf, size_t count, uint64_t addr)
{
	return disk->ops->read(disk, buf, count, addr);
}

int64_t disk_write(struct disk *disk, const void *buf, size_t count,
	uint64_t addr)
{
	return disk->ops->write(disk, buf, count, addr);
}

