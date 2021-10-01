#pragma once

typedef union {
	uint64_t val;
	struct {
		uint64_t rev_id : 8;
		uint64_t num_tim_cap : 5;
		uint64_t count_size_cap : 1;
		uint64_t _0 : 1;
		uint64_t leg_rt_cap : 1;
		uint64_t vendor_id : 16;
		uint64_t cnt_clk_period : 32;
	} __attribute__ ((packed));
} hpet_caps_id_t;

typedef union {
	uint64_t val;
	struct {
		uint64_t enable_cnf : 1;
		uint64_t leg_rt_cnf : 1;
		uint64_t _0 : 62;
	};
} hpet_cfg_t;

struct hpet_regs {
	hpet_caps_id_t caps;
	uint64_t _0;
	hpet_cfg_t cfg;
	uint64_t _1;
	uint64_t isr;
	uint64_t _2[25];
	uint64_t main_cnt_val;
	uint64_t _3;
};

