#ifndef __LEDS_H__
#define __LEDS_H__

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#define NUM_LEDS    2 // eventually dependant on target

/* holds color information */
typedef struct color_t_struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;

void leds_init();
void leds_task();
void leds_set(uint8_t led, uint8_t mode, uint16_t period, color_t color);

#endif // __LEDS_H__