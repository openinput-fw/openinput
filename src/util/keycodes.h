/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

enum keycodes_t {
	keycode_none = 0x00, /* No key pressed */
	keycode_err_rollover = 0x01, /* Keyboard Error Roll Over ("Phantom Key") */
	keycode_err_post_fail = 0x02, /* Keyboard POST Fail */
	keycode_error_undefined = 0x03, /* Keyboard Error Undefined */
	keycode_A = 0x04, /* Keyboard a and A */
	keycode_B = 0x05, /* Keyboard b and B */
	keycode_C = 0x06, /* Keyboard c and C */
	keycode_D = 0x07, /* Keyboard d and D */
	keycode_E = 0x08, /* Keyboard e and E */
	keycode_F = 0x09, /* Keyboard f and F */
	keycode_G = 0x0a, /* Keyboard g and G */
	keycode_H = 0x0b, /* Keyboard h and H */
	keycode_I = 0x0c, /* Keyboard i and I */
	keycode_J = 0x0d, /* Keyboard j and J */
	keycode_K = 0x0e, /* Keyboard k and K */
	keycode_L = 0x0f, /* Keyboard l and L */
	keycode_M = 0x10, /* Keyboard m and M */
	keycode_N = 0x11, /* Keyboard n and N */
	keycode_O = 0x12, /* Keyboard o and O */
	keycode_P = 0x13, /* Keyboard p and P */
	keycode_Q = 0x14, /* Keyboard q and Q */
	keycode_R = 0x15, /* Keyboard r and R */
	keycode_S = 0x16, /* Keyboard s and S */
	keycode_T = 0x17, /* Keyboard t and T */
	keycode_U = 0x18, /* Keyboard u and U */
	keycode_V = 0x19, /* Keyboard v and V */
	keycode_W = 0x1a, /* Keyboard w and W */
	keycode_X = 0x1b, /* Keyboard x and X */
	keycode_Y = 0x1c, /* Keyboard y and Y */
	keycode_Z = 0x1d, /* Keyboard z and Z */
	keycode_1 = 0x1e, /* Keyboard 1 and ! */
	keycode_2 = 0x1f, /* Keyboard 2 and @ */
	keycode_3 = 0x20, /* Keyboard 3 and # */
	keycode_4 = 0x21, /* Keyboard 4 and $ */
	keycode_5 = 0x22, /* Keyboard 5 and % */
	keycode_6 = 0x23, /* Keyboard 6 and ^ */
	keycode_7 = 0x24, /* Keyboard 7 and &amp; */
	keycode_8 = 0x25, /* Keyboard 8 and * */
	keycode_9 = 0x26, /* Keyboard 9 and ( */
	keycode_0 = 0x27, /* Keyboard 0 and ) */
	keycode_enter = 0x28, /* Keyboard Return (ENTER) */
	keycode_esc = 0x29, /* Keyboard ESCAPE */
	keycode_backspace = 0x2a, /* Keyboard DELETE (Backspace) */
	keycode_tab = 0x2b, /* Keyboard Tab */
	keycode_space = 0x2c, /* Keyboard Spacebar */
	keycode_minus = 0x2d, /* Keyboard - and _ */
	keycode_equal = 0x2e, /* Keyboard = and + */
	keycode_left_brace = 0x2f, /* Keyboard [ and { */
	keycode_right_brace = 0x30, /* Keyboard ] and } */
	keycode_backslash = 0x31, /* Keyboard \ and | */
	keycode_hashtilde = 0x32, /* Keyboard Non-US # and ~ */
	keycode_semicolon = 0x33, /* Keyboard ; and : */
	keycode_apostrophe = 0x34, /* Keyboard ' and " */
	keycode_grave = 0x35, /* Keyboard ` and ~ */
	keycode_comma = 0x36, /* Keyboard , and LESSTHAN &lt; */
	keycode_period = 0x37, /* Keyboard . and > */
	keycode_forwardslash = 0x38, /* Keyboard / and ? */
	keycode_capslock = 0x39, /* Keyboard Caps Lock */
	keycode_f1 = 0x3a, /* Keyboard F1 */
	keycode_f2 = 0x3b, /* Keyboard F2 */
	keycode_f3 = 0x3c, /* Keyboard F3 */
	keycode_f4 = 0x3d, /* Keyboard F4 */
	keycode_f5 = 0x3e, /* Keyboard F5 */
	keycode_f6 = 0x3f, /* Keyboard F6 */
	keycode_f7 = 0x40, /* Keyboard F7 */
	keycode_f8 = 0x41, /* Keyboard F8 */
	keycode_f9 = 0x42, /* Keyboard F9 */
	keycode_f10 = 0x43, /* Keyboard F10 */
	keycode_f11 = 0x44, /* Keyboard F11 */
	keycode_f12 = 0x45, /* Keyboard F12 */
	keycode_printscr = 0x46, /* Keyboard Print Screen */
	keycode_scrolllock = 0x47, /* Keyboard Scroll Lock */
	keycode_pause = 0x48, /* Keyboard Pause */
	keycode_insert = 0x49, /* Keyboard Insert */
	keycode_home = 0x4a, /* Keyboard Home */
	keycode_pageup = 0x4b, /* Keyboard Page Up */
	keycode_delete = 0x4c, /* Keyboard Delete Forward */
	keycode_end = 0x4d, /* Keyboard End */
	keycode_pagedown = 0x4e, /* Keyboard Page Down */
	keycode_right_arrow = 0x4f, /* Keyboard Right Arrow */
	keycode_left_arrow = 0x50, /* Keyboard Left Arrow */
	keycode_down_arrow = 0x51, /* Keyboard Down Arrow */
	keycode_up_arrow = 0x52, /* Keyboard Up Arrow */
	keycode_np_numlock = 0x53, /* Keyboard Num Lock and Clear */
	keycode_np_slash = 0x54, /* Keypad / */
	keycode_np_asterisk = 0x55, /* Keypad * */
	keycode_np_minus = 0x56, /* Keypad - */
	keycode_np_plus = 0x57, /* Keypad + */
	keycode_np_enter = 0x58, /* Keypad ENTER */
	keycode_np_1 = 0x59, /* Keypad 1 and End */
	keycode_np_2 = 0x5a, /* Keypad 2 and Down Arrow */
	keycode_np_3 = 0x5b, /* Keypad 3 and PageDn */
	keycode_np_4 = 0x5c, /* Keypad 4 and Left Arrow */
	keycode_np_5 = 0x5d, /* Keypad 5 */
	keycode_np_6 = 0x5e, /* Keypad 6 and Right Arrow */
	keycode_np_7 = 0x5f, /* Keypad 7 and Home */
	keycode_np_8 = 0x60, /* Keypad 8 and Up Arrow */
	keycode_np_9 = 0x61, /* Keypad 9 and Page Up */
	keycode_np_0 = 0x62, /* Keypad 0 and Insert */
	keycode_np_dot = 0x63, /* Keypad . and Delete */
	keycode_102nd = 0x64, /* Keyboard Non-US \ and | */
	keycode_compose = 0x65, /* Keyboard Application */
	keycode_power = 0x66, /* Keyboard Power */
	keycode_np_equalqual = 0x67, /* Keypad = */
	keycode_f13 = 0x68, /* Keyboard F13 */
	keycode_f14 = 0x69, /* Keyboard F14 */
	keycode_f15 = 0x6a, /* Keyboard F15 */
	keycode_f16 = 0x6b, /* Keyboard F16 */
	keycode_f17 = 0x6c, /* Keyboard F17 */
	keycode_f18 = 0x6d, /* Keyboard F18 */
	keycode_f19 = 0x6e, /* Keyboard F19 */
	keycode_f20 = 0x6f, /* Keyboard F20 */
	keycode_f21 = 0x70, /* Keyboard F21 */
	keycode_f22 = 0x71, /* Keyboard F22 */
	keycode_f23 = 0x72, /* Keyboard F23 */
	keycode_f24 = 0x73, /* Keyboard F24 */
	keycode_open = 0x74, /* Keyboard Execute */
	keycode_help = 0x75, /* Keyboard Help */
	keycode_menu = 0x76, /* Keyboard Menu */
	keycode_select = 0x77, /* Keyboard Select */
	keycode_stop = 0x78, /* Keyboard Stop */
	keycode_again = 0x79, /* Keyboard Again */
	keycode_undo = 0x7a, /* Keyboard Undo */
	keycode_cut = 0x7b, /* Keyboard Cut */
	keycode_copy = 0x7c, /* Keyboard Copy */
	keycode_paste = 0x7d, /* Keyboard Paste */
	keycode_find = 0x7e, /* Keyboard Find */
	keycode_mute = 0x7f, /* Keyboard Mute */
	keycode_volume_up = 0x80, /* Keyboard Volume Up */
	keycode_volume_down = 0x81, /* Keyboard Volume Down */
	keycode_capslock_locking = 0x82, /* Keyboard Locking Caps Lock */
	keycode_numlock_locking = 0x83, /* Keyboard Locking Num Lock */
	keycode_scrolllock_locking = 0x84, /* Keyboard Locking Scroll Lock */
	keycode_br_comma = 0x85, /* Keypad Brazilian Comma */
	keycode_br_equal = 0x86, /* Keyboard Brazilian Equal Sign */
	keycode_ro = 0x87, /* Keyboard International1 */
	keycode_katakana_hiragana = 0x88, /* Keyboard International2 */
	keycode_yen = 0x89, /* Keyboard International3 */
	keycode_henken = 0x8a, /* Keyboard International4 */
	keycode_muhenkan = 0x8b, /* Keyboard International5 */
	keycode_jp_comma = 0x8c, /* Keyboard International6 */

