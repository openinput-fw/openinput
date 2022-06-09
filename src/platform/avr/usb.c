/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <perigoso@riseup.net>
 */

#include <avr/interrupt.h>

#include "protocol/reports.h"
#include "usb.h"
#include "util/hid_descriptors.h"
#include "util/types.h"

#include <USB.h>

/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static u8 openinput_hid_report_buff[sizeof(struct oi_report_t)];
static u8 mouse_hid_report_buff[sizeof(struct mouse_report)];
static u8 keyboard_hid_report_buff[sizeof(struct keyboard_report)];

static u8 new_oi_report;
static struct oi_report_t oi_report;
static u8 oi_report_size;

static u8 new_mouse_report;
static struct mouse_report mouse_report;
static u8 mouse_report_size;

static u8 new_keyboard_report;
static struct keyboard_report keyboard_report;
static u8 keyboard_report_size;

static struct protocol_config_t protocol_config;

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t openinput_hid_interface = {
	.Config =
		{
			.InterfaceNumber = 0,
			.ReportINEndpoint =
				{
					.Address = 0x81,
					.Size = 64,
					.Banks = 1,
				},
			.PrevReportINBuffer = openinput_hid_report_buff,
			.PrevReportINBufferSize = sizeof(openinput_hid_report_buff),
		},
};

USB_ClassInfo_HID_Device_t mouse_hid_interface = {
	.Config =
		{
			.InterfaceNumber = 1,
			.ReportINEndpoint =
				{
					.Address = 0x83,
					.Size = 64,
					.Banks = 1,
				},
			.PrevReportINBuffer = mouse_hid_report_buff,
			.PrevReportINBufferSize = sizeof(mouse_hid_report_buff),
		},
};

USB_ClassInfo_HID_Device_t keyboard_hid_interface = {
	.Config =
		{
			.InterfaceNumber = 2,
			.ReportINEndpoint =
				{
					.Address = 0x84,
					.Size = 64,
					.Banks = 1,
				},
			.PrevReportINBuffer = keyboard_hid_report_buff,
			.PrevReportINBufferSize = sizeof(keyboard_hid_report_buff),
		},
};

void usb_init()
{
	/* Init USB stack */
	USB_Init();
}

void usb_task()
{
	USB_USBTask();

	HID_Device_USBTask(&openinput_hid_interface);
	HID_Device_USBTask(&mouse_hid_interface);
	HID_Device_USBTask(&keyboard_hid_interface);
}

void usb_attach_protocol_config(struct protocol_config_t config)
{
	protocol_config = config;
}

void usb_write_descriptor(u8 interface, u8 *report_data, u16 report_size)
{
	if (interface == openinput_hid_interface.Config.InterfaceNumber) {
		memcpy(&oi_report, report_data, report_size);
		oi_report_size = report_size;
		new_oi_report = 1;
	} else if (interface == mouse_hid_interface.Config.InterfaceNumber) {
		memcpy(&mouse_report, report_data, report_size);
		mouse_report_size = report_size;
		new_mouse_report = 1;
	} else if (interface == keyboard_hid_interface.Config.InterfaceNumber) {
		memcpy(&keyboard_report, report_data, report_size);
		keyboard_report_size = report_size;
		new_keyboard_report = 1;
	}
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs and stops the USB management task.
 */
void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	HID_Device_ConfigureEndpoints(&openinput_hid_interface);
	HID_Device_ConfigureEndpoints(&mouse_hid_interface);
	HID_Device_ConfigureEndpoints(&keyboard_hid_interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(
	USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo,
	uint8_t *const ReportID,
	const uint8_t ReportType,
	void *ReportData,
	uint16_t *const ReportSize)
{
	/* Determine which interface must have its report generated */
	if (HIDInterfaceInfo == &openinput_hid_interface) {
		if (new_oi_report == 1) {
			memcpy(ReportData, &oi_report, oi_report_size);
			*ReportSize = oi_report_size;
			new_oi_report = 0;
			return true;
		} else {
			return false;
		}
	} else if (HIDInterfaceInfo == &mouse_hid_interface) {
		if (new_mouse_report == 1) {
			memcpy(ReportData, &mouse_report, mouse_report_size);
			*ReportSize = mouse_report_size;
			new_mouse_report = 0;
			return true;
		} else {
			return false;
		}
	} else if (HIDInterfaceInfo == &keyboard_hid_interface) {
		if (new_keyboard_report == 1) {
			memcpy(ReportData, &keyboard_report, keyboard_report_size);
			*ReportSize = keyboard_report_size;
			new_keyboard_report = 0;
			return true;
		} else {
			return false;
		}
	}
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(
	USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo,
	const uint8_t ReportID,
	const uint8_t ReportType,
	const void *ReportData,
	const uint16_t ReportSize)
{
	if (HIDInterfaceInfo == &openinput_hid_interface) {
	} else if (HIDInterfaceInfo == &keyboard_hid_interface) {
		protocol_dispatch(protocol_config, (u8 *) ReportData, ReportSize);
	}
}
