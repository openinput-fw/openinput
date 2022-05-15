/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "util/data.h"

#include "platform/samx7x/eefc.h"
#include "platform/samx7x/pmc.h"

static struct pmc_clock_tree_t pmc_clock_tree;

static inline u32 pmc_us_to_moscxtst(u32 startup_us, u32 slowck_freq)
{
	u32 moscxtst = (startup_us * slowck_freq / 8 / 1000000);
	return (moscxtst < 0x100 ? moscxtst : 0xFF);
}

static u32 pmc_osc_is_ready_32kxtal()
{
	return ((SUPC->SUPC_SR & SUPC_SR_OSCSEL_Msk) && (PMC->PMC_SR & PMC_SR_OSCSELS_Msk));
}

static u32 pmc_osc_is_ready_mainck()
{
	return (PMC->PMC_SR & PMC_SR_MOSCSELS_Msk);
}

static void pmc_osc_disable_fastrc(void)
{
	/* Disable Fast RC oscillator */
	PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCRCEN_Msk & ~CKGR_MOR_MOSCRCF_Msk) | CKGR_MOR_KEY_PASSWD;
}

static void pmc_osc_disable_xtal(u32 bypass)
{
	/* Disable xtal oscillator */
	if (bypass) {
		PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCXTBY_Msk) | CKGR_MOR_KEY_PASSWD;
	} else {
		PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCXTEN_Msk) | CKGR_MOR_KEY_PASSWD;
	}
}

void pmc_switch_sclk_to_32kxtal(u8 bypass)
{
	/* Set Bypass mode if required */
	if (bypass) {
		SUPC->SUPC_MR |= SUPC_MR_KEY_PASSWD | SUPC_MR_OSCBYPASS_Msk;
	}

	SUPC->SUPC_CR = SUPC_CR_KEY_PASSWD | SUPC_CR_XTALSEL_Msk;
}

void pmc_switch_mainck_to_fastrc(u32 moscrcf)
{
	/* Enable Fast RC oscillator but DO NOT switch to RC now */
	PMC->CKGR_MOR |= (CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN_Msk);

	/* Wait the Fast RC to stabilize */
	while (!(PMC->PMC_SR & PMC_SR_MOSCRCS_Msk)) continue;

	/* Change Fast RC oscillator frequency */
	PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk) | CKGR_MOR_KEY_PASSWD | moscrcf;

	/* Wait the Fast RC to stabilize */
	while (!(PMC->PMC_SR & PMC_SR_MOSCRCS_Msk)) continue;

	/* Switch to Fast RC */
	PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCSEL_Msk) | CKGR_MOR_KEY_PASSWD;
}

void pmc_switch_mainck_to_xtal(u32 bypass, u32 xtal_startup_time)
{
	/* Enable Main Xtal oscillator */
	if (bypass) {
		PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCXTEN_Msk) | CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTBY_Msk |
				CKGR_MOR_MOSCSEL_Msk;
	} else {
		PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCXTBY_Msk) | CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTEN_Msk |
				CKGR_MOR_MOSCXTST(xtal_startup_time);

		/* Wait the Xtal to stabilize */
		while (!(PMC->PMC_SR & PMC_SR_MOSCXTS_Msk)) continue;

		PMC->CKGR_MOR |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCSEL_Msk;
	}
}

void pmc_switch_udpck_to_pllack(u32 div)
{
	if (div) /* 0 is divide by 1*/
		div--;

	PMC->PMC_USB = PMC_USB_USBDIV(div);
}

void pmc_switch_udpck_to_upllck(u32 div)
{
	if (div) /* 0 is divide by 1*/
		div--;

	PMC->PMC_USB = PMC_USB_USBS | PMC_USB_USBDIV(div);
}

void pmc_enable_udpck()
{
	PMC->PMC_SCER = PMC_SCER_USBCLK;
}

void pmc_disable_udpck()
{
	PMC->PMC_SCDR = PMC_SCER_USBCLK;
}

