/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <perigoso@riseup.net>
 */

#include <string.h>

#include <USB.h>

#include "util/data.h"
#include "util/types.h"

/* Device descriptor */
const u8 PROGMEM desc_device[] = {
	/* clang-format off */
	0x12,				/* LENGTH (18) */
	0x01,				/* DESCRIPTOR TYPE (Device) */
	0x10, 0x01,			/* USB SPEC VERSION (0x0110) */
	0x00,				/* DEVICE CLASS */
	0x00,				/* DEVICE SUBCLASS */
	0x00,				/* DEVICE PROTOCOL */
	0x40,				/* MAX EP0 PACKET SIZE (32) */
	0x50, 0x1D,			/* VENDOR ID (0x1D50) */
	0x6A, 0x61,			/* PRODUCT ID (0x616A) */
	0x00, 0x01,			/* DEVICE RELEASE */
	0x01,				/* MANUFACTURER STRING INDEX */
	0x02,				/* PRODUCT STRING INDEX */
	0x00,				/* SERIAL STRING INDEX (None) */
	0x01,				/* NO CONFIGURATIONS */
	/* clang-format on */
};

const u8 PROGMEM oi_rdesc[] = {
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

/* HID Mouse report descriptor */
const u8 PROGMEM desc_hid_mouse_report[] = {
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

/* HID keyboard report descriptor */
const u8 PROGMEM desc_hid_keyboard_report[] = {
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

const u8 PROGMEM oi_hid_desc[] = {
	/* HID */
	0x09, /* LENGTH */
	0x21, /* DESCRIPTOR TYPE (hid) */
	0x11,
	0x01, /* HID VERSION (0x0111) */
	0x00, /* COUNTRY CODE (None) */
	0x01, /* NO DESCRIPTORS (1) */
	0x22, /* DESCRIPTOR TYPE (Report) */
	sizeof(oi_rdesc),
	0x00, /* DESCRIPTOR LENGTH () */
};

const u8 PROGMEM mouse_hid_desc[] = {
	/* HID */
	0x09, /* LENGTH */
	0x21, /* DESCRIPTOR TYPE (hid) */
	0x11,
	0x01, /* HID VERSION (0x0111) */
	0x00, /* COUNTRY CODE (None) */
	0x01, /* NO DESCRIPTORS (1) */
	0x22, /* DESCRIPTOR TYPE (Report) */
	sizeof(desc_hid_mouse_report),
	0x00, /* DESCRIPTOR LENGTH () */
};

const u8 PROGMEM keyboard_hid_desc[] = {
	/* HID */
	0x09, /* LENGTH */
	0x21, /* DESCRIPTOR TYPE (hid) */
	0x11,
	0x01, /* HID VERSION (0x0111) */
	0x00, /* COUNTRY CODE (None) */
	0x01, /* NO DESCRIPTORS (1) */
	0x22, /* DESCRIPTOR TYPE (Report) */
	sizeof(desc_hid_keyboard_report),
	0x00, /* DESCRIPTOR LENGTH () */
};

/* Configuration Descriptor */
const u8 PROGMEM desc_configuration[] = {
	/* clang-format off */
	/* configuration */
	0x09,			/* LENGTH */
	0x02,			/* DESCRIPTOR TYPE (Configuration) */
	0x62, 0x00,		/* TOTAL LENGTH (98) */
	0x03,			/* NUM INTERFACES (3) */
	0x01,			/* CONFIG NUMBER (1) */
	0x00,			/* STRING INDEX (null) */
	0xA0,			/* ATTRIBUTES (Bus powered, Remote wakeup) */
	0x32,			/* MAX POWER (100mA) */

	/* Interface 0 - openinput protocol */
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
	sizeof(oi_rdesc),
	0x00,			/* DESCRIPTOR LENGTH () */
	/* Endpoint in */
	0x07,			/* LENGTH */
	0x05,			/* DESCRIPTOR TYPE (Endpoint) */
	0x81,			/* ENDPOINT ADDRESS (Endpoint 1, IN) */
	0x03,			/* ATTRIBUTES (Interrupt) */
	0x40, 0x00,		/* MAX PACKET SIZE (64) */
	0x0A,			/* POLLING INTERVAL (100Hz) */
	/* Endpoint out */
	0x07,			/* LENGTH */
	0x05,			/* DESCRIPTOR TYPE (Endpoint) */
	0x01,			/* ENDPOINT ADDRESS (Endpoint 1, OUT) */
	0x03,			/* ATTRIBUTES (Interrupt) */
	0x40, 0x00,		/* MAX PACKET SIZE (64) */
	0x0A,			/* POLLING INTERVAL (100Hz) */


	/* Interface 1 - HID Mouse */
	0x09,			/* LENGTH */
	0x04,			/* DESCRIPTOR TYPE (Interface) */
	0x01,			/* INTERFACE NO (1) */
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
	sizeof(desc_hid_mouse_report),
	0x00,			/* DESCRIPTOR LENGTH () */
	/* Endpoint in */
	0x07,			/* LENGTH */
	0x05,			/* DESCRIPTOR TYPE (Endpoint) */
	0x82,			/* ENDPOINT ADDRESS (Endpoint 2, IN) */
	0x03,			/* ATTRIBUTES (Interrupt) */
	0x40, 0x00,		/* MAX PACKET SIZE (64) */
	0x01,			/* POLLING INTERVAL (1000Hz) */


	/* Interface 2 - HID Keyboard */
	0x09,			/* LENGTH */
	0x04,			/* DESCRIPTOR TYPE (Interface) */
	0x02,			/* INTERFACE NO (2) */
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
	sizeof(desc_hid_keyboard_report),
	0x00,			/* DESCRIPTOR LENGTH () */
	/* Endpoint in */
	0x07,			/* LENGTH */
	0x05,			/* DESCRIPTOR TYPE (Endpoint) */
	0x83,			/* ENDPOINT ADDRESS (Endpoint 3, IN) */
	0x03,			/* ATTRIBUTES (Interrupt) */
	0x40, 0x00,		/* MAX PACKET SIZE (64) */
	0x0A,			/* POLLING INTERVAL (100Hz) */
	/* Endpoint out */
	0x07,			/* LENGTH */
	0x05,			/* DESCRIPTOR TYPE (Endpoint) */
	0x03,			/* ENDPOINT ADDRESS (Endpoint 3, OUT) */
	0x03,			/* ATTRIBUTES (Interrupt) */
	0x40, 0x00,		/* MAX PACKET SIZE (64) */
	0x0A,			/* POLLING INTERVAL (100Hz) */
	/* clang-format on */
};

/* String Descriptors */
const USB_Descriptor_String_t PROGMEM lang_str = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM manu_str = USB_STRING_DESCRIPTOR(L"Openinput");
const USB_Descriptor_String_t PROGMEM prod_str = USB_STRING_DESCRIPTOR(L"Openinput Device");

/**
 * This function is called by the library when in device mode, and must be overridden (see LUFA library "USB Descriptors"
 * documentation) by the application code so that the address and size of a requested descriptor can be given
 * to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 * is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 * USB host.
 */
u16 CALLBACK_USB_GetDescriptor(const u16 w_value, const u16 w_index, const void **const descriptor_address)
{
	const u8 descriptor_type = (w_value >> 8);
	const u8 descriptor_number = (w_value & 0xFF);

	const void *addr = NULL;
	u16 size = 0;

	switch (descriptor_type) {
		case DTYPE_Device:
			addr = desc_device;
			size = sizeof(desc_device);
			break;

		case DTYPE_Configuration:
			addr = desc_configuration;
			size = sizeof(desc_configuration);
			break;

		case DTYPE_String:
			switch (descriptor_number) {
				case 0:
					addr = &lang_str;
					size = pgm_read_byte(&lang_str.Header.Size);
					break;
				case 1:
					addr = &manu_str;
					size = pgm_read_byte(&manu_str.Header.Size);
					break;
				case 2:
					addr = &prod_str;
					size = pgm_read_byte(&prod_str.Header.Size);
					break;
			}

			break;

		case HID_DTYPE_HID:
			switch (w_index) {
				case 0:
					addr = oi_hid_desc;
					size = sizeof(oi_hid_desc);
					break;
				case 1:
					addr = mouse_hid_desc;
					size = sizeof(mouse_hid_desc);
					break;
				case 2:
					addr = keyboard_hid_desc;
					size = sizeof(keyboard_hid_desc);
					break;
			}
			break;

		case HID_DTYPE_Report:
			switch (w_index) {
				case 0:
					addr = oi_rdesc;
					size = sizeof(oi_rdesc);
					break;
				case 1:
					addr = desc_hid_mouse_report;
					size = sizeof(desc_hid_mouse_report);
					break;
				case 2:
					addr = desc_hid_keyboard_report;
					size = sizeof(desc_hid_keyboard_report);
					break;
			}
			break;
	}

	*descriptor_address = addr;
	return size;
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the keyboard and mouse device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	/* Setup Openinput Report Endpoints */
	Endpoint_ConfigureEndpoint(0x81, EP_TYPE_INTERRUPT, 0x40, 1); // IN
	Endpoint_ConfigureEndpoint(0x01, EP_TYPE_INTERRUPT, 0x40, 1); // OUT

	/* Setup Mouse HID Report Endpoint */
	Endpoint_ConfigureEndpoint(0x82, EP_TYPE_INTERRUPT, 0x40, 1); // IN

	/* Setup Keyboard HID Report Endpoints */
	Endpoint_ConfigureEndpoint(0x83, EP_TYPE_INTERRUPT, 0x40, 1); // IN
	Endpoint_ConfigureEndpoint(0x03, EP_TYPE_INTERRUPT, 0x40, 1); // OUT
}
