/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <perigoso@riseup.net>
 */

#include "util/data.h"
#include "util/types.h"

#include "platform/sam3u/eefc.h"
#include "platform/sam3u/pio.h"
#include "platform/sam3u/pmc.h"
#include "platform/sam3u/systick.h"
#include "platform/sam3u/wdt.h"

#include <sam.h>

void main()
{
	pmc_init(EXTERNAL_CLOCK_VALUE, 0UL);
	pmc_update_clock_tree();

	systick_init();

	wdt_disable();

	pio_init();

	const struct pio_pin_t red_led_io = LED_R;
	const struct pio_pin_t green_led_io = LED_G;
	const struct pio_pin_t blue_led_io = LED_B;

	pio_config(red_led_io, PIO_DIRECTION_OUT, 1, PIO_PULL_NONE, PIO_MUX_A, PIO_PERIPHERAL_CTRL);
	pio_peripheral_mux(red_led_io, PIO_MUX_B);

	pio_config(green_led_io, PIO_DIRECTION_OUT, 1, PIO_PULL_NONE, PIO_MUX_A, PIO_PERIPHERAL_CTRL);
	pio_peripheral_mux(green_led_io, PIO_MUX_A);

	pio_config(blue_led_io, PIO_DIRECTION_OUT, 1, PIO_PULL_NONE, PIO_MUX_A, PIO_PERIPHERAL_CTRL);
	pio_peripheral_mux(blue_led_io, PIO_MUX_A);

	// pio_set((struct pio_pin_t)LED_R, 0);

	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS; /*  Disable TC clock. */
	TC0->TC_CHANNEL[0].TC_IDR = 0xFFFFFFFF; /*  Disable interrupts. */
	TC0->TC_CHANNEL[0].TC_SR; /*  Clear status register. */
	TC0->TC_CHANNEL[0].TC_CMR; /*  Set mode. */

	TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS; /*  Disable TC clock. */
	TC0->TC_CHANNEL[1].TC_IDR = 0xFFFFFFFF; /*  Disable interrupts. */
	TC0->TC_CHANNEL[1].TC_CMR; /*  Set mode. */

	TC0->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKDIS; /*  Disable TC clock. */
	TC0->TC_CHANNEL[2].TC_IDR = 0xFFFFFFFF; /*  Disable interrupts. */
	TC0->TC_CHANNEL[2].TC_CMR; /*  Set mode. */

	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; /*  Set mode. */
	TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; /*  Set mode. */
	TC0->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; /*  Set mode. */

	TC0->TC_CHANNEL[0].TC_RA = 128; /*  Set RA. */
	TC0->TC_CHANNEL[1].TC_RA = 128;
	TC0->TC_CHANNEL[2].TC_RA = 128;

	for (;;) {
		// static uint8_t curr_led;

		// switch (curr_led) {
		// 	case 0:
		// 		LED_R_PORT->PIO_CODR = (1 << LED_R_PIN);
		// 		LED_G_PORT->PIO_SODR = (1 << LED_G_PIN);
		// 		LED_B_PORT->PIO_SODR = (1 << LED_B_PIN);
		// 		break;

		// 	case 1:
		// 		LED_R_PORT->PIO_SODR = (1 << LED_R_PIN);
		// 		LED_G_PORT->PIO_CODR = (1 << LED_G_PIN);
		// 		LED_B_PORT->PIO_SODR = (1 << LED_B_PIN);
		// 		break;

		// 	case 2:
		// 		LED_R_PORT->PIO_SODR = (1 << LED_R_PIN);
		// 		LED_G_PORT->PIO_SODR = (1 << LED_G_PIN);
		// 		LED_B_PORT->PIO_CODR = (1 << LED_B_PIN);
		// 		break;
		// }

		// curr_led++;
		// curr_led %= 3;

		delay_ms(100);
	}
}
