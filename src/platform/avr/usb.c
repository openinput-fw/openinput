/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <perigoso@riseup.net>
 */

#include <avr/interrupt.h>

#include "protocol/reports.h"
#include "usb.h"
#include "util/types.h"

#include <USB.h>

static struct protocol_config_t protocol_config;

void usb_attach_protocol_config(struct protocol_config_t config)
{
	protocol_config = config;
}

void usb_init()
{
	/* Init USB stack */
	USB_Init();
}

void usb_task()
{
	USB_USBTask();
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

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	struct oi_report_t oi_report;
	uint8_t *ReportData;
	uint8_t ReportSize;

	/* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest) {
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
				Endpoint_ClearSETUP();

				// /* Determine if it is the mouse or the keyboard data that is being requested */
				// if (USB_ControlRequest.wIndex == 0) { // openinput
				// } else if (USB_ControlRequest.wIndex == 1) { // mouse
				// } else if (USB_ControlRequest.wIndex == 2) { // keyboard
				// }

				// /* Write the report data to the control endpoint */
				// Endpoint_Write_Control_Stream_LE(ReportData, ReportSize);
				Endpoint_ClearOUT();
			}

			break;
		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
				Endpoint_ClearSETUP();

				/* Wait until the report has been sent by the host */
				while (!(Endpoint_IsOUTReceived())) {
					if (USB_DeviceState == DEVICE_STATE_Unattached)
						return;
				}

				if (USB_ControlRequest.wIndex == 0) {
					// for (size_t i = 0; i < USB_ControlRequest.wLength; i++)
					// {
					// 	((uint8_t *)(&oi_report))[i] = Endpoint_Read_8();
					// }
					// protocol_dispatch(protocol_config, (uint8_t *)(&oi_report), USB_ControlRequest.wLength);

				} else if (USB_ControlRequest.wIndex == 2) {
					/* Read in the LED report from the host */
					uint8_t LEDStatus = Endpoint_Read_8();
				}

				Endpoint_ClearOUT();
				Endpoint_ClearStatusStage();
			}

			break;
	}
}
