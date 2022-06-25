/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "platform/sam3u/eefc.h"

void eefc_config_waitstates(const u32 frequency)
{
	if (frequency <= 27000000UL)
		EFC0->EEFC_FMR = (EFC0->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (0 << EEFC_FMR_FWS_Pos);
	else if (frequency <= 47000000UL)
		EFC0->EEFC_FMR = (EFC0->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (1 << EEFC_FMR_FWS_Pos);
	else if (frequency <= 84000000UL)
		EFC0->EEFC_FMR = (EFC0->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (2 << EEFC_FMR_FWS_Pos);
	else
		EFC0->EEFC_FMR = (EFC0->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (3 << EEFC_FMR_FWS_Pos);

#ifdef _SAM3U_EFC1_INSTANCE_
	if (frequency <= 27000000UL)
		EFC1->EEFC_FMR = (EFC0->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (0 << EEFC_FMR_FWS_Pos);
	else if (frequency <= 47000000UL)
		EFC1->EEFC_FMR = (EFC0->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (1 << EEFC_FMR_FWS_Pos);
	else if (frequency <= 84000000UL)
		EFC1->EEFC_FMR = (EFC0->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (2 << EEFC_FMR_FWS_Pos);
	else
		EFC1->EEFC_FMR = (EFC0->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | (3 << EEFC_FMR_FWS_Pos);
#endif /* _SAM3U_EFC1_INSTANCE_ */
}
