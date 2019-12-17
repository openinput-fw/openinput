#include <libopencm3/stm32/rcc.h>

#include "leds.h"


static void clock_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

int main(void)
{
	clock_setup();

	leds_init();


	for(;;)
	{
		static color_t red = {.r = 255, .g = 0, .b = 0};
		static color_t green = {.r = 0, .g = 255, .b = 0};
		static color_t blue = {.r = 0, .g = 0, .b = 255};
		static color_t off = {.r = 0, .g = 0, .b = 0};

		leds_set(0, 0, 0, red);
		leds_set(1, 0, 0, green);
		for (uint8_t i = 0; i < 255; i++)
		{
			for (int ii = 0; ii < 128000; ii++) __asm__("nop");
			leds_task();
		}

		leds_set(0, 0, 0, green);
		leds_set(1, 0, 0, blue);
		for (uint8_t i = 0; i < 255; i++)
		{
			for (int ii = 0; ii < 128000; ii++) __asm__("nop");
			leds_task();
		}

		leds_set(0, 0, 0, blue);
		leds_set(1, 0, 0, red);
		for (uint8_t i = 0; i < 255; i++)
		{
			for (int ii = 0; ii < 128000; ii++) __asm__("nop");
			leds_task();
		}

		leds_set(0, 0, 0, off);
		leds_set(1, 0, 0, off);
		for (uint8_t i = 0; i < 255; i++)
		{
			for (int ii = 0; ii < 128000; ii++) __asm__("nop");
			leds_task();
		}
	}

	return 0;
}
