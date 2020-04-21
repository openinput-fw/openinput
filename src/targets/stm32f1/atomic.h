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

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

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