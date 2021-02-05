// SPDX-License-Identifier: MIT

#pragma once

#include "util/types.h"

struct oi_report_t {
	u8 id;
	u8 function_page;
	u8 function;
	u8 data[29];
} __attribute__((__packed__));

static const u8 oi_rdesc[] = {
	/* clang-format off */
	/* short report */
	0x06, 0x00, 0xff,	/* USAGE_PAGE (Vendor Page) */
	0x09, 0x00,		/* USAGE (Vendor Usage 0) */
	0xa1, 0x01,		/* COLLECTION (Application) */
	0x85, 0x20,			/*  REPORT_ID (0x20) */
	0x75, 0x08,			/*  REPORT_SIZE (8) */
	0x95, 0x08,			/*  REPORT_COUNT (8) */
	0x15, 0x00,			/*  LOGICAL MINIMUM (0) */
	0x26, 0xff, 0x00,		/*  LOGICAL MAXIMUM (255) */
	0x09, 0x00,			/*  USAGE (Vendor Usage 0) */
	0x81, 0x00,			/*  INPUT (Data,Arr,Abs) */
	0x09, 0x00,			/*  USAGE (Vendor Usage 0) */
	0x91, 0x00,			/*  OUTPUT (Data,Arr,Abs) */
	0xc0,			/* END_COLLECTION */
	/* long report */
	0x06, 0x00, 0xff,	/* USAGE_PAGE (Vendor Page) */
	0x09, 0x00,		/* USAGE (Vendor Usage 0) */
	0xa1, 0x01,		/* COLLECTION (Application) */
	0x85, 0x21,			/*  REPORT_ID (0x21) */
	0x75, 0x08,			/*  REPORT_SIZE (8) */
	0x95, 0x20,			/*  REPORT_COUNT (32) */
	0x15, 0x00,			/*  LOGICAL MINIMUM (0) */
	0x26, 0xff, 0x00,		/*  LOGICAL MAXIMUM (255) */
	0x09, 0x00,			/*  USAGE (Vendor Usage 0) */
	0x81, 0x00,			/*  INPUT (Data,Arr,Abs) */
	0x09, 0x00,			/*  USAGE (Vendor Usage 0) */
	0x91, 0x00,			/*  OUTPUT (Data,Arr,Abs) */
	0xc0,			/* END_COLLECTION */
	/* clang-format on */
};
