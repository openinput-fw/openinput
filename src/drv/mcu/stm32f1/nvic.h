// SPDX-License-Identifier: MIT

#ifndef __NVIC_H__
#define __NVIC_H__

//--------------------------------------------------------------------
// Interrupt utility definitions
//--------------------------------------------------------------------

#define IRQ_GROUP_BITS          3
#define IRQ_SUB_GROUP_BITS      1
#define IRQ_SET_PRIO(i, g, s)   NVIC->IP[i] = ((((g) & ((1 << IRQ_GROUP_BITS) - 1)) << IRQ_SUB_GROUP_BITS) | ((s) & ((1 << IRQ_SUB_GROUP_BITS) - 1))) << (8 - __NVIC_PRIO_BITS)
#define IRQ_GET_PRIO(i)         (NVIC->IP[i] >> (8 - __NVIC_PRIO_BITS))
#define IRQ_ENABLE(i)           NVIC->ISER[i >> 5] = 1 << (i & 0x1F)
#define IRQ_DISABLE(i)          NVIC->ICER[i >> 5] = 1 << (i & 0x1F)
#define IRQ_SET(i)              NVIC->ISPR[i >> 5] = 1 << (i & 0x1F)
#define IRQ_CLEAR(i)            NVIC->ICPR[i >> 5] = 1 << (i & 0x1F)
#define IRQ_STATUS(i)           (NVIC->ISER[i >> 5] & (1 << (i & 0x1F)))
#define IRQ_PENDING(i)          (NVIC->ISPR[i >> 5] & (1 << (i & 0x1F)))

#endif