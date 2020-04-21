/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 openinput (https://github.com/openinput-fw)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "target.h"
#include "sensors.h"
#include "systick.h"
#include "rcc.h"
#include "nvic.h"
#include "gpio.h"
#include "flash.h"

//--------------------------------------------------------------------
// Forward USB interrupt events to TinyUSB IRQ Handler
//--------------------------------------------------------------------

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

//--------------------------------------------------------------------
// Sensor API
//--------------------------------------------------------------------



//--------------------------------------------------------------------
// Target Porting API
//--------------------------------------------------------------------

void target_tasks()
{

}

uint64_t target_ticks()
{
	return systemTick;
}

void target_delay_ms(uint16_t ticks)
{
	delay_ms(ticks);
}

//--------------------------------------------------------------------
// outputs API
//--------------------------------------------------------------------

void target_leds_write(uint8_t r, uint8_t g, uint8_t b)
{
  if(g > 127)
	GPIOC->BSRR = BIT(13);
  else
  	GPIOC->BSRR = (BIT(13) << 16);
}

//--------------------------------------------------------------------
// Inputs API
//--------------------------------------------------------------------

deltas_t target_sensor_deltas_get()
{
	deltas_t deltas;

	uint8_t state = 0;
	switch(state)
	{
	case 0:
		deltas.dx = 2;
		deltas.dy = 0;
		break;
	case 1:
		deltas.dx = 0;
		deltas.dy = 2;
		break;
	case 2:
		deltas.dx = -2;
		deltas.dy = 0;
		break;
	case 3:
		deltas.dx = 0;
		deltas.dy = -2;
		break;
	}

	static uint64_t lastStateChange = 0;
	if(systemTick > (lastStateChange + 100))
	{
		++state &= 0x3;
		lastStateChange = systemTick;
	}

	return deltas;
}

uint8_t target_mouse_btns_get()
{
 //     Byte | D7      D6      D5      D4      D3      D2      D1      D0
 //    ------+---------------------------------------------------------------------
 //      0   |  0       0       0    Forward  Back    Middle  Right   Left (Button)

  return  (!(GPIOB->IDR & BIT(0)) << 0) |
          (!(GPIOB->IDR & BIT(1)) << 1) |
          (!(GPIOB->IDR & BIT(2)) << 2) |
          (!(GPIOB->IDR & BIT(3)) << 3) |
          (!(GPIOB->IDR & BIT(4)) << 4);
}

int8_t target_wheel_get()
{
	return 0;
}

//--------------------------------------------------------------------
// Target initialization
//--------------------------------------------------------------------

void init()
{
	flash_latency_config(72000000); // Configure flash latency

	rcc_init(RCC_CFGR_PLLMULL9); // switch to HSE/PLL
	rcc_update_clocks(); // Update clock values

	systick_init(); // Init system tick

	gpio_init_t gpiocfg;

	gpiocfg.port[0].MODE_CFG[0] = (IO_MODE_IN | IO_CFG_IN_FLOAT);   // PA0
	gpiocfg.port[0].MODE_CFG[1] = (IO_MODE_IN | IO_CFG_IN_FLOAT);   // PA1
	gpiocfg.port[0].MODE_CFG[2] = (IO_MODE_IN | IO_CFG_IN_FLOAT);   // PA2
	gpiocfg.port[0].MODE_CFG[3] = (IO_MODE_IN | IO_CFG_IN_FLOAT);	// PA3
	gpiocfg.port[0].MODE_CFG[4] = (IO_MODE_IN | IO_CFG_IN_FLOAT);   // PA4
	gpiocfg.port[0].MODE_CFG[5] = (IO_MODE_IN | IO_CFG_IN_FLOAT);   // PA5
	gpiocfg.port[0].MODE_CFG[6] = (IO_MODE_IN | IO_CFG_IN_FLOAT);   // PA6
	gpiocfg.port[0].MODE_CFG[7] = (IO_MODE_IN | IO_CFG_IN_FLOAT);   // PA7
	gpiocfg.port[0].MODE_CFG[8] = (IO_MODE_IN | IO_CFG_IN_FLOAT);   // PA8
	gpiocfg.port[0].MODE_CFG[9] = (IO_MODE_IN | IO_CFG_IN_FLOAT);   // PA9
	gpiocfg.port[0].MODE_CFG[10] = (IO_MODE_IN | IO_CFG_IN_FLOAT);  // PA10
	gpiocfg.port[0].MODE_CFG[11] = (IO_MODE_IN | IO_CFG_IN_FLOAT);  // PA11 - USB DM
	gpiocfg.port[0].MODE_CFG[12] = (IO_MODE_IN | IO_CFG_IN_FLOAT);  // PA12 - USB DP
	gpiocfg.port[0].MODE_CFG[13] = (IO_MODE_IN | IO_CFG_IN_FLOAT);  // PA13
	gpiocfg.port[0].MODE_CFG[14] = (IO_MODE_IN | IO_CFG_IN_FLOAT);  // PA14
	gpiocfg.port[0].MODE_CFG[15] = (IO_MODE_IN | IO_CFG_IN_FLOAT);  // PA15
	gpiocfg.port[0].ODR = 0;

	gpiocfg.port[1].MODE_CFG[0] = (IO_MODE_IN | IO_CFG_IN_PULL);  	// PB0
	gpiocfg.port[1].MODE_CFG[1] = (IO_MODE_IN | IO_CFG_IN_PULL);  	// PB1
	gpiocfg.port[1].MODE_CFG[2] = (IO_MODE_IN | IO_CFG_IN_PULL);  	// PB2
	gpiocfg.port[1].MODE_CFG[3] = (IO_MODE_IN | IO_CFG_IN_PULL);  	// PB3
	gpiocfg.port[1].MODE_CFG[4] = (IO_MODE_IN | IO_CFG_IN_PULL);  	// PB4
	gpiocfg.port[1].MODE_CFG[5] = (IO_MODE_IN | IO_CFG_IN_FLOAT); 	// PB5
	gpiocfg.port[1].MODE_CFG[6] = (IO_MODE_IN | IO_CFG_IN_FLOAT); 	// PB6
	gpiocfg.port[1].MODE_CFG[7] = (IO_MODE_IN | IO_CFG_IN_FLOAT); 	// PB7
	gpiocfg.port[1].ODR = BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4);	// pull-ups

	gpiocfg.port[2].MODE_CFG[13] = (IO_MODE_OU | IO_CFG_OUT_2M); 	// PC13 - LED

	gpiocfg.port[3].MODE_CFG[0] = (IO_MODE_IN | IO_CFG_IN_FLOAT); 	// PD0 - OSC_IN
	gpiocfg.port[3].MODE_CFG[1] = (IO_MODE_IN | IO_CFG_IN_FLOAT); 	// PD1 - OSC_OUT
	gpiocfg.port[3].ODR = 0;

	gpio_init(&gpiocfg);

	// Pin Remapping
	RCC->APB2RSTR |= RCC_APB2RSTR_AFIORST; // Reset peripheral
	RCC->APB2RSTR &= ~RCC_APB2RSTR_AFIORST;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // Enable AFIO peripheral clock

	AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE; // Disable JTAG and remap TIM3

	// Enable USB peripheral clock
	RCC->APB1RSTR |= RCC_APB1RSTR_USBRST; // Reset peripheral clock
	RCC->APB1RSTR &= ~RCC_APB1RSTR_USBRST;
	RCC->APB1ENR |= RCC_APB1ENR_USBEN; // Enable USB peripheral clock
}