void pmc_osc_enable(enum pmc_osc_t osc)
{
	u32 moscxtst;

	switch (osc) {
		case OSC_SLCK_32K_RC:
			break;
		case OSC_SLCK_32K_XTAL:
			pmc_switch_sclk_to_32kxtal(0);
			break;
		case OSC_SLCK_32K_BYPASS:
			pmc_switch_sclk_to_32kxtal(1);
			break;
		case OSC_MAINCK_4M_RC:
			pmc_switch_mainck_to_fastrc(CKGR_MOR_MOSCRCF_4_MHz);
			break;
		case OSC_MAINCK_8M_RC:
			pmc_switch_mainck_to_fastrc(CKGR_MOR_MOSCRCF_8_MHz);
			break;
		case OSC_MAINCK_12M_RC:
			pmc_switch_mainck_to_fastrc(CKGR_MOR_MOSCRCF_12_MHz);
			break;
		case OSC_MAINCK_XTAL:
			moscxtst = pmc_us_to_moscxtst(PMC_OSC_STARTUP_US, 32000);
			pmc_switch_mainck_to_xtal(0, moscxtst);
			break;
		case OSC_MAINCK_BYPASS:
			pmc_switch_mainck_to_xtal(1, 0);
			break;
	}
}

void pmc_osc_disable(enum pmc_osc_t osc)
{
	switch (osc) {
		case OSC_SLCK_32K_RC:
		case OSC_SLCK_32K_XTAL:
		case OSC_SLCK_32K_BYPASS:
			break;

		case OSC_MAINCK_4M_RC:
		case OSC_MAINCK_8M_RC:
		case OSC_MAINCK_12M_RC:
			pmc_osc_disable_fastrc();
			break;

		case OSC_MAINCK_XTAL:
			pmc_osc_disable_xtal(0);
			break;

		case OSC_MAINCK_BYPASS:
			pmc_osc_disable_xtal(1);
			break;
	}
}

u8 pmc_osc_is_ready(enum pmc_osc_t osc)
{
	switch (osc) {
		case OSC_SLCK_32K_RC:
			return 1;

		case OSC_SLCK_32K_XTAL:
		case OSC_SLCK_32K_BYPASS:
			return pmc_osc_is_ready_32kxtal();

		case OSC_MAINCK_4M_RC:
		case OSC_MAINCK_8M_RC:
		case OSC_MAINCK_12M_RC:
		case OSC_MAINCK_XTAL:
		case OSC_MAINCK_BYPASS:
			return pmc_osc_is_ready_mainck();
	}

	return 0;
}

void pmc_enable_pllack(u32 div, u32 mul)
{
	if (!mul)
		return;

	pmc_disable_pllack(); // Always stop PLL first!
	PMC->CKGR_PLLAR = CKGR_PLLAR_ONE_Msk | CKGR_PLLAR_MULA(mul - 1) | CKGR_PLLAR_DIVA(div) | CKGR_PLLAR_PLLACOUNT(0x3fU);
}

void pmc_disable_pllack()
{
	PMC->CKGR_PLLAR = CKGR_PLLAR_ONE_Msk | CKGR_PLLAR_MULA(0) | CKGR_PLLAR_DIVA(0);
}

u32 pmc_pllack_is_locked()
{
	return (PMC->PMC_SR & PMC_SR_LOCKA_Msk);
}

void pmc_enable_upllck()
{
	PMC->CKGR_UCKR = CKGR_UCKR_UPLLCOUNT(0x3fU) | CKGR_UCKR_UPLLEN_Msk;
}

void pmc_disable_upllck()
{
	PMC->CKGR_UCKR &= ~CKGR_UCKR_UPLLEN_Msk;
}

u32 pmc_upllck_is_locked()
{
	return (PMC->PMC_SR & PMC_SR_LOCKU_Msk);
}

u32 pmc_switch_mck_to_mainck(u32 pres)
{
	u32 timeout;

	PMC->PMC_MCKR = (PMC->PMC_MCKR & (~PMC_MCKR_CSS_Msk)) | PMC_MCKR_CSS_MAIN_CLK;

	for (timeout = PMC_TIMEOUT; !(PMC->PMC_SR & PMC_SR_MCKRDY_Msk); --timeout) {
		if (timeout == 0)
			return 1;
	}

	PMC->PMC_MCKR = (PMC->PMC_MCKR & (~PMC_MCKR_PRES_Msk)) | PMC_MCKR_PRES(pres);
	for (timeout = PMC_TIMEOUT; !(PMC->PMC_SR & PMC_SR_MCKRDY_Msk); --timeout) {
		if (timeout == 0)
			return 1;
	}

	return 0;
}

