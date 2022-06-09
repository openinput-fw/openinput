/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "protocol/protocol.h"

void usb_init();
void usb_task();
void usb_attach_protocol_config(struct protocol_config_t config);
void usb_write_descriptor(u8 interface, u8 *report_data, u16 report_size);
