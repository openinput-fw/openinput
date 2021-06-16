/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <em_device.h>

#include "platform/efm32gg/cmu.h"

#define LFXO_CTUNE_TO_PF(x)  (8.f + (x) *0.25f)
#define HFXO_CTUNE_TO_PF(x)  (8.7f + (x) *0.084f)
#define HFXO_IBTRIM_TO_UA(x) (1280 * (((x) &0x600) >> 9) + 2 * (((x) &0x0FF) >> 0))

static struct cmu_clock_tree_t cmu_clock_tree;

void cmu_config_waitstates(u32 frequency)
{
	if (frequency <= 32000000)
		CMU->CTRL &= ~CMU_CTRL_WSHFLE;
	else
		CMU->CTRL |= CMU_CTRL_WSHFLE;
}

void cmu_hfrco_calib(u32 calibration, u32 target_frequency)
{
	if (CMU->STATUS & CMU_STATUS_DPLLENS)
		return;

	while (CMU->SYNCBUSY & CMU_SYNCBUSY_HFRCOBSY) continue;

	CMU->HFRCOCTRL = calibration;

	while (CMU->SYNCBUSY & CMU_SYNCBUSY_HFRCOBSY) continue;

	cmu_clock_tree.hfrco_freq = target_frequency;
}

void cmu_ushfrco_calib(u8 enable, u32 calibration, u32 target_frequency)
{
	if (CMU->USBCRCTRL & CMU_USBCRCTRL_USBCREN)
		return;

	if (!enable) {
		CMU->OSCENCMD = CMU_OSCENCMD_USHFRCODIS;
		while (CMU->STATUS & CMU_STATUS_USHFRCOENS) continue;

		return;
	}

	while (CMU->SYNCBUSY & CMU_SYNCBUSY_USHFRCOBSY) continue;

	CMU->USHFRCOCTRL = calibration;

	while (CMU->SYNCBUSY & CMU_SYNCBUSY_USHFRCOBSY) continue;

	if (enable && !(CMU->STATUS & CMU_STATUS_USHFRCOENS)) {
		CMU->OSCENCMD = CMU_OSCENCMD_USHFRCOEN;

		while (!(CMU->STATUS & CMU_STATUS_USHFRCORDY)) continue;
	}

	cmu_clock_tree.ushfrco_freq = target_frequency;
}

void cmu_auxhfrco_calib(uint8_t enable, u32 calibration, u32 target_frequency)
{
	if (!enable) {
		CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCODIS;
		while (CMU->STATUS & CMU_STATUS_AUXHFRCOENS) continue;

		return;
	}

	while (CMU->SYNCBUSY & CMU_SYNCBUSY_AUXHFRCOBSY) continue;

	CMU->AUXHFRCOCTRL = calibration;

	while (CMU->SYNCBUSY & CMU_SYNCBUSY_AUXHFRCOBSY) continue;

	if (enable && !(CMU->STATUS & CMU_STATUS_AUXHFRCOENS)) {
		CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;

		while (!(CMU->STATUS & CMU_STATUS_AUXHFRCORDY)) continue;
	}

	cmu_clock_tree.auxhfrco_freq = target_frequency;
}

void cmu_hfxo_startup_calib(u16 ib_trim, u16 c_tune)
{
	if (CMU->STATUS & CMU_STATUS_HFXOENS)
		return;

	CMU->HFXOSTARTUPCTRL =
		(CMU->HFXOSTARTUPCTRL & ~(_CMU_HFXOSTARTUPCTRL_CTUNE_MASK | _CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_MASK)) |
		(((uint32_t) c_tune << _CMU_HFXOSTARTUPCTRL_CTUNE_SHIFT) & _CMU_HFXOSTARTUPCTRL_CTUNE_MASK) |
		(((uint32_t) ib_trim << _CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_SHIFT) & _CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_MASK);
}

