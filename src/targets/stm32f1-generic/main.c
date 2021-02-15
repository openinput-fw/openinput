/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/rcc.h"

int main()
{
	rcc_init(EXTERNAL_CLOCK_VALUE);
}