u32 pmc_switch_mck_to_pllack(u32 pres)
{
	u32 timeout;

	PMC->PMC_MCKR = (PMC->PMC_MCKR & (~PMC_MCKR_PRES_Msk)) | PMC_MCKR_PRES(pres);
	for (timeout = PMC_TIMEOUT; !(PMC->PMC_SR & PMC_SR_MCKRDY_Msk); --timeout) {
		if (timeout == 0)
			return 1;
	}

	PMC->PMC_MCKR = (PMC->PMC_MCKR & (~PMC_MCKR_CSS_Msk)) | PMC_MCKR_CSS_PLLA_CLK;

	for (timeout = PMC_TIMEOUT; !(PMC->PMC_SR & PMC_SR_MCKRDY_Msk); --timeout) {
		if (timeout == 0)
			return 1;
	}

	return 0;
}

u32 pmc_switch_mck_to_upllck(u32 pres)
{
	u32 timeout;

	PMC->PMC_MCKR = (PMC->PMC_MCKR & (~PMC_MCKR_PRES_Msk)) | PMC_MCKR_PRES(pres);
	for (timeout = PMC_TIMEOUT; !(PMC->PMC_SR & PMC_SR_MCKRDY_Msk); --timeout) {
		if (timeout == 0)
			return 1;
	}

	PMC->PMC_MCKR = (PMC->PMC_MCKR & (~PMC_MCKR_CSS_Msk)) | PMC_MCKR_CSS_UPLL_CLK;
	for (timeout = PMC_TIMEOUT; !(PMC->PMC_SR & PMC_SR_MCKRDY_Msk); --timeout) {
		if (timeout == 0)
			return 1;
	}

	return 0;
}

void pmc_upllckdiv_division(u32 div)
{
	switch (div) {
		default:
		case 1:
			PMC->PMC_MCKR &= ~PMC_MCKR_UPLLDIV2_Msk;
			break;
		case 2:
			PMC->PMC_MCKR |= PMC_MCKR_UPLLDIV2_Msk;
			break;
	}
}

void pmc_mck_set_division(u32 div)
{
	switch (div) {
		case 1:
			div = PMC_MCKR_MDIV_EQ_PCK;
			break;
		case 2:
			div = PMC_MCKR_MDIV_PCK_DIV2;
			break;
		case 3:
			div = PMC_MCKR_MDIV_PCK_DIV3;
			break;
		case 4:
			div = PMC_MCKR_MDIV_PCK_DIV4;
			break;
		default:
			div = PMC_MCKR_MDIV_EQ_PCK;
			break;
	}

	PMC->PMC_MCKR = (PMC->PMC_MCKR & (~PMC_MCKR_MDIV_Msk)) | div;
	while (!(PMC->PMC_SR & PMC_SR_MCKRDY_Msk)) continue;
}

void pmc_pck_clock_config(enum pmc_pck_id_t id, u8 enable, u32 source, u16 prescaler)
{
	if (id > PMC_PCK_TC0)
		return;

	if (!enable) {
		PMC->PMC_SCDR = BIT(id + PMC_SCDR_PCK0_Pos);

		while (PMC->PMC_SR & BIT(id + PMC_SR_PCKRDY0_Pos)) continue;

		return;
	}

	PMC->PMC_PCK[id] = PMC_PCK_PRES(prescaler) | (source & PMC_PCK_CSS_Msk);
	PMC->PMC_SCER = BIT(id + PMC_SCER_PCK0_Pos);

	while (!(PMC->PMC_SR & BIT(id + PMC_SR_PCKRDY0_Pos))) continue;
}

u32 pmc_pck_clock_get_freq(enum pmc_pck_id_t id)
{
	if (id > 7)
		return 0;

	if (!(PMC->PMC_SR & BIT(id + 8)))
		return 0;

	u32 ulFreq = 0;

	switch (PMC->PMC_PCK[id] & PMC_PCK_CSS_Msk) {
		case PMC_PCK_CSS_SLOW_CLK:
			ulFreq = pmc_clock_tree.slck_freq;
			break;
		case PMC_PCK_CSS_MAIN_CLK:
			ulFreq = pmc_clock_tree.mainck_freq;
			break;
		case PMC_PCK_CSS_UPLL_CLK:
			ulFreq = pmc_clock_tree.upllck_freq >> !!(PMC->PMC_MCKR & PMC_MCKR_UPLLDIV2_Msk);
			break;
		case PMC_PCK_CSS_PLLA_CLK:
			ulFreq = pmc_clock_tree.pllack_freq;
			break;
		case PMC_PCK_CSS_MCK:
			ulFreq = pmc_clock_tree.mck_freq;
			break;
	}

	return ulFreq / (((PMC->PMC_PCK[id] & PMC_PCK_PRES_Msk) >> PMC_PCK_PRES_Pos) + 1);
}

