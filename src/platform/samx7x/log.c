/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <stdarg.h>
#include <stdio.h>

void platform_log_msg(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

int _write(int handle, char *data, int size)
{
	(void) handle; // unused

	int count = size;

	while (count--) {
		//outputByte( data[count] ) ;  // Your low-level output function here.
	}

	return size;
}
