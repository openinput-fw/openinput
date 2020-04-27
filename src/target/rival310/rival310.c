// SPDX-License-Identifier: MIT

#include "config.h"
#include "target.h"
#include "systick.h"
#include "rcc.h"
#include "nvic.h"
#include "spi.h"
#include "gpio.h"
#include "spi_eeprom.h"
#include "flash.h"
#include "leds.h"
#include "encoder.h"

#if !defined (SENSOR_NONE)
	#include SENSOR_INCLUDE
#endif

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

void dcd_connect(uint8_t rhport)
{
  (void) rhport;
  GPIOA->BSRR = BIT(15);
}

void dcd_disconnect(uint8_t rhport)
{
  (void) rhport;
  GPIOA->BSRR = (BIT(15) << 16);
}

//--------------------------------------------------------------------
// Sensor API
//--------------------------------------------------------------------

void _exti3_isr()
{
  if(EXTI->PR & EXTI_PR_PR3)
  {
    EXTI->PR = EXTI_PR_PR3;

    truemove3_motion_event();
  }
}

uint8_t truemove3_spi_transfer(uint8_t byte)
{
  return spi_transfer_byte(byte);
}

void truemove3_select()
{
  GPIOA->BSRR = (BIT(4) << 16);
}

void truemove3_unselect()
{
  GPIOA->BSRR = BIT(4);
}

void truemove3_delay_ms(uint32_t ticks)
{
  delay_ms(ticks);
}

void truemove3_delay_us(uint32_t ticks)
{
  delay_us(ticks);
}

//--------------------------------------------------------------------
// Target Porting API
//--------------------------------------------------------------------

void target_tasks()
{
  truemove3_task();

  encoder_task();
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
  led_logo_write(r, g, b);
  led_wheel_write(r, g, b);
}

//--------------------------------------------------------------------
// Inputs API
//--------------------------------------------------------------------

deltas_t target_sensor_deltas_get()
{
  return truemove3_get_deltas();
}

uint8_t target_mouse_btns_get()
{
 //     Byte | D7      D6      D5      D4      D3      D2      D1      D0
 //    ------+---------------------------------------------------------------------
 //      0   |  0       0       0    Forward  Back    Middle  Right   Left (Button)

  return  (!(GPIOB->IDR & BIT(0)) << 0) |
          (!(GPIOB->IDR & BIT(1)) << 1) |
          (!(GPIOB->IDR & BIT(2)) << 2) |
          (!(GPIOB->IDR & BIT(6)) << 3) |
          (!(GPIOB->IDR & BIT(7)) << 4);
}

int8_t target_wheel_get()
{
  return encoder_get_delta();
}

//--------------------------------------------------------------------
// Target initialization
//--------------------------------------------------------------------