void pmc_generic_clock_config(u8 id, u8 enable, u32 source, u16 prescaler)
{
	if (id > 127)
		return;

	PMC->PMC_PCR = (id << PMC_PCR_PID_Pos); /* Set peripheral ID first to load current values */

	if (!enable) {
		PMC->PMC_PCR = (PMC->PMC_PCR & ~PMC_PCR_GCLKEN_Msk) | PMC_PCR_CMD_Msk;

		return;
	}

	if (prescaler) /* 0 is divide by 1*/
		prescaler--;

	PMC->PMC_PCR |= PMC_PCR_CMD_Msk;

	PMC->PMC_PCR = (PMC->PMC_PCR & ~(PMC_PCR_GCLKCSS_Msk | PMC_PCR_GCLKDIV_Msk)) | (source & PMC_PCR_GCLKCSS_Msk) |
		       PMC_PCR_GCLKDIV(prescaler) | PMC_PCR_GCLKEN_Msk;
}

u32 pmc_generic_clock_get_freq(u8 id)
{
	if (id > 127)
		return 0;

	PMC->PMC_PCR = (id << PMC_PCR_PID_Pos);

	if (!(PMC->PMC_PCR & PMC_PCR_GCLKEN_Msk))
		return 0;

	u32 ulFreq = 0;

	switch (PMC->PMC_PCR & PMC_PCR_GCLKCSS_Msk) {
		case PMC_PCR_GCLKCSS_SLOW_CLK:
			ulFreq = pmc_clock_tree.slck_freq;
			break;
		case PMC_PCR_GCLKCSS_MAIN_CLK:
			ulFreq = pmc_clock_tree.mainck_freq;
			break;
		case PMC_PCR_GCLKCSS_PLLA_CLK:
			ulFreq = pmc_clock_tree.upllck_freq >> !!(PMC->PMC_MCKR & PMC_MCKR_UPLLDIV2_Msk);
			break;
		case PMC_PCR_GCLKCSS_UPLL_CLK:
			ulFreq = pmc_clock_tree.pllack_freq;
			break;
		case PMC_PCR_GCLKCSS_MCK_CLK:
			ulFreq = pmc_clock_tree.mck_freq;
			break;
	}

	return ulFreq / (((PMC->PMC_PCR & PMC_PCR_GCLKDIV_Msk) >> PMC_PCR_GCLKDIV_Pos) + 1);
}

void pmc_peripheral_clock_gate(u8 id, u8 enable)
{
	if (id > 127)
		return;

	PMC->PMC_PCR = (id << PMC_PCR_PID_Pos); /* Set peripheral ID first to load current values */
	PMC->PMC_PCR = (PMC->PMC_PCR & ~PMC_PCR_EN_Msk) | (enable ? PMC_PCR_EN_Msk : 0) | PMC_PCR_CMD_Msk;
}

void pmc_init(u32 hfxo_freq, u32 lfxo_freq)
{
	pmc_clock_tree.hfxo_freq = hfxo_freq;
	pmc_clock_tree.lfxo_freq = lfxo_freq;

	pmc_clock_tree.hfrco_freq = 12000000UL;
	pmc_clock_tree.lfrco_freq = 32000UL;

	/* Configure flash waitstates */
	eefc_set_waitstates(150000000);

	pmc_osc_enable(OSC_MAINCK_XTAL);

	while (!(PMC->PMC_SR & PMC_SR_MOSCSELS_Msk)) continue;

	/* 300 MHz */
	pmc_enable_pllack(1, (300000000UL / pmc_clock_tree.hfxo_freq));

	while (!pmc_pllack_is_locked()) continue;

	pmc_mck_set_division(1);

	/* pres 1 = div by 2, 150MHz */
	pmc_switch_mck_to_pllack(1);
}

void pmc_init_usb()
{
	if (pmc_clock_tree.hfxo_freq == 12000000UL)
		UTMI->UTMI_CKTRIM = UTMI_CKTRIM_FREQ_XTAL12;
	else if (pmc_clock_tree.hfxo_freq == 16000000UL)
		UTMI->UTMI_CKTRIM = UTMI_CKTRIM_FREQ_XTAL16;
	else
		return;

	/* UPLLCK = 480MHz, UPLLCKDIV = 240MHz */
	pmc_upllckdiv_division(2);

	pmc_enable_upllck();

	while (!pmc_upllck_is_locked()) continue;

	/* USB_48M = UPLLCKDIV/5 = 48MHz */
	pmc_switch_udpck_to_upllck(5);

	pmc_enable_udpck();
}

