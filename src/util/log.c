/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

/*
 * avoid linking error if project is compiled with verbose,
 * but the log function is not implemented
 */
__attribute__((weak)) void platform_log_msg(const char *format, ...)
{
}
