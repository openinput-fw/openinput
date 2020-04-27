// SPDX-License-Identifier: MIT

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#include "config.h"
#include <stm32f1xx.h>

//--------------------------------------------------------------------
// Atomic blocks
//--------------------------------------------------------------------

static inline uint32_t __iEnableIRQRetVal()
{
    __asm__ volatile ("cpsie i" ::: "memory");

    return 1;
}
static inline uint32_t __iDisableIRQRetVal()
{
    __asm__ volatile ("cpsid i" ::: "memory");

    return 1;
}
static inline void __iEnableIRQParam(const uint32_t *__s)
{
    __asm__ volatile ("cpsie i" ::: "memory");
    (void)__s;
}
static inline void __iDisableIRQParam(const uint32_t *__s)
{
    __asm__ volatile ("cpsid i" ::: "memory");
    (void)__s;
}
static inline void __iRestore(const uint32_t *__s)
{
    if(!*__s)
        __asm__ volatile ("cpsie i" ::: "memory");
}

#define ATOMIC_BLOCK(type) for (type, __ToDo = __iDisableIRQRetVal(); __ToDo; __ToDo = 0)
#define NONATOMIC_BLOCK(type) for (type, __ToDo = __iEnableIRQRetVal(); __ToDo; __ToDo = 0)

#define ATOMIC_RESTORESTATE uint32_t primask_save __attribute__((__cleanup__(__iRestore))) = __get_PRIMASK()
#define ATOMIC_FORCEON uint32_t primask_save __attribute__((__cleanup__(__iEnableIRQParam))) = 0
#define NONATOMIC_RESTORESTATE uint32_t primask_save __attribute__((__cleanup__(__iRestore))) = __get_PRIMASK()
#define NONATOMIC_FORCEOFF uint32_t primask_save __attribute__((__cleanup__(__iDisableIRQParam))) = 0

#endif // _ATOMIC_H_