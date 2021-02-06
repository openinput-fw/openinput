// SPDX-License-Identifier: MIT

#include "protocol/reports.h"
#include "util/types.h"

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
#define OI_ERROR_INVALID_VALUE	      0x01
#define OI_ERROR_UNSUPPORTED_FUNCTION 0x02
#define OI_ERROR_CUSTOM		      0xFE

/* supported functions enum */
enum supported_pages_index {
	INFO,
	GIMMICKS,
	DEBUG,
	PAGE_COUNT /* this will hold the number of supported function pages */
};

static const u8 supported_pages[] = {
	/* clang-format off */
	OI_PAGE_INFO,
	OI_PAGE_GIMMICKS,
	OI_PAGE_DEBUG
	/* clang-format on */
};

_Static_assert(sizeof(supported_pages) == PAGE_COUNT, "invalid size");

struct protocol_config_t {
	char *device_name;
	u8 *functions[PAGE_COUNT];
	u8 functions_size[PAGE_COUNT];
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

int protocol_is_supported(struct protocol_config_t config, u8 function_page, u8 function);

void protocol_dispatch(struct protocol_config_t config, u8 *buffer, size_t buffer_size);

void protocol_send_report(struct protocol_config_t config, struct oi_report_t msg);
void protocol_send_error(struct protocol_config_t config, struct oi_report_t msg, struct protocol_error_t error);

/* protocol functions */
void protocol_info_version(struct protocol_config_t config, struct oi_report_t msg);
void protocol_info_fw_info(struct protocol_config_t config, struct oi_report_t msg);
