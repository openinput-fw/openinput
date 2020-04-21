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
 */

#include <stm32f1xx.h>

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

    while(1);
}

void memmanage_trace_stack(uint32_t *pulFaultStackAddress)
{
    (void) pulFaultStackAddress;

    while(1);
}

void busfault_trace_stack(uint32_t *pulFaultStackAddress)
{
    (void) pulFaultStackAddress;

    while(1);
}

void usagefault_trace_stack(uint32_t *pulFaultStackAddress)
{
    (void) pulFaultStackAddress;

    while(1);
}