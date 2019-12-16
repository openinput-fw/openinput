#include "leds.h"

/* holds information about a led instance */
typedef struct led_t_struct {
    uint32_t mode; // upper 8 bits = mode, lower 16 bits = period, leftover is reserved for now
    color_t color;
    int32_t timer; // hardware timer this led is coupled to
    int32_t redChannel;
    int32_t GreenChannel;
    int32_t BlueChannel;
} led_t;

led_t leds[NUM_LEDS];

/* initialize a timer for led rgb */
static void rgb_pwm_timer_setup(int32_t timer)
{
	timer_disable_counter(timer);
	/*timer_reset(timer);*/

	timer_set_mode(timer,
		       TIM_CR1_CKD_CK_INT,
		       TIM_CR1_CMS_EDGE,
		       TIM_CR1_DIR_UP);

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

/* initialize leds, eventually dependant on target */
void leds_init()
{
    /* ---------------------------------------------------------------------- */
    /* setup pin modes*/
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);

	/* TIM1 gpio setup */
	gpio_set_mode(GPIOA,
		      GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
		      GPIO8 | GPIO9 | GPIO10);

	/* TIM2 gpio setup */
	gpio_set_mode(GPIOA,
		      GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
		      GPIO0 | GPIO1 | GPIO2);

	/* enable timers */
	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_TIM2);

	/* configure timers to generate a pwm signal to control rgb leds */
	rgb_pwm_timer_setup(TIM1);
	rgb_pwm_timer_setup(TIM2);

    leds[0].mode = 0x00000000;
    leds[0].timer = TIM1;
    leds[0].color.r = 0x00;
    leds[0].color.g = 0x00;
    leds[0].color.b = 0x00;
	leds[0].redChannel = TIM_OC2;
    leds[0].GreenChannel = TIM_OC1;
    leds[0].BlueChannel = TIM_OC3;

    leds[1].mode = 0x00000000;
    leds[1].timer = TIM2;
    leds[1].color.r = 0x00;
    leds[1].color.g = 0x00;
    leds[1].color.b = 0x00;
	leds[1].redChannel = TIM_OC1;
    leds[1].GreenChannel = TIM_OC2;
    leds[1].BlueChannel = TIM_OC3;
}
/* periodic task for leds, temporary implementation for testing purposes */
void leds_task()
{
    for(uint8_t i = 0; i < NUM_LEDS; i++) // cycle through leds, try to achieve target color
    {
        static color_t current_color[NUM_LEDS] = {};

        if(current_color[i].r < leds[i].color.r) current_color[i].r += 1;
		if(current_color[i].r > leds[i].color.r) current_color[i].r -= 1;

		if(current_color[i].g < leds[i].color.g) current_color[i].g += 1;
		if(current_color[i].g > leds[i].color.g) current_color[i].g -= 1;

		if(current_color[i].b < leds[i].color.b) current_color[i].b += 1;
		if(current_color[i].b > leds[i].color.b) current_color[i].b -= 1;

		timer_set_oc_value(leds[i].timer, leds[i].redChannel, current_color[i].r);
		timer_set_oc_value(leds[i].timer, leds[i].GreenChannel, current_color[i].g);
		timer_set_oc_value(leds[i].timer, leds[i].BlueChannel, current_color[i].b);
    }
}
/* modify led instance */
void leds_set(uint8_t led, uint8_t mode, uint16_t period, color_t color)
{
    if((led < 0) || (led >= NUM_LEDS)) return;

    leds[led].mode = (mode << 24) | period;
    leds[led].color = color;
}