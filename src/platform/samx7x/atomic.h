/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include <sam.h>

#include "util/types.h"

static inline u32 __iEnableIRQRetVal()
{
	__asm__ volatile("cpsie i" ::: "memory");

	return 1;
}
static inline u32 __iDisableIRQRetVal()
{
	__asm__ volatile("cpsid i" ::: "memory");

	return 1;
}
static inline void __iEnableIRQParam(const u32 *__s)
{
	__asm__ volatile("cpsie i" ::: "memory");
	(void) __s;
}
static inline void __iDisableIRQParam(const u32 *__s)
{
	__asm__ volatile("cpsid i" ::: "memory");
	(void) __s;
}
static inline void __iRestore(const u32 *__s)
{
	if (!*__s)
		__asm__ volatile("cpsie i" ::: "memory");
}

#define ATOMIC_BLOCK(type)    for (type, __ToDo = __iDisableIRQRetVal(); __ToDo; __ToDo = 0)
#define NONATOMIC_BLOCK(type) for (type, __ToDo = __iEnableIRQRetVal(); __ToDo; __ToDo = 0)

#define ATOMIC_RESTORESTATE    u32 primask_save __attribute__((__cleanup__(__iRestore))) = __get_PRIMASK()
#define ATOMIC_FORCEON	       u32 primask_save __attribute__((__cleanup__(__iEnableIRQParam))) = 0
#define NONATOMIC_RESTORESTATE u32 primask_save __attribute__((__cleanup__(__iRestore))) = __get_PRIMASK()
#define NONATOMIC_FORCEOFF     u32 primask_save __attribute__((__cleanup__(__iDisableIRQParam))) = 0
