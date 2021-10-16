/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
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
void _supc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rstc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rtc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rtt_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _wdt_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pmc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _efc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pioa_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _piob_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pioc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usart2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _piod_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pioe_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _hsmci_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _twihs0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _twihs1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _spi0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _ssc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc0_ch0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc0_ch1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc0_ch2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc1_ch0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc1_ch1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc1_ch2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _afec0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _dacc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pwm0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _icm_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _acc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _usbhs_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _mcan0_int0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _mcan0_int1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _mcan1_int0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _mcan1_int1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _gmac_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _afec1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _twihs2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _spi1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _qspi_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _uart4_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc2_ch0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc2_ch1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc2_ch2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc3_ch0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc3_ch1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _tc3_ch2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _mlb_int0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _mlb_int1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _aes_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _trng_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _xdmac_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _isi_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _pwm1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _fpu_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _sdramc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _rswdt_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _cache_ecc_warning_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _cache_ecc_fault_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _gmac_q1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _gmac_q2_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _fpu_ixc_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _i2sc0_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _i2sc1_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _gmac_q3_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _gmac_q4_isr() __attribute__((weak, alias(DEFAULT_ISR)));
void _gmac_q5_isr() __attribute__((weak, alias(DEFAULT_ISR)));

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
	_supc_isr,
	_rstc_isr,
	_rtc_isr,
	_rtt_isr,
	_wdt_isr,
	_pmc_isr,
	_efc_isr,
	_uart0_isr,
	_uart1_isr,
	0,
	_pioa_isr,
	_piob_isr,
	_pioc_isr,
	_usart0_isr,
	_usart1_isr,
	_usart2_isr,
	_piod_isr,
	_pioe_isr,
	_hsmci_isr,
	_twihs0_isr,
	_twihs1_isr,
	_spi0_isr,
	_ssc_isr,
	_tc0_ch0_isr,
	_tc0_ch1_isr,
	_tc0_ch2_isr,
	_tc1_ch0_isr,
	_tc1_ch1_isr,
	_tc1_ch2_isr,
	_afec0_isr,
	_dacc_isr,
	_pwm0_isr,
	_icm_isr,
	_acc_isr,
	_usbhs_isr,
	_mcan0_int0_isr,
	_mcan0_int1_isr,
	_mcan1_int0_isr,
	_mcan1_int1_isr,
	_gmac_isr,
	_afec1_isr,
	_twihs2_isr,
	_spi1_isr,
	_qspi_isr,
	_uart2_isr,
	_uart3_isr,
	_uart4_isr,
	_tc2_ch0_isr,
	_tc2_ch1_isr,
	_tc2_ch2_isr,
	_tc3_ch0_isr,
	_tc3_ch1_isr,
	_tc3_ch2_isr,
	_mlb_int0_isr,
	_mlb_int1_isr,
	0,
	_aes_isr,
	_trng_isr,
	_xdmac_isr,
	_isi_isr,
	_pwm1_isr,
	_fpu_isr,
	_sdramc_isr,
	_rswdt_isr,
	_cache_ecc_warning_isr,
	_cache_ecc_fault_isr,
	_gmac_q1_isr,
	_gmac_q2_isr,
	_fpu_ixc_isr,
	_i2sc0_isr,
	_i2sc1_isr,
	_gmac_q3_isr,
	_gmac_q4_isr,
	_gmac_q5_isr,
};
