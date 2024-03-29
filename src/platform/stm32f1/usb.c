/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/gpio.h"
#include "platform/stm32f1/usb.h"
#include "util/types.h"

#define CFG_TUSB_CONFIG_FILE "targets/stm32f1-generic/tusb_config.h"
#include "tusb.h"

static struct protocol_config_t protocol_config;

void usb_init()
{
	/* Enable USB peripheral clock */
	RCC->APB1RSTR |= RCC_APB1RSTR_USBRST; /* Reset peripheral clock */
	RCC->APB1RSTR &= ~RCC_APB1RSTR_USBRST;
	RCC->APB1ENR |= RCC_APB1ENR_USBEN; /* Enable USB peripheral clock */

	/* Init USB stack */
	tusb_init(); /* USB Stack handles the rest of the peripheral init */
}

void usb_attach_protocol_config(struct protocol_config_t config)
{
	protocol_config = config;
}

/* USB ISR mapping */

void _usb_hp_can_tx_isr()
{
	tud_int_handler(0);
}

void _usb_lp_can_rx0_isr()
{
	tud_int_handler(0);
}

void _usb_wakeup_isr()
{
	tud_int_handler(0);
}

/* TinyUSB port* */

void dcd_connect(u8 rhport)
{
	(void) rhport;

	struct gpio_pin_t usb_dp_pu_io = USB_DP_PU_IO;

	gpio_set(usb_dp_pu_io, 1);
}

void dcd_disconnect(u8 rhport)
{
	(void) rhport;

	struct gpio_pin_t usb_dp_pu_io = USB_DP_PU_IO;

	gpio_set(usb_dp_pu_io, 1);
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

/*
 * Invoked when received GET_REPORT control request
 * Application must fill buffer report's content and return its length.
 * Return zero will cause the stack to STALL request
*/
u16 tud_hid_get_report_cb(u8 itf, u8 report_id, hid_report_type_t report_type, u8 *buffer, u16 reqlen)
{
	(void) itf;
	(void) report_id;
	(void) report_type;
	(void) buffer;
	(void) reqlen;

	return 0;
}

/*
 * Invoked when received SET_REPORT control request or
 * received data on OUT endpoint ( Report ID = 0, Type = 0 )
*/
void tud_hid_set_report_cb(u8 itf, u8 report_id, hid_report_type_t report_type, u8 const *buffer, u16 bufsize)
{
	(void) itf;
	(void) report_id;
	(void) report_type;

	if (itf == 0)
		protocol_dispatch(protocol_config, (u8 *) buffer, bufsize);
}
