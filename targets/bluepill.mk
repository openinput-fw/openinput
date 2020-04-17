# ---------------------------------------
# FAMILY
# ---------------------------------------

include targets/generic/stm32f1.mk

# ---------------------------------------
# LINKER
# ---------------------------------------

LD_FILE = targets/ld/bluepill.ld

# ---------------------------------------
# FLAGS
# ---------------------------------------

CFLAGS += -DSTM32F103xB \
		  -DSTM32F10X_MD

# ---------------------------------------
# SOURCES
# ---------------------------------------

SRC_C += $(subst ,,$(wildcard src/target/$(TARGET)/*.c))

SRC_S += $(CMSIS_ST_DIR)/Source/Templates/gcc/startup_stm32f103xb.s

INC += src/target/$(TARGET)