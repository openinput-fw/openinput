/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

enum stm32f1_external_clock_value {
	STM32F1_CLOCK_8MHZ,
	STM32F1_CLOCK_12MHZ,
};

void rcc_init(enum stm32f1_external_clock_value clock);
