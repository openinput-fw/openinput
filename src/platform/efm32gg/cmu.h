/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

/* Oscillator calibration register values */
/*
 *  It is not confirmed on all devices, but most likely, that all calibration values have CLKDIV set to 0.
 *  It is then easy to divide and get frequencies that are not calibrated on the DI.
 *  It is not confirmed on all devices, but most likely, that the FREQRANGE of all oscillators is the same:
 *  FREQRANGE = 0   FINETUNINGEN = 0     1 — 10 MHz
 *  FREQRANGE = 3   FINETUNINGEN = 0     2 — 17 MHz
 *  FREQRANGE = 6   FINETUNINGEN = 0     4 — 30 MHz
 *  FREQRANGE = 7   FINETUNINGEN = 0     5 — 34 MHz
 *  FREQRANGE = 8   FINETUNINGEN = 0     7 — 42 MHz
 *  FREQRANGE = 10  FINETUNINGEN = 0    12 — 58 MHz
 *  FREQRANGE = 11  FINETUNINGEN = 0    15 — 68 MHz
 *  FREQRANGE = 12  FINETUNINGEN = 0    18 — 83 MHz
 *  FREQRANGE = 13  FINETUNINGEN = 0    24 — 100 MHz
 *  FREQRANGE = 14  FINETUNINGEN = 0    28 — 119 MHz
 *  FREQRANGE = 15  FINETUNINGEN = 0    33 — 138 MHz
 *  FREQRANGE = 16  FINETUNINGEN = 0    43 — 163 MHz
*/
#define HFRCO_CALIB_1M	(DEVINFO->HFRCOCAL0 | CMU_HFRCOCTRL_CLKDIV_DIV4)
#define HFRCO_CALIB_2M	(DEVINFO->HFRCOCAL0 | CMU_HFRCOCTRL_CLKDIV_DIV2)
#define HFRCO_CALIB_4M	(DEVINFO->HFRCOCAL0)
#define HFRCO_CALIB_7M	(DEVINFO->HFRCOCAL3)
#define HFRCO_CALIB_8M	(DEVINFO->HFRCOCAL7 | CMU_HFRCOCTRL_CLKDIV_DIV2)
#define HFRCO_CALIB_12M (DEVINFO->HFRCOCAL13 | CMU_HFRCOCTRL_CLKDIV_DIV4)
#define HFRCO_CALIB_13M (DEVINFO->HFRCOCAL6)
#define HFRCO_CALIB_14M (DEVINFO->HFRCOCAL14 | CMU_HFRCOCTRL_CLKDIV_DIV4)
#define HFRCO_CALIB_16M (DEVINFO->HFRCOCAL7)
#define HFRCO_CALIB_18M (DEVINFO->HFRCOCAL16 | CMU_HFRCOCTRL_CLKDIV_DIV4)
#define HFRCO_CALIB_19M (DEVINFO->HFRCOCAL8)
#define HFRCO_CALIB_24M (DEVINFO->HFRCOCAL13 | CMU_HFRCOCTRL_CLKDIV_DIV2)
#define HFRCO_CALIB_26M (DEVINFO->HFRCOCAL10)
#define HFRCO_CALIB_28M (DEVINFO->HFRCOCAL14 | CMU_HFRCOCTRL_CLKDIV_DIV2)
#define HFRCO_CALIB_32M (DEVINFO->HFRCOCAL11)
#define HFRCO_CALIB_36M (DEVINFO->HFRCOCAL16 | CMU_HFRCOCTRL_CLKDIV_DIV2)
#define HFRCO_CALIB_38M (DEVINFO->HFRCOCAL12)
#define HFRCO_CALIB_48M (DEVINFO->HFRCOCAL13)
#define HFRCO_CALIB_56M (DEVINFO->HFRCOCAL14)
#define HFRCO_CALIB_64M (DEVINFO->HFRCOCAL15)
#define HFRCO_CALIB_72M (DEVINFO->HFRCOCAL16)

