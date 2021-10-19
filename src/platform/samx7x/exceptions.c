/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

void __attribute__((naked, aligned(4))) _hardfault_isr()
{
	__asm__ volatile(" tst lr, #4                                        \n"
			 " ite eq                                                    \n"
			 " mrseq r0, msp                                             \n"
			 " mrsne r0, psp                                             \n"
			 " ldr r1, [r0, #24]                                         \n"
			 " ldr r2, hardfault_trace_stack_addr                        \n"
			 " bx r2                                                     \n"
			 " hardfault_trace_stack_addr: .word hardfault_trace_stack   \n");
}

void __attribute__((naked, aligned(4))) _memmanage_isr()
{
	__asm__ volatile(" tst lr, #4                                        \n"
			 " ite eq                                                    \n"
			 " mrseq r0, msp                                             \n"
			 " mrsne r0, psp                                             \n"
			 " ldr r1, [r0, #24]                                         \n"
			 " ldr r2, memmanage_trace_stack_addr                        \n"
			 " bx r2                                                     \n"
			 " memmanage_trace_stack_addr: .word memmanage_trace_stack   \n");
}

void __attribute__((naked, aligned(4))) _busfault_isr()
{
	__asm__ volatile(" tst lr, #4                                        \n"
			 " ite eq                                                    \n"
			 " mrseq r0, msp                                             \n"
			 " mrsne r0, psp                                             \n"
			 " ldr r1, [r0, #24]                                         \n"
			 " ldr r2, busfault_trace_stack_addr                         \n"
			 " bx r2                                                     \n"
			 " busfault_trace_stack_addr: .word busfault_trace_stack     \n");
}

void __attribute__((naked, aligned(4))) _usagefault_isr()
{
	__asm__ volatile(" tst lr, #4                                        \n"
			 " ite eq                                                    \n"
			 " mrseq r0, msp                                             \n"
			 " mrsne r0, psp                                             \n"
			 " ldr r1, [r0, #24]                                         \n"
			 " ldr r2, usagefault_trace_stack_addr                       \n"
			 " bx r2                                                     \n"
			 " usagefault_trace_stack_addr: .word usagefault_trace_stack \n");
}

void hardfault_trace_stack(uint32_t *pulFaultStackAddress)
{
	(void) pulFaultStackAddress;
	while (1) continue;
}

void memmanage_trace_stack(uint32_t *pulFaultStackAddress)
{
	(void) pulFaultStackAddress;
	while (1) continue;
}

void busfault_trace_stack(uint32_t *pulFaultStackAddress)
{
	(void) pulFaultStackAddress;
	while (1) continue;
}

void usagefault_trace_stack(uint32_t *pulFaultStackAddress)
{
	(void) pulFaultStackAddress;
	while (1) continue;
}
