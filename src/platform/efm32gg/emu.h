/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

void emu_init(u8 immediate_switch); /* immediate_switch [0, 1], should be set to 1 when DCDC is not available/used */
void emu_reg_init(float target_voltage); /* 5V susbsystem regulator config, sets the out voltage of the integrated regulator */
#if defined(DCDC_PRESENT)
void emu_dcdc_init(float target_voltage, float max_ln_current, float max_lp_current, float max_reverse_current);
#endif