void cmu_hfxo_steady_calib(u16 ib_trim, u16 c_tune)
{
	if (CMU->STATUS & CMU_STATUS_HFXOENS)
		return;

	CMU->HFXOSTEADYSTATECTRL =
		(CMU->HFXOSTEADYSTATECTRL &
		 ~(_CMU_HFXOSTEADYSTATECTRL_CTUNE_MASK | _CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_MASK)) |
		(((uint32_t) c_tune << _CMU_HFXOSTEADYSTATECTRL_CTUNE_SHIFT) & _CMU_HFXOSTEADYSTATECTRL_CTUNE_MASK) |
		(((uint32_t) ib_trim << _CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_SHIFT) &
		 _CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_MASK);
}

void cmu_lfxo_calib(uint8_t c_tune)
{
	if (CMU->STATUS & CMU_STATUS_LFXOENS)
		return;

	float c_load = LFXO_CTUNE_TO_PF(c_tune) / 2.f;
	uint8_t gain = 0;

	if (c_load <= 6.f)
		gain = 0;
	else if (c_load <= 8.f)
		gain = 1;
	else if (c_load <= 12.5f)
		gain = 2;
	else if (c_load <= 18.f)
		gain = 3;
	else
		return;

	CMU->LFXOCTRL = (CMU->LFXOCTRL & ~(_CMU_LFXOCTRL_GAIN_MASK | _CMU_LFXOCTRL_TUNING_MASK)) |
			((uint32_t) gain << _CMU_LFXOCTRL_GAIN_SHIFT) |
			(((uint32_t) c_tune << _CMU_LFXOCTRL_TUNING_SHIFT) & _CMU_LFXOCTRL_TUNING_MASK);
}

