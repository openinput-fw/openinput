/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

#define MOUSE_REPORT_ID	   0x01
#define KEYBOARD_REPORT_ID 0x02

struct mouse_report {
	u8 id;
	s8 x;
	s8 y;
	s8 wheel;
	u8 button1 : 1;
	u8 button2 : 1;
	u8 button3 : 1;
} __attribute__((__packed__));

/* HID Mouse report descriptor */
static const u8 desc_hid_mouse_report[] = {
	/* clang-format off */
	0x05, 0x01,	/* USAGE_PAGE (Generic Desktop) */
	0x09, 0x02,	/* USAGE (Mouse) */
	0xa1, 0x01,	/* COLLECTION (Application) */
	0x09, 0x01,		/* USAGE (Pointer) */
	0xa1, 0x00,		/* COLLECTION (Physical) */
	0x85, 0x01,			/* REPORT_ID (0x01) */
	0x05, 0x01,			/* USAGE_PAGE (Generic Desktop) */
	0x09, 0x30,			/* USAGE (X) */
	0x09, 0x31,			/* USAGE (Y) */
	0x09, 0x38,			/* USAGE (WHEEL) */
	0x15, 0x81,			/* LOGICAL_MINIMUM (-127) */
	0x25, 0x7f,			/* LOGICAL_MAXIMUM (127) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x95, 0x03,			/* REPORT_COUNT (3) */
	0x81, 0x06,			/* INPUT (Data,Var,Rel) */
	0x05, 0x09,			/* USAGE_PAGE (Button) */
	0x19, 0x01,			/* USAGE_MINIMUM (Button 1) */
	0x29, 0x03,			/* USAGE_MAXIMUM (Button 3) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0x01,			/* LOGICAL_MAXIMUM (1) */
	0x95, 0x03,			/* REPORT_COUNT (3) */
	0x75, 0x01,			/* REPORT_SIZE (1) */
	0x81, 0x02,			/* INPUT (Data,Var,Abs) */
	0x95, 0x01,			/* REPORT_COUNT (1) */
	0x75, 0x05,			/* REPORT_SIZE (5) */
	0x81, 0x01,			/* INPUT (Cnst,Var,Abs) */
	0xc0,			/* END_COLLECTION */
	0xc0,		/* END_COLLECTION */
	/* clang-format on */
};

struct keyboard_report {
	u8 id;
	u8 modifiers;
	u8 reserved;
	u8 keys[6];
} __attribute__((__packed__));

/* HID keyboard report descriptor */
static const u8 desc_hid_keyboard_report[] = {
	/* clang-format off */
	0x05, 0x01,	/* USAGE_PAGE (Generic Desktop) */
	0x09, 0x06,	/* USAGE (keyboard) */
	0xa1, 0x01,	/* COLLECTION (Application) */
	/* Report ID if any */
	0x85, 0x02,			/* REPORT_ID (0x02) */
	/* 8 bits Modifier Keys (Shfit, Control, Alt) */
	0x05, 0x07,			/* USAGE_PAGE (keyboard) */
	0x19, 0xE0,			/* USAGE_MINIMUM (224) */
	0x29, 0xE7,			/* USAGE_MAXIMUM (231) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0x01,			/* LOGICAL_MAXIMUM (1) */
	0x95, 0x08,			/* REPORT_COUNT (8) */
	0x75, 0x01,			/* REPORT_SIZE (1) */
	0x81, 0x02,			/* INPUT (Data,Var,Abs) */
	/* 8 bit reserved */
	0x95, 0x01,			/* REPORT_COUNT (1) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x81, 0x01,			/* INPUT (Const) */
	/* 6-byte Keycodes */
	0x05, 0x07,			/* USAGE_PAGE (keyboard) */
	0x19, 0x00,			/* USAGE_MINIMUM (0) */
	0x29, 0xFF,			/* USAGE_MAXIMUM (255) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0xFF,			/* LOGICAL_MAXIMUM (255) */
	0x95, 0x06,			/* REPORT_COUNT (6) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x81, 0x00,			/* INPUT (Data,Arr,Abs) */
	/* 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | NumLock */
	0x05, 0x08,			/* USAGE_PAGE (led) */
	0x19, 0x00,			/* USAGE_MINIMUM (0) */
	0x29, 0x01,			/* USAGE_MAXIMUM (1) */
	0x95, 0x05,			/* REPORT_COUNT (5) */
	0x75, 0x01,			/* REPORT_SIZE (1) */
	0x91, 0x02,			/* OUTPUT (Data,Var,Abs) */
	/* led padding */
	0x95, 0x01,			/* REPORT_COUNT (5) */
	0x75, 0x03,			/* REPORT_SIZE (1) */
	0x91, 0x01,			/* INPUT (Const) */
	0xc0,		/* END_COLLECTION */
	/* clang-format on */
};