	// 0x8d  Keyboard International7
	// 0x8e  Keyboard International8
	// 0x8f  Keyboard International9

	keycode_hanguel = 0x90, /* Keyboard LANG1 */
	keycode_hanja = 0x91, /* Keyboard LANG2 */
	keycode_katakana = 0x92, /* Keyboard LANG3 */
	keycode_hiragana = 0x93, /* Keyboard LANG4 */
	keycode_zenkaku_hankaku = 0x94, /* Keyboard LANG5 */

	// 0x95  Keyboard LANG6
	// 0x96  Keyboard LANG7
	// 0x97  Keyboard LANG8
	// 0x98  Keyboard LANG9
	// 0x99  Keyboard Alternate Erase
	// 0x9a  Keyboard SysReq/Attention
	// 0x9b  Keyboard Cancel
	// 0x9c  Keyboard Clear
	// 0x9d  Keyboard Prior
	// 0x9e  Keyboard Return
	// 0x9f  Keyboard Separator
	// 0xa0  Keyboard Out
	// 0xa1  Keyboard Oper
	// 0xa2  Keyboard Clear/Again
	// 0xa3  Keyboard CrSel/Props
	// 0xa4  Keyboard ExSel

	// 0xb0  Keypad 00
	// 0xb1  Keypad 000
	// 0xb2  Thousands Separator
	// 0xb3  Decimal Separator
	// 0xb4  Currency Unit
	// 0xb5  Currency Sub-unit

