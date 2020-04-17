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

ST_HAL_DRIVER_DIR = src/al/STM32F1xx_HAL_Driver
CMSIS_ST_DIR = src/cmsis/st/STM32F1xx
CMSIS_DIR = src/cmsis/core

SRC_C += \
  $(CMSIS_ST_DIR)/src/system_stm32f1xx.c \
  $(ST_HAL_DRIVER_DIR)/src/stm32f1xx_hal.c \
  $(ST_HAL_DRIVER_DIR)/src/stm32f1xx_hal_cortex.c \
  $(ST_HAL_DRIVER_DIR)/src/stm32f1xx_hal_rcc.c \
  $(ST_HAL_DRIVER_DIR)/src/stm32f1xx_hal_rcc_ex.c \
  $(ST_HAL_DRIVER_DIR)/src/stm32f1xx_hal_gpio.c

SRC_S += \
  $(CMSIS_ST_DIR)/src/startup_stm32f103xb.s

INC += \
  $(CMSIS_DIR)/CMSIS/Core/include \
  $(CMSIS_ST_DIR)/include \
  $(ST_HAL_DRIVER_DIR)/include \

# ---------------------------------------
# Tiny USB
# ---------------------------------------

TUSB_VENDOR = st
TUSB_CHIP_FAMILY = stm32_fsdev

CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_STM32F1

include targets/generic/tinyusb.mk