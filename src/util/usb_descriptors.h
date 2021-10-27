/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

#define VENDOR_ID  0x1D50
#define PRODUCT_ID 0x616A
#define RELEASE	   0x0001

/* Device descriptor */
static const u8 desc_device[] = {
	/* clang-format off */
	0x12,				/* LENGTH (18) */
	0x01,				/* DESCRIPTOR TYPE (Device) */
	0x00, 0x02,			/* USB SPEC VERSION (0x0200) */
	0x00,				/* DEVICE CLASS */
	0x00,				/* DEVICE SUBCLASS */
	0x00,				/* DEVICE PROTOCOL */
	0x40,				/* MAX EP0 PACKET SIZE */
	0x50, 0x1D,			/* VENDOR ID (0x1D50) */
	0x6A, 0x61,			/* PRODUCT ID (0x616A) */
	0x00, 0x01,			/* DEVICE RELEASE */
	0x01,				/* MANUFACTURER STRING INDEX */
	0x02,				/* PRODUCT STRING INDEX */
	0x00,				/* SERIAL STRING INDEX (None) */
	0x01,				/* NO CONFIGURATIONS */
	/* clang-format on */
};

/* Per USB specs: high speed capable device must report device_qualifier and other_speed_configuration */
static const u8 desc_device_qualifier[] = {
	/* clang-format off */
	0x0A,				/* LENGTH (10) */
	0x06,				/* DESCRIPTOR TYPE (Device qualifier) */
	0x00, 0x02,			/* USB SPEC VERSION (0x0200) */
	0x00,				/* DEVICE CLASS */
	0x00,				/* DEVICE SUBCLASS */
	0x00,				/* DEVICE PROTOCOL */
	0x40,				/* MAX EP0 PACKET SIZE */
	0x01,				/* NO CONFIGURATIONS */
	0x00,				/* RESERVED */
	/* clang-format on */
};