	keycode_np_left_parentheses = 0xb6, /* Keypad ( */
	keycode_np_right_parentheses = 0xb7, /* Keypad ) */

	// 0xb8  Keypad {
	// 0xb9  Keypad }
	// 0xba  Keypad Tab
	// 0xbb  Keypad Backspace
	// 0xbc  Keypad A
	// 0xbd  Keypad B
	// 0xbe  Keypad C
	// 0xbf  Keypad D
	// 0xc0  Keypad E
	// 0xc1  Keypad F
	// 0xc2  Keypad XOR
	// 0xc3  Keypad ^
	// 0xc4  Keypad %
	// 0xc5  Keypad <
	// 0xc6  Keypad >
	// 0xc7  Keypad &
	// 0xc8  Keypad &&
	// 0xc9  Keypad |
	// 0xca  Keypad ||
	// 0xcb  Keypad :
	// 0xcc  Keypad #
	// 0xcd  Keypad Space
	// 0xce  Keypad @
	// 0xcf  Keypad !
	// 0xd0  Keypad Memory Store
	// 0xd1  Keypad Memory Recall
	// 0xd2  Keypad Memory Clear
	// 0xd3  Keypad Memory Add
	// 0xd4  Keypad Memory Subtract
	// 0xd5  Keypad Memory Multiply
	// 0xd6  Keypad Memory Divide
	// 0xd7  Keypad +/-
	// 0xd8  Keypad Clear
	// 0xd9  Keypad Clear Entry
	// 0xda  Keypad Binary
	// 0xdb  Keypad Octal
	// 0xdc  Keypad Decimal
	// 0xdd  Keypad Hexadecimal

	keycode_left_control = 0xe0, /* Keyboard Left Control */
	keycode_left_shift = 0xe1, /* Keyboard Left Shift */
	keycode_left_alt = 0xe2, /* Keyboard Left Alt */
	keycode_left_meta = 0xe3, /* Keyboard Left GUI */
	keycode_right_control = 0xe4, /* Keyboard Right Control */
	keycode_right_shift = 0xe5, /* Keyboard Right Shift */
	keycode_right_alt = 0xe6, /* Keyboard Right Alt */
	keycode_right_meta = 0xe7, /* Keyboard Right GUI */

	keycode_media_play_pause = 0xe8, /* Keyboard Media Play/Plause */
	keycode_media_stop_cd = 0xe9, /* Keyboard Media StopCD */
	keycode_media_previous = 0xea, /* Keyboard Media Previous */
	keycode_media_next = 0xeb, /* Keyboard Media Next */
	keycode_media_eject_cd = 0xec, /* Keyboard Media Eject CD */
	keycode_media_volume_up = 0xed, /* Keyboard Media Volume UP */
	keycode_media_volume_down = 0xee, /* Keyboard Media Volume Down */
	keycode_media_mute = 0xef, /* Keyboard Media Mute */
	keycode_media_www = 0xf0, /* Keyboard Media WWW Web Browser */
	keycode_media_back = 0xf1, /* Keyboard Media Back */
	keycode_media_forward = 0xf2, /* Keyboard Media Forward */
	keycode_media_stop = 0xf3, /* Keyboard Media Stop */
	keycode_media_find = 0xf4, /* Keyboard Media Find */
	keycode_media_scrollup = 0xf5, /* Keyboard Media Scroll Up */
	keycode_media_Scrolldown = 0xf6, /* Keyboard Media Scroll Down */
	keycode_media_edit = 0xf7, /* Keyboard Media Edit */
	keycode_media_sleep = 0xf8, /* Keyboard Media Sleep */
	keycode_media_coffee = 0xf9, /* Keyboard Media Coffee */
	keycode_media_refresh = 0xfa, /* Keyboard Media Refresh */
	keycode_media_calculator = 0xfb, /* Keyboard Media Calc */
};
