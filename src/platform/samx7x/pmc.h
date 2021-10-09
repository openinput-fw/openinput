/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

#define PMC_OSC_STARTUP_US 15625UL
#define PMC_TIMEOUT	   4096

struct pmc_clock_tree_t {
	u32 hfxo_freq;
	u32 hfrco_freq;
	u32 lfxo_freq;
	u32 lfrco_freq;
	u32 mainck_freq;
	u32 slck_freq;
	u32 pllack_freq;
	u32 upllck_freq;
	u32 mck_freq;
	u32 fclk_freq;
};

enum pmc_osc_t {
	OSC_SLCK_32K_RC = 0,
	OSC_SLCK_32K_XTAL = 1,
	OSC_SLCK_32K_BYPASS = 2,
	OSC_MAINCK_4M_RC = 3,
	OSC_MAINCK_8M_RC = 4,
	OSC_MAINCK_12M_RC = 5,
	OSC_MAINCK_XTAL = 6,
	OSC_MAINCK_BYPASS = 7,
};

enum pmc_pck_id_t {
	PMC_PCK0 = 0,
	PMC_PCK1 = 1,
	PMC_PCK2 = 2,
	PMC_PCK_ETM = 3,
	PMC_PCK_UART = 4,
	PMC_PCK_MCAN = 5,
	PMC_PCK_TC = 6,
	PMC_PCK_TC0 = 7,
};

/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "util/data.h"

#include "platform/samx7x/eefc.h"
#include "platform/samx7x/pmc.h"

static struct pmc_clock_tree_t pmc_clock_tree;

void pmc_switch_sclk_to_32kxtal(u8 bypass);
void pmc_switch_mainck_to_fastrc(u32 moscrcf);
void pmc_switch_mainck_to_xtal(u32 bypass, u32 xtal_startup_time);

void pmc_switch_udpck_to_pllack(u32 div);
void pmc_switch_udpck_to_upllck(u32 div);
void pmc_enable_udpck();
void pmc_disable_udpck();

void pmc_osc_enable(enum pmc_osc_t osc);
void pmc_osc_disable(enum pmc_osc_t osc);
u8 pmc_osc_is_ready(enum pmc_osc_t osc);

void pmc_enable_pllack(u32 div, u32 mul);
void pmc_disable_pllack();
u32 pmc_pllack_is_locked();

void pmc_enable_upllck();
void pmc_disable_upllck();
u32 pmc_upllck_is_locked();

u32 pmc_switch_mck_to_mainck(u32 pres);
u32 pmc_switch_mck_to_pllack(u32 pres);
u32 pmc_switch_mck_to_upllck(u32 pres);

void pmc_upllckdiv_division(u32 div);
void pmc_mck_set_division(u32 div);

void pmc_pck_clock_config(enum pmc_pck_id_t id, u8 enable, u32 source, u16 prescaler);
u32 pmc_pck_clock_get_freq(enum pmc_pck_id_t id);
void pmc_generic_clock_config(u8 id, u8 enable, u32 source, u16 prescaler);
u32 pmc_generic_clock_get_freq(u8 id);
void pmc_peripheral_clock_gate(u8 id, u8 enable);

void pmc_init(u32 hfxo_freq, u32 lfxo_freq);
void pmc_init_usb();
void pmc_update_clock_tree();
struct pmc_clock_tree_t pmc_get_clock_tree();