void pmc_update_clock_tree()
{
	switch (SUPC->SUPC_SR & SUPC_SR_OSCSEL_Msk) {
		case SUPC_SR_OSCSEL_RC:
			pmc_clock_tree.slck_freq = pmc_clock_tree.lfrco_freq;
			break;
		case SUPC_SR_OSCSEL_CRYST:
			pmc_clock_tree.slck_freq = pmc_clock_tree.lfxo_freq;
			break;
	}

	switch (PMC->CKGR_MOR & CKGR_MOR_MOSCSEL_Msk) {
		case 0:
			pmc_clock_tree.mainck_freq = pmc_clock_tree.hfrco_freq;
			break;
		case CKGR_MOR_MOSCSEL_Msk:
			pmc_clock_tree.mainck_freq = pmc_clock_tree.hfxo_freq;
			break;
	}

	float fPLLADiv = (PMC->CKGR_PLLAR & CKGR_PLLAR_DIVA_Msk) >> CKGR_PLLAR_DIVA_Pos;
	float fPLLAMul = (PMC->CKGR_PLLAR & CKGR_PLLAR_MULA_Msk) >> CKGR_PLLAR_MULA_Pos;

	if (fPLLADiv == 0 || fPLLAMul == 0)
		pmc_clock_tree.pllack_freq = 0;
	else
		pmc_clock_tree.pllack_freq = ((float) pmc_clock_tree.mainck_freq / fPLLADiv) * (fPLLAMul + 1.f);

	if (!(PMC->CKGR_UCKR & CKGR_UCKR_UPLLEN_Msk))
		pmc_clock_tree.upllck_freq = 0;
	else
		pmc_clock_tree.upllck_freq = 480000000UL;

	switch (PMC->PMC_MCKR & PMC_MCKR_CSS_Msk) {
		case PMC_MCKR_CSS_SLOW_CLK:
			pmc_clock_tree.fclk_freq = pmc_clock_tree.slck_freq;
			break;
		case PMC_MCKR_CSS_MAIN_CLK:
			pmc_clock_tree.fclk_freq = pmc_clock_tree.mainck_freq;
			break;
		case PMC_MCKR_CSS_UPLL_CLK:
			pmc_clock_tree.fclk_freq = pmc_clock_tree.upllck_freq;
			break;
		case PMC_MCKR_CSS_PLLA_CLK:
			pmc_clock_tree.fclk_freq = pmc_clock_tree.pllack_freq;
			break;
	}

	switch (PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) {
		case PMC_MCKR_PRES_CLK_1:
			pmc_clock_tree.fclk_freq >>= 0;
			break;
		case PMC_MCKR_PRES_CLK_2:
			pmc_clock_tree.fclk_freq >>= 1;
			break;
		case PMC_MCKR_PRES_CLK_4:
			pmc_clock_tree.fclk_freq >>= 2;
			break;
		case PMC_MCKR_PRES_CLK_8:
			pmc_clock_tree.fclk_freq >>= 3;
			break;
		case PMC_MCKR_PRES_CLK_16:
			pmc_clock_tree.fclk_freq >>= 4;
			break;
		case PMC_MCKR_PRES_CLK_32:
			pmc_clock_tree.fclk_freq >>= 5;
			break;
		case PMC_MCKR_PRES_CLK_3:
			pmc_clock_tree.fclk_freq /= 3;
			break;
	}

	pmc_clock_tree.mck_freq = pmc_clock_tree.fclk_freq;

	switch (PMC->PMC_MCKR & PMC_MCKR_MDIV_Msk) {
		case PMC_MCKR_MDIV_EQ_PCK:
			pmc_clock_tree.mck_freq >>= 0;
			break;
		case PMC_MCKR_MDIV_PCK_DIV2:
			pmc_clock_tree.mck_freq >>= 1;
			break;
		case PMC_MCKR_MDIV_PCK_DIV4:
			pmc_clock_tree.mck_freq >>= 2;
			break;
		case PMC_MCKR_MDIV_PCK_DIV3:
			pmc_clock_tree.mck_freq /= 3;
			break;
	}
}

struct pmc_clock_tree_t pmc_get_clock_tree()
{
	return pmc_clock_tree;
}
