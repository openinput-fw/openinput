/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#include "protocol/reports.h"
#include "util/data.h"

#define CFG_TUSB_CONFIG_FILE "targets/stm32f1-generic/tusb_config.h"
#include "tusb.h"

/* Device descriptor */
const u8 desc_device[] = {
	/* clang-format off */
	0x12,				/* LENGTH (18) */
	0x01,				/* DESCRIPTOR TYPE (Device) */
	0x00, 0x02,			/* USB SPEC VERSION (0x0200) */
	0x00,				/* DEVICE CLASS */
	0x00,				/* DEVICE SUBCLASS */
	0x00,				/* DEVICE PROTOCOL */
	0x40,				/* MAX EP0 PACKET SIZE */
	(u8)(USB_VID & 0x00ff),			/* VENDOR ID */
	(u8)((USB_VID >> 8) & 0x00ff),
	(u8)(USB_PID & 0x00ff),			/* PRODUCT ID */
	(u8)((USB_PID >> 8) & 0x00ff),
	0x00, 0x01,			/* DEVICE RELEASE */
	0x01,				/* MANUFACTURER STRING INDEX */
	0x02,				/* PRODUCT STRING INDEX */
	0x00,				/* SERIAL STRING INDEX (None) */
	0x01,				/* NO CONFIGURATIONS */
	/* clang-format on */
};

/* HID Mouse report descriptor */
const u8 desc_hid_mouse_report[] = {
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

/* HID Report Descriptor */
static u8 desc_hid_report[sizeof(oi_rdesc) + sizeof(desc_hid_mouse_report)];

/* Configuration Descriptor */
const u8 desc_configuration[] = {
	/* clang-format off */
	/* configuration */
	0x09,			/* LENGTH */
	0x02,			/* DESCRIPTOR TYPE (Configuration) */
	0x22, 0x00,		/* TOTAL LENGTH [Configuration(9) + Interface(9) + HID(9) + Endpoint(7) = 34] */
	0x01,			/* NUM INTERFACES (1) */
	0x01,			/* CONFIG NUMBER (1) */
	0x00,			/* STRING INDEX (null) */
	0xA0,			/* ATTRIBUTES (Bus powered, Remote wakeup) */
	0x32,			/* MAX POWER (100mA) */
	/* Interface */
	0x09,			/* LENGTH */
	0x04,			/* DESCRIPTOR TYPE (Interface) */
	0x00,			/* INTERFACE NO (0) */
	0x00,			/* ALTERNATE SETTING (none) */
	0x01,			/* NUM ENDPOINTS (1) */
	0x03,			/* INTERFACE CLASS (HID) */
	0x00,			/* INTERFACE SUBCLASS (None) */
	0x00,			/* INTERFACE PROTOCOL (None) */
	0x00,			/* INTERFACE STRING (Null) */
	/* HID */
	0x09,			/* LENGTH */
	0x21,			/* DESCRIPTOR TYPE (hid) */
	0x11, 0x01,		/* HID VERSION (0x0111) */
	0x00,			/* COUNTRY CODE (None) */
	0x01,			/* NO DESCRIPTORS (1) */
	0x22,			/* DESCRIPTOR TYPE (Report) */
	sizeof(desc_hid_report),
	0x00,			/* DESCRIPTOR LENGTH () */
	/* Endpoint in */
	0x07,			/* LENGTH */
	0x05,			/* DESCRIPTOR TYPE (Endpoint) */
	0x81,			/* ENDPOINT ADDRESS (Endpoint 1, IN) */
	0x03,			/* ATTRIBUTES (Interrupt) */
	0x20, 0x00,		/* MAX PACKET SIZE (32) */
	0x01,			/* POLLING INTERVAL (1000Hz) */
	/* clang-format on */
};

/* Invoked when received GET DEVICE DESCRIPTOR */
/* Application returns pointer to descriptor */
u8 const *tud_descriptor_device_cb(void)
{
	return desc_device;
}

/* Invoked when received GET HID REPORT DESCRIPTOR */
/* Application return pointer to descriptor */
/* Descriptor contents must exist long enough for transfer to complete */
u8 const *tud_hid_descriptor_report_cb(void)
{
	memcpy(desc_hid_report, oi_rdesc, sizeof(oi_rdesc)); /* protocol report descriptor */
	memcpy(desc_hid_report + sizeof(oi_rdesc),
	       desc_hid_mouse_report,
	       sizeof(desc_hid_mouse_report)); /* mouse report descriptor */

	return (u8 const *) desc_hid_report;
}

/* Invoked when received GET CONFIGURATION DESCRIPTOR */
/* Application return pointer to descriptor */
/* Descriptor contents must exist long enough for transfer to complete */
u8 const *tud_descriptor_configuration_cb(u8 index)
{
	(void) index; /* For multiple configurations */
	return desc_configuration;
}

/* String Descriptors */

/* array of pointer to string descriptors */
char const *string_desc_arr[] = {
	(const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
	"Openinput", // 1: Manufacturer
	"Openinput Device", // 2: Product
};

static u16 _desc_str[32];

/* Invoked when received GET STRING DESCRIPTOR request */
/* Application return pointer to descriptor, whose contents must exist long enough for transfer to complete */
u16 const *tud_descriptor_string_cb(u8 index, u16 langid)
{
	(void) langid;

	u8 chr_count;

	if (index == 0) {
		memcpy(&_desc_str[1], string_desc_arr[0], 2);
		chr_count = 1;
	} else {
		/* Convert ASCII string into UTF-16 */

		if (index > 2)
			return NULL;

		const char *str = string_desc_arr[index];

		/* Cap at max char */
		chr_count = strlen(str);
		if (chr_count > 31)
			chr_count = 31;

		for (u8 i = 0; i < chr_count; i++) {
			_desc_str[1 + i] = str[i];
		}
	}

	/* first byte is length (including header), second byte is string type */
	_desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

	return _desc_str;
}
