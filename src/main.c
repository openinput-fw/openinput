/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 openinput (https://github.com/openinput-fw)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "target.h"
#include "tusb.h"

#include "usb_descriptors.h"

//--------------------------------------------------------------------
// Enums and data types
//--------------------------------------------------------------------

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 500,
  BLINK_MOUNTED = 250,
  BLINK_SUSPENDED = 2500,
};

//--------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

//--------------------------------------------------------------------
// Function declaration
//--------------------------------------------------------------------

void leds_task(void);
void hid_task(void);

//--------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------

/*------------- MAIN -------------*/
int main(void)
{
  tusb_init();

  while (1)
  {
    target_tasks();

    hid_task();

    tud_task(); // tinyusb device task

    leds_task();
  }

  return 0;
}

/*------------- USB device callbacks -------------*/

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

/*------------- USB HID -------------*/

void hid_task(void)
{
  /*------------- Mouse -------------*/
  if(tud_hid_ready())
  {
    //
    // Wheel Mouse
    //
    // Input report - 5 bytes
    //
    //     Byte | D7      D6      D5      D4      D3      D2      D1      D0
    //    ------+---------------------------------------------------------------------
    //      0   |  0       0       0    Forward  Back    Middle  Right   Left (Button)
    //      1   |                             X
    //      2   |                             Y
    //      3   |                       Vertical Wheel
    //      4   |                    Horizontal (Tilt) Wheel
    //

    uint8_t buttons = target_mouse_btns_get();
    deltas_t deltas = target_sensor_deltas_get();
    int8_t vertical = target_wheel_get();

    tud_hid_mouse_report(REPORT_ID_MOUSE, buttons, deltas.dx, deltas.dy, vertical, 0);
  }
}


// Invoked when received GET_REPORT control request
uint16_t tud_hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request
void tud_hid_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) bufsize;
}

/*------------- LED task -------------*/

void leds_task(void)
{
  static uint64_t start_ms = 0;
  static uint8_t led_state = 0;

  // Blink every interval ms
  if(target_ticks() - start_ms < blink_interval_ms) return; // not enough time
  start_ms = target_ticks() ;

  if(led_state)
    target_leds_write(0, 255, 0);
  else
    target_leds_write(0, 55, 0);

  led_state ^= 1; // toggle
}
