# ---------------------------------------
# FAMILY
# ---------------------------------------

include targets/generic/stm32f1.mk

# ---------------------------------------
# LINKER
# ---------------------------------------

LD_FILE = targets/ld/rival310.ld

# ---------------------------------------
# FLAGS
# ---------------------------------------

CFLAGS += -DSTM32F103x6 \
		  -DSTM32F10X_LD

# ---------------------------------------
# SOURCES
# ---------------------------------------

SRC_C += $(subst ,,$(wildcard src/target/$(TARGET)/*.c))

SRC_S += $(CMSIS_ST_DIR)/Source/Templates/gcc/startup_stm32f103x6.s

INC += src/target/$(TARGET) 
