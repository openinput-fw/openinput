/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <em_device.h>

#include "platform/efm32gg/emu.h"

void emu_init(u8 immediate_switch)
{
	EMU->PWRCTRL =
		(immediate_switch ? EMU_PWRCTRL_IMMEDIATEPWRSWITCH : 0) | EMU_PWRCTRL_REGPWRSEL_DVDD | EMU_PWRCTRL_ANASW_AVDD;
}

void emu_reg_init(float target_voltage)
{
	if (target_voltage < 2300.f || target_voltage >= 3800.f)
		return;

	u8 level = ((target_voltage - 2300.f) / 100.f);

	EMU->R5VCTRL = EMU_R5VCTRL_INPUTMODE_AUTO;

	EMU->R5VOUTLEVEL = level;
}

#if defined(DCDC_PRESENT)
void emu_dcdc_init(float target_voltage, float max_ln_current, float max_lp_current, float max_reverse_current)
{
	if (target_voltage < 1800.f || target_voltage >= 3000.f)
		return;

	if (max_ln_current <= 0.f || max_ln_current > 200.f)
		return;

	if (max_lp_current <= 0.f || max_lp_current > 10000.f)
		return;

	if (max_reverse_current < 0.f || max_reverse_current > 160.f)
		return;

	/* Low Power & Low Noise current limit */
	u8 lp_bias = 0;

	if (max_lp_current < 75.f)
		lp_bias = 0;
	else if (max_lp_current < 500.f)
		lp_bias = 1;
	else if (max_lp_current < 2500.f)
		lp_bias = 2;
	else
		lp_bias = 3;

	EMU->DCDCMISCCTRL = (EMU->DCDCMISCCTRL & ~_EMU_DCDCMISCCTRL_LPCMPBIASEM234H_MASK) |
			    ((u32) lp_bias << _EMU_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT);
	EMU->DCDCMISCCTRL |= EMU_DCDCMISCCTRL_LNFORCECCM; /* Force CCM to prevent reverse current */
	EMU->DCDCLPCTRL |= EMU_DCDCLPCTRL_LPVREFDUTYEN; /* Enable duty cycling of the bias for LP mode */
	EMU->DCDCLNFREQCTRL = (EMU->DCDCLNFREQCTRL & ~_EMU_DCDCLNFREQCTRL_RCOBAND_MASK) | 4; /* Set RCO Band to 7MHz */

	u8 fet_count = 0;

	if (max_ln_current < 20.f)
		fet_count = 4;
	else if (max_ln_current >= 20.f && max_ln_current < 40.f)
		fet_count = 8;
	else
		fet_count = 16;

	EMU->DCDCMISCCTRL = (EMU->DCDCMISCCTRL & ~_EMU_DCDCMISCCTRL_NFETCNT_MASK) |
			    ((u32) (fet_count - 1) << _EMU_DCDCMISCCTRL_NFETCNT_SHIFT);
	EMU->DCDCMISCCTRL = (EMU->DCDCMISCCTRL & ~_EMU_DCDCMISCCTRL_PFETCNT_MASK) |
			    ((u32) (fet_count - 1) << _EMU_DCDCMISCCTRL_PFETCNT_SHIFT);

	u8 ln_current_limit = (((max_ln_current + 40.f) * 1.5f) / (5.f * fet_count)) - 1;
	u8 lp_current_limit = 1; /* Recommended value */

	EMU->DCDCMISCCTRL =
		(EMU->DCDCMISCCTRL & ~(_EMU_DCDCMISCCTRL_LNCLIMILIMSEL_MASK | _EMU_DCDCMISCCTRL_LPCLIMILIMSEL_MASK)) |
		((u32) ln_current_limit << _EMU_DCDCMISCCTRL_LNCLIMILIMSEL_SHIFT) |
		((u32) lp_current_limit << _EMU_DCDCMISCCTRL_LPCLIMILIMSEL_SHIFT);

	u8 z_det_limit = ((max_reverse_current + 40.f) * 1.5f) / (2.5f * fet_count);

	EMU->DCDCZDETCTRL = (EMU->DCDCZDETCTRL & ~_EMU_DCDCZDETCTRL_ZDETILIMSEL_MASK) |
			    ((u32) z_det_limit << _EMU_DCDCZDETCTRL_ZDETILIMSEL_SHIFT);

	EMU->DCDCCLIMCTRL |=
		EMU_DCDCCLIMCTRL_BYPLIMEN; /* Enable bypass current limiter to prevent overcurrent when switching modes */

	/* Output Voltage */
	if (target_voltage > 1800.f) {
		float max_vout = 3000.f;
		float min_vout = 1800.f;
		float diff_vout = max_vout - min_vout;

		u8 ln_vref_high = (DEVINFO->DCDCLNVCTRL0 & _DEVINFO_DCDCLNVCTRL0_3V0LNATT1_MASK) >>
				  _DEVINFO_DCDCLNVCTRL0_3V0LNATT1_SHIFT;
		u8 ln_vref_low = (DEVINFO->DCDCLNVCTRL0 & _DEVINFO_DCDCLNVCTRL0_1V8LNATT1_MASK) >>
				 _DEVINFO_DCDCLNVCTRL0_1V8LNATT1_SHIFT;

		u8 ln_vref = ((target_voltage - min_vout) * (float) (ln_vref_high - ln_vref_low)) / diff_vout;
		ln_vref += ln_vref_low;

		EMU->DCDCLNVCTRL = (ln_vref << _EMU_DCDCLNVCTRL_LNVREF_SHIFT) | EMU_DCDCLNVCTRL_LNATT;

		u8 lp_vref_low = 0;
		u8 lp_vref_high = 0;

		switch (lp_bias) {
			case 0: {
				lp_vref_high = (DEVINFO->DCDCLPVCTRL2 & _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS0_MASK) >>
					       _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS0_SHIFT;
				lp_vref_low = (DEVINFO->DCDCLPVCTRL2 & _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS0_MASK) >>
					      _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS0_SHIFT;
			} break;
			case 1: {
				lp_vref_high = (DEVINFO->DCDCLPVCTRL2 & _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS1_MASK) >>
					       _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS1_SHIFT;
				lp_vref_low = (DEVINFO->DCDCLPVCTRL2 & _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS1_MASK) >>
					      _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS1_SHIFT;
			} break;
			case 2: {
				lp_vref_high = (DEVINFO->DCDCLPVCTRL3 & _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS2_MASK) >>
					       _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS2_SHIFT;
				lp_vref_low = (DEVINFO->DCDCLPVCTRL3 & _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS2_MASK) >>
					      _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS2_SHIFT;
			} break;
			case 3: {
				lp_vref_high = (DEVINFO->DCDCLPVCTRL3 & _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS3_MASK) >>
					       _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS3_SHIFT;
				lp_vref_low = (DEVINFO->DCDCLPVCTRL3 & _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS3_MASK) >>
					      _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS3_SHIFT;
			} break;
		}

		u8 lp_vref = ((target_voltage - min_vout) * (float) (lp_vref_high - lp_vref_low)) / diff_vout;
		lp_vref += lp_vref_low;

		EMU->DCDCLPVCTRL = (lp_vref << _EMU_DCDCLPVCTRL_LPVREF_SHIFT) | EMU_DCDCLPVCTRL_LPATT;
	} else {
		float max_vout = 1800.f;
		float min_vout = 1200.f;
		float diff_vout = max_vout - min_vout;

		u8 ln_vref_high = (DEVINFO->DCDCLNVCTRL0 & _DEVINFO_DCDCLNVCTRL0_1V8LNATT0_MASK) >>
				  _DEVINFO_DCDCLNVCTRL0_1V8LNATT0_SHIFT;
		u8 ln_vref_low = (DEVINFO->DCDCLNVCTRL0 & _DEVINFO_DCDCLNVCTRL0_1V2LNATT0_MASK) >>
				 _DEVINFO_DCDCLNVCTRL0_1V2LNATT0_SHIFT;

		u8 ln_vref = ((target_voltage - min_vout) * (float) (ln_vref_high - ln_vref_low)) / diff_vout;
		ln_vref += ln_vref_low;

		EMU->DCDCLNVCTRL = ln_vref << _EMU_DCDCLNVCTRL_LNVREF_SHIFT;

		u8 lp_vref_low = 0;
		u8 lp_vref_high = 0;

		switch (lp_bias) {
			case 0: {
				lp_vref_high = (DEVINFO->DCDCLPVCTRL0 & _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS0_MASK) >>
					       _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS0_SHIFT;
				lp_vref_low = (DEVINFO->DCDCLPVCTRL0 & _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS0_MASK) >>
					      _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS0_SHIFT;
			} break;
			case 1: {
				lp_vref_high = (DEVINFO->DCDCLPVCTRL0 & _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS1_MASK) >>
					       _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS1_SHIFT;
				lp_vref_low = (DEVINFO->DCDCLPVCTRL0 & _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS1_MASK) >>
					      _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS1_SHIFT;
			} break;
			case 2: {
				lp_vref_high = (DEVINFO->DCDCLPVCTRL1 & _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS2_MASK) >>
					       _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS2_SHIFT;
				lp_vref_low = (DEVINFO->DCDCLPVCTRL1 & _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS2_MASK) >>
					      _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS2_SHIFT;
			} break;
			case 3: {
				lp_vref_high = (DEVINFO->DCDCLPVCTRL1 & _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS3_MASK) >>
					       _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS3_SHIFT;
				lp_vref_low = (DEVINFO->DCDCLPVCTRL1 & _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS3_MASK) >>
					      _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS3_SHIFT;
			} break;
		}

		u8 lp_vref = ((target_voltage - min_vout) * (float) (lp_vref_high - lp_vref_low)) / diff_vout;
		lp_vref += lp_vref_low;

		EMU->DCDCLPVCTRL = lp_vref << _EMU_DCDCLPVCTRL_LPVREF_SHIFT;
	}

	EMU->DCDCLPCTRL = (EMU->DCDCLPCTRL & ~_EMU_DCDCLPCTRL_LPCMPHYSSELEM234H_MASK) |
			  (((DEVINFO->DCDCLPCMPHYSSEL1 & (((u32) 0xFF) << (lp_bias * 8))) >> (lp_bias * 8))
			   << _EMU_DCDCLPCTRL_LPCMPHYSSELEM234H_SHIFT);

	while (EMU->DCDCSYNC & EMU_DCDCSYNC_DCDCCTRLBUSY) continue; /* Wait for configuration to write */

	/* Calibration */
	//EMU->DCDCLNCOMPCTRL = 0x57204077; /* Compensation for 1uF DCDC capacitor */
	EMU->DCDCLNCOMPCTRL = 0xB7102137; /* Compensation for 4.7uF DCDC capacitor */

	/* Enable DCDC converter */
	EMU->DCDCCTRL =
		EMU_DCDCCTRL_DCDCMODEEM4_EM4LOWPOWER | EMU_DCDCCTRL_DCDCMODEEM23_EM23LOWPOWER | EMU_DCDCCTRL_DCDCMODE_LOWNOISE;

	/* Switch digital domain to DVDD */
	EMU->PWRCTRL = EMU_PWRCTRL_REGPWRSEL_DVDD | EMU_PWRCTRL_ANASW_AVDD;
}
#endif
