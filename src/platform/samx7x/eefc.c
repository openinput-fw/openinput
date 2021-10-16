/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "platform/samx7x/eefc.h"

void eefc_config_waitstates(u32 frequency)
{
	if (frequency <= 23000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (0 << EEFC_FMR_FWS_Pos);
	else if (frequency <= 46000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (1 << EEFC_FMR_FWS_Pos);
	else if (frequency <= 69000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (2 << EEFC_FMR_FWS_Pos);
	else if (frequency <= 92000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (3 << EEFC_FMR_FWS_Pos);
	else if (frequency <= 115000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (4 << EEFC_FMR_FWS_Pos);
	else if (frequency <= 138000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (5 << EEFC_FMR_FWS_Pos);
	else
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (6 << EEFC_FMR_FWS_Pos);
}