#define AUXHFRCO_CALIB_1M  (DEVINFO->AUXHFRCOCAL0 | CMU_AUXHFRCOCTRL_CLKDIV_DIV4)
#define AUXHFRCO_CALIB_2M  (DEVINFO->AUXHFRCOCAL0 | CMU_AUXHFRCOCTRL_CLKDIV_DIV2)
#define AUXHFRCO_CALIB_4M  (DEVINFO->AUXHFRCOCAL0)
#define AUXHFRCO_CALIB_7M  (DEVINFO->AUXHFRCOCAL3)
#define AUXHFRCO_CALIB_8M  (DEVINFO->AUXHFRCOCAL7 | CMU_AUXHFRCOCTRL_CLKDIV_DIV2)
#define AUXHFRCO_CALIB_12M (DEVINFO->AUXHFRCOCAL13 | CMU_AUXHFRCOCTRL_CLKDIV_DIV4)
#define AUXHFRCO_CALIB_13M (DEVINFO->AUXHFRCOCAL6)
#define AUXHFRCO_CALIB_16M (DEVINFO->AUXHFRCOCAL7)
#define AUXHFRCO_CALIB_19M (DEVINFO->AUXHFRCOCAL8)
#define AUXHFRCO_CALIB_24M (DEVINFO->AUXHFRCOCAL13 | CMU_AUXHFRCOCTRL_CLKDIV_DIV2)
#define AUXHFRCO_CALIB_25M (DEVINFO->AUXHFRCOCAL14 | CMU_AUXHFRCOCTRL_CLKDIV_DIV2)
#define AUXHFRCO_CALIB_26M (DEVINFO->AUXHFRCOCAL10)
#define AUXHFRCO_CALIB_32M (DEVINFO->AUXHFRCOCAL11)
#define AUXHFRCO_CALIB_38M (DEVINFO->AUXHFRCOCAL12)
#define AUXHFRCO_CALIB_48M (DEVINFO->AUXHFRCOCAL13)
#define AUXHFRCO_CALIB_50M (DEVINFO->AUXHFRCOCAL14)

#define USHFRCO_CALIB_4M  (DEVINFO->USHFRCOCAL7 | CMU_USHFRCOCTRL_CLKDIV_DIV4)
#define USHFRCO_CALIB_8M  (DEVINFO->USHFRCOCAL7 | CMU_USHFRCOCTRL_CLKDIV_DIV2)
#define USHFRCO_CALIB_12M (DEVINFO->USHFRCOCAL13 | CMU_USHFRCOCTRL_CLKDIV_DIV4)
#define USHFRCO_CALIB_16M (DEVINFO->USHFRCOCAL7)
#define USHFRCO_CALIB_24M (DEVINFO->USHFRCOCAL13 | CMU_USHFRCOCTRL_CLKDIV_DIV2)
#define USHFRCO_CALIB_25M (DEVINFO->USHFRCOCAL14 | CMU_USHFRCOCTRL_CLKDIV_DIV2)
#define USHFRCO_CALIB_32M (DEVINFO->USHFRCOCAL11)
#define USHFRCO_CALIB_48M (DEVINFO->USHFRCOCAL13)
#define USHFRCO_CALIB_50M (DEVINFO->USHFRCOCAL14)

struct cmu_clock_tree_t {
	u32 hfxo_freq;
	u32 lfxo_freq;
	u32 hfrco_freq;
	u32 ushfrco_freq;
	u32 auxhfrco_freq;
	u32 lfrco_freq;
	u32 ulfrco_freq;
	u32 hfsrc_freq;
	u32 hf_freq;
	u32 hfbus_freq;
	u32 hfcore_freq;
	u32 hfexp_freq;
	u32 hfper_freq;
	u32 hfperb_freq;
	u32 hfperc_freq;
	u32 hfle_freq;
	u32 qspi_freq;
	u32 sdio_freq;
	u32 usb_freq;
	u32 adc0_freq;
	u32 adc1_freq;
	u32 dbg_freq;
	u32 aux_freq;
	u32 lfa_freq;
	u32 lesense_freq;
	u32 rtc_freq;
	u32 lcd_freq;
	u32 letimer0_freq;
	u32 letimer1_freq;
	u32 lfb_freq;
	u32 leuart0_freq;
	u32 leuart1_freq;
	u32 systick_freq;
	u32 csen_freq;
	u32 lfc_freq;
	u32 lfe_freq;
	u32 rtcc_freq;
};

void cmu_config_waitstates(u32 frequency);
void cmu_hfrco_calib(u32 calibration, u32 target_frequency);
void cmu_ushfrco_calib(u8 enable, u32 calibration, u32 target_frequency);
void cmu_auxhfrco_calib(uint8_t enable, u32 calibration, u32 target_frequency);
void cmu_hfxo_startup_calib(u16 ib_trim, u16 c_tune);
void cmu_hfxo_steady_calib(u16 ib_trim, u16 c_tune);
void cmu_lfxo_calib(uint8_t c_tune);
void cmu_init(u32 external_clock_value);
void cmu_update_clock_tree();
struct cmu_clock_tree_t cmu_get_clock_tree();