void init()
{
  flash_latency_config(72000000); // Configure flash latency

  rcc_init(RCC_CFGR_PLLMULL6); // switch to HSE/PLL
  rcc_update_clocks(); // Update clock values

  systick_init(); // Init system tick

  gpio_init_t gpiocfg;

  gpiocfg.port[0].MODE_CFG[0] = (IO_MODE_OUT_10M | IO_CFG_OUT_AF_PP);   // PA0  - AF (TIM2CC)       - Rival310 Scroll RGB Green
  gpiocfg.port[0].MODE_CFG[1] = (IO_MODE_OUT_10M | IO_CFG_OUT_AF_PP);   // PA1  - AF (TIM2CC)       - Rival310 Scroll RGB Red
  gpiocfg.port[0].MODE_CFG[2] = (IO_MODE_OUT_10M | IO_CFG_OUT_AF_PP);   // PA2  - AF (TIM2CC)       - Rival310 Scroll RGB Blue
  gpiocfg.port[0].MODE_CFG[3] = (IO_MODE_IN | IO_CFG_IN_PULL);          // PA3  - GP (IRQ)          - TrueMove3 Motion
  gpiocfg.port[0].MODE_CFG[4] = (IO_MODE_OUT_10M | IO_CFG_OUT_GP_PP);   // PA4  - GP (Chip select)  - TrueMove3(NCS) Flash(CS)
  gpiocfg.port[0].MODE_CFG[5] = (IO_MODE_OUT_50M | IO_CFG_OUT_AF_PP);   // PA5  - AF (SCK)          - TrueMove3 + Flash SCK
  gpiocfg.port[0].MODE_CFG[6] = (IO_MODE_IN | IO_CFG_IN_FLOAT);         // PA6  - AF (MISO)         - TrueMove3 + Flash MISO
  gpiocfg.port[0].MODE_CFG[7] = (IO_MODE_OUT_50M | IO_CFG_OUT_AF_PP);   // PA7  - AF (MOSI)         - TrueMove3 + Flash MOSI
  gpiocfg.port[0].MODE_CFG[8] = (IO_MODE_OUT_10M | IO_CFG_OUT_AF_PP);   // PA8  - AF (TIM1CC)       - Rival 310 Logo RGB Green
  gpiocfg.port[0].MODE_CFG[9] = (IO_MODE_OUT_10M | IO_CFG_OUT_AF_PP);   // PA9  - AF (TIM1CC)       - Rival 310 Logo RGB Red
  gpiocfg.port[0].MODE_CFG[10] = (IO_MODE_OUT_10M | IO_CFG_OUT_AF_PP);  // PA10 - AF (TIM1CC)       - Rival 310 Logo RGB Blue
  gpiocfg.port[0].MODE_CFG[11] = (IO_MODE_IN | IO_CFG_IN_FLOAT);        // PA11 -                   - Rival 310 USB DM
  gpiocfg.port[0].MODE_CFG[12] = (IO_MODE_IN | IO_CFG_IN_FLOAT);        // PA12 -                   - Rival 310 USB DP
  gpiocfg.port[0].MODE_CFG[13] = (IO_MODE_OUT_50M | IO_CFG_OUT_AF_PP);  // PA13 - AF (SWDIO) Output 50MHz push-pull
  gpiocfg.port[0].MODE_CFG[14] = (IO_MODE_OUT_50M | IO_CFG_OUT_AF_PP);  // PA14 - AF (SWCLK) Output 50MHz push-pull
  gpiocfg.port[0].MODE_CFG[15] = (IO_MODE_OUT_2M | IO_CFG_OUT_GP_PP);   // PA15 - Rival 310 USB DP pull-up
  gpiocfg.port[0].ODR = 0;

  gpiocfg.port[1].MODE_CFG[0] = (IO_MODE_IN | IO_CFG_IN_PULL);  // PB0 - GP (IO)        - Rival310 Left Button
  gpiocfg.port[1].MODE_CFG[1] = (IO_MODE_IN | IO_CFG_IN_PULL);  // PB1 - GP (IO)        - Rival310 Scroll Button
  gpiocfg.port[1].MODE_CFG[2] = (IO_MODE_IN | IO_CFG_IN_PULL);  // PB2 - GP (IO)        - Rival310 Right Button
  gpiocfg.port[1].MODE_CFG[3] = (IO_MODE_IN | IO_CFG_IN_PULL);  // PB3 - AF (SWO) Output 50MHz push-pull    - Rival310 DPI Button
  gpiocfg.port[1].MODE_CFG[4] = (IO_MODE_IN | IO_CFG_IN_PULL);  // PB4 - AF (TIM3CH1(Remaped))   - Rival310 Encoder A
  gpiocfg.port[1].MODE_CFG[5] = (IO_MODE_IN | IO_CFG_IN_PULL);  // PB5 - AF (TIM3CH2(Remaped))   - Rival310 Encoder B
  gpiocfg.port[1].MODE_CFG[6] = (IO_MODE_IN | IO_CFG_IN_PULL);  // PB6 - GP (IO)        - Rival310 Back Button
  gpiocfg.port[1].MODE_CFG[7] = (IO_MODE_IN | IO_CFG_IN_PULL);  // PB7 - GP (IO)        - Rival310 Forward Button
  gpiocfg.port[1].ODR = BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7);  // pull-ups

  gpiocfg.port[3].MODE_CFG[0] = (IO_MODE_IN | IO_CFG_IN_FLOAT); // PD0 - OSC_IN
  gpiocfg.port[3].MODE_CFG[1] = (IO_MODE_IN | IO_CFG_IN_FLOAT); // PD1 - OSC_OUT
  gpiocfg.port[3].ODR = 0;

  gpio_init(&gpiocfg);

  // AFIO
  RCC->APB2RSTR |= RCC_APB2RSTR_AFIORST; // Reset peripheral
  RCC->APB2RSTR &= ~RCC_APB2RSTR_AFIORST;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // Enable AFIO peripheral clock

  // Pin Remapping
  AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE | AFIO_MAPR_TIM3_REMAP_PARTIALREMAP; // Disable JTAG and remap TIM3 (Partial remap (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1))

  // External Interrupts
  AFIO->EXTICR[0] = IO_EXTI_CFG(3, IO_EXTI_PORTA);
  AFIO->EXTICR[1] = 0;
  AFIO->EXTICR[2] = 0;
  AFIO->EXTICR[3] = 0;

  leds_init();
  target_leds_write(255, 255, 0);

  spi_init(SPI_MODE3, SPI_CLOCK_DIV_64, SPI_MSB_FIRST);

  uint8_t truemove3Firmware[4094];
  for(uint16_t b = 0; b < 4094; b += 128)
  {
    spi_eeprom_read(0x6100 + b, truemove3Firmware + b, 128);
  }

  if(!truemove3_init(truemove3Firmware)) // initialize sensor with local firmware
  {
    while(1) // failed initialization
    {
      uint64_t lastBlink = 0;
      if(systemTick > (lastBlink + 250))
      {
        static uint8_t state;
        target_leds_write(state*255, 0, 0);
        state ^= 1;
        lastBlink = systemTick;
      }
    }
  }

  // EXTI 3 - Truemove3 motion input
  EXTI->IMR |= EXTI_IMR_MR3; // Enable interrupt
  EXTI->EMR |= EXTI_EMR_MR3; // Enable event
  EXTI->FTSR |= EXTI_FTSR_TR3; // Enable falling trigger
  EXTI->RTSR &= ~EXTI_RTSR_TR3; // Disable rising trigger
  IRQ_SET_PRIO(EXTI3_IRQn, 1, 0);
  IRQ_CLEAR(EXTI3_IRQn);
  IRQ_ENABLE(EXTI3_IRQn);

  encoder_init();

  // Enable USB peripheral clock
  RCC->APB1RSTR |= RCC_APB1RSTR_USBRST; // Reset peripheral clock
  RCC->APB1RSTR &= ~RCC_APB1RSTR_USBRST;
  RCC->APB1ENR |= RCC_APB1ENR_USBEN; // Enable USB peripheral clock
}