# ---------------------------------------
# SOURCES
# ---------------------------------------

TUSB_SRC_DIR = src/tinyusb

# TinyUSB Stack source
SRC_C += \
	$(TUSB_SRC_DIR)/tusb.c \
	$(TUSB_SRC_DIR)/common/tusb_fifo.c \
	$(TUSB_SRC_DIR)/device/usbd.c \
	$(TUSB_SRC_DIR)/device/usbd_control.c \
	$(TUSB_SRC_DIR)/class/cdc/cdc_device.c \
	$(TUSB_SRC_DIR)/class/dfu/dfu_rt_device.c \
	$(TUSB_SRC_DIR)/class/hid/hid_device.c \
	$(TUSB_SRC_DIR)/class/midi/midi_device.c \
	$(TUSB_SRC_DIR)/class/msc/msc_device.c \
	$(TUSB_SRC_DIR)/class/net/net_device.c \
	$(TUSB_SRC_DIR)/class/usbtmc/usbtmc_device.c \
	$(TUSB_SRC_DIR)/class/vendor/vendor_device.c \
	$(TUSB_SRC_DIR)/portable/$(VENDOR)/$(CHIP_FAMILY)/dcd_$(CHIP_FAMILY).c

# TinyUSB stack include
INC += $(TUSB_SRC_DIR)