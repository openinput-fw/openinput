/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "platform/samx7x/pmc.h"
#include "platform/samx7x/usb.h"
#include "util/types.h"

#define CFG_TUSB_CONFIG_FILE "targets/sams70-generic/tusb_config.h"
#include "tusb.h"

static struct protocol_config_t protocol_config;

void usb_attach_protocol_config(struct protocol_config_t config)
{
	protocol_config = config;
}

void usb_init()
{
	/* Enable USB peripheral clock */
	pmc_peripheral_clock_gate(USBHS_CLOCK_ID, 1); // Enable peripheral clock

	/* Wait for the clock to be usable */
	while (!(USBHS->USBHS_SR & USBHS_SR_CLKUSABLE_Msk))
		;

	/* Init USB stack */
	tusb_init(); /* USB Stack handles the rest of the peripheral init */
}

/* USB ISR mapping */

void _usbhs_isr()
{
	tud_int_handler(0);
}

/* TinyUSB Callbacks */

/* Invoked when device is mounted */
void tud_mount_cb(void)
{
}

/* Invoked when device is unmounted */
void tud_umount_cb(void)
{
}

/* Invoked when usb bus is suspended */
void tud_suspend_cb(bool remote_wakeup_en)
{
	(void) remote_wakeup_en;
}

/* Invoked when usb bus is resumed */
void tud_resume_cb(void)
{
}

/* Invoked when received GET_REPORT control request */
u16 tud_hid_get_report_cb(u8 itf, u8 report_id, hid_report_type_t report_type, u8 *buffer, u16 reqlen)
{
	(void) itf;
	(void) report_id;
	(void) report_type;
	(void) buffer;
	(void) reqlen;

	return 0;
}

/* Invoked when received SET_REPORT control request */
void tud_hid_set_report_cb(u8 itf, u8 report_id, hid_report_type_t report_type, u8 const *buffer, u16 bufsize)
{
	(void) report_id;
	(void) report_type;

	if (itf == 0)
		protocol_dispatch(protocol_config, (u8 *) buffer, bufsize);
}
