/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "util/data.h"

#include "platform/sam3u/eefc.h"
#include "platform/sam3u/pmc.h"

static struct pmc_clock_tree_t pmc_clock_tree;

void pmc_init(u32 hfxo_freq, u32 lfxo_freq)
{
	if (hfxo_freq != 12000000UL) // this is a preset for 12MHz Crystal
		return;

	pmc_clock_tree.hfxo_freq = hfxo_freq;
	pmc_clock_tree.lfxo_freq = lfxo_freq;

	/* Configure flash waitstates */
	eefc_config_waitstates(96000000);

	/* 12MHz external xtal, maximum possible startup time */
	pmc_hfxo_enable(false, 0xFF);

	/* Select as main oscillator */
	pmc_mainck_source_set(PMC_MAINCK_HFXO);
	pmc_mck_source_set(PMC_MCK_MAINCK, 1);

	/* Multiply by 8 for 96MHz */
	pmc_pllack_enable(8, 1);
	pmc_upllck_enable();

	/* Select as MCK source */
	pmc_mck_source_set(PMC_MCK_PLLACK, 1);
}

void pmc_update_clock_tree()
{
	pmc_clock_tree.lfrco_freq = CHIP_FREQ_SLCK_RC;

	switch (SUPC->SUPC_SR & SUPC_SR_OSCSEL) {
		case SUPC_SR_OSCSEL_RC:
			pmc_clock_tree.slck_freq = pmc_clock_tree.lfrco_freq;
			break;
		case SUPC_SR_OSCSEL_CRYST:
			pmc_clock_tree.slck_freq = pmc_clock_tree.lfxo_freq;
			break;
	}

	switch (PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk) {
		case CKGR_MOR_MOSCRCF_4_MHz:
			pmc_clock_tree.hfrco_freq = CHIP_FREQ_MAINCK_RC_4MHZ;
			break;
		case CKGR_MOR_MOSCRCF_8_MHz:
			pmc_clock_tree.hfrco_freq = CHIP_FREQ_MAINCK_RC_8MHZ;
			break;
		case CKGR_MOR_MOSCRCF_12_MHz:
			pmc_clock_tree.hfrco_freq = CHIP_FREQ_MAINCK_RC_12MHZ;
			break;
	}

	switch (PMC->CKGR_MOR & CKGR_MOR_MOSCSEL) {
		case 0:
			pmc_clock_tree.mainck_freq = pmc_clock_tree.hfrco_freq;
			break;
		case CKGR_MOR_MOSCSEL:
			pmc_clock_tree.mainck_freq = pmc_clock_tree.hfxo_freq;
			break;
	}

	float plla_div = (PMC->CKGR_PLLAR & CKGR_PLLAR_DIVA_Msk) >> CKGR_PLLAR_DIVA_Pos;
	float plla_mul = (PMC->CKGR_PLLAR & CKGR_PLLAR_MULA_Msk) >> CKGR_PLLAR_MULA_Pos;

	if (plla_div == 0 || plla_mul == 0)
		pmc_clock_tree.pllack_freq = 0;
	else
		pmc_clock_tree.pllack_freq = (uint32_t) (((float) pmc_clock_tree.mainck_freq / plla_div) * (plla_mul + 1.f));

	if (!(PMC->CKGR_UCKR & CKGR_UCKR_UPLLEN))
		pmc_clock_tree.upllck_freq = 0;
	else
		pmc_clock_tree.upllck_freq = CHIP_FREQ_UTMIPLL;

	switch (PMC->PMC_MCKR & PMC_MCKR_CSS_Msk) {
		case PMC_MCKR_CSS_SLOW_CLK:
			pmc_clock_tree.mck_freq = pmc_clock_tree.slck_freq;
			break;
		case PMC_MCKR_CSS_MAIN_CLK:
			pmc_clock_tree.mck_freq = pmc_clock_tree.mainck_freq;
			break;
		case PMC_MCKR_CSS_UPLL_CLK:
			pmc_clock_tree.mck_freq = pmc_clock_tree.upllck_freq / 2;
			break;
		case PMC_MCKR_CSS_PLLA_CLK:
			pmc_clock_tree.mck_freq = pmc_clock_tree.pllack_freq;
			break;
	}

	switch (PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) {
		case PMC_MCKR_PRES_CLK_1:
			pmc_clock_tree.mck_freq >>= 0;
			break;
		case PMC_MCKR_PRES_CLK_2:
			pmc_clock_tree.mck_freq >>= 1;
			break;
		case PMC_MCKR_PRES_CLK_4:
			pmc_clock_tree.mck_freq >>= 2;
			break;
		case PMC_MCKR_PRES_CLK_8:
			pmc_clock_tree.mck_freq >>= 3;
			break;
		case PMC_MCKR_PRES_CLK_16:
			pmc_clock_tree.mck_freq >>= 4;
			break;
		case PMC_MCKR_PRES_CLK_32:
			pmc_clock_tree.mck_freq >>= 5;
			break;
		case PMC_MCKR_PRES_CLK_64:
			pmc_clock_tree.mck_freq >>= 6;
			break;
		case PMC_MCKR_PRES_CLK_3:
			pmc_clock_tree.mck_freq /= 3;
			break;
	}

	pmc_clock_tree.fclk_freq = pmc_clock_tree.mck_freq;
}

