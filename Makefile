# ---------------------------------------
# TARGETS
# ---------------------------------------

# extracts target list from targets/*.mk files
TARGET_LIST = $(sort $(patsubst %.mk,%,$(subst targets/,,$(wildcard targets/*.mk))))

# verifies specified target
ifeq ($(filter $(TARGET),$(TARGET_LIST)),)
  $(info You must provide a TARGET parameter with 'TARGET=', supported targets are:)
  $(foreach b,$(TARGET_LIST),$(info - $(b)))
  $(error Invalid TARGET specified)
endif

# ---------------------------------------
# DIRECTORIES
# ---------------------------------------

# Build directory
BUILD = build/build-$(TARGET)

# ---------------------------------------
# CROSS COMPILER
# ---------------------------------------

# target specific define
include targets/$(TARGET).mk

# ---------- Cross Compiler  ------------
# Can be set by target, default to ARM GCC
CROSS_COMPILER ?= arm-none-eabi-

CC = $(CROSS_COMPILER)gcc
CXX = $(CROSS_COMPILER)g++
OBJCOPY = $(CROSS_COMPILER)objcopy
SIZE = $(CROSS_COMPILER)size
MKDIR = mkdir
SED = sed
CP = cp
RM = rm

# ---------------------------------------
# SOURCES
# ---------------------------------------

SRC_C += $(wildcard src/*.c)
SRC_C += src/target/target.c

INC += \
	src \
	src/target \

# Assembly files can be name with upper case .S, convert it to .s
SRC_S := $(SRC_S:.S=.s)

# ---------------------------------------
# FLAGS
# ---------------------------------------

# Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -Og -ggdb
else
  CFLAGS += -Os
endif

# TUSB Logging option
ifneq ($(LOG),)
  CFLAGS += -DCFG_TUSB_DEBUG=$(LOG)
endif

# libc
LIBS += -lgcc -lm -lnosys -lc

CFLAGS += $(addprefix -I,$(INC))

LDFLAGS += $(CFLAGS) -fshort-enums -Wl,-T,$(LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nosys.specs -specs=nano.specs
ASFLAGS += $(CFLAGS)

# ---------------------------------------
# OBJECTS
# ---------------------------------------

# assembly file should be placed first in linking order
OBJ += $(addprefix $(BUILD)/obj/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/obj/, $(SRC_C:.c=.o))

# ---------------------------------------
# VERBOSE
# ---------------------------------------

# Verbose mode
ifeq ($(V),1)
$(info TARGET_LIST )
$(foreach b,$(TARGET_LIST),$(info - $(b))) $(info )
$(info TARGET = $(TARGET) ) $(info )
$(info DEBUG = $(DEBUG) )
$(info LOG = $(LOG) ) $(info )
$(info COMPILER )
$(info CROSS_COMPILER = $(CROSS_COMPILER))
$(info CC = $(CC))
$(info CXX = $(CXX))
$(info OBJCOPY = $(OBJCOPY))
$(info SIZE = $(SIZE))
$(info MKDIR = $(MKDIR))
$(info SED = $(SED))
$(info CP = $(CP))
$(info RM = $(RM)) $(info )
$(info SRC_C )
$(foreach b,$(SRC_C),$(info - $(b))) $(info )
$(info SRC_S )
$(foreach b,$(SRC_S),$(info - $(b))) $(info )
$(info OBJ )
$(foreach b,$(OBJ),$(info - $(b))) $(info )
$(info INC  )
$(foreach b,$(INC),$(info - $(b))) $(info )
$(info LIBS )
$(foreach b,$(LIBS),$(info - $(b))) $(info )
$(info CFLAGS )
$(foreach b,$(CFLAGS),$(info - $(b))) $(info )
$(info ASFLAGS )
$(foreach b,$(ASFLAGS),$(info - $(b))) $(info )
$(info LDFLAGS )
$(foreach b,$(LDFLAGS),$(info - $(b))) $(info )
endif

# ---------------------------------------
# TASKS
# ---------------------------------------

# Set all as default goal
.DEFAULT_GOAL := all

all: clean-bin compile size

compile: $(BUILD)/$(TARGET)-firmware.bin $(BUILD)/$(TARGET)-firmware.hex size

# make directories
OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	@$(MKDIR) -p $@

# create bin file
$(BUILD)/$(TARGET)-firmware.bin: $(BUILD)/$(TARGET)-firmware.elf
	@echo Creating BIN file \'$@\'...
	@$(OBJCOPY) -O binary $^ $@

# create hex file
$(BUILD)/$(TARGET)-firmware.hex: $(BUILD)/$(TARGET)-firmware.elf
	@echo Creating HEX file \'$@\'...
	@$(OBJCOPY) -O ihex $^ $@

# build elf file
$(BUILD)/$(TARGET)-firmware.elf: $(OBJ)
	@echo Creating ELF file \'$@\'...
	@$(CC) -o $@ $(LDFLAGS) $^ -Wl,--start-group $(LIBS) -Wl,--end-group

# build c sources
$(BUILD)/obj/%.o: %.c
	@echo Compilling C file \'$<\' \> \'$@\'...
	@$(CC) $(CFLAGS) -MD -c -o $@ $<

# build assembly sources
$(BUILD)/obj/%.o: %.s
	@echo Compilling ASM file \'$<\' \> \'$@\'...
	@$(CC) -x assembler-with-cpp $(ASFLAGS) -c -o $@ $<

size: $(BUILD)/$(TARGET)-firmware.elf
	-@echo ''
	@$(SIZE) $<
	-@echo ''

clean-bin:
	@rm -f $(BUILD)/$(TARGET)-firmware.*.lss
	@rm -f $(BUILD)/$(TARGET)-firmware.*.hex
	@rm -f $(BUILD)/$(TARGET)-firmware.*.bin
	@rm -f $(BUILD)/$(TARGET)-firmware.*.map
	@rm -f $(BUILD)/$(TARGET)-firmware.*.elf

clean: clean-bin
	@rm -rf $(BUILD)/obj/*

-include $(OBJECTS:.o=.d)

.PHONY: clean clean-bin compile size all