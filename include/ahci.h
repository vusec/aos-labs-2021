#pragma once

#include <types.h>

#define HBA_PxCMD_ST  (1 << 0)
#define HBA_PxCMD_FRE (1 << 4)
#define HBA_PxCMD_FR  (1 << 14)
#define HBA_PxCMD_CR  (1 << 15)

#define HBA_PxIS_TFES (1 << 30)

#define HBA_PxSCTL_DET_COMRESET (1 << 0)

struct hba_prdt {
	uint64_t data_base;
	uint32_t _0;
	uint32_t size : 22;
	uint32_t _1 : 9;
	uint32_t i : 1;
} __attribute__ ((packed));

struct hba_cmd_hdr {
	uint8_t cmd_fis_len : 5;
	uint8_t atapi : 1;
	uint8_t write : 1;
	uint8_t prefetchable : 1;
	uint8_t reset : 1;
	uint8_t bist : 1;
	uint8_t clear_busy : 1;
	uint8_t _0 : 1;
	uint8_t pmp : 4;
	uint16_t prdt_len;
	volatile uint32_t prdt_count;
	uint64_t cmd_base;
	uint32_t _1[4];
} __attribute__ ((packed));

struct hba_cmd_tbl {
	uint8_t cmd_fis[64];
	uint8_t atapi_cmd[16];
	uint8_t _0[48];
	struct hba_prdt prdts[1];
} __attribute__ ((packed));

struct hba_port {
	uint64_t cmd_base;
	uint64_t fis_base;
	uint32_t int_stat;
	uint32_t int_en;
	uint32_t cmd;
	uint32_t _0;
	uint32_t task_file_data;
	uint32_t sig;
	uint32_t sata_stat;
	uint32_t sata_ctl;
	uint32_t sata_err;
	uint32_t sata_act;
	uint32_t cmd_issue;
	uint32_t sata_ntfy;
	uint32_t fis_switch;
	uint32_t _1[11];
	uint32_t vendor[4];
} __attribute__ ((packed));

struct hba_mem {
	uint32_t cap;
	uint32_t glob_host_ctl;
	uint32_t int_stat;
	uint32_t pts_impl;
	uint32_t ver;
	uint32_t ccc_ctl;
	uint32_t ccc_pts;
	uint32_t em_loc;
	uint32_t em_ctl;
	uint32_t cap2;
	uint32_t bohc;
	uint8_t _0[0xa0 - 0x2c];
	uint8_t vendor[0x100 - 0xa0];
	struct hba_port ports[1];
} __attribute__ ((packed));

struct fis_reg_h2d {
	uint8_t fis_type;
	uint8_t pm_port : 4;
	uint8_t _0 : 3;
	uint8_t c : 1;
	uint8_t cmd;
	uint8_t feat_lo;
	uint8_t lba0;
	uint8_t lba1;
	uint8_t lba2;
	uint8_t dev;
	uint8_t lba3;
	uint8_t lba4;
	uint8_t lba5;
	uint8_t feat_hi;
	uint16_t count;
	uint8_t icc;
	uint8_t ctrl;
	uint8_t _1[4];
} __attribute__ ((packed));

#define SATA_SIG_ATA   0x00000101
#define SATA_SIG_ATAPI 0xeb140101
#define SATA_SIG_SEMB  0xc33c0101
#define SATA_SIG_PM    0x96690101

enum {
	FIS_TYPE_REG_H2D   = 0x27,
	FIS_TYPE_REG_D2H   = 0x34,
	FIS_TYPE_DMA_ACT   = 0x39,
	FIS_TYPE_DMA_SETUP = 0x41,
	FIS_TYPE_DATA      = 0x46,
	FIS_TYPE_BIST      = 0x58,
	FIS_TYPE_PIO_SETUP = 0x5f,
	FIS_TYPE_DEV_BITS  = 0xa1,
};

enum {
	AHCI_DEV_NULL = 0,
	AHCI_DEV_SATA,
	AHCI_DEV_SEMB,
	AHCI_DEV_PM,
	AHCI_DEV_SATAPI,
};

