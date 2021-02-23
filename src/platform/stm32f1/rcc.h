/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

#include "util/types.h"

enum stm32f1_external_clock_value {
	STM32F1_CLOCK_8MHZ = 8000000U,
	STM32F1_CLOCK_12MHZ = 12000000U,
};

struct rcc_clock_tree_t {
	u32 sys_clock_freq;
	u32 ahb_clock_freq;
	u32 apb1_clock_freq;
	u32 apb2_clock_freq;
	u32 apb1_tim_clock_freq;
	u32 apb2_tim_clock_freq;
	u32 adc_clock_freq;
};

void rcc_init(enum stm32f1_external_clock_value clock);
void rcc_update_clock_tree();
struct rcc_clock_tree_t rcc_get_clock_tree();
