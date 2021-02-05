// SPDX-License-Identifier: MIT

#include "protocol/reports.h"
#include "util/types.h"

struct protocol_config_t {
	char *device_name;
	u16 *supported_functions;
	size_t supported_functions_lenght;
	void *hid_interface;
};

int protocol_is_supported(struct protocol_config_t config, u16 function);

void protocol_dispatch(struct protocol_config_t config, u8 *buffer, size_t buffer_size);

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

/* protocol functions */
void protocol_info_version(struct protocol_config_t config, struct oi_report_t msg);
