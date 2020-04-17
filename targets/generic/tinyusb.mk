# ---------------------------------------
# SOURCES
# ---------------------------------------

# TinyUSB Stack source
SRC_C += \
	src/tinyusb/tusb.c \
	src/tinyusb/common/tusb_fifo.c \
	src/tinyusb/device/usbd.c \
	src/tinyusb/device/usbd_control.c \
	src/tinyusb/class/cdc/cdc_device.c \
	src/tinyusb/class/dfu/dfu_rt_device.c \
	src/tinyusb/class/hid/hid_device.c \
	src/tinyusb/class/midi/midi_device.c \
	src/tinyusb/class/msc/msc_device.c \
	src/tinyusb/class/net/net_device.c \
	src/tinyusb/class/usbtmc/usbtmc_device.c \
	src/tinyusb/class/vendor/vendor_device.c \
	src/tinyusb/portable/$(VENDOR)/$(CHIP_FAMILY)/dcd_$(CHIP_FAMILY).c

# TinyUSB stack include
INC += src/tinyusb