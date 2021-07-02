/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

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
	SCB->AIRCR = 0x05FA0000 | (4 << 8); /* Interrupt priority - 3 bits Group, 1 bit Sub-group */
	SCB->SHCSR = SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
		     SCB_SHCSR_MEMFAULTENA_Msk; /* Enable separate fault handlers */

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
void _supc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rstc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rtc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rtt_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _wdt_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pmc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _efc0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
#ifdef _SAM3U_EFC1_INSTANCE_
void _efc1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
#endif /* _SAM3U_EFC1_INSTANCE_ */
void _uart_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pioa_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _piob_isr() __attribute__((weak, alias(DEFAULT_ISR)));
#ifdef _SAM3U_PIOC_INSTANCE_
void _pioc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
#endif /* _SAM3U_PIOC_INSTANCE_ */
void _usart0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
#ifdef _SAM3U_USART3_INSTANCE_
void _usart3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
#endif /* _SAM3U_USART3_INSTANCE_ */
void _hsmci_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _twi0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _twi1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _spi_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _ssc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pwm_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _adc12b_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _adc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dmac_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _udphs_isr() __attribute__((weak, alias(DEFAULT_ISR)));

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
	_supc_isr, /* 0  Supply Controller */
	_rstc_isr, /* 1  Reset Controller */
	_rtc_isr, /* 2  Real Time Clock */
	_rtt_isr, /* 3  Real Time Timer */
	_wdt_isr, /* 4  Watchdog Timer */
	_pmc_isr, /* 5  Power Management Controller */
	_efc0_isr, /* 6  Enhanced Embedded Flash Controller 0 */
#ifdef _SAM3U_EFC1_INSTANCE_
	_efc1_isr, /* 7  Enhanced Embedded Flash Controller 1 */
#else
	0, /* 7  Reserved */
#endif /* _SAM3U_EFC1_INSTANCE_ */
	_uart_isr, /* 8  Universal Asynchronous Receiver Transmitter */
	0, /* 9  Reserved */
	_pioa_isr, /* 10 Parallel I/O Controller A, */
	_piob_isr, /* 11 Parallel I/O Controller B */
#ifdef _SAM3U_PIOC_INSTANCE_
	_pioc_isr, /* 12 Parallel I/O Controller C */
#else
	0, /* 12 Reserved */
#endif /* _SAM3U_PIOC_INSTANCE_ */
	_usart0_isr, /* 13 USART 0 */
	_usart1_isr, /* 14 USART 1 */
	_usart2_isr, /* 15 USART 2 */
#ifdef _SAM3U_USART3_INSTANCE_
	_usart3_isr, /* 16 USART 3 */
#else
	0, /* 16 Reserved */
#endif /* _SAM3U_USART3_INSTANCE_ */
	_hsmci_isr, /* 17 High Speed Multimedia Card Interface */
	_twi0_isr, /* 18 Two-Wire Interface 0 */
	_twi1_isr, /* 19 Two-Wire Interface 1 */
	_spi_isr, /* 20 Serial Peripheral Interface */
	_ssc_isr, /* 21 Synchronous Serial Controller */
	_tc0_isr, /* 22 Timer Counter 0 */
	_tc1_isr, /* 23 Timer Counter 1 */
	_tc2_isr, /* 24 Timer Counter 2 */
	_pwm_isr, /* 25 Pulse Width Modulation Controller */
	_adc12b_isr, /* 26 12-bit ADC Controller */
	_adc_isr, /* 27 10-bit ADC Controller */
	_dmac_isr, /* 28 DMA Controller */
	_udphs_isr, /* 29 USB Device High Speed */
};
