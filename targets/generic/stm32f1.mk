# ---------------------------------------
# CROSS COMPILER
# ---------------------------------------

CROSS_COMPILE = arm-none-eabi-

# ---------------------------------------
# FLAGS
# ---------------------------------------

CFLAGS += \
	-flto \
	-mthumb \
	-mabi=aapcs \
	-mcpu=cortex-m3 \
	-mfloat-abi=soft \
	-nostdlib -nostartfiles \
	-fdata-sections \
	-ffunction-sections \
	-fsingle-precision-constant \
	-fno-strict-aliasing \
	-Wdouble-promotion \
	-Wstrict-prototypes \
	-Wall \
	-Wextra \
	-Werror \
	-Wfatal-errors \
	-Werror-implicit-function-declaration \
	-Wfloat-equal \
	-Wundef \
	-Wshadow \
	-Wwrite-strings \
	-Wsign-compare \
	-Wmissing-format-attribute \
	-Wunreachable-code

# ---------------------------------------
# SOURCES
# ---------------------------------------

ST_HAL_DRIVER = src/al/STM32F1xx_HAL_Driver
ST_CMSIS = src/cmsis/st/STM32F1xx
CMSIS = src/cmsis/core

SRC_C += \
  $(ST_CMSIS)/src/system_stm32f1xx.c \
  $(ST_HAL_DRIVER)/src/stm32f1xx_hal.c \
  $(ST_HAL_DRIVER)/src/stm32f1xx_hal_cortex.c \
  $(ST_HAL_DRIVER)/src/stm32f1xx_hal_rcc.c \
  $(ST_HAL_DRIVER)/src/stm32f1xx_hal_rcc_ex.c \
  $(ST_HAL_DRIVER)/src/stm32f1xx_hal_gpio.c

SRC_S += \
  $(ST_CMSIS)/src/startup_stm32f103xb.s

INC += \
  $(CMSIS)/include \
  $(ST_CMSIS)/include \
  $(ST_HAL_DRIVER)/include \

# ---------------------------------------
# Tiny USB
# ---------------------------------------

VENDOR = st
CHIP_FAMILY = stm32_fsdev

# For freeRTOS port source
FREERTOS_PORT = ARM_CM3

CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_STM32F1

include targets/generic/tinyusb.mk