void cmu_init(u32 external_clock_value)
{
	/* clock tree default clocks */
	cmu_clock_tree.hfrco_freq = 19000000UL;
	cmu_clock_tree.ushfrco_freq = 48000000UL;
	cmu_clock_tree.auxhfrco_freq = 19000000UL;
	cmu_clock_tree.lfrco_freq = 32768UL;
	cmu_clock_tree.ulfrco_freq = 1000UL;

	/* clock tree external clocks */
	cmu_clock_tree.hfxo_freq = external_clock_value;
	cmu_clock_tree.lfxo_freq = 0;

	/* Disable DPLL if enabled */
	if (CMU->STATUS & CMU_STATUS_DPLLENS) {
		CMU->OSCENCMD = CMU_OSCENCMD_DPLLDIS;
		while (CMU->STATUS & CMU_STATUS_DPLLENS) continue;
	}

	/* Disable HFXO if enabled */
	if (CMU->STATUS & CMU_STATUS_HFXOENS) {
		CMU->OSCENCMD = CMU_OSCENCMD_HFXODIS;
		while (CMU->STATUS & CMU_STATUS_HFXOENS) continue;
	}

	/* Setup HFXO */
	CMU->HFXOCTRL = CMU_HFXOCTRL_PEAKDETMODE_AUTOCMD | CMU_HFXOCTRL_MODE_XTAL;
	CMU->HFXOCTRL1 = CMU_HFXOCTRL1_PEAKDETTHR_DEFAULT;
	CMU->HFXOSTEADYSTATECTRL |= CMU_HFXOSTEADYSTATECTRL_PEAKMONEN;
	CMU->HFXOTIMEOUTCTRL = (7 << _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_SHIFT) |
			       (8 << _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_SHIFT) |
			       (12 << _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_SHIFT);

	/* Enable HFXO and wait for it to be ready */
	CMU->OSCENCMD = CMU_OSCENCMD_HFXOEN;
	while (!(CMU->STATUS & CMU_STATUS_HFXORDY)) continue;

	/* Switch main clock to HFXO and wait for it to be selected */
	CMU->HFCLKSEL = CMU_HFCLKSEL_HF_HFXO;
	while ((CMU->HFCLKSTATUS & _CMU_HFCLKSTATUS_SELECTED_MASK) != CMU_HFCLKSTATUS_SELECTED_HFXO) continue;

	/* Calibrate HFRCO for 72MHz and enable tunning by PLL */
	cmu_hfrco_calib(HFRCO_CALIB_72M | CMU_HFRCOCTRL_FINETUNINGEN, 72000000);

	/* Setup the PLL */
	u32 pllfactor = (u32) ((72000000.0f / (float) external_clock_value) * 100.0f);

	CMU->DPLLCTRL =
		CMU_DPLLCTRL_REFSEL_HFXO | CMU_DPLLCTRL_AUTORECOVER | CMU_DPLLCTRL_EDGESEL_RISE | CMU_DPLLCTRL_MODE_FREQLL;
	CMU->DPLLCTRL1 = ((pllfactor - 1) << _CMU_DPLLCTRL1_N_SHIFT) |
			 (99 << _CMU_DPLLCTRL1_M_SHIFT); /* fHFRCO = fHFXO * (N + 1) / (M + 1) */

	/* Enable the DPLL and wait for it to be ready */
	CMU->OSCENCMD = CMU_OSCENCMD_DPLLEN;
	while (!(CMU->STATUS & CMU_STATUS_DPLLRDY)) continue;

	// Config peripherals for the new frequency */
	cmu_config_waitstates(36000000);
	//msc_config_waitstates(72000000);

	/* Set prescalers */
	CMU->HFPRESC = CMU_HFPRESC_HFCLKLEPRESC_DIV2 | CMU_HFPRESC_PRESC_NODIVISION;
	CMU->HFBUSPRESC = 1 << _CMU_HFBUSPRESC_PRESC_SHIFT;
	CMU->HFCOREPRESC = 0 << _CMU_HFCOREPRESC_PRESC_SHIFT;
	CMU->HFPERPRESC = 1 << _CMU_HFPERPRESC_PRESC_SHIFT;
	CMU->HFEXPPRESC = 0 << _CMU_HFEXPPRESC_PRESC_SHIFT;
	CMU->HFPERPRESCB = 0 << _CMU_HFPERPRESCB_PRESC_SHIFT;
	CMU->HFPERPRESCC = 1 << _CMU_HFPERPRESCC_PRESC_SHIFT;

	/* Enable clock to peripherals */
	CMU->CTRL |= CMU_CTRL_HFPERCLKEN;

	/* Switch main clock to HFRCO and wait for it to be selected */
	CMU->HFCLKSEL = CMU_HFCLKSEL_HF_HFRCO;
	while ((CMU->HFCLKSTATUS & _CMU_HFCLKSTATUS_SELECTED_MASK) != CMU_HFCLKSTATUS_SELECTED_HFRCO) continue;

	/* LFA Clock */
	CMU->LFACLKSEL = CMU_LFACLKSEL_LFA_LFRCO;

	/* LFB Clock */
	CMU->LFBCLKSEL = CMU_LFBCLKSEL_LFB_LFRCO;

	/* LFC Clock */
	CMU->LFCCLKSEL = CMU_LFCCLKSEL_LFC_LFRCO;

	/* LFE Clock */
	CMU->LFECLKSEL = CMU_LFECLKSEL_LFE_ULFRCO;
}

