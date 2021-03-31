/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

#include "protocol/protocol.h"

void usb_init();
void usb_attach_protocol_config(struct protocol_config_t config);
