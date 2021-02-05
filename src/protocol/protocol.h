// SPDX-License-Identifier: MIT

#include "protocol/reports.h"
#include "util/types.h"

#define FN(page, function) (page << 8) + function

/* version */
#define OI_PROTOCOL_VERSION_MAJOR 0
#define OI_PROTOCOL_VERSION_MINOR 0
#define OI_PROTOCOL_VERSION_PATCH 0

/* reports */
#define OI_REPORT_SHORT	     0x20
#define OI_REPORT_LONG	     0x21
#define OI_REPORT_SHORT_SIZE 8
#define OI_REPORT_LONG_SIZE  32

/* protocol function pages */
#define OI_PAGE_INFO		 0x00
#define OI_PAGE_GENERAL_PROFILES 0x01
#define OI_PAGE_GIMMICKS	 0xFD
#define OI_PAGE_DEBUG		 0xFE
#define OI_PAGE_ERROR		 0xFF

/* info page (0x00) functions */
#define OI_FUNCTION_VERSION 0x00
#define OI_FUNCTION_FW_INFO 0x01

/* error page (0xFF) */
#define OI_ERROR_INVALID_VALUE 0x01
#define OI_ERROR_CUSTOM	       0xFE

struct protocol_config_t {
	char *device_name;
	u16 *supported_functions;
	size_t supported_functions_lenght;
	void *hid_interface;
};

struct protocol_error_t {
	u8 id;
	union {
		/* 0x01 - Invalid value */
		struct {
			u8 position;
		} invalid_value;
		/* 0xFE - Custom error */
		struct {
			char description[29];
		};
	} args;
};

int protocol_is_supported(struct protocol_config_t config, u16 function);

void protocol_dispatch(struct protocol_config_t config, u8 *buffer, size_t buffer_size);

void protocol_send_report(struct protocol_config_t config, struct oi_report_t msg);
void protocol_send_error(struct protocol_config_t config, struct oi_report_t msg, struct protocol_error_t error);

/* protocol functions */
void protocol_info_version(struct protocol_config_t config, struct oi_report_t msg);
