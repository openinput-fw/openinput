/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#include <em_device.h>

#include "util/types.h"

/* Stack pointer */
extern void _estack(); /* Not a function, just to be compatible with the vector table array */

/* Variables */
extern u32 _svect; /* ISR Vectors */
extern u32 _evect;

extern u32 _stext; /* Main program */
extern u32 _etext;

extern u32 _sidata; /* Data source */
extern u32 _sdata; /* Data destination */
extern u32 _edata;

extern u32 _sbss; /* BSS destination */
extern u32 _ebss;

extern u32 _end;

/* Functions */
void _default_isr()
{
	while (1) continue;
}

void __attribute__((weak)) __libc_init_array()
{
}

extern void __attribute__((noreturn)) main();

#define DEFAULT_ISR "_default_isr"

void _reset_isr()
{
	volatile u32 *src, *dst;

	src = &_sidata;
	dst = &_sdata;

	while (dst < &_edata) /* Copy data */
		*(dst++) = *(src++);

	src = 0;
	dst = &_sbss;

	while (dst < &_ebss) /* Zero BSS */
		*(dst++) = 0;

	__libc_init_array();

	SCB->VTOR = (u32) &_svect; /* ISR Vectors offset */
	SCB->AIRCR = 0x05FA0000 | (5 << 8); /* Interrupt priority - 2 bits Group, 1 bit Sub-group */
	SCB->SHCSR = SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
		     SCB_SHCSR_MEMFAULTENA_Msk; /* Enable separate fault handlers */
	SCB->SCR |= SCB_SCR_SEVONPEND_Msk; /* Pending disabled interrupt generates event */
	SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk; /* Enable division by zero faults */
	SCB->CPACR |= 0xF << 20; /* Enable CP10 & CP11 (FPU) in priv. and non priv. mode */

	main();

	__disable_irq();
	while (1) continue;
}

/* Cortex-M Exception Handlers */
void _nmi_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _hardfault_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _memmanage_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _busfault_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usagefault_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _svc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _debugmon_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pendsv_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _systick_isr() __attribute__((weak, alias(DEFAULT_ISR)));
/* External interrupts */
void _emu_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _wdog0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _ldma_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _gpio_even_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _smu_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _timer0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart0_rx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart0_tx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _acmp0_1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _adc0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _idac0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _i2c0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _i2c1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _gpio_odd_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _timer1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _timer2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _timer3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart1_rx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart1_tx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart2_rx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart2_tx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart0_rx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart0_tx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart1_rx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart1_tx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _leuart0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _leuart1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _letimer0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pcnt0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pcnt1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pcnt2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rtcc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _cmu_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _msc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _crypto0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _cryotimer_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _fpueh_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart3_rx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart3_tx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart4_rx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart4_tx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _wtimer0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _wtimer1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _vdac0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _csen_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _lesense_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _ebi_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _acmp2_3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _adc1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _lcd_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _sdio_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _can0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _can1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usb_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rtc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _wdog1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _letimer1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _trng0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _qspi0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pdm_isr() __attribute__((weak, alias(DEFAULT_ISR)));

/* Vector table */
__attribute__((section(".isr_vector"), used)) void (*const g_pfnVectors[])() = {
	/* Cortex-M Exception Handlers */
	_estack, /* Stack pointer */
	_reset_isr, /* Reset */
	_nmi_isr, /* Non-maskable interrupt */
	_hardfault_isr, /* Hard fault */
	_memmanage_isr, /*  */
	_busfault_isr, /* Bus Fault */
	_usagefault_isr, /* Usage fault */
	0, /* Reserved */
	0, /* Reserved */
	0, /* Reserved */
	0, /* Reserved */
	_svc_isr, /* SVCall */
	_debugmon_isr, /* Debug Monitor */
	0, /* Reserved */
	_pendsv_isr, /* PendSV */
	_systick_isr, /* System tick */
	/* External interrupts */
	_emu_isr, /* 0 Energy management unit */
	_wdog0_isr, /* 1 Watch dog timer 0 */
	_ldma_isr, /* 2 Linked DMA */
	_gpio_even_isr, /* 3 Even numbered GPIO interrupt */
	_smu_isr, /* 4 */
	_timer0_isr, /* 5 */
	_usart0_rx_isr, /* 6 */
	_usart0_tx_isr, /* 7 */
	_acmp0_1_isr, /* 8 */
	_adc0_isr, /* 9 */
	_idac0_isr, /* 10 */
	_i2c0_isr, /* 11 */
	_i2c1_isr, /* 12 */
	_gpio_odd_isr, /* 13 Odd numbered GPIO interrupt */
	_timer1_isr, /* 14 Timer counter 1 */
	_timer2_isr, /* 15 Timer counter 2 */
	_timer3_isr, /* 16 Timer counter 3 */
	_usart1_rx_isr, /* 17 */
	_usart1_tx_isr, /* 18 */
	_usart2_rx_isr, /* 19 */
	_usart2_tx_isr, /* 20 */
	_uart0_rx_isr, /* 21 */
	_uart0_tx_isr, /* 22 */
	_uart1_rx_isr, /* 23 */
	_uart1_tx_isr, /* 24 */
	_leuart0_isr, /* 25 */
	_leuart1_isr, /* 26 */
	_letimer0_isr, /* 27 */
	_pcnt0_isr, /* 28 */
	_pcnt1_isr, /* 29 */
	_pcnt2_isr, /* 30 */
	_rtcc_isr, /* 31 */
	_cmu_isr, /* 32 */
	_msc_isr, /* 33 */
	_crypto0_isr, /* 34 */
	_cryotimer_isr, /* 35 */
	_fpueh_isr, /* 36 */
	_usart3_rx_isr, /* 37 */
	_usart3_tx_isr, /* 38 */
	_usart4_rx_isr, /* 39 */
	_usart4_tx_isr, /* 40 */
	_wtimer0_isr, /* 41 */
	_wtimer1_isr, /* 42 */
	_vdac0_isr, /* 43 Digital to analog coverter */
	_csen_isr, /* 44 */
	_lesense_isr, /* 45 Low energy sensor interface */
	_ebi_isr, /* 46 External bus interface */
	_acmp2_3_isr, /* 47 Analog comparator 2 & 3 */
	_adc1_isr, /* 48 ADC channel 1 */
	_lcd_isr, /* 49 LCd Driver */
	_sdio_isr, /* 50 */
	_can0_isr, /* 51 */
	_can1_isr, /* 52 */
	_usb_isr, /* 53 USB controller */
	_rtc_isr, /* 54 */
	_wdog1_isr, /* 55 Watchdog timer 1 */
	_letimer1_isr, /* 56 Low energy timer 1 */
	_trng0_isr, /* 57 True random number generator */
	_qspi0_isr, /* 58 Quad SPI flash controller */
	_pdm_isr, /* 59 PDM interface */
};
