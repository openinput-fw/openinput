#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

static void rgb_pwm_timer_setup(int32_t timer)
{
	timer_disable_counter(timer);
	/*timer_reset(timer);*/

	timer_set_mode(timer, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	timer_set_prescaler(timer, 6);
	timer_set_period(timer, 255);

	/* only needed for advanced timers */
	timer_enable_break_main_output(timer);

	timer_enable_preload(timer);
	timer_continuous_mode(timer);

	/* configure OC (output-compare) channels */
	timer_disable_oc_output(timer, TIM_OC1);
	timer_disable_oc_output(timer, TIM_OC2);
	timer_disable_oc_output(timer, TIM_OC3);

	/* set to operate in PWM1 mode */
	/* PWM1 states the output will be high when the count is less than the
	   configur oc_value and low when higher */
	timer_set_oc_mode(timer, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(timer, TIM_OC2, TIM_OCM_PWM1);
	timer_set_oc_mode(timer, TIM_OC3, TIM_OCM_PWM1);

	timer_set_oc_value(timer, TIM_OC1, 0);
	timer_set_oc_value(timer, TIM_OC2, 0);
	timer_set_oc_value(timer, TIM_OC3, 0);

	timer_enable_oc_output(timer, TIM_OC1);
	timer_enable_oc_output(timer, TIM_OC2);
	timer_enable_oc_output(timer, TIM_OC3);

	timer_enable_counter(timer);
}

static void timer_setup(void)
{
	/* setup pin modes*/
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);

	/* TIM1 gpio setup */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO8 | GPIO9 | GPIO10);

	/* TIM2 gpio setup */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0 | GPIO1 | GPIO2);

	/* enable timers */
	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_TIM2);

	/* configure timers to generate a pwm signal to control rgb leds */
	rgb_pwm_timer_setup(TIM1);
	rgb_pwm_timer_setup(TIM2);
}

static void clock_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

struct color_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

void set_color(int32_t timer, struct color_t *color, uint8_t r, uint8_t g, uint8_t b)
{
	while (color->r != r || color->g != g || color->b != b) {
		if (color->r < r)
			color->r += 1;
		if (color->r > r)
			color->r -= 1;

		if (color->g < g)
			color->g += 1;
		if (color->g > g)
			color->g -= 1;

		if (color->b < b)
			color->b += 1;
		if (color->b > b)
			color->b -= 1;

		timer_set_oc_value(timer, TIM_OC1, color->r);
		timer_set_oc_value(timer, TIM_OC2, color->g);
		timer_set_oc_value(timer, TIM_OC3, color->b);

		for (int ii = 0; ii < 128000; ii++)
			__asm__("nop");
	}
}

int main(void)
{
	clock_setup();
	timer_setup();

	struct color_t color = {};

	for (;;) {
		set_color(TIM1, &color, 255, 0, 0);
		set_color(TIM1, &color, 0, 255, 0);
		set_color(TIM1, &color, 0, 0, 255);
		set_color(TIM1, &color, 0, 0, 0);

		set_color(TIM2, &color, 255, 0, 0);
		set_color(TIM2, &color, 0, 255, 0);
		set_color(TIM2, &color, 0, 0, 255);
		set_color(TIM2, &color, 0, 0, 0);
	}

	return 0;
}
