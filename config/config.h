// SPDX-License-Identifier: MIT

#ifndef _CONFIG_H_
#define _CONFIG_H_

#if !defined TARGET_CONFIG_FILE
	#error "No target config"
#endif

#include TARGET_CONFIG_FILE


//--------------------------------------------------------------------
// MCU Family handling
//--------------------------------------------------------------------

#if !defined (STM32F1)

	#error "No valid family defined"

#endif

//--------------------------------------------------------------------
// MCU handling
//--------------------------------------------------------------------

#if !defined (STM32F100xB) && !defined (STM32F100xE) && !defined (STM32F101x6) && \
    !defined (STM32F101xB) && !defined (STM32F101xE) && !defined (STM32F101xG) && !defined (STM32F102x6) && !defined (STM32F102xB) && !defined (STM32F103x6) && \
    !defined (STM32F103xB) && !defined (STM32F103xE) && !defined (STM32F103xG) && !defined (STM32F105xC) && !defined (STM32F107xC)

	#error "No valid MCU defined"

#endif

//--------------------------------------------------------------------
// Optical sensor
//--------------------------------------------------------------------

#if !defined (SENSOR_NONE) && !defined (SENSOR_TRUEMOVE3)

	#warning "SENSOR not defined, defaulting to none"
	#define SENSOR_NONE

#elif defined (SENSOR_TRUEMOVE3)

	#define SENSOR_INCLUDE "truemove3.h"

#endif

//--------------------------------------------------------------------
// Oscillator frequency
//--------------------------------------------------------------------

#if !defined (EXT_OSC_VALUE)

	#define EXT_OSC_VALUE 0UL
	#warning "No EXT_OSC_VALUE defined, defaulting to 0, this defines the external oscillator frequency"

#endif


#endif // _CONFIG_H_