void cmu_update_clock_tree()
{
	if ((CMU->STATUS & CMU_STATUS_DPLLRDY) == CMU_STATUS_DPLLRDY) {
		float pll_mul = (float) (((CMU->DPLLCTRL1 & _CMU_DPLLCTRL1_N_MASK) >> _CMU_DPLLCTRL1_N_SHIFT) + 1) /
				(((CMU->DPLLCTRL1 & _CMU_DPLLCTRL1_M_MASK) >> _CMU_DPLLCTRL1_M_SHIFT) + 1);

		switch (CMU->DPLLCTRL & _CMU_DPLLCTRL_REFSEL_MASK) {
			case CMU_DPLLCTRL_REFSEL_HFXO:
				cmu_clock_tree.hfrco_freq = cmu_clock_tree.hfxo_freq * pll_mul;
				break;
			case CMU_DPLLCTRL_REFSEL_LFXO:
				cmu_clock_tree.hfrco_freq = cmu_clock_tree.lfxo_freq * pll_mul;
				break;
			case CMU_DPLLCTRL_REFSEL_USHFRCO:
				cmu_clock_tree.hfrco_freq = cmu_clock_tree.ushfrco_freq * pll_mul;
				break;
			case CMU_DPLLCTRL_REFSEL_CLKIN0:
				cmu_clock_tree.hfrco_freq = 0 * pll_mul; /* TODO: Support external clock */
				break;
		}
	}

	cmu_clock_tree.aux_freq = cmu_clock_tree.auxhfrco_freq;

	switch (CMU->HFCLKSTATUS & _CMU_HFCLKSTATUS_SELECTED_MASK) {
		case CMU_HFCLKSTATUS_SELECTED_HFRCO:
			cmu_clock_tree.hfsrc_freq = cmu_clock_tree.hfrco_freq;
			break;
		case CMU_HFCLKSTATUS_SELECTED_HFXO:
			cmu_clock_tree.hfsrc_freq = cmu_clock_tree.hfxo_freq;
			break;
		case CMU_HFCLKSTATUS_SELECTED_LFRCO:
			cmu_clock_tree.hfsrc_freq = cmu_clock_tree.lfrco_freq;
			break;
		case CMU_HFCLKSTATUS_SELECTED_LFXO:
			cmu_clock_tree.hfsrc_freq = cmu_clock_tree.lfxo_freq;
			break;
		case CMU_HFCLKSTATUS_SELECTED_HFRCODIV2:
			cmu_clock_tree.hfsrc_freq = cmu_clock_tree.hfrco_freq >> 1;
			break;
		case CMU_HFCLKSTATUS_SELECTED_USHFRCO:
			cmu_clock_tree.hfsrc_freq = cmu_clock_tree.ushfrco_freq;
			break;
		case CMU_HFCLKSTATUS_SELECTED_CLKIN0:
			cmu_clock_tree.hfsrc_freq = 0; /* TODO: Support external clock */
			break;
	}

	cmu_clock_tree.hf_freq =
		cmu_clock_tree.hfsrc_freq / (((CMU->HFPRESC & _CMU_HFPRESC_PRESC_MASK) >> _CMU_HFPRESC_PRESC_SHIFT) + 1);

	switch (CMU->HFPRESC & _CMU_HFPRESC_HFCLKLEPRESC_MASK) {
		case CMU_HFPRESC_HFCLKLEPRESC_DIV2:
			cmu_clock_tree.hfle_freq = cmu_clock_tree.hf_freq >> 1;
			break;
		case CMU_HFPRESC_HFCLKLEPRESC_DIV4:
			cmu_clock_tree.hfle_freq = cmu_clock_tree.hf_freq >> 2;
			break;
		case CMU_HFPRESC_HFCLKLEPRESC_DIV8:
			cmu_clock_tree.hfle_freq = cmu_clock_tree.hf_freq >> 3;
			break;
	}

	cmu_clock_tree.hfbus_freq =
		cmu_clock_tree.hf_freq / (((CMU->HFBUSPRESC & _CMU_HFBUSPRESC_PRESC_MASK) >> _CMU_HFBUSPRESC_PRESC_SHIFT) + 1);
	cmu_clock_tree.hfcore_freq = cmu_clock_tree.hf_freq /
				     (((CMU->HFCOREPRESC & _CMU_HFCOREPRESC_PRESC_MASK) >> _CMU_HFCOREPRESC_PRESC_SHIFT) + 1);
	cmu_clock_tree.hfexp_freq =
		cmu_clock_tree.hf_freq / (((CMU->HFEXPPRESC & _CMU_HFEXPPRESC_PRESC_MASK) >> _CMU_HFEXPPRESC_PRESC_SHIFT) + 1);
	cmu_clock_tree.hfper_freq =
		cmu_clock_tree.hf_freq / (((CMU->HFPERPRESC & _CMU_HFPERPRESC_PRESC_MASK) >> _CMU_HFPERPRESC_PRESC_SHIFT) + 1);
	cmu_clock_tree.hfperb_freq = cmu_clock_tree.hf_freq /
				     (((CMU->HFPERPRESCB & _CMU_HFPERPRESCB_PRESC_MASK) >> _CMU_HFPERPRESCB_PRESC_SHIFT) + 1);
	cmu_clock_tree.hfperc_freq = cmu_clock_tree.hf_freq /
				     (((CMU->HFPERPRESCC & _CMU_HFPERPRESCC_PRESC_MASK) >> _CMU_HFPERPRESCC_PRESC_SHIFT) + 1);

	switch (CMU->SDIOCTRL & _CMU_SDIOCTRL_SDIOCLKSEL_MASK) {
		case CMU_SDIOCTRL_SDIOCLKSEL_HFRCO:
			cmu_clock_tree.sdio_freq = cmu_clock_tree.hfrco_freq;
			break;
		case CMU_SDIOCTRL_SDIOCLKSEL_HFXO:
			cmu_clock_tree.sdio_freq = cmu_clock_tree.hfxo_freq;
			break;
		case CMU_SDIOCTRL_SDIOCLKSEL_AUXHFRCO:
			cmu_clock_tree.sdio_freq = cmu_clock_tree.aux_freq;
			break;
		case CMU_SDIOCTRL_SDIOCLKSEL_USHFRCO:
			cmu_clock_tree.sdio_freq = cmu_clock_tree.ushfrco_freq;
			break;
	}

	switch (CMU->QSPICTRL & _CMU_QSPICTRL_QSPI0CLKSEL_MASK) {
		case CMU_QSPICTRL_QSPI0CLKSEL_HFRCO:
			cmu_clock_tree.qspi_freq = cmu_clock_tree.hfrco_freq;
			break;
		case CMU_QSPICTRL_QSPI0CLKSEL_HFXO:
			cmu_clock_tree.qspi_freq = cmu_clock_tree.hfxo_freq;
			break;
		case CMU_QSPICTRL_QSPI0CLKSEL_AUXHFRCO:
			cmu_clock_tree.qspi_freq = cmu_clock_tree.aux_freq;
			break;
		case CMU_QSPICTRL_QSPI0CLKSEL_USHFRCO:
			cmu_clock_tree.qspi_freq = cmu_clock_tree.ushfrco_freq;
			break;
	}

	switch (CMU->USBCTRL & _CMU_USBCTRL_USBCLKSEL_MASK) {
		case CMU_USBCTRL_USBCLKSEL_USHFRCO:
			cmu_clock_tree.usb_freq = cmu_clock_tree.ushfrco_freq;
			break;
		case CMU_USBCTRL_USBCLKSEL_HFXO:
			cmu_clock_tree.usb_freq = cmu_clock_tree.hfxo_freq;
			break;
		case CMU_USBCTRL_USBCLKSEL_HFXOX2: {
			if (CMU->HFXOCTRL & CMU_HFXOCTRL_HFXOX2EN)
				cmu_clock_tree.usb_freq = cmu_clock_tree.hfxo_freq << 1;
			else
				cmu_clock_tree.usb_freq = cmu_clock_tree.hfxo_freq;
		} break;
		case CMU_USBCTRL_USBCLKSEL_HFRCO:
			cmu_clock_tree.usb_freq = cmu_clock_tree.hfrco_freq;
			break;
		case CMU_USBCTRL_USBCLKSEL_LFXO:
			cmu_clock_tree.usb_freq = cmu_clock_tree.lfxo_freq;
			break;
		case CMU_USBCTRL_USBCLKSEL_LFRCO:
			cmu_clock_tree.usb_freq = cmu_clock_tree.lfrco_freq;
			break;
	}

	switch (CMU->DBGCLKSEL & _CMU_DBGCLKSEL_DBG_MASK) {
		case CMU_DBGCLKSEL_DBG_AUXHFRCO:
			cmu_clock_tree.dbg_freq = cmu_clock_tree.aux_freq;
			break;
		case CMU_DBGCLKSEL_DBG_HFCLK:
			cmu_clock_tree.dbg_freq = cmu_clock_tree.hf_freq;
			break;
		case CMU_DBGCLKSEL_DBG_HFRCODIV2:
			cmu_clock_tree.dbg_freq = cmu_clock_tree.hfrco_freq >> 1;
			break;
	}

	switch (CMU->ADCCTRL & _CMU_ADCCTRL_ADC0CLKSEL_MASK) {
		case CMU_ADCCTRL_ADC0CLKSEL_DISABLED:
			cmu_clock_tree.adc0_freq = 0;
			break;
		case CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO:
			cmu_clock_tree.adc0_freq = cmu_clock_tree.aux_freq;
			break;
		case CMU_ADCCTRL_ADC0CLKSEL_HFXO:
			cmu_clock_tree.adc0_freq = cmu_clock_tree.hfxo_freq;
			break;
		case CMU_ADCCTRL_ADC0CLKSEL_HFSRCCLK:
			cmu_clock_tree.adc0_freq = cmu_clock_tree.hfsrc_freq;
			break;
	}

	cmu_clock_tree.adc0_freq /= (((CMU->ADCCTRL & _CMU_ADCCTRL_ADC0CLKDIV_MASK) >> _CMU_ADCCTRL_ADC0CLKDIV_SHIFT) + 1);

	switch (CMU->ADCCTRL & _CMU_ADCCTRL_ADC1CLKSEL_MASK) {
		case CMU_ADCCTRL_ADC1CLKSEL_DISABLED:
			cmu_clock_tree.adc1_freq = 0;
			break;
		case CMU_ADCCTRL_ADC1CLKSEL_AUXHFRCO:
			cmu_clock_tree.adc1_freq = cmu_clock_tree.aux_freq;
			break;
		case CMU_ADCCTRL_ADC1CLKSEL_HFXO:
			cmu_clock_tree.adc1_freq = cmu_clock_tree.hfxo_freq;
			break;
		case CMU_ADCCTRL_ADC1CLKSEL_HFSRCCLK:
			cmu_clock_tree.adc1_freq = cmu_clock_tree.hfsrc_freq;
			break;
	}

	cmu_clock_tree.adc1_freq /= (((CMU->ADCCTRL & _CMU_ADCCTRL_ADC1CLKDIV_MASK) >> _CMU_ADCCTRL_ADC1CLKDIV_SHIFT) + 1);

	switch (CMU->LFACLKSEL & _CMU_LFACLKSEL_LFA_MASK) {
		case CMU_LFACLKSEL_LFA_DISABLED:
			cmu_clock_tree.lfa_freq = 0;
			break;
		case CMU_LFACLKSEL_LFA_LFRCO:
			cmu_clock_tree.lfa_freq = cmu_clock_tree.lfrco_freq;
			break;
		case CMU_LFACLKSEL_LFA_LFXO:
			cmu_clock_tree.lfa_freq = cmu_clock_tree.lfxo_freq;
			break;
		case CMU_LFACLKSEL_LFA_ULFRCO:
			cmu_clock_tree.lfa_freq = cmu_clock_tree.ulfrco_freq;
			break;
	}

	cmu_clock_tree.lesense_freq = cmu_clock_tree.lfa_freq
				      << ((CMU->LFAPRESC0 & _CMU_LFAPRESC0_LESENSE_MASK) >> _CMU_LFAPRESC0_LESENSE_SHIFT);
	cmu_clock_tree.rtc_freq = cmu_clock_tree.lfa_freq
				  << ((CMU->LFAPRESC0 & _CMU_LFAPRESC0_RTC_MASK) >> _CMU_LFAPRESC0_RTC_SHIFT);
	cmu_clock_tree.lcd_freq = cmu_clock_tree.lfa_freq
				  << ((CMU->LFAPRESC0 & _CMU_LFAPRESC0_LCD_MASK) >> _CMU_LFAPRESC0_LCD_SHIFT);
	cmu_clock_tree.letimer0_freq = cmu_clock_tree.lfa_freq
				       << ((CMU->LFAPRESC0 & _CMU_LFAPRESC0_LETIMER0_MASK) >> _CMU_LFAPRESC0_LETIMER0_SHIFT);
	cmu_clock_tree.letimer1_freq = cmu_clock_tree.lfa_freq
				       << ((CMU->LFAPRESC0 & _CMU_LFAPRESC0_LETIMER1_MASK) >> _CMU_LFAPRESC0_LETIMER1_SHIFT);

	switch (CMU->LFBCLKSEL & _CMU_LFBCLKSEL_LFB_MASK) {
		case CMU_LFBCLKSEL_LFB_DISABLED:
			cmu_clock_tree.lfb_freq = 0;
			break;
		case CMU_LFBCLKSEL_LFB_LFRCO:
			cmu_clock_tree.lfb_freq = cmu_clock_tree.lfrco_freq;
			break;
		case CMU_LFBCLKSEL_LFB_LFXO:
			cmu_clock_tree.lfb_freq = cmu_clock_tree.lfxo_freq;
			break;
		case CMU_LFBCLKSEL_LFB_HFCLKLE:
			cmu_clock_tree.lfb_freq = cmu_clock_tree.hfle_freq;
			break;
		case CMU_LFBCLKSEL_LFB_ULFRCO:
			cmu_clock_tree.lfb_freq = cmu_clock_tree.ulfrco_freq;
			break;
	}

	cmu_clock_tree.leuart0_freq = cmu_clock_tree.lfb_freq
				      << ((CMU->LFBPRESC0 & _CMU_LFBPRESC0_LEUART0_MASK) >> _CMU_LFBPRESC0_LEUART0_SHIFT);
	cmu_clock_tree.leuart1_freq = cmu_clock_tree.lfb_freq
				      << ((CMU->LFBPRESC0 & _CMU_LFBPRESC0_LEUART1_MASK) >> _CMU_LFBPRESC0_LEUART1_SHIFT);
	cmu_clock_tree.systick_freq = cmu_clock_tree.lfb_freq
				      << ((CMU->LFBPRESC0 & _CMU_LFBPRESC0_SYSTICK_MASK) >> _CMU_LFBPRESC0_SYSTICK_SHIFT);
	cmu_clock_tree.csen_freq = cmu_clock_tree.lfb_freq
				   << (((CMU->LFBPRESC0 & _CMU_LFBPRESC0_CSEN_MASK) >> _CMU_LFBPRESC0_CSEN_SHIFT) + 4);

	switch (CMU->LFCCLKSEL & _CMU_LFCCLKSEL_LFC_MASK) {
		case CMU_LFCCLKSEL_LFC_DISABLED:
			cmu_clock_tree.lfc_freq = 0;
			break;
		case CMU_LFCCLKSEL_LFC_LFRCO:
			cmu_clock_tree.lfc_freq = cmu_clock_tree.lfrco_freq;
			break;
		case CMU_LFCCLKSEL_LFC_LFXO:
			cmu_clock_tree.lfc_freq = cmu_clock_tree.lfxo_freq;
			break;
		case CMU_LFCCLKSEL_LFC_ULFRCO:
			cmu_clock_tree.lfc_freq = cmu_clock_tree.ulfrco_freq;
			break;
	}

	switch (CMU->LFECLKSEL & _CMU_LFECLKSEL_LFE_MASK) {
		case CMU_LFECLKSEL_LFE_DISABLED:
			cmu_clock_tree.lfe_freq = 0;
			break;
		case CMU_LFECLKSEL_LFE_LFRCO:
			cmu_clock_tree.lfe_freq = cmu_clock_tree.lfrco_freq;
			break;
		case CMU_LFECLKSEL_LFE_LFXO:
			cmu_clock_tree.lfe_freq = cmu_clock_tree.lfxo_freq;
			break;
		case CMU_LFECLKSEL_LFE_ULFRCO:
			cmu_clock_tree.lfe_freq = cmu_clock_tree.ulfrco_freq;
			break;
	}

	cmu_clock_tree.rtcc_freq = cmu_clock_tree.lfe_freq
				   << ((CMU->LFEPRESC0 & _CMU_LFEPRESC0_RTCC_MASK) >> _CMU_LFEPRESC0_RTCC_SHIFT);
}

struct cmu_clock_tree_t cmu_get_clock_tree()
{
	return cmu_clock_tree;
}
