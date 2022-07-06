/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <stm32f1xx.h>

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
	SCB->AIRCR = 0x05FA0000 | (4 << 8); /* Interrupt priority - 3 bits Group, 1 bit Sub-group */
	SCB->SHCSR = SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
		     SCB_SHCSR_MEMFAULTENA_Msk; /* Enable separate fault handlers */

	main();

	__disable_irq();
	while (1) continue;
}

/* Vector table */
void _nmi_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _hardfault_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _memmanage_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _busfault_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usagefault_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _svc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _debugmon_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pendsv_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _systick_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _wwdg_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pvd_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tamper_stamp_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rtc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _flash_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rcc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _exti0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _exti1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _exti2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _exti3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _exti4_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma1_channel1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma1_channel2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma1_channel3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma1_channel4_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma1_channel5_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma1_channel6_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma1_channel7_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _adc1_2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usb_hp_can_tx_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usb_lp_can_rx0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _can_rx1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _can_sce_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _exti9_5_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim1_brk_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim1_up_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim1_trg_com_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim1_cc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim4_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _i2c1_ev_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _i2c1_er_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _i2c2_ev_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _i2c2_er_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _spi1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _spi2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _exti15_10_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rtc_alarm_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usb_wakeup_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim8_brk_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim8_up_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim8_trg_com_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim8_cc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _adc3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _fsmc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _sdio_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim5_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _spi3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart4_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart5_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim6_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tim7_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma2_channel1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma2_channel2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma2_channel3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dma2_channel4_5_isr() __attribute__((weak, alias(DEFAULT_ISR)));

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
	_wwdg_isr, /* 0 */
	_pvd_isr, /* 1 */
	_tamper_stamp_isr, /* 2 */
	_rtc_isr, /* 3 Real time clock */
	_flash_isr, /* 4 Flash controller */
	_rcc_isr, /* 5 */
	_exti0_isr, /* 6 */
	_exti1_isr, /* 7 */
	_exti2_isr, /* 8 */
	_exti3_isr, /* 9 */
	_exti4_isr, /* 10 */
	_dma1_channel1_isr, /* 11 */
	_dma1_channel2_isr, /* 12 */
	_dma1_channel3_isr, /* 13 */
	_dma1_channel4_isr, /* 14 */
	_dma1_channel5_isr, /* 15 */
	_dma1_channel6_isr, /* 16 */
	_dma1_channel7_isr, /* 17 */
	_adc1_2_isr, /* 18 ADC Channel 1 & 2 */
	_usb_hp_can_tx_isr, /* 19 USB high priority / CAN tx */
	_usb_lp_can_rx0_isr, /* 20 USB low priority / CAN rx0 */
	_can_rx1_isr, /* 21 CAN rx1 */
	_can_sce_isr, /* 22 */
	_exti9_5_isr, /* 23 */
	_tim1_brk_isr, /* 24 */
	_tim1_up_isr, /* 25 */
	_tim1_trg_com_isr, /* 26 */
	_tim1_cc_isr, /* 27 */
	_tim2_isr, /* 28 */
	_tim3_isr, /* 29 */
	_tim4_isr, /* 30 */
	_i2c1_ev_isr, /* 31 */
	_i2c1_er_isr, /* 32 */
	_i2c2_ev_isr, /* 33 */
	_i2c2_er_isr, /* 34 */
	_spi1_isr, /* 35 */
	_spi2_isr, /* 36 */
	_usart1_isr, /* 37 */
	_usart2_isr, /* 38 */
	_usart3_isr, /* 39 */
	_exti15_10_isr, /* 40 */
	_rtc_alarm_isr, /* 41 */
	_usb_wakeup_isr, /* 42 */
	_tim8_brk_isr, /* 43 */
	_tim8_up_isr, /* 44 */
	_tim8_trg_com_isr, /* 45 */
	_tim8_cc_isr, /* 46 */
	_adc3_isr, /* 47 */
	_fsmc_isr, /* 48 */
	_sdio_isr, /* 49 */
	_tim5_isr, /* 50 */
	_spi3_isr, /* 51 */
	_uart4_isr, /* 52 */
	_uart5_isr, /* 53 */
	_tim6_isr, /* 54 */
	_tim7_isr, /* 55 */
	_dma2_channel1_isr, /* 56 */
	_dma2_channel2_isr, /* 57 */
	_dma2_channel3_isr, /* 58 */
	_dma2_channel4_5_isr, /* 59 */
	0, /* Reserved */
	0, /* Reserved */
	0, /* Reserved */
	0, /* Reserved */
	0, /* Reserved */
	0, /* Reserved */
	0, /* Reserved */
	0, /* Reserved */
};