const struct pmc_clock_tree_t *pmc_get_clock_tree()
{
	return (const struct pmc_clock_tree_t *) &pmc_clock_tree;
}

void pmc_hfxo_enable(bool bypass, u8 startup_time)
{
	if (bypass) {
		PMC->CKGR_MOR = (PMC->CKGR_MOR & ~(CKGR_MOR_MOSCXTEN)) | CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTBY;
	} else {
		PMC->CKGR_MOR = (PMC->CKGR_MOR & ~(CKGR_MOR_MOSCXTST_Msk | CKGR_MOR_MOSCXTBY)) | CKGR_MOR_KEY_PASSWD |
				CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCXTST(startup_time);
	}

	while (!(PMC->PMC_SR & PMC_SR_MOSCXTS)) continue;
}

void pmc_hfxo_disable()
{
	PMC->CKGR_MOR = (PMC->CKGR_MOR & ~(CKGR_MOR_MOSCXTBY | CKGR_MOR_MOSCXTEN)) | CKGR_MOR_KEY_PASSWD;
}

void pmc_hfrco_enable(enum pmc_hfrco_freq_t freq)
{
	PMC->CKGR_MOR |= (CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN);

	while (!(PMC->PMC_SR & PMC_SR_MOSCRCS)) continue;

	PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk) | CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCF(freq);

	while (!(PMC->PMC_SR & PMC_SR_MOSCRCS)) continue;
}

void pmc_hfrco_disable()
{
	PMC->CKGR_MOR = (PMC->CKGR_MOR & ~(CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCRCF_Msk)) | CKGR_MOR_KEY_PASSWD;
}

void pmc_pllack_enable(u8 mul, u8 div)
{
	pmc_pllack_disable();

	if (!mul || !div)
		mul = div = 1;

	PMC->CKGR_PLLAR = CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(mul - 1) | CKGR_PLLAR_DIVA(div) | CKGR_PLLAR_PLLACOUNT(63);

	while (!(PMC->PMC_SR & PMC_SR_LOCKA)) continue;
}

void pmc_pllack_disable()
{
	PMC->CKGR_PLLAR = CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(0) | CKGR_PLLAR_DIVA(0);
}

void pmc_upllck_enable()
{
	pmc_upllck_disable();

	PMC->CKGR_UCKR = CKGR_UCKR_UPLLCOUNT(255) | CKGR_UCKR_UPLLEN;

	while (!(PMC->PMC_SR & PMC_SR_LOCKU)) continue;
}

void pmc_upllck_disable()
{
	PMC->CKGR_UCKR &= ~CKGR_UCKR_UPLLEN;
}

void pmc_slck_source_set(enum pmc_slck_src_t src)
{
	if (src == PMC_SCLK_LFXO_BYPASS)
		SUPC->SUPC_MR |= (SUPC_MR_KEY_PASSWD | SUPC_MR_OSCBYPASS);
	else
		SUPC->SUPC_MR = (SUPC->SUPC_MR & ~SUPC_MR_OSCBYPASS) | SUPC_MR_KEY_PASSWD;

	if (src == PMC_SCLK_LFRCO)
		SUPC->SUPC_CR = (SUPC->SUPC_CR & ~SUPC_CR_XTALSEL) | SUPC_CR_KEY_PASSWD;
	else
		SUPC->SUPC_CR |= (SUPC_CR_KEY_PASSWD | SUPC_CR_XTALSEL);
}

void pmc_mainck_source_set(enum pmc_mainck_src_t src)
{
	u32 ckgr_mor_val = 0;

	if (src == PMC_MAINCK_HFXO)
		ckgr_mor_val = CKGR_MOR_MOSCSEL;

	PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCSEL) | CKGR_MOR_KEY_PASSWD | ckgr_mor_val;
}

void pmc_mck_source_set(enum pmc_mck_src_t src, u8 prescaler)
{
	if (!prescaler)
		prescaler = 1;

	u32 pmc_mckr_val = 0;
	if (src == PMC_MCK_UPLLCK2)
		pmc_mckr_val = PMC_MCKR_UPLLDIV2;

	PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(PMC_MCKR_PRES_Msk | PMC_MCKR_CSS_Msk)) | PMC_MCKR_CSS(src) |
			PMC_MCKR_PRES(prescaler - 1) | pmc_mckr_val;

	while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) continue;
}

void pmc_peripheral_clock_enable(u8 pid)
{
	if (pid > 31)
		return;

	PMC->PMC_PCER0 = 1 << pid;
}

void pmc_peripheral_clock_disable(u8 pid)
{
	if (pid > 31)
		return;

	PMC->PMC_PCDR0 = 1 << pid;
}
