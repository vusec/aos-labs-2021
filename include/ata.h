#pragma once

#include <types.h>

struct ata_chan {
	uint16_t data;
	uint16_t err;
	uint16_t sector_count;

	union {
		uint16_t sector;
		uint16_t lba_lo;
	};

	union {
		uint16_t cyl_lo;
		uint16_t lba_mid;
	};

	union {
		uint16_t cyl_hi;
		uint16_t lba_hi;
	};

	union {
		uint16_t drive;
		uint16_t head;
	};

	union {
		uint16_t cmd;
		uint16_t status;
	};

	union {
		uint16_t ctrl;
		uint16_t alt_status;
	};
};

#define ATA_DATA     0x0
#define ATA_ERR      0x1
#define ATA_FEATURES 0x1
#define ATA_NSECT0   0x2
#define ATA_LBA0     0x3
#define ATA_LBA1     0x4
#define ATA_LBA2     0x5
#define ATA_DEV_SEL  0x6
#define ATA_CMD      0x7
#define ATA_STAT     0x7
#define ATA_NSECT1   0x8
#define ATA_LBA3     0x9
#define ATA_LBA4     0xa
#define ATA_LBA5     0xb
#define ATA_CTRL     0xc

/* Control register */
#define ATA_ALT_STAT 0
#define ATA_DEV_ADDR 1

#define ATA_STAT_ERR     (1 << 0)
#define ATA_STAT_IDX     (1 << 1)
#define ATA_STAT_CORR    (1 << 2)
#define ATA_STAT_DRQ     (1 << 3)
#define ATA_STAT_DSC     (1 << 4)
#define ATA_STAT_DF      (1 << 5)
#define ATA_STAT_READY   (1 << 6)
#define ATA_STAT_BUSY    (1 << 7)

#define ATA_CMD_READ         0x20
#define ATA_CMD_READ_DMA_EX  0x25
#define ATA_CMD_WRITE        0x30
#define ATA_CMD_WRITE_DMA_EX 0x35
#define ATA_CMD_PCKT         0xa0
#define ATA_CMD_IDENT_PCKT   0xa1
#define ATA_CMD_FLUSH        0xe7
#define ATA_CMD_IDENT        0xec

struct ata_ident_dev {
	uint16_t dev_type;
	uint16_t cylinders;
	uint16_t specific_conf;
	uint16_t heads;
	uint16_t _0[2];
	uint16_t sect_per_track;
	uint16_t _1[3];
	uint8_t serial[20];
	uint16_t _2[3];
	uint8_t firmware_rev[8];
	uint8_t model[40];
	uint8_t max_block_xfer;
	uint8_t _3;
	uint16_t trusted_computing;
	uint32_t caps;
	uint16_t _4[2];
	uint16_t _5;
	uint16_t cur_cylinders;
	uint16_t cur_heads;
	uint16_t cur_sect_per_track;
	uint32_t cur_sector_capacity;
	uint8_t cur_multisector_setting;
	uint8_t _6;
	uint32_t max_lba;
} __attribute__ ((packed));

#define ATA_PRIMARY   0
#define ATA_SECONDARY 1

enum ata_type {
	ATA_DEV_NULL = 0,
	ATA_DEV_ATA,
	ATA_DEV_ATAPI,
};

