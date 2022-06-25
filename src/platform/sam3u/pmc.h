/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "sam.h"

#include <stdbool.h>

#include "util/types.h"

struct pmc_clock_tree_t {
	u32 hfxo_freq;
	u32 hfrco_freq;
	u32 lfxo_freq;
	u32 lfrco_freq;
	u32 slck_freq;
	u32 mainck_freq;
	u32 pllack_freq;
	u32 upllck_freq;
	u32 mck_freq;
	u32 fclk_freq;
};

enum pmc_hfrco_freq_t {
	PMC_HFRCO_4MHZ = 0x0u,
	PMC_HFRCO_8MHZ = 0x1u,
	PMC_HFRCO_12MHZ = 0x2u,
};

enum pmc_slck_src_t {
	PMC_SCLK_LFRCO = 0,
	PMC_SCLK_LFXO = 1,
	PMC_SCLK_LFXO_BYPASS = 2,
};

enum pmc_mainck_src_t {
	PMC_MAINCK_HFRCO = 0,
	PMC_MAINCK_HFXO = 1,
};

enum pmc_mck_src_t {
	PMC_MCK_SLCK = 0x0u,
	PMC_MCK_MAINCK = 0x1u,
	PMC_MCK_PLLACK = 0x2u,
	PMC_MCK_UPLLCK2 = 0x3u,
};

void pmc_init(u32 hfxo_freq, u32 lfxo_freq);

void pmc_update_clock_tree();
const struct pmc_clock_tree_t *pmc_get_clock_tree();

void pmc_hfxo_enable(bool bypass, u8 startup_time);
void pmc_hfxo_disable();

void pmc_hfrco_enable(enum pmc_hfrco_freq_t freq);
void pmc_hfrco_disable();

void pmc_pllack_enable(u8 mul, u8 div);
void pmc_pllack_disable();

void pmc_upllck_enable();
void pmc_upllck_disable();

void pmc_slck_source_set(enum pmc_slck_src_t src);
void pmc_mainck_source_set(enum pmc_mainck_src_t src);
void pmc_mck_source_set(enum pmc_mck_src_t src, u8 prescaler);

void pmc_peripheral_clock_enable(u8 pid);
void pmc_peripheral_clock_disable(u8 pid);
