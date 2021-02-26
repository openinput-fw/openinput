/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

/* Common Configuration */

#define CFG_TUSB_MCU OPT_MCU_STM32F1

#define CFG_TUSB_OS OPT_OS_NONE

#define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE

/*
 * USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN __attribute__((aligned(4)))
#endif

/* Device Configuration */

#define CFG_TUD_ENDPOINT0_SIZE 64

/* Class */
#define CFG_TUD_HID    1
#define CFG_TUD_CDC    0
#define CFG_TUD_MSC    0
#define CFG_TUD_MIDI   0
#define CFG_TUD_VENDOR 0

/* HID buffer size Should be sufficient to hold ID (if any) + Data */
#define CFG_TUD_HID_BUFSIZE 32
