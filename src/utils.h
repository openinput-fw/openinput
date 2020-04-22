// SPDX-License-Identifier: MIT

#ifndef __UTILS_H__
#define __UTILS_H__

//--------------------------------------------------------------------
// Utility macros
//--------------------------------------------------------------------

// Macro to make a dummy read
#define REG_DISCARD(reg) __asm__ volatile ("" : : "r" (*(volatile uint32_t *)(reg)))

// Macro to get the bit value
#define BIT(x) (1 << (x))

// Sign of, 1 if (+) 0 if (-)
#define SIGN(a)  (!(a < 0))

// Absolute value of
#define ABS(a)  ((a) < 0 ? (-(a)) : (a))

// Macros that get the lesser/greater of two values
#define MAX(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define CLIP(l,a,h) (MAX(MIN(h,a),l))

// Swap two variables
#define SWAP(a, b)  do{ typeof(a) SWAP = a; a = b; b = SWAP; }while(0)

#endif
