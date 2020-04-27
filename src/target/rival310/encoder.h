// SPDX-License-Identifier: MIT

#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "config.h"
#include <stm32f1xx.h>
#include "rcc.h"

//--------------------------------------------------------------------
// Function declarations
//--------------------------------------------------------------------

void encoder_init();
void encoder_task();
int8_t encoder_get_delta();

#endif // _ENCODER_H_