// SPDX-License-Identifier: MIT

#include <stm32f1xx.h>
#include "utils.h"

//--------------------------------------------------------------------
// Exception handlers
//--------------------------------------------------------------------

void trace_stack(uint32_t *pulFaultStackAddress);

void __attribute__ ((naked)) _hardfault_isr()
{
    __asm__ volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, hardfault_trace_stack_addr                        \n"
        " bx r2                                                     \n"
        " hardfault_trace_stack_addr: .word hardfault_trace_stack   \n"
    );
}

void __attribute__ ((naked)) _memmanage_isr()
{
    __asm__ volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, memmanage_trace_stack_addr                        \n"
        " bx r2                                                     \n"
        " memmanage_trace_stack_addr: .word memmanage_trace_stack   \n"
    );
}

void __attribute__ ((naked)) _busfault_isr()
{
    __asm__ volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, busfault_trace_stack_addr                         \n"
        " bx r2                                                     \n"
        " busfault_trace_stack_addr: .word busfault_trace_stack     \n"
    );
}

void __attribute__ ((naked)) _usagefault_isr()
{
    __asm__ volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, usagefault_trace_stack_addr                       \n"
        " bx r2                                                     \n"
        " usagefault_trace_stack_addr: .word usagefault_trace_stack \n"
    );
}

void hardfault_trace_stack(uint32_t *pulFaultStackAddress)
{
    (void) pulFaultStackAddress;
    GPIOA->BSRR = BIT(1) | BIT(9); // Static red Leds for signaling
    while(1);
}

void memmanage_trace_stack(uint32_t *pulFaultStackAddress)
{
    (void) pulFaultStackAddress;
    GPIOA->BSRR = BIT(1) | BIT(9); // Static red Leds for signaling
    while(1);
}

void busfault_trace_stack(uint32_t *pulFaultStackAddress)
{
    (void) pulFaultStackAddress;
    GPIOA->BSRR = BIT(1) | BIT(9); // Static red Leds for signaling
    while(1);
}

void usagefault_trace_stack(uint32_t *pulFaultStackAddress)
{
    (void) pulFaultStackAddress;
    GPIOA->BSRR = BIT(1) | BIT(9); // Static red Leds for signaling
    while(1);
}