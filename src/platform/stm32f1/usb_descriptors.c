/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "util/usb_descriptors.h"
#include "protocol/reports.h"
#include "util/data.h"
#include "util/hid_descriptors.h"

#define CFG_TUSB_CONFIG_FILE "targets/stm32f1-generic/tusb_config.h"
#include "tusb.h"

/* HID Report Descriptor */
static u8 desc_hid_report[sizeof(oi_rdesc) + sizeof(desc_hid_mouse_report)];

/* Configuration Descriptor */
const u8 desc_configuration[] = {
	/* clang-format off */
	/* configuration */
	0x09,			/* LENGTH */
	0x02,			/* DESCRIPTOR TYPE (Configuration) */
	0x29, 0x00,		/* TOTAL LENGTH [Configuration(9)+Interface(9)+HID(9)+Edptin(7)+Edptout(7)=41] */
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
	0x02,			/* NUM ENDPOINTS (2) */
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
	0x40, 0x00,		/* MAX PACKET SIZE (64) */
	0x01,			/* POLLING INTERVAL (1000Hz) */
	/* Endpoint out */
	0x07,			/* LENGTH */
	0x05,			/* DESCRIPTOR TYPE (Endpoint) */
	0x01,			/* ENDPOINT ADDRESS (Endpoint 1, IN) */
	0x03,			/* ATTRIBUTES (Interrupt) */
	0x40, 0x00,		/* MAX PACKET SIZE (64) */
	0x0A,			/* POLLING INTERVAL (100Hz) */
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
u8 const *tud_hid_descriptor_report_cb(u8 itf)
{
	(void) itf;

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
