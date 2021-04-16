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
	u32 *src, *dst;

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

__attribute__((section(".isr_vector"), used)) void (*const g_pfnVectors[])() = {
	/* Cortex-M Exception Handlers */
	_estack,
	_reset_isr,
	_nmi_isr,
	_hardfault_isr,
	_memmanage_isr,
	_busfault_isr,
	_usagefault_isr,
	0,
	0,
	0,
	0,
	_svc_isr,
	_debugmon_isr,
	0,
	_pendsv_isr,
	_systick_isr,
	/* External interrupts */
	_emu_isr,
	_wdog0_isr,
	_ldma_isr,
	_gpio_even_isr,
	_smu_isr,
	_timer0_isr,
	_usart0_rx_isr,
	_usart0_tx_isr,
	_acmp0_1_isr,
	_adc0_isr,
	_idac0_isr,
	_i2c0_isr,
	_i2c1_isr,
	_gpio_odd_isr,
	_timer1_isr,
	_timer2_isr,
	_timer3_isr,
	_usart1_rx_isr,
	_usart1_tx_isr,
	_usart2_rx_isr,
	_usart2_tx_isr,
	_uart0_rx_isr,
	_uart0_tx_isr,
	_uart1_rx_isr,
	_uart1_tx_isr,
	_leuart0_isr,
	_leuart1_isr,
	_letimer0_isr,
	_pcnt0_isr,
	_pcnt1_isr,
	_pcnt2_isr,
	_rtcc_isr,
	_cmu_isr,
	_msc_isr,
	_crypto0_isr,
	_cryotimer_isr,
	_fpueh_isr,
	_usart3_rx_isr,
	_usart3_tx_isr,
	_usart4_rx_isr,
	_usart4_tx_isr,
	_wtimer0_isr,
	_wtimer1_isr,
	_vdac0_isr,
	_csen_isr,
	_lesense_isr,
	_ebi_isr,
	_acmp2_3_isr,
	_adc1_isr,
	_lcd_isr,
	_sdio_isr,
	_can0_isr,
	_can1_isr,
	_usb_isr,
	_rtc_isr,
	_wdog1_isr,
	_letimer1_isr,
	_trng0_isr,
	_qspi0_isr,
	_pdm_isr,
};
