/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include <sam.h>

#include "util/types.h"

struct flash_info_t {
	u32 base_addr;
	u32 size; /* Flash size in bytes */
	u32 page_size; /* Page size in bytes */
};

void eefc_init();

void eefc_set_waitstates(u32 frequency);

void eefc_tcm_disable();

const struct flash_info_t *eefc_get_info();

s32 eefc_addr_to_page(u32 addr);
u32 eefc_page_to_addr(u32 page);

u32 eefc_erase_16(u32 addr);
void eefc_read(void *buffer, u32 addr, u32 size);
void eefc_write(u32 addr, void *buffer, u32 